
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

#define max(a, b)                                                              \
	({                                                                     \
		__typeof__(a) _a = (a);                                        \
		__typeof__(b) _b = (b);                                        \
		_a > _b ? _a : _b;                                             \
	})

void dump_struct(void *strct, size_t size)
{
	size_t i;
	uint8_t *start = (uint8_t *)strct;

	printf("%p -> ", start);
	for (i = 0; i < size; i++) {
		printf("%x ", start[i]);
	}
	printf("/ size:%lx\n", size);
}

int parse_img(struct bitmap *bmp, FILE *in)
{
	size_t i;

	// --- read the headers ---
	// read bitmap image header
	if (fread(&bmp->header, 1, sizeof(struct bmp_header), in) !=
	    sizeof(struct bmp_header)) {
		return 1;
	}

	// read the info header
	if (fread(&bmp->info, 1, sizeof(struct bmp_infoheader), in) !=
	    sizeof(struct bmp_infoheader)) {
		return 2;
	}

	// at the moment only the bitfields compression method is supported
	if (bmp->info.compression_method != 0x3) {
		return 3;
	}

	// --- read color table ---
	// jump to the start of the color table
	if (fseek(in, bmp->info.header_size + bmp_header_size, SEEK_SET)) {
		return 4;
	}

	bmp->red_offset = __builtin_ffs(bmp->info.red_mask) - 1;
	bmp->green_offset = __builtin_ffs(bmp->info.green_mask) - 1;
	bmp->blue_offset = __builtin_ffs(bmp->info.blue_mask) - 1;

	// get color table and size
	if (bmp->info.num_colors_in_palette) {
		bmp->color_table_size = bmp->info.num_colors_in_palette;
	} else {
		// calculate the size with the max index of any color
		bmp->color_table_size =
			max(max(bmp->info.red_mask >> bmp->red_offset,
				bmp->info.green_mask >> bmp->green_offset),
			    bmp->info.blue_mask >> bmp->blue_offset);
	}

	bmp->color_table = malloc(bmp->color_table_size * 4);
	if (bmp->color_table == NULL) {
		return 5;
	}

	// read color table
	if (fread(bmp->color_table, 4, bmp->color_table_size, in) !=
	    bmp->color_table_size) {
		free(bmp->color_table);
		return 6;
	}

	// --- read image array ---
	// jump to the image array
	if (fseek(in, bmp->header.pixel_array_offset, SEEK_SET)) {
		free(bmp->color_table);
		return 7;
	}

	// calculate size and allocate array for the offset data
	bmp->image_size = (bmp->info.colordepth * bmp->info.bmp_width / 32) *
			  bmp->info.bmp_height;

	bmp->image =
		malloc(bmp->image_size * 4 * sizeof(struct bmp_image_member));
	if (bmp->image == NULL) {
		free(bmp->color_table);
		return 8;
	}

	// convert the 32b chunk into a bmp_image_member
	// and add that to the array
	{
		for (i = 0; i < bmp->image_size; i++) {
			uint32_t buffer;

			uint32_t red_index;
			uint32_t green_index;
			uint32_t blue_index;

			// read one image chunk
			if (fread(&buffer, 4, 1, in) != 1) {
				free(bmp->image);
				free(bmp->color_table);
				return 9;
			}

			// calculate index into color table
			red_index = (bmp->info.red_mask & buffer) >>
				    bmp->red_offset;

			green_index = (bmp->info.green_mask & buffer) >>
				      bmp->green_offset;

			blue_index = (bmp->info.blue_mask & buffer) >>
				     bmp->blue_offset;

			// fetch color from colortable
			bmp->image[i].none = buffer;

			bmp->image[i].red = (bmp->color_table[red_index] &
					     bmp->info.red_mask) >>
					    bmp->red_offset;

			bmp->image[i].green = (bmp->color_table[green_index] &
					       bmp->info.green_mask) >>
					      bmp->green_offset;

			bmp->image[i].blue = (bmp->color_table[blue_index] &
					      bmp->info.blue_mask) >>
					     bmp->blue_offset;
		}
	}

	return 0;
}

void close_img(struct bitmap *bmp)
{
	free(bmp->image);
	bmp->image_size = 0;

	free(bmp->color_table);
	bmp->color_table_size = 0;
}

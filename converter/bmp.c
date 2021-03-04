
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

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

	// at the moment only the bitfields compression is supported
	if (bmp->info.compression_method != 0x3) {
		return 3;
	}

	// get color table and size
	bmp->color_table_size = bmp->info.num_colors_in_palette;

	bmp->color_table = malloc(bmp->color_table_size * 4);
	if (bmp->color_table == NULL) {
		return 4;
	}

	if (fread(&bmp->color_table, 4, bmp->color_table_size, in) !=
	    bmp->color_table_size) {
		return 5;
	}

	// skip to image array
	if (fseek(in, bmp->header.pixel_array_offset, SEEK_SET)) {
		return 6;
	}

	// calculate size and allocate array for the offset data
	bmp->image_size = (bmp->info.colordepth * bmp->info.bmp_width / 32) *
			  bmp->info.bmp_height;

	bmp->image =
		malloc(bmp->image_size * 4 * sizeof(struct bmp_image_member));
	if (bmp->image == NULL) {
		return 7;
	}

	// convert the 32b chunk into a bmp_image_member
	// and add that to the array
	{
		uint8_t alpha_offset = __builtin_ffs(bmp->info.alpha_mask) - 1;
		uint8_t red_offset = __builtin_ffs(bmp->info.red_mask) - 1;
		uint8_t green_offset = __builtin_ffs(bmp->info.green_mask) - 1;
		uint8_t blue_offset = __builtin_ffs(bmp->info.blue_mask) - 1;

		for (i = 0; i < bmp->image_size; i++) {
			uint32_t buffer;

			uint32_t alpha_index;
			uint32_t red_index;
			uint32_t green_index;
			uint32_t blue_index;

			// read one image chunk
			if (fread(&buffer, 4, 1, in) != 1) {
				free(bmp->image);
				return 5;
			}

			// calculate index into color table
			alpha_index =
				(bmp->info.alpha_mask & buffer) >> alpha_offset;

			red_index = (bmp->info.red_mask & buffer) >> red_offset;

			green_index =
				(bmp->info.green_mask & buffer) >> green_offset;

			blue_index =
				(bmp->info.blue_mask & buffer) >> blue_offset;

			// fetch color from colortable
			bmp->image[i].none = buffer;

			bmp->image[i].alpha = (bmp->color_table[alpha_index] &
					       bmp->info.alpha_mask) >>
					      alpha_offset;

			bmp->image[i].red = (bmp->color_table[red_index] &
					     bmp->info.red_mask) >>
					    red_offset;

			bmp->image[i].green = (bmp->color_table[green_index] &
					       bmp->info.green_mask) >>
					      green_offset;

			bmp->image[i].blue = (bmp->color_table[blue_index] &
					      bmp->info.blue_mask) >>
					     blue_offset;
		}
	}

	return 0;
}

void close_img(struct bitmap *bmp)
{
	free(bmp->image);
	bmp->image_size = 0;
}


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
	int rv;
	size_t i;

	bmp->color_table = NULL;
	bmp->image = NULL;

	// --- read the headers ---
	// read bitmap image header
	if (fread(&bmp->header, 1, sizeof(struct bmp_header), in) !=
	    sizeof(struct bmp_header)) {
		rv = 1;
		goto error_exit;
	}

	// read the info header
	if (fread(&bmp->info, 1, sizeof(struct bmp_infoheader), in) !=
	    sizeof(struct bmp_infoheader)) {
		rv = 2;
		goto error_exit;
	}

	// at the moment only the bitfields compression method is supported (barely)
	if (bmp->info.compression_method != 0x3) {
		rv = 3;
		goto error_exit;
	}

	bmp->alpha_offset = __builtin_ffs(bmp->info.red_mask) - 1;
	bmp->red_offset = __builtin_ffs(bmp->info.red_mask) - 1;
	bmp->green_offset = __builtin_ffs(bmp->info.green_mask) - 1;
	bmp->blue_offset = __builtin_ffs(bmp->info.blue_mask) - 1;

	// --- read color table ---
	// get color table and size
	bmp->color_table_size = bmp->info.num_colors_in_palette;

	printf("color_table_size:%ld\n", bmp->color_table_size);

	if (bmp->info.color_space == LCS_CALIBRATED_RGB) {
		// jump to the start of the color table
		if (fseek(in, bmp->info.header_size + bmp_header_size,
			  SEEK_SET)) {
			rv = 4;
			goto error_exit;
		}

		// color table is actualy only 3 byte big
		bmp->color_table = malloc(bmp->color_table_size * 4);
		if (bmp->color_table == NULL) {
			rv = 5;
			goto error_exit;
		}

		// read color table width padding of 1 byte
		for (i = 0; i < bmp->color_table_size; i++) {
			uint32_t entry;

			// read color table
			if (fread(&entry, 3, 1, in) != 1) {
				rv = 6;
				goto error_exit;
			}

			bmp->color_table[i] = entry;
		}
	}

	// --- read image array ---
	// jump to the image array
	if (fseek(in, bmp->header.pixel_array_offset, SEEK_SET)) {
		rv = 7;
		goto error_exit;
	}

	// calculate size and allocate array for the offset data
	{
		size_t color_width;

		color_width = bmp->info.colordepth * bmp->info.bmp_width;

		// compensate for padding in size
		if (color_width % 32 != 0) {
			color_width += 32 - (color_width % 32);
		}

		bmp->image_size = (color_width / 32) * bmp->info.bmp_height;

		bmp->image = malloc(bmp->image_size * 4 *
				    sizeof(struct bmp_image_member));
		if (bmp->image == NULL) {
			rv = 8;
			goto error_exit;
		}

		// convert with colortable (TODO either test or remove support)
		if (bmp->info.color_space == LCS_CALIBRATED_RGB) {
			// convert the 32b chunk into a bmp_image_member
			// and add that to the array
			for (i = 0; i < bmp->image_size; i++) {
				uint32_t buffer;

				uint32_t alpha_index;
				uint32_t red_index;
				uint32_t green_index;
				uint32_t blue_index;

				// read one image chunk
				if (fread(&buffer, 4, 1, in) != 1) {
					rv = 9;
					goto error_exit;
				}

				// calculate index into color table
				alpha_index = (bmp->info.alpha_mask & buffer) >>
					      bmp->alpha_offset;

				red_index = (bmp->info.red_mask & buffer) >>
					    bmp->red_offset;

				green_index = (bmp->info.green_mask & buffer) >>
					      bmp->green_offset;

				blue_index = (bmp->info.blue_mask & buffer) >>
					     bmp->blue_offset;

				//printf("buffer:%x\n", buffer);
				//printf("%ld: r:%x g:%x b:%x\n", i, red_index,
				//       green_index, blue_index);

				// check if any index is bigger than the maximum index
				if (alpha_index >= bmp->color_table_size ||
				    red_index >= bmp->color_table_size ||
				    green_index >= bmp->color_table_size ||
				    blue_index >= bmp->color_table_size) {
					printf("Index out of bounds!\nalpha:%d / red:%d / green:%d / blue:%d / size:%ld\n",
					       alpha_index, red_index,
					       green_index, blue_index,
					       bmp->color_table_size);
					rv = 10;
					goto error_exit;
				}

				// fetch color from colortable
				bmp->image[i].none = buffer;

				bmp->image[i].alpha =
					(bmp->color_table[alpha_index] &
					 bmp->info.alpha_mask) >>
					bmp->alpha_offset;

				bmp->image[i].red =
					(bmp->color_table[red_index] &
					 bmp->info.red_mask) >>
					bmp->red_offset;

				bmp->image[i].green =
					(bmp->color_table[green_index] &
					 bmp->info.green_mask) >>
					bmp->green_offset;

				bmp->image[i].blue =
					(bmp->color_table[blue_index] &
					 bmp->info.blue_mask) >>
					bmp->blue_offset;
			}
		} else {
			// convert using pixel array only
			for (i = 0; i < bmp->image_size; i++) {
				uint32_t buffer;

				// read one image chunk
				if (fread(&buffer, 4, 1, in) != 1) {
					rv = 9;
					goto error_exit;
				}

				bmp->image[i].none = buffer;

				bmp->image[i].alpha =
					(buffer & bmp->info.alpha_mask) >>
					bmp->alpha_offset;

				bmp->image[i].red =
					(buffer & bmp->info.red_mask) >>
					bmp->red_offset;

				bmp->image[i].green =
					(buffer & bmp->info.green_mask) >>
					bmp->green_offset;

				bmp->image[i].blue =
					(buffer & bmp->info.blue_mask) >>
					bmp->blue_offset;
			}
		}
	}
	return 0;

error_exit:
	close_img(bmp);
	return rv;
}

void close_img(struct bitmap *bmp)
{
	if (bmp->image != NULL) {
		free(bmp->image);
		bmp->image = NULL;
		bmp->image_size = 0;
	}

	if (bmp->color_table != NULL) {
		free(bmp->color_table);
		bmp->color_table = NULL;
		bmp->color_table_size = 0;
	}
}

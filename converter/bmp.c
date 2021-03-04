
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

	// skip to image array
	if (fseek(in, bmp->header.pixel_array_offset, SEEK_SET)) {
		return 3;
	}

	// calculate size and allocate array
	bmp->image_size = (bmp->info.colordepth * bmp->info.bmp_width / 32) *
			  bmp->info.bmp_heigth;

	bmp->image =
		malloc(bmp->image_size * 4 * sizeof(struct bmp_image_member));
	if (bmp->image == NULL) {
		return 4;
	}

	// convert the 32b chunk into a bmp_image_member
	// and add that to the array
	{
		uint8_t alpha_offset = __builtin_ffs(bmp->info.alpha_mask) - 1;
		uint8_t green_offset = __builtin_ffs(bmp->info.green_mask) - 1;
		uint8_t red_offset = __builtin_ffs(bmp->info.red_mask) - 1;
		uint8_t blue_offset = __builtin_ffs(bmp->info.blue_mask) - 1;

		for (i = 0; i < bmp->image_size; i++) {
			uint32_t buffer;

			// read one image chunk
			if (fread(&buffer, 4, 1, in) != 1) {
				free(bmp->image);
				return 5;
			}

			bmp->image[i].none = buffer;

			bmp->image[i].alpha =
				(bmp->info.alpha_mask & buffer) >> alpha_offset;

			bmp->image[i].green =
				(bmp->info.green_mask & buffer) >> green_offset;

			bmp->image[i].red =
				(bmp->info.red_mask & buffer) >> red_offset;

			bmp->image[i].blue =
				(bmp->info.blue_mask & buffer) >> blue_offset;
		}
	}

	return 0;
}

void close_img(struct bitmap *bmp)
{
	free(bmp->image);
	bmp->image_size = 0;
}


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
			  4 * bmp->info.bmp_heigth;

	bmp->image = malloc(bmp->image_size);
	if (bmp->image == NULL) {
		return 4;
	}

	// read image array
	if (fread(bmp->image, 1, bmp->image_size, in) != bmp->image_size) {
		return 5;
	}

	return 0;
}

void close_img(struct bitmap *bmp)
{
	free(bmp->image);
	bmp->image_size = 0;
}

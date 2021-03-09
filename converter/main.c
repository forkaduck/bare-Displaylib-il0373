
/* This is extremly simple application which parses
 * a bmp file with a color depth of 1 into the fitting
 * array format for the use in the display lib.
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <math.h>

#include <linux/limits.h>

#include "bmp.h"

struct args {
	char input[PATH_MAX];
	char output[PATH_MAX];

	uint32_t thresh_red;
	uint32_t thresh_black;

	uint32_t res_width;
	uint32_t res_height;
};

size_t strlen_s(char *input, size_t max)
{
	size_t i;

	for (i = 0; i < max; i++) {
		if (input[i] == '\0') {
			break;
		}
	}
	return i;
}

int handle_args(struct args *args, int argc, char *argv[])
{
	size_t i;

	for (i = 0; i < argc; i++) {
		switch (argv[i][1]) {
		case 'i':
			memcpy(args->input, argv[i + 1],
			       strlen_s(argv[i + 1], PATH_MAX));
			printf("Input:%s\n", args->input);
			break;

		case 'o':
			memcpy(args->output, argv[i + 1],
			       strlen_s(argv[i + 1], PATH_MAX));
			printf("Output:%s\n", args->output);
			break;

		case 't':
			switch (argv[i][2]) {
			case 'r':
				args->thresh_red =
					strtoll(argv[i + 1], NULL, 0);
				printf("Threshold red:%d\n", args->thresh_red);
				break;

			case 'b':
				args->thresh_black =
					strtoll(argv[i + 1], NULL, 0);
				printf("Threshold black:%d\n",
				       args->thresh_black);
				break;
			}
			break;

		case 'r':
			switch (argv[i][2]) {
			case 'w':
				args->res_width = strtoll(argv[i + 1], NULL, 0);
				printf("Target width:%d\n", args->res_width);
				break;

			case 'h':
				args->res_height =
					strtoll(argv[i + 1], NULL, 0);
				printf("Target height:%d\n", args->res_height);
				break;
			}
		}
	}

	if (*args->input == '\0' || *args->output == '\0' ||
	    args->thresh_red == 0 || args->thresh_black == 0 ||
	    args->res_width == 0 || args->res_height == 0) {
		printf("Not enougth arguments given!\n");
		return 1;
	}
	return 0;
}

void print_file(FILE *out, uint8_t *data, size_t size, size_t width,
		size_t height)
{
	size_t i;

	char head[] = { "#ifndef IMG_H\n"
			"#define IMG_H\n"
			"#include <stdint.h>\n"
			"#define img_size" };

	char mid[] = { "/ 8\n"
		       "uint8_t img_map[img_size] = {\n" };

	char tail[] = { "\n};\n"
			"#endif\n" };

	// print head and mid with size
	fprintf(out, "%s %lu * %lu %s", head, width, height, mid);

	for (i = 0; i < size; i++) {
		fprintf(out, "0x%x, ", data[i]);

		if ((i + 1) % ((width / 32 * 8) - 1) == 0) {
			fprintf(out, "\n");
		}
	}

	// print tail
	fprintf(out, "%s", tail);
}

int main(int argc, char *argv[])
{
	int rv;
	struct args args;
	FILE *in, *out;

	memset(&args, 0, sizeof(struct args));

	rv = handle_args(&args, argc, argv);
	if (rv) {
		printf("handle_args failed! (%d)\n", rv);
		return 1;
	}

	// open input and output file
	in = fopen(args.input, "rb");
	if (in == NULL) {
		perror("fopen failed");
		return 1;
	}

	out = fopen(args.output, "wb");
	if (out == NULL) {
		perror("fopen failed");
		return 2;
	}

	{
		uint8_t *buffer;
		size_t i;
		struct bitmap bmp;

		printf("Parsing image...\n");
		rv = parse_img(&bmp, in);
		if (rv) {
			printf("parse_img failed! (%d)\n", rv);
			return 3;
		}

		dump_struct(&bmp.header, sizeof(struct bmp_header));
		dump_struct(&bmp.info, sizeof(struct bmp_infoheader));

		printf("width: %x / height: %x\n", bmp.info.bmp_width,
		       bmp.info.bmp_height);

		printf("colordepth: %x\n", bmp.info.colordepth);

		printf("compression: %x\n", bmp.info.compression_method);

		printf("color masks / red: %x / green: %x / blue: %x\n",
		       bmp.info.red_mask, bmp.info.green_mask,
		       bmp.info.blue_mask);

		printf("\nFirst 32b chunk:\n");
		printf("none: %x / red: %x / green: %x / blue: %x\n",
		       bmp.image[0].none, bmp.image[0].red, bmp.image[0].green,
		       bmp.image[0].blue);

		printf("Color table sample:\n");
		for (i = 0; i < 5; i++) {
			printf("0x%x ", bmp.color_table[i]);
		}
		printf("\n");

		buffer = malloc(bmp.image_size * 4);
		if (buffer == NULL) {
			perror("malloc failed");
			return 4;
		}

		{
			size_t j;
			size_t offset;
			size_t out_size = args.res_width * args.res_height / 8;

			uint32_t mapped_thresh_black =
				((double)args.thresh_black / 100.0f) *
				(double)bmp.color_table_size;

			uint32_t mapped_thresh_red =
				((double)args.thresh_red / 100.0f) *
				(double)bmp.color_table_size;

			memset(buffer, 0, out_size * 2);

			printf("Mapped b/w thresh:%d\n", mapped_thresh_black);
			printf("Mapped red thresh:%d\n", mapped_thresh_red);
			printf("Transforming image data...\n");

			// write b/w data from which average of rgb data is over threshold
			offset = 0;
			for (i = 0; i < out_size; i++) {
				for (j = 0; j < 8; j++) {
					// transforming b/w data
					uint8_t op = 0x0;
					uint32_t current_thresh_black =
						((bmp.image[offset].red +
						  bmp.image[offset].green +
						  bmp.image[offset].blue) /
						 3);

					if (current_thresh_black <=
					    mapped_thresh_black) {
						// write white
						op = 0x1;
					}
					buffer[i] |= op << (7 - j);

					// transforming red data
					op = 0x0;
					if (bmp.image[offset].red <=
					    mapped_thresh_red) {
						op = 0x1;
					}
					buffer[i + out_size] |= op << (7 - j);

					offset++;
				}
			}

			printf("Writing image data to file...\n");
			print_file(out, buffer, out_size, args.res_width,
				   args.res_height);
			close_img(&bmp);
		}
		free(buffer);
	}

	fclose(in);
	fclose(out);
	return rv;
}

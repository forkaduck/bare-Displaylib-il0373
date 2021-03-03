
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
			break;

		case 'o':
			memcpy(args->output, argv[i + 1],
			       strlen_s(argv[i + 1], PATH_MAX));
			break;
		}
	}

	if (*args->input == '\0' || *args->output == '\0') {
		printf("Not enougth arguments given!\n");
		return 1;
	}
	return 0;
}

struct bmpctx {
	FILE *in, *out;
};

int open_files(struct bmpctx *ctx, struct args args)
{
	// open input and output file
	ctx->in = fopen(args.input, "rb");
	if (ctx->in == NULL) {
		perror("fopen failed");
		return 1;
	}

	ctx->out = fopen(args.output, "wb");
	if (ctx->out == NULL) {
		perror("fopen failed");
		return 2;
	}

	return 0;
}

void close_files(struct bmpctx *ctx)
{
	fclose(ctx->in);
	fclose(ctx->out);
}

int main(int argc, char *argv[])
{
	struct args args;
	struct bmpctx ctx;

	memset(&args, 0, sizeof(struct args));

	if (handle_args(&args, argc, argv)) {
		printf("handle_args failed!\n");
		return 1;
	}

	printf("Input:%s\nOutput:%s\n", args.input, args.output);

	if (open_files(&ctx, args)) {
		printf("open_files failed!\n");
		return 2;
	}

	{
		size_t i;
		struct bitmap bmp;

		parse_img(&bmp, ctx.in);
		dump_struct(&bmp.header, sizeof(struct bmp_header));
		dump_struct(&bmp.info, sizeof(struct bmp_infoheader));

		printf("%d x %d\n", bmp.info.bmp_width, bmp.info.bmp_heigth);
		for (i = 0; i < bmp.image_size; i++) {
			printf("%x \n", bmp.image[i]);
		}
	}

	close_files(&ctx);
	return 0;
}
/*
	{
		{
			uint32_t size_in_chunks =
				(data.pixelwidth / 32) * data.pixelheigth;

			// size of imagearray in bytes
			data.imagesize = size_in_chunks * 4;

			printf("Arraysize: %zuB / %d 32b chunks\n",
			       data.imagesize, size_in_chunks);

			// allocate image data array
			data.image = malloc(data.imagesize);
			if (!data.image) {
				perror("malloc failed");
				return 14;
			}

			// read the pixel array
			if (fread(data.image, sizeof(uint32_t), size_in_chunks,
				  in) != size_in_chunks) {
				perror("fread failed");
				return 15;
			}

			{
				size_t i;

				char head[] = { "#ifndef IMG_H\n"
						"#define IMG_H\n"
						"#include <stdint.h>\n"
						"#define img_size" };

				char mid[] = {
					"/ 8\n"
					"uint8_t img_map[img_size] = {\n"
				};

				char tail[] = { "\n};\n"
						"#endif\n" };

				// print head and mid with size
				fprintf(out, "%s %d * %d %s", head,
					data.pixelwidth, data.pixelheigth, mid);

				for (i = 0; i < data.imagesize; i++) {
					fprintf(out, "0x%x, ",
						((uint8_t *)data.image)[i]);

					if ((i +
					     1) % ((data.pixelwidth / 32 * 8) -
						   1) ==
					    0) {
						fprintf(out, "\n");
					}
				}

				// print tail
				fprintf(out, "%s", tail);
			}

			free(data.image);
		}

		fclose(in);
		fclose(out);
	}

	return 0;
}*/

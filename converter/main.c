
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
	uint32_t red_thresh;
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

		case 'r':
			args->red_thresh = strtoll(argv[i + 1], NULL, 0);

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
	struct bmpctx ctx;

	memset(&args, 0, sizeof(struct args));

	rv = handle_args(&args, argc, argv);
	if (rv) {
		printf("handle_args failed! (%d)\n", rv);
		return 1;
	}

	printf("Input:%s\nOutput:%s\n", args.input, args.output);

	rv = open_files(&ctx, args);
	if (rv) {
		printf("open_files failed! (%d)\n", rv);
		return 2;
	}

	{
		size_t i;
		struct bitmap bmp;

		rv = parse_img(&bmp, ctx.in);
		if (rv) {
			printf("parse_img failed! (%d)\n", rv);
			return 3;
		}

		dump_struct(&bmp.header, sizeof(struct bmp_header));
		dump_struct(&bmp.info, sizeof(struct bmp_infoheader));

		printf("width: %x / height: %x\n", bmp.info.bmp_width,
		       bmp.info.bmp_heigth);

		printf("compression: %x\n", bmp.info.compression_method);

		printf("color masks / alpha: %x / red: %x / green: %x / blue: %x\n",
		       bmp.info.alpha_mask, bmp.info.red_mask,
		       bmp.info.green_mask, bmp.info.blue_mask);

		for (i = 0; i < 10; i++) {
			printf("none: %x / alpha: %x / red: %x / green: %x / blue: %x\n",
			       bmp.image[i].none, bmp.image[i].alpha,
			       bmp.image[i].red, bmp.image[i].green,
			       bmp.image[i].blue);
		}
		close_img(&bmp);
	}

	close_files(&ctx);
	return 0;
}

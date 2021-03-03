#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdint.h>

// The underscore (_) marks useless fields in headers

// The Bitmap file header
struct bmp_header {
    uint16_t header_field;
    uint32_t file_size;
    uint16_t _reserved;
    uint16_t _reserved2;
    uint32_t pixel_array_offset;
}__attribute__((packed));

// The added DIB header (BITMAPINFOHEADER)
struct bmp_infoheader {
    uint32_t header_size;
    uint32_t bmp_width;
    uint32_t bmp_heigth;
    uint16_t num_planes;
    uint16_t colordepth;
    uint32_t compression_method;
    uint32_t image_size;
    int32_t horizontal_res;
    int32_t vertical_res;
    uint32_t num_colors_in_palette;
    uint32_t _important_colors;
}__attribute__((packed));

// A representation of a bitmap
// At the moment only mandatory fields are programmed
struct bitmap {
    struct bmp_header header;
    struct bmp_infoheader info;

    size_t image_size; // image size in bytes
    uint32_t *image;
};


int parse_img(struct bitmap *bmp, FILE *in);
void dump_struct(void *strct, size_t size);
#endif

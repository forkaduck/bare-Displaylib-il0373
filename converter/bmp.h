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

// some colorspace stuff (idk)
struct bmp_color_in_colorspace {
    uint32_t x;
    uint32_t y;
    uint32_t z;
}__attribute__((packed));

struct bmp_three_color {
    struct bmp_color_in_colorspace red;
    struct bmp_color_in_colorspace green;
    struct bmp_color_in_colorspace blue;
}__attribute__((packed));


// The added DIB header 
struct bmp_infoheader {
    // BITMAPINFOHEADER
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

    // BITMAPV5HEADER
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    uint32_t alpha_mask;
    uint32_t color_space;
    struct bmp_three_color endpoints;
    uint32_t gamma_red;
    uint32_t gamma_green;
    uint32_t gamma_blue;
    uint32_t intent;
    uint32_t profile_data;
    uint32_t profile_size;
    uint32_t _reserved;
}__attribute__((packed));

// A representation of a bitmap
// At the moment only mandatory fields are programmed
struct bitmap {
    struct bmp_header header;
    struct bmp_infoheader info;

    size_t image_size; // image size in 32b chunks
    uint32_t *image;
};


void dump_struct(void *strct, size_t size);
int parse_img(struct bitmap *bmp, FILE *in);
void close_img(struct bitmap *bmp);

#endif

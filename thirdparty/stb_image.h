#ifndef STB_IMAGE_H
#define STB_IMAGE_H

enum
{
    STBI_default = 0, // only used for req_comp

    STBI_grey       = 1,
    STBI_grey_alpha = 2,
    STBI_rgb        = 3,
    STBI_rgb_alpha  = 4
};

extern "C"
{
    unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
    void stbi_image_free(void *data);
}

#endif

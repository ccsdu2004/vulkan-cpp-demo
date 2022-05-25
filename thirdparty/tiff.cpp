#include "tiff.h"
#include "tiff/tiffio.h"
#include <stdlib.h>

int writeTiff(char const *const filename, char const *const data,
              const VkExtent2D size, const size_t nchannels)
{
    TIFF *tif = TIFFOpen(filename, "w");
    if (tif == NULL)
        return 1;

    const size_t sample_bits = 8;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, size.width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, size.height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, nchannels);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, sample_bits);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    if (nchannels == 1)
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    else
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    for (size_t i = 0; i < size.height; i++) {
        if (TIFFWriteScanline(tif, (void *) &data[i * size.width * nchannels], i, 0) < 0) {
            TIFFClose(tif);
            return 1;
        }
    }

    TIFFClose(tif);
    return 0;
}

int readTiffRGBA(char const *const filename, VkExtent2D *const size,
                 VkSubresourceLayout *layout, uint8_t *const data)
{
    const uint32_t nchannels = 4;

    TIFF *tif = TIFFOpen(filename, "r");
    if (tif == NULL)
        return 1;

    if (data == NULL) {
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &size->height);
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &size->width);
    } else {
        if (layout == NULL || size == NULL)
            goto cleanup;

        char *const temp = (char *)malloc(nchannels * size->width * size->height);
        if (!TIFFReadRGBAImage(tif, size->width, size->height, (uint32_t *) temp, 0))
            goto cleanup;

        for (size_t x = 0; x < size->width; x++)
            for (size_t y = 0; y < size->height; y++)
                for (size_t ch = 0; ch < nchannels; ch++) {
                    data[ch + x * nchannels + y * layout->rowPitch] =
                        temp[ch + x * nchannels + y * size->width * nchannels];
                }
        free(temp);
    }

    TIFFClose(tif);
    return 0;
cleanup:
    TIFFClose(tif);
    return 1;
}

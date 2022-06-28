#ifndef _BMP_H_
#define _BMP_H_

#include "stdafx.h"

enum BmpCompresion
{
    BI_RGB = 0,
    BI_RLE8 = 1,
    BI_RLE4 = 2,
    BI_BITFIELDS = 3
};

enum ColorModes
{
    CM_16BIT = 16,
    CM_8BIT = 8,
    CM_4BIT = 4
};

#pragma pack(push)
#pragma pack(1)
struct RGB4 {
    uint8_t pixel0 :4;
    uint8_t pixel1 :4;
};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
struct RGB1 {
    uint8_t pixel0 :1;
    uint8_t pixel1 :1;
    uint8_t pixel2 :1;
    uint8_t pixel3 :1;
    uint8_t pixel4 :1;
    uint8_t pixel5 :1;
    uint8_t pixel6 :1;
    uint8_t pixel7 :1;
};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
struct RGB32 {
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbAlpha;
};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
struct RGB24 {
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct tagRGBQUAD {
  uint8_t rgbBlue;
  uint8_t rgbGreen;
  uint8_t rgbRed;
  uint8_t rgbReserved;
} RGBQUAD;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
    typedef struct {
        uint16_t sys;                   //2 Contains the characters "BM" that identify the file type
        uint32_t file_size;             //4 File size
        uint16_t reserved1;             //2 Unused
        uint16_t reserved2;             //2 Unused
        uint32_t starting;              //4 Offset to start of pixel data
        uint32_t header_size;           //4 Header size-Must be at least 40
        int32_t witdth;                 //4 signed, Image width
        int32_t height;                 //4 signed, Image height
        uint16_t color_planes;          //2 Must be 1
        uint16_t bits_per_pixel;        //2 Bits per pixel- 1, 4, 8, 16, 24, or 32
        uint32_t compression_method;    //4 Compression type- BI_RGB=0, BI_RLE8=1, BI_RLE4=2, or BI_BITFIELDS=3
        uint32_t image_raw_size;        //4 Image Size-May be zero if not compressed
        int32_t horizontal_resolution;  //4 signed, Preferred resolution in pixels per meter
        int32_t vertical_resolution;    //4 signed, Preferred resolution in pixels per meter
        uint32_t colours_per_pallete;   //4 Number of entries in the colour map that are actually used
        uint32_t num_important_colours; //4 Number of significant colors
    } BmpHeader;
#pragma pack(pop)

class Bmp {

    private:
        uint32_t width;
        uint32_t height;

        ColorModes bits_per_pixel;
        uint32_t colours_per_pallete;
        uint32_t image_raw_size;

        bool has_palette;
        bool has_alpha;

        RGBQUAD *palette;

        uint8_t *image;

        uint8_t *alpha;

    protected:
        static void printHeader(BmpHeader header);

        void convertFrom32Bits2Native16(RGB32 * buffer);
        void convertFrom24Bits2Native16(RGB24 * buffer);
        void convertFrom16Bits2Native16(uint16_t * buffer, bool format);
        void convertFrom8Bits2Native8(uint8_t * buffer);
        void convertFrom4Bits2Native4(RGB4 * buffer);

        void convertFrom8To16Bits();
        void convertFrom4To16Bits();

        void turnImage();

    public:
        //BmpHeader header;

        static Bmp fromBmpFile(const char *fileName);

        int convertTo16Bits();

        int saveImage(const char *fileDest);

        int saveDump(const char *fileDest);

        int savePalette(const char *fileDest);

        int saveTiles(const char *fileDest);

        int saveSprites(const char *fileDest);

        RGBQUAD * getPalette();

        uint8_t * getImage();

        int32_t getNumPixels();

        uint16_t getNumOfColours();

        uint8_t getBitsPerPixel();

        uint8_t getPaletteSize();

        int32_t getWidth();

        int32_t getHeight();

        int hasPalette();
};

#endif

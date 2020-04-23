/**
Created by jmorales on 22/17/20.

 @author jmorales (jmorales@unc.edu.ar)
 @details https://itnext.io/bits-to-bitmaps-a-simple-walkthrough-of-bmp-image-format-765dc6857393
 @TODO Bloque 3 de paleta de colores
 @TODO detectar headers no validos, solo bmp 24bits?
 @TODO Algo para BW?
*/

#ifndef _SIMPLE_BMP_H_
#define _SIMPLE_BMP_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BITMAPINFOHEADER 40
#define TIFF_MAGIC_NUMBER ((0x4D << 8) | 0x42)

enum sbmp_codes {
  SBMP_ERROR_PARAM = -2,
  SBMP_ERROR_FILE,
  SBMP_OK = 0
};
// STRUCTURE OF IMAGE
/**
 * \brief Block 1: File Type Data, information about BMP File
 * This block is a BMP Header labeled as BITMAPFILEHEADER (the name comes from c++ struct in Windows OS)
 * \details https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-BITMAPINFOHEADER
 */
typedef struct _sbmp_ftype_data {
  /// A 2 character string value in ASCII to specify a DIB file type.
  /// It must be 'BM' or '0x42 0x4D' in hexadecimals for modern compatibility reasons.
  uint16_t file_type; // (Endianness) 'BM' - '0x42 0x4D' (Endianness)
  /// Entire file size in bytes. This value is basically the number of bytes in a BMP image file.
  uint32_t file_size;
  /// It should be initialized to '0' integer (unsigned) value.
  uint32_t reserved;
  ///  the offset of actual pixel data in bytes.
  /// In nutshell:- it is the number of bytes between start of the file (0) and the first byte of the pixel data.
  uint32_t data_offset;
} __attribute__ ((__packed__)) sbmp_ftype_data;

typedef struct _sbmp_iinfo_data {
  /**  The size of the header in bytes.
   * It should be '40' in decimal to represent BITMAPINFOHEADER header type. */
  uint32_t header_size;
  /// The width of the final image in pixels
  int32_t image_width;
  /// Height of the final image in pixels.
  int32_t image_height;
  /// The number of color planes of the target device. Should be '1' in decimal.
  uint16_t planes;
  /// The number of bits (memory) a pixel takes (in pixel data) to represent a color.
  uint16_t bit_per_pixel;
  /// The value of compression to use. Should be '0' in decimal to represent no-compression (identified by 'BI_RGB').
  uint32_t compression;
  /// The final size of the compressed image. Should be '0' in decimal when no compression algorithm is used.
  uint32_t image_size;
  /// The horizontal resolution of the target device. This parameter will be adjusted by the image processing
  /// application but should be set to '0' to indicate no preference.
  int32_t xpix_per_meter;
  /// the verical resolution of the target device
  int32_t ypix_per_meter;
  /// The number of colors in the color pallet (size of the color pallet or color table).
  /// If this is set to '0' in decimal :- 2^BitsPerPixel colors are used.
  uint32_t total_colors;
  /// The number of important colors. Generally ignored by setting '0' decimal value.
  uint32_t important_colors;

} __attribute__ ((__packed__)) sbmp_iinfo_data;

/// Pixel 24-bits description
typedef struct _sbmp_raw_data {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
} __attribute__ ((__packed__)) sbmp_raw_data;

typedef struct _sbmp_image {
  sbmp_ftype_data type;
  sbmp_iinfo_data info;
  sbmp_raw_data **data;
} sbmp_image;

// Funciones


enum sbmp_codes sbmp_initialize_bmp (sbmp_image *image, uint32_t height, uint32_t width);
enum sbmp_codes sbmp_save_bmp (const char *filename, const sbmp_image *image);
enum sbmp_codes sbmp_load_bmp (const char *filename, sbmp_image *image);

#endif //_SIMPLE_BMP_H_

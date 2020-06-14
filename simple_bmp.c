/**
 Created by jmorales on 4/22/20.
*/

#include "simple_bmp.h"
/**
 * Initialize a new image
 *
 * Initialize a new BMP image: 24 bits, without compression, without color pallet (2^BitsPerPixel colors are use)
 * @param [out] image Clear (not null) sbmp_image to initialize
 * @param [in] height in pixels (>0)
 * @param [in] width in pixels (>0)
 * @return The result of the Initialization, begin SBMP_OK if all worked OK \n
 *          or  SBMP_ERROR_PARAM if the parameters are not valid
 */
enum sbmp_codes sbmp_initialize_bmp (sbmp_image *image, uint32_t height, uint32_t width)
{

  if ( image == NULL ||  height == 0 || width == 0
      || height > INT32_MAX || width > INT32_MAX )
    {
      return SBMP_ERROR_PARAM;
    }

  // Headerd
  image->type.file_type = TIFF_MAGIC_NUMBER;
  image->type.data_offset = sizeof (sbmp_ftype_data) + BITMAPINFOHEADER; // arreglar sizeof + size of
  image->type.file_size = (uint32_t) image->type.data_offset;
  image->type.file_size += (((uint32_t) sizeof (sbmp_raw_data)) * width + width % 4) * height;
  image->type.reserved = 0;

  image->info.header_size = BITMAPINFOHEADER;
  image->info.image_width = (int32_t) width;
  image->info.image_height = (int32_t) height;
  image->info.planes = 1;
  image->info.bit_per_pixel = 24;
  image->info.compression = 0;
  image->info.image_size = 0;
  image->info.xpix_per_meter = 0;
  image->info.ypix_per_meter = 0;
  image->info.total_colors = 0;
  image->info.important_colors = 0;

  image->data = calloc (height, sizeof (sbmp_raw_data *));
  if (image->data == NULL)
    {
      fprintf (stderr, "Error alocando memoria");
      exit (EXIT_FAILURE);
    }

  for (uint32_t i = 0; i < height; i++)
    {
      image->data[i] = calloc (width, sizeof (sbmp_raw_data));
      if (image->data[i] == NULL) /* Meeh ?*/
        {
          fprintf (stderr, "Error alocando memoria");
          exit (EXIT_FAILURE);
        }
    }

  return SBMP_OK;
}

/**
 * Save image in the filesystem
 * @param [in] filename File path
 * @param [in] image Image to save
 * @return begin SBMP_OK if all worked OK or  SBMP_ERROR_FILE if you can't create the file
 */
enum sbmp_codes sbmp_save_bmp (const char *filename, const sbmp_image *image)
{

  FILE *fd = fopen (filename, "w");
  if (fd == NULL)
    {
      fprintf (stderr, "Error: %s\n", strerror (errno));
      return SBMP_ERROR_FILE;
    }

  // Write the headers
  fwrite (&image->type, sizeof (image->type), 1, fd);
  fwrite (&image->info, sizeof (image->info), 1, fd);

  // Padding is necessary
  size_t padd_size = ((size_t) image->info.image_width * sizeof (sbmp_raw_data)) % PADDINGSIZE;
  uint8_t *zero_pad = NULL;

  if (0 != padd_size)
    { // Yes
      padd_size = PADDINGSIZE - padd_size;
      zero_pad = (uint8_t *) calloc (PADDINGSIZE, sizeof (uint8_t));
    }

  for (int32_t i = image->info.image_height - 1; i >= 0; i--)
    {
      fwrite (image->data[i],
              sizeof (sbmp_raw_data),
              (uint32_t) image->info.image_width,
              fd);
      if (NULL != zero_pad)
        fwrite (zero_pad, sizeof (uint8_t), padd_size, fd);
    }

  if (NULL != zero_pad)
    free (zero_pad);

  return SBMP_OK;
}

/**
 * Load image from the disk in sbmp_image
 * @param [in] filename File path to be load
 * @param [out] image  Not null pointer where image to be load
 * @return begin SBMP_OK if all worked OK or  SBMP_ERROR_FILE if you can't read the file
 */
enum sbmp_codes sbmp_load_bmp (const char *filename, sbmp_image *image)
{

  FILE *fd = fopen (filename, "r");
  if (fd == NULL)
    {
      fprintf (stderr, "Error: %s\n", strerror (errno));
      return SBMP_ERROR_FILE;
    }

  fread (&image->type, sizeof (image->type), 1, fd);
  fread (&image->info, sizeof (image->info), 1, fd);
  image->data = calloc ((size_t) image->info.image_height, sizeof (sbmp_raw_data *));
  if (image->data == NULL)
    {
      fprintf (stderr, "Error: %s\n", strerror (errno));
      fclose (fd);
      return SBMP_ERROR_FILE;
    }

  for (int32_t i = image->info.image_height - 1; i >= 0; i--)
    {
      image->data[i] = calloc ((size_t) image->info.image_width, sizeof (sbmp_raw_data));
      fread (image->data[i],
             sizeof (sbmp_raw_data),
             (uint32_t) image->info.image_width, fd);
    }
  fclose (fd);
  return SBMP_OK;
}
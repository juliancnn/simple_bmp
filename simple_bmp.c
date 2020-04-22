/**
 Created by jmorales on 22/17/20.
*/

#include "simple_bmp.h"

enum sbmp_codes sbmp_create_bmp (sbmp_image *image, uint32_t height, uint32_t width)
{

  // Falta chequeo > 32bits, of
  if (image == NULL || height == 0 || width == 0)
    {
      return SBMP_ERROR_PARAM;
    }

  // Headerd
  image->type.file_type = TIFF_MAGIC_NUMBER;
  image->type.file_size = (((uint32_t) sizeof (sbmp_raw_data)) * width + width % 4 + height);
  image->type.reserved = 0;
  image->type.data_offset = 14 + BITMAPINFOHEADER;

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

  for (int32_t i =  image->info.image_height - 1; i >= 0; i--)
    {
      fwrite (image->data[i],
          sizeof (sbmp_raw_data),
          (uint32_t) (image->info.image_width + image->info.image_width % 4),
           fd);
    }

  return SBMP_OK;
}





#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <libnet.h>

#include "simple_bmp.h"

uint64_t rdtsc ();
void focus_center (sbmp_raw_data **, int32_t, int32_t);
void conv2d_monocore (sbmp_raw_data **base_img, sbmp_raw_data **new_img,
                      int32_t height, int32_t width);
void kernel_setup(uint16_t **kern, int16_t ksize);
int main ()
{
  uint16_t **kernel = calloc(11, sizeof(int *));
  for(int k = 0; k< 11; k++)
    kernel[k] = calloc(11,sizeof(uint16_t));
  kernel_setup (kernel, 11);
  exit(0);
  kernel_setup (kernel, 11);
  sbmp_image test_img, dup;
  sbmp_load_bmp ("base.bmp", &test_img);
  //sbmp_initialize_bmp (&dup, (uint32_t) test_img.info.image_height, (uint32_t) test_img.info.image_width);
  sbmp_load_bmp ("base.bmp", &dup);
  printf ("altura : %d\n", test_img.info.image_height);
  printf ("ancho : %d\n", test_img.info.image_width);
  conv2d_monocore (test_img.data, dup.data, test_img.info.image_height, test_img.info.image_width);
  sbmp_save_bmp ("frac.bmp", &dup);
}

/*
 * \@details http://www.albertostrumia.it/sites/default/files/Fractals/FractalGallery2/EW2.html
 */

void focus_center (sbmp_raw_data **img_data, int32_t height, int32_t width)
{
  const int radio = width / 4;
  const int centerx = abs (width / 2);
  const int centery = abs (height / 2);

  for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
        {
          if ((centerx - x) * (centerx - x) + (centery - y) * (centery - y) <= radio * radio)
            {
              //img->img_pixels[y][x] = BMP_PIXEL (255,0,255);

              img_data[y][x].red = (uint8_t)
                  (img_data[y][x].red * 1.5 > 255 ? 255 : (uint8_t) (img_data[y][x].red * 1.5));

              img_data[y][x].green = (uint8_t)
                  (img_data[y][x].green * 1.5 > 255 ? 255 : (uint8_t) (img_data[y][x].green * 1.5));
              img_data[y][x].blue = (uint8_t)
                  (img_data[y][x].blue * 1.5 > 255 ? 255 : (uint8_t) (img_data[y][x].blue * 1.5));

            }
        }
    }

}

uint64_t rdtsc ()
{
  unsigned int lo, hi;
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return ((uint64_t) hi << 32) | lo;
}


/**
 * Convolucion monocore
 *
 * EL tratamiento de bordes es Not Process
 *
 * @param [in] kernel Puntero al kernel
 * @param [in] k_size Tamano de la matrix cuadrada del kernel
 * @param [in] img  Puntero a la imagen a procesar
 * @param [in] i_size_x  tamano en pixeles del ancho de la imagen
 * @param [in] i_size_y  tamano en pixeles del alto de la imagen
 * @return Resultado de la convolucion
 *
 * @note alloca memoria para el resultado igual que la imagen de entrada
 */
void conv2d_monocore (sbmp_raw_data **base_img, sbmp_raw_data **new_img,
                      int32_t height, int32_t width)
{
  /* uint8_t kernel[5][5] = {
                          {2, 1, 1, 0, 0},
                          {1, 1, 2, 1, 0},
                          {0, 0, 1, 1, 1},
                          {0, 0, 0, 1, 2},
                          {0, 0, 0, 0, 1}
                         }; //5x5
                         */
  uint8_t kernel[11][11] = {
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
      {1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1},
      {1, 2, 3, 4, 4, 4, 4, 4, 3, 2, 1},
      {1, 2, 3, 4, 5, 5, 5, 4, 3, 2, 1},
      {1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1},
      {1, 2, 3, 4, 5, 5, 5, 4, 3, 2, 1},
      {1, 2, 3, 4, 4, 4, 4, 4, 3, 2, 1},
      {1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1},
      {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  }; //5x5
  int8_t k_size = 11;
  int8_t k_mid = 5;

  /* Timmer */
  uint64_t t1, t2, t3;
  struct timeval tiempo_1, tiempo_2, tiempo_3;
  int32_t time_ms;

  uint32_t pixelOverRed;
  uint32_t pixelOverBlue;
  uint32_t pixelOverGreen;

#ifdef DEBUG_MODE
  printf ("Porcentaje : 00");
#endif
  t1 = rdtsc ();
  t2 = rdtsc ();
  gettimeofday (&tiempo_1, NULL);
  gettimeofday (&tiempo_2, NULL);

      /* Avanzo sobre las filas de la imagen, tipo not process  */
      for (int pos_ix = (k_size >> 1); pos_ix < width - (k_size >> 1); pos_ix++)
        {
          /* Avanzo sobre las columnas de la imagen, tipo not process  */
          for (int pos_iy = (k_size >> 1); pos_iy < height - (k_size >> 1); pos_iy++)
            {
              pixelOverRed = 0;
              pixelOverGreen = 0;
              pixelOverBlue = 0;
              for (int pos_kx = 0; pos_kx < k_size; pos_kx++)
                {
                  for (int pos_ky = 0; pos_ky < k_size; pos_ky++)
                    {
                      pixelOverRed += (uint16_t) ((base_img[pos_ix - k_mid + pos_kx][pos_iy - k_mid + pos_ky].red
                                                   * kernel[pos_kx][pos_ky]));
                      pixelOverBlue += (uint16_t) ((base_img[pos_ix - k_mid + pos_kx][pos_iy - k_mid + pos_ky].blue
                                                    * kernel[pos_kx][pos_ky]));
                      pixelOverGreen += (uint16_t) ((base_img[pos_ix - k_mid + pos_kx][pos_iy - k_mid + pos_ky].green
                                                     * kernel[pos_kx][pos_ky]));
                    }

                }
              pixelOverRed /= 286;
              pixelOverGreen /= 286;
              pixelOverBlue /= 286;
              new_img[pos_ix][pos_iy].red = (uint8_t) ((pixelOverRed) > 255) ? 255 : ((uint8_t) pixelOverRed);
              new_img[pos_ix][pos_iy].blue = (uint8_t) ((pixelOverBlue) > 255) ? 255 : ((uint8_t) pixelOverBlue);
              new_img[pos_ix][pos_iy].green = (uint8_t) ((pixelOverGreen) > 255) ? 255 : ((uint8_t) pixelOverGreen);
            }

        }


  gettimeofday (&tiempo_3, NULL);
  t3 = rdtsc ();
  time_ms = (int32_t) ((float) (tiempo_3.tv_usec - tiempo_2.tv_usec - (tiempo_2.tv_usec - tiempo_1.tv_usec))
                       + ((float) (tiempo_3.tv_sec - tiempo_2.tv_sec - (tiempo_2.tv_sec - tiempo_1.tv_sec))
                          * 1000000.0f)) / 1000;
#ifdef DEBUG_MODE
  printf ("\b\b%%100\n");
  fflush (stdout);
#endif
  printf ("[Monocore  v1] %15lu ciclos     %10i ms\n", (t3 - t2) - (t2 - t1), time_ms);

}
void kernel_setup(uint16_t **kern, int16_t ksize)
{
  uint16_t st_val = 1;
  for (int i = 0; i < ksize; i++)
    {
      for (int j = 0; j < ksize; j++)
        {
          kern[i][j] = st_val;
        }
    }
  for (int i = 1; i < ksize / 2 +1; i++)
    {
      for (int j = 0; j < ksize ; j++)
        {
          if (j >= i && j < (ksize - i))
            kern[i][j] = (uint16_t) (kern[i-1][j] + (uint16_t) 1);
          else
            kern[i][j] = kern[i-1][j];
        }

    }
  for (int i = 1 ; i < ksize/2; i++)
    {
      for (int j = 0; j < ksize ; j++)
        {
            kern[i + ksize/2][j] = kern[ksize/2 - i][j];
        }

    }
  for (int i = 0; i < ksize; i++)
    {
      for (int j = 0; j < ksize; j++)
        {
          printf ("%3hu ", kern[i][j]);
        }
      printf ("\n");
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <libnet.h>

#include "simple_bmp.h"

uint64_t rdtsc ();
void focus_center (sbmp_raw_data **, int32_t, int32_t);
float *conv2d_monocore (float *img, int i_size_x, int i_size_y);

int main ()
{
  sbmp_image test_img, dup;
  sbmp_load_bmp ("guarani.bmp", &test_img);
  sbmp_initialize_bmp (&dup, (uint32_t) test_img.info.image_height, (uint32_t) test_img.info.image_width);

  printf ("altura : %d\n", test_img.info.image_height);
  printf ("ancho : %d\n", test_img.info.image_width);
  focus_center (test_img.data, test_img.info.image_height, test_img.info.image_width);
  sbmp_save_bmp ("frac.bmp", &test_img);
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
float *conv2d_monocore (float *img)
{
  uint16_t kernel[] = { 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 2, 4, 2, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1 }; //5x5
  uint16_t k_size = 5;
  float baias = 36;

  /* Timmer */
  uint64_t t1, t2, t3;
  struct timeval tiempo_1, tiempo_2, tiempo_3;
  int32_t time_ms;


  float *ptr_img_x = img + (k_size >> 1) * i_size_x;
  float *ptr_img_o = data_out + (k_size >> 1) * i_size_x;
  float *ptr_ker_x;
  float *pixelOver;

#ifdef DEBUG_MODE
  printf ("Porcentaje : 00");
#endif
  t1 = rdtsc ();
  t2 = rdtsc ();
  gettimeofday (&tiempo_1, NULL);
  gettimeofday (&tiempo_2, NULL);
  /* Avanzo sobre las filas de la imagen, tipo not process  */
  for (int pos_ix = (k_size >> 1); pos_ix < i_size_x - (k_size >> 1); pos_ix++)
    {
      /* Avanzo sobre las columnas de la imagen, tipo not process  */
      for (int pos_iy = (k_size >> 1); pos_iy < i_size_y - (k_size >> 1); pos_iy++)
        {
          /* Calculo sobre el kernel */
          ptr_ker_x = kernel;
          for (int pos_kx = 0; pos_kx < k_size; pos_kx++)
            {
              for (int pos_ky = 0; pos_ky < k_size; pos_ky++)
                {
                  pixelOver = (ptr_img_x + (-(k_size >> 1) + pos_kx) * i_size_x) + (pos_iy - (k_size >> 1) + pos_ky);

                  /* *(ptr_img_o + pos_iy) += *pixelOver < 250 ? 0 :
                                           (*pixelOver > 1000 ? 1000 : (*pixelOver
                                                                        * *(kernel + pos_kx * k_size + pos_ky))); */
                  *(ptr_img_o + pos_iy) += *pixelOver * *(kernel + pos_kx * k_size + pos_ky);

                }

              ptr_ker_x += k_size;

            }

        }
      ptr_img_x += i_size_x;
      ptr_img_o += i_size_x;

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

  return data_out;
}

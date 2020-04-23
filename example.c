#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "simple_bmp.h"

void fractal_generate (sbmp_raw_data **, int32_t, int32_t);
void focus_center (sbmp_raw_data **, int32_t, int32_t);
int main ()
{
  sbmp_image test_img;
  sbmp_load_bmp ("base.bmp", &test_img);
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
  const int radio = width / 4 ;
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

void fractal_generate (sbmp_raw_data **img_data, int32_t height, int32_t width)
{
  const double Radius = 0.8;
  const double Cx = 0.0;
  const double Cy = 0.0;
  const double Side = 0.15;
  const uint16_t Num = 64;

  int p, q, n, w;
  double x, y, xx, yy, Incx, Incy;

  for (p = 0; p < height; p++)
    {
      Incy = -Side + 2 * Side / height * p;

      //printf("%i %%\n", p*100/M);

      for (q = 0; q < width; q++)
        {
          //Incx = -Side + 2 * Side / (width *2) * q;
          Incx = -Side + 2 * Side / width * q;

          x = Incx;
          y = Incy;
          w = 0;

          for (n = 1; n <= Num; ++n)
            {
              xx = 5 * x / 6.0
                   - x * (x * x * x * x - 10 * x * x * y * y + 5 * y * y * y * y) / (x * x + y * y) / (x * x + y * y)
                     / (x * x + y * y) / (x * x + y * y) / (x * x + y * y) / 6.0;

              yy = 5 * y / 6.0
                   + y * (5 * x * x * x * x - 10 * x * x * y * y + y * y * y * y) / (x * x + y * y) / (x * x + y * y)
                     / (x * x + y * y) / (x * x + y * y) / (x * x + y * y) / 6.0;

              x = xx;
              y = yy;
//              printf("%f - %f\n",x,y);

              if ((x - Cx) * (x - Cx) + (y - Cy) * (y - Cy) < Radius)
                {
                  w = n;

                  n = Num;
                }

            }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
          img_data[p][q] = (sbmp_raw_data) {
              //(uint8_t)((float) img->data[p][q].blue + (float) w/128),
              ((w > 30) ? (uint8_t) (abs ((int32_t) ((float) img_data[p][q].blue * 1.2 - 46))) : (uint8_t) abs (
                  (int32_t) img_data[p][q].blue + 10)),
              ((w > 30) ? (uint8_t) (abs ((int32_t) ((float) img_data[p][q].green * 1.2 - 46))) : (uint8_t) abs (
                  (int32_t) img_data[p][q].green + 10)),
              ((w > 30) ? (uint8_t) (abs ((int32_t) ((float) img_data[p][q].red * 1.2 - 46))) : (uint8_t) abs (
                  (int32_t) img_data[p][q].red + 10)),
          };
#pragma GCC diagnostic pop
        }
    }
}

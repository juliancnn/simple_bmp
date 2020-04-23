#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "simple_bmp.h"


void fractal_generate (sbmp_raw_data **, int32_t, int32_t);

int main(){
  sbmp_image test_img;
  sbmp_load_bmp ("base.bmp", &test_img);
  printf("altura : %d\n", test_img.info.image_height);
  printf("ancho : %d\n", test_img.info.image_width);
  fractal_generate (test_img.data, test_img.info.image_height, test_img.info.image_width);
  sbmp_save_bmp ("frac.bmp", &test_img);
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
                  //printf ("%i\n", n);
                  w = n;
                  n = Num;
                }

            }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
          img_data[p][q] = (sbmp_raw_data) {(uint8_t)(w*1.5), 0, (uint8_t) (w - 1)};
#pragma GCC diagnostic pop
        }
    }
}

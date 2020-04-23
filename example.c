#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "simple_bmp.h"

/* definition of constants */
#define Radius 0.8
#define Cx 0
#define Cy 0
#define Side 0.15
#define M 2047
#define Num 64

int main ()
{
  sbmp_image *img = calloc (1, sizeof (sbmp_image));
  //sbmp_initialize_bmp (img, M+1, (M+1)*2);
  sbmp_initialize_bmp (img, "base.bmp");

  int p, q, n, w;
  double x, y, xx, yy, Incx, Incy;

  for (p = 1; p <= M; p++)
    {
      Incy = -Side + 2 * Side / M * p;

      //printf("%i %%\n", p*100/M);

      for (q = 1; q <= M*2; q++)
        {
          Incx = -Side + 2 * Side / (M *2) * q;

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
          img->data[p][q] = (sbmp_raw_data) {(uint8_t)(w*1.5), 0, (uint8_t) (w - 1)};
#pragma GCC diagnostic pop
        }
    }
  sbmp_save_bmp("test.bmp",img);

  return 0;
}

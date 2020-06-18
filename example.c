#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <libnet.h>

#include "simple_bmp.h"

int main ()
{
  sbmp_image test_img, dup;
  //sbmp_load_bmp ("base.bmp", &test_img);

  sbmp_image imgtest = {0};
  int32_t check = sbmp_initialize_bmp (&imgtest, 121, 123);

  if (SBMP_OK != check)
    {
      exit (-1);
    }
  for (int i = 0; i < imgtest.info.image_height; ++i)
    {
      for (int j = 0; j < imgtest.info.image_width; ++j)
        {
          switch (i % 3)
            {
              case 0:imgtest.data[i][j] = (sbmp_raw_data) {255, 0, 0};
              break;
              case 1:imgtest.data[i][j] = (sbmp_raw_data) {0, 0, 255};
              break;
              case 2:imgtest.data[i][j] = (sbmp_raw_data) {0, 255, 0};
              break;
              default:imgtest.data[i][j] = (sbmp_raw_data) {255, 255, 255};
            }

        }
    }

  sbmp_save_bmp ("testeo.bmp", &imgtest);
  sbmp_free (&imgtest);


}

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "simple_bmp.h"

struct pixel{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

int main ()
{
  sbmp_image* img = calloc(1, sizeof(sbmp_image));
  sbmp_create_bmp(img, 200,300);


  for (int i=0;i < img->info.image_height ; i++){
      for (int j=0;j < img->info.image_width ; j++){
        img->data[i][j] = (sbmp_raw_data) {255,0,0};
      }
  }

  sbmp_save_bmp("test.bmp",img);

  return 0;
}

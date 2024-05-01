#include <stdio.h>

#include "image.h"
#include <string.h>
#define DIRECTORY "./"

int main(int argc, char* argv[]){
  int helo[2] = {atoi(argv[1]),atoi(argv[2])};
  
  struct bin_image* img = new_image(helo[0],helo[1]);
  
  char temp[256];
  strcpy(temp,DIRECTORY);
  strcat(temp,argv[3]);

  save_image(temp,img);
  free_img_buf(&img);
}

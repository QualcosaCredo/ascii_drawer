#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_WIDTH UINT16_MAX
#define MAX_HEIGHT UINT16_MAX

struct pixel{
  uint8_t value;
};

struct bin_image{
  struct pixel** pixels;
  uint16_t height;
  uint16_t width;
};

struct bin_image* new_image(uint8_t width,uint8_t height){
  if(height < 1 || height > MAX_HEIGHT || width < 1 || width > MAX_WIDTH){
    printf("image height and width must be both atleast 1px\n");
    return NULL;
  }
  
  struct bin_image* img = (struct bin_image*)malloc(sizeof(struct bin_image));

  img->height = height;
  img->width = width;

  img->pixels = (struct pixel**)malloc(sizeof(struct pixel*) * img->height);
  int i,j;

  for(i = 0; i < img->height; i++){
    img->pixels[i] = (struct pixel*)malloc(sizeof(struct pixel) * img->width);
    for(j = 0; j < img->width; j++){
      img->pixels[i][j].value = 0;
    }
  }

  return img;
}

void free_img_buf(struct bin_image** img){
  for(int i = 0; i < (*img)->width; i++){
    free((*img)->pixels[i]);
  }
  free((*img)->pixels);
  
  free(*img);
}

int save_image(const char* name,struct bin_image* img_data){
  FILE* img;

  if((img = fopen(name,"w")) == NULL){
    perror("Error saving image");
    return 0;
  }

  assert(fwrite(&img_data->height,sizeof(uint16_t),1,img));
  assert(fwrite(&img_data->width,sizeof(uint16_t),1,img));  
  assert(fwrite(img_data->pixels,sizeof(uint8_t),img_data->height * img_data->width,img));
  
  fclose(img);

  return 1;
}

struct bin_image* open_image(const char* name){ 
  FILE* img;

  if((img = fopen(name,"r")) == NULL){
    perror("Error reading image");
    return NULL;
  }
  
  struct bin_image* img_data = (struct bin_image*)malloc(sizeof(struct bin_image));

  assert(fread(&img_data->height,sizeof(uint16_t),1,img));
    assert(fread(&img_data->width,sizeof(uint16_t),1,img));

  img_data->pixels = (struct pixel**)malloc(sizeof(struct pixel*) * img_data->height);
  int i,j;

  for(i = 0; i < img_data->height; i++){
    img_data->pixels[i] = (struct pixel*)malloc(sizeof(struct pixel) * img_data->width);
    for(j = 0; j < img_data->width; j++){
      img_data->pixels[i][j].value = 0;
    }
  }

  assert(fread(img_data->pixels,sizeof(uint8_t),img_data->height * img_data->width,img));
  
  fclose(img);

  return img_data;
}

#endif



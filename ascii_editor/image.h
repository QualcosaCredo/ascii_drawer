#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_WIDTH UINT16_MAX
#define MAX_HEIGHT UINT16_MAX

struct bin_image{
  uint8_t** pixels;
  uint16_t height;
  uint16_t width;
};

struct bin_image* new_image(uint16_t width,uint16_t height){
  if(height < 1 || height > MAX_HEIGHT || width < 1 || width > MAX_WIDTH){
    printf("image height and width must be both atleast 1px\n");
    return NULL;
  }
  
  struct bin_image* img = (struct bin_image*)malloc(sizeof(struct bin_image));
  
  if(!img){
    perror("");
    exit(1);
  }

  img->height = height;
  img->width = width;

  img->pixels = (uint8_t**)malloc(sizeof(uint8_t*) * img->height);
  int i,j;

  for(i = 0; i < img->height; i++){
    img->pixels[i] = (uint8_t*)malloc(sizeof(uint8_t) * img->width + 1);
    for(j = 0; j < img->width; j++){
      img->pixels[i][j] = ' ';
    }
    img->pixels[i][j] = '\0';
  }

  return img;
}

void free_img_buf(struct bin_image** img){

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

  for(int i = 0; i < img_data->height; i++){
    assert(fwrite(img_data->pixels[i],sizeof(uint8_t) * img_data->width+1,1,img));
  }
  
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

  if(img_data == NULL){
    perror("");
    return NULL;
  }

  assert(fread(&img_data->height,sizeof(uint16_t),1,img));
  assert(fread(&img_data->width,sizeof(uint16_t),1,img));

  img_data->pixels = (uint8_t**)malloc(sizeof(uint8_t*) * img_data->height);
  int i,j;
  for(i = 0; i < img_data->height; i++){
    img_data->pixels[i] = (uint8_t*)malloc(sizeof(uint8_t) * img_data->width + 1);
    for(j = 0; j < img_data->width; j++){
      img_data->pixels[i][j] = ' ';
    }
    img_data->pixels[i][j] = '\0';
  }
  
  for(int i = 0; i < img_data->height; i++){
    assert(fread(img_data->pixels[i],sizeof(uint8_t) * img_data->width+1,1,img));
  }

  fclose(img);

  return img_data;
}

#endif



#ifndef FINDER_H
#define FINDER_H

#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DIR_ID 0x41ed

#define MAX_PATH 256
#define PATH_DELIM "/"

struct file_list{
  char path[MAX_PATH];
  char name[MAX_PATH];

  int index;
  struct file_list* next;
};

struct file_manager{
  int file_found;
  struct file_list* list;
  struct file_list* head;
};

struct file_manager* init_manager(){
  struct file_manager* fm = (struct file_manager*)malloc(sizeof(struct file_manager));
  fm->file_found = 0;
  fm->list = NULL;
  fm->head = NULL;
   
  return fm;
}

static void add_file_to_list(struct file_manager** fm,const char* path,char* file_name){
  struct file_list* _new = (struct file_list*)malloc(sizeof(struct file_list));
  strcpy(_new->path,path);
  _new->path[MAX_PATH - 1] = '\0';
  strcpy(_new->name,file_name);
  _new->name[MAX_PATH - 1] = '\0';
  _new->next = NULL;
  _new->index = (*fm)->file_found++;
  
  
  if((*fm)->head == NULL){
    (*fm)->head = _new;
  }else{
    struct file_list* current = (*fm)->head;

    while(current->next != NULL){
      current = current->next;
    }

    current->next = _new;
    
  } 
}

int is_dir(char* path){
    struct stat st;
    assert(!stat(path,&st));

    if(st.st_mode == DIR_ID){
      return 1;
    }
    return 0;
}

void walk_dir(struct file_manager** fm, char* starter_path,const char* ext){
  DIR* dir;
  struct dirent *file;
  
  if(!(dir = opendir(starter_path))){
    perror("Error opening directory");
    return;
  }
  
  char fullpath[MAX_PATH];

  while((file = readdir(dir)) != NULL){
    if(file->d_name[0] == '.'){
      continue;
    }
    
    strncpy(fullpath,starter_path,MAX_PATH);
    if(fullpath[strlen(fullpath) - 1] != '/'){
      strcat(fullpath,PATH_DELIM);
    }
    strcat(fullpath,file->d_name);
    int len = strlen(fullpath);

    fullpath[len + 1] = '\0';
    
    if(is_dir(fullpath)){

      walk_dir(fm,fullpath,ext);
    }else if(strstr(file->d_name,ext) != NULL){
      
      //printf("Added %s\n",file->d_name);
      add_file_to_list(fm,fullpath,file->d_name);
    }
  }
}

void print_list(struct file_manager* fm){

  if(fm->file_found <= 0){
    printf("NONE");
    return;
  }
  fm->list = fm->head;
  while(fm->list != NULL){
    printf("index: %d. fullpath: %s. name:%s \n",fm->list->index,fm->list->path,fm->list->name);
    fm->list = fm->list->next;
  }
}

void destroy_manager(struct file_manager** fm){
  struct file_list* temp = (*fm)->list;
  while((*fm)->list != NULL){
    (*fm)->list = (*fm)->list->next;
    free(temp);
    temp = (*fm)->list;
  }

  free(*fm);
  *fm = NULL;
}

void rescan(struct file_manager** fm, char* path,const char* ext){
  destroy_manager(fm);
  walk_dir(fm,path,ext);
}

int to_index(struct file_list** fm,int index){
  while(*fm != NULL){
    if((*fm)->index == index){
      return 1;
    }else{
      (*fm) = (*fm)->next;
    }
  }

  return 0;
}


#endif

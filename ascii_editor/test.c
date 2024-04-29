#include <stdio.h>
#include "finder.h"
#define DIRECTORY "/home/carlos/Scripts/C/code/"

int main(){
  struct file_manager* fm = init_manager();

  walk_dir(&fm,DIRECTORY);
  
;

  print_list(fm);
  destroy_manager(&fm);
}

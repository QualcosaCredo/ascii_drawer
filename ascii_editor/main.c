#include "editor.h"

int main(){
  struct window* screen = ed_init_win(60,30,0);

  if(has_colors() == FALSE){
    ed_close(screen);
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }

  colors_init();
  
  int sel = 0;
  int ch;
  while(ch != 'q'){
    draw_bottom_bar(screen,ch);
    draw_menu(screen,sel);
    switch(ch){

      case 'l':
      case KEY_RIGHT:
        if(sel < TOPBAR_BUTTONS - 1)
          sel++;
      break;

      case 'h':
      case KEY_LEFT:
        if(sel > 0)
          sel--;
      break;

      case KEY_ENTER:
      case '\n':
        switch(sel){

          case 0:
            //create new image;
          break;

          case 1:
            // look a directory for an image to edit
          break;

          default:
          break;
        }
      break;

      default:
      break;
    }
    
    ch = getch();
  }

  ed_close(screen);
  return 0;
}

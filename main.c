#include "editor.h"

int main(){
  struct window* screen = ed_init_win(60,20,TM_FULLSCREEN);
  struct file_manager* fm = NULL;

  if(has_colors() == FALSE){
    ed_close(screen);
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }

  colors_init();
  
  int menu_sel = 0;
  int ch;

  //i should have used more files
  while(ch != 'Q'){
    werase(screen->win);
    draw_menu_bottom_bar(screen,ch);
    draw_menu(screen,menu_sel);
    
    if(fm == NULL){
      fm = init_manager();
      walk_dir(&fm,START_DIR,".bim"); 
    }

    draw_image_paths(screen,fm,-1);
    wrefresh(screen->win);
    
    switch(ch){
      case 'l':
      case KEY_RIGHT:
        if(menu_sel < TOPBAR_BUTTONS - 1)
          menu_sel++;
      break;

      case 'h':
      case KEY_LEFT:
        if(menu_sel > 0)
          menu_sel--;
      break;

      case KEY_ENTER:
      case '\n':
        switch(menu_sel){
          case 0:
            destroy_manager(&fm);
          break;

          case 1:
            werase(screen->win);

            int edit_sel = 0;
            int edit_ch = 0;
            while((edit_ch = getch()) != 'Q'){
              werase(screen->win);
              draw_image_paths(screen,fm,edit_sel);
              draw_edit_bottom_bar(screen,fm,edit_sel);
              draw_menu(screen,-1);
              wrefresh(screen->win);
              switch(edit_ch){
                case 'j':
                case KEY_DOWN:
                  if(edit_sel < fm->file_found)
                    edit_sel++;
                break;

                case 'k':
                case KEY_UP:
                  if(edit_sel > 0)
                    edit_sel--;
                break;

                case KEY_ENTER:
                case '\n':
                  fm->list = fm->head;
                  to_index(&fm->list,edit_sel);
                  
                  start_editing(screen,fm->list->path); 
                break;
                  
                default:
                break;
              }
               
            }
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

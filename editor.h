#ifndef EDITOR_H
#define EDITOR_H

#include "image.h"
#include "finder.h"
#include <curses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>


#define MAX_WIN_HEIGHT 400
#define MAX_WIN_WIDTH 800

#define MIN_WIN_HEIGHT 20
#define MIN_WIN_WIDTH 20

#define MAIN_MENU_TOPBAR_OFFSET 3
#define TOPBAR_BUTTONS 2

#define BOTTOMBAR_OFFSET 2 //from the bottom

#define BRUSH_MODE_TOPOFFSET 3
#define NUM_BRUSH_MODES 3

#define START_DIR "./"

#define CURSOR_CHAR ' '

#define MAX_EXT_SIZE 16

struct window{
  WINDOW* win;
  int width;
  int height;
};

enum brush_mode{
  DRAW,
  ERASE,
  PICK
};

struct brush{
  enum brush_mode mode;
  char ch;
};

struct cursor{
  int x;
  int y;
};

enum FLAGS{
  TM_NONE,
  TM_FULLSCREEN
};

enum COLORS{
  BLANK,
  YELLOW,
  CYAN,
  RED,
  CURSOR
};

WINDOW* create_win(int width,int height,int x,int y){
  WINDOW* win = newwin(height,width,y,x);
  wrefresh(win);
  return win;
}

struct window* ed_init_win(int width,int height,int flags){
  switch(flags){
    case TM_FULLSCREEN:
      struct winsize tm;
      ioctl(0,TIOCGWINSZ,&tm);

      width = tm.ws_col;
      height = tm.ws_row;
    break;
    default:
    break;
  }

  if(height < MIN_WIN_HEIGHT || height > UINT16_MAX || width < MIN_WIN_WIDTH || width > UINT16_MAX){
    system("clear");
    printf("window height and width must be both atleast %dx%d and smaller than %dx%d\n",MIN_WIN_WIDTH,MIN_WIN_HEIGHT,MAX_WIN_WIDTH,MAX_WIN_HEIGHT);
    sleep(1);
    exit(1);
  }

  initscr();
  cbreak();
  keypad(stdscr,TRUE);
  nodelay(stdscr,TRUE);
  noecho();
  raw();
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  
  refresh();
  static struct window screen;
  
  screen.win = create_win(width,height,0,0);
  screen.width = width;
  screen.height = height;
  return &screen;
}

void colors_init(){
  start_color();
  init_pair(YELLOW,COLOR_YELLOW,COLOR_BLACK);
  init_pair(CYAN,COLOR_BLACK,COLOR_CYAN);
  init_pair(RED,COLOR_RED,COLOR_BLACK);
  init_pair(CURSOR,COLOR_WHITE,COLOR_WHITE);
}

void ed_close(struct window* screen){
  delwin(screen->win);
  endwin();
}

void draw_menu_text(struct window* screen,int sel){
  int text_offset = screen->width/TOPBAR_BUTTONS;

  char text[TOPBAR_BUTTONS][64] = {"Rescan directory","Edit image"};

  int cnt = 0;

  for(int i = text_offset - text_offset/2; i < screen->width; i += text_offset){
    if(cnt == sel){
      wattron(screen->win,COLOR_PAIR(YELLOW)); 
    }
    mvwprintw(screen->win,1,i - strlen(text[cnt])/2,"%s",text[cnt]);
    wattroff(screen->win,COLOR_PAIR(YELLOW));
    cnt++;
  }
}

void draw_menu(struct window* screen,int sel){
  mvwhline(screen->win,MAIN_MENU_TOPBAR_OFFSET,1,0,screen->width-2);

  int btn_offset = screen->width/TOPBAR_BUTTONS;

  for(int i = btn_offset; i < screen->width; i+=btn_offset){
    mvwvline(screen->win,1,i,0,MAIN_MENU_TOPBAR_OFFSET - 1);
  }
  box(screen->win,0,0);
  draw_menu_text(screen,sel);
}

void draw_menu_bottom_bar(struct window* screen,int last_ch){
  char ch = (last_ch >= 33 && last_ch <= 126) ? last_ch : ch; 
  mvwhline(screen->win,screen->height - BOTTOMBAR_OFFSET - 1,0,0,screen->width);
  mvwprintw(screen->win,screen->height - 2,2,"%s: x:%d y:%d  | last: %c","Window size",screen->width,screen->height,ch);
}

void draw_image_paths(struct window* screen,struct file_manager* fm,int sel){
  int top_offset = MAIN_MENU_TOPBAR_OFFSET + 1;
  int bot_offset = screen->height - (BOTTOMBAR_OFFSET + 1);

  int start_line = (sel >= bot_offset - top_offset) ? sel : 0;

  if(fm->file_found <= 0){
    mvwprintw(screen->win,screen->height/2,(screen->width/2) - (strlen("No files")/2) ,"No files");
    return;
  }

  fm->list = fm->head;
  to_index(&fm->list,start_line);

  for(int i = top_offset; i < bot_offset && fm->list != NULL; i++){
    if(i - top_offset + start_line == sel){
      wattron(screen->win,COLOR_PAIR(CYAN));
    }        
    mvwprintw(screen->win,i,1,"%d) %s",fm->list->index,fm->list->name);
    fm->list = fm->list->next;
       
    wattroff(screen->win,COLOR_PAIR(CYAN));
  }
}

void draw_edit_bottom_bar(struct window* screen,struct file_manager* fm,int sel){
  mvwhline(screen->win,screen->height - BOTTOMBAR_OFFSET - 1,0,0,screen->width);
  
  struct file_list* current = fm->head; 
  if(!to_index(&current,sel)){
    mvwprintw(screen->win,screen->height - 2, 2,"could not get the path");
    return;
  }
  mvwprintw(screen->win,screen->height - 2, 2,"path: %s",current->path);
}

void draw_cursor(struct window* screen,struct cursor cursor){
  wattron(screen->win,COLOR_PAIR(CURSOR));
  mvwprintw(screen->win,cursor.y,cursor.x,"%c",CURSOR_CHAR);
  wattroff(screen->win,COLOR_PAIR(CURSOR));
}
void draw_brush_top_bar(struct window* screen,struct cursor cursor,int offset_x,int offset_y){

  mvwhline(screen->win,MAIN_MENU_TOPBAR_OFFSET,1,0,screen->width-2);
  mvwprintw(screen->win,1,1,"cursor:{ X: %d, Y: %d}",cursor.x,cursor.y);
  
  mvwprintw(screen->win,2,1,"offset:{ X: %d, Y: %d}",offset_x,offset_y);

  mvwprintw(screen->win,1,screen->width/2,"exit: BACKSPACE");
  mvwprintw(screen->win,2,screen->width/2,"change mode: \\");

}

void draw_brush_mode_bottom_bar(struct window* screen,struct bin_image* img_data,struct brush brush,const char* path){ 
  const char brush_modes[][16] = {"DRAW","ERASE","PICK"};
  mvwhline(screen->win,screen->height - BOTTOMBAR_OFFSET - 1,0,0,screen->width);
  mvwprintw(screen->win,screen->height - 2,60,"current: %s",path);
  mvwprintw(screen->win,screen->height - 2, 2,"width: %d | height: %d",img_data->width,img_data->height);
  mvwprintw(screen->win,screen->height - 2,30,"Brush:{Mode: %s, char: %c}",brush_modes[brush.mode],brush.ch);
}

#define ENABLE_MOUSE_MOVEMENT_EVENTS printf("\033[?1003h\n")
#define DISABLE_MOUSE_MOVEMENT_EVENTS printf("\033[?1003l\n")


char* check_extension( char* name,const char* ext){
  
  if(!strstr(name,ext)){
    char temp[MAX_EXT_SIZE];
    if(ext[0] != '.'){
      strcpy(temp,".");
      strcat(temp,ext);
    }
    strcat(name,temp);
    int len = strlen(name);
    name[len + 1] = '\0'; 
  }

  return name;
}

void draw_rectangle(struct window* screen,int y1, int x1, int y2, int x2){
  mvwhline(screen->win,y1, x1, 0, x2-x1);
  mvwhline(screen->win,y2, x1, 0, x2-x1);
  mvwvline(screen->win,y1, x1, 0, y2-y1);
  mvwvline(screen->win,y1, x2, 0, y2-y1);
  mvwaddch(screen->win,y1, x1, ACS_ULCORNER);
  mvwaddch(screen->win,y2, x1, ACS_LLCORNER);
  mvwaddch(screen->win,y1, x2, ACS_URCORNER);
  mvwaddch(screen->win,y2, x2, ACS_LRCORNER);
}

void start_editing(struct window* screen,const char* path){
  int bot_offset = screen->height - (BOTTOMBAR_OFFSET + 1);
  
  int y_offset = 0;
  int x_offset = 0;

  struct cursor cursor = {.x = 0,.y = 0};

  struct brush brush = {
    .mode = DRAW,
    .ch = '#',
  };
  
  struct bin_image* img_data = open_image(path);

  if(img_data == NULL){
    return;
  }
  
  int ch;
  int drawing = 1;
  
  MEVENT event;

  ENABLE_MOUSE_MOVEMENT_EVENTS; //enable mouse movement events
  int mouse_down = 0;
  while(drawing){
    werase(screen->win);
    draw_rectangle(screen,0,0,img_data->height + 4  - y_offset,img_data->width + 1 - x_offset);
    draw_brush_top_bar(screen,cursor,x_offset,y_offset);
    draw_brush_mode_bottom_bar(screen,img_data,brush,path);

    box(screen->win,0,0);
    uint8_t buf[MAX_WIN_WIDTH + 1];

    int i,j;

    for(int i = 0; i < MAX_WIN_WIDTH; i++){
      buf[i] = 0;
    }


    for(int i = 0; i + y_offset < img_data->height && i < bot_offset - BRUSH_MODE_TOPOFFSET - 1; i++){
      for(int j = 0; j + x_offset < img_data->width && j < screen->width - 2; j++){
        buf[j] = img_data->pixels[i + y_offset][j + x_offset];
      }

      mvwprintw(screen->win,i + BRUSH_MODE_TOPOFFSET + 1,1,"%s",buf);
    }

    draw_cursor(screen,cursor);
    wrefresh(screen->win);

    switch((ch = getch())){
      case 127:
      case '\b':
      case KEY_BACKSPACE:
        drawing = 0;
      break;
      case 's' & 0x1F: //ctrl key
        save_image(path,img_data);
      break;
      case '\\':
        brush.mode++;

        if(brush.mode == NUM_BRUSH_MODES){
          brush.mode = 0;
        }
      break;

      case KEY_DOWN:
        if(y_offset > 0)
          y_offset--;
      break;
      case KEY_UP:
        if(y_offset < img_data->height - 1)
          y_offset++;
      break;
      case KEY_RIGHT:
        if(x_offset > 0)
          x_offset--;
      break;

      case KEY_LEFT:
          if(x_offset < img_data->width - 1)

            x_offset++;
        default:
          if(ch >= 33 && ch <= 125) 
            brush.ch = ch;
        break;
      }
  
      if(getmouse(&event) == OK){
        cursor.x = event.x;
        cursor.y = event.y;
        if(event.bstate & BUTTON1_PRESSED){
          mouse_down = 1;
        }else if(event.bstate & BUTTON1_RELEASED){
          mouse_down = 0;
        }
      }

      if(mouse_down){
        if(cursor.x + x_offset < img_data->width && cursor.x >= 0 && cursor.y + y_offset <= img_data->height + BRUSH_MODE_TOPOFFSET && cursor.y - BRUSH_MODE_TOPOFFSET > 0){
          switch(brush.mode){
          case DRAW:
          if(brush.ch >= 33 && brush.ch <= 126)
            img_data->pixels[cursor.y - BRUSH_MODE_TOPOFFSET - 1 + y_offset][cursor.x - 1 + x_offset] = brush.ch;
        break;
        case ERASE:
          img_data->pixels[cursor.y - BRUSH_MODE_TOPOFFSET - 1 + y_offset][cursor.x - 1 + x_offset] = ' ';
        break;
        case PICK:
          if(img_data->pixels[cursor.y - BRUSH_MODE_TOPOFFSET - 1 + y_offset][cursor.x - 1 + x_offset] >= 33 &&  img_data->pixels[cursor.y][cursor.x + x_offset] <= 126)

          brush.ch = img_data->pixels[cursor.y - BRUSH_MODE_TOPOFFSET - 1 + y_offset][cursor.x - 1 + x_offset];
        break;
        default:
        break;
        }
      }
    }
  }

  DISABLE_MOUSE_MOVEMENT_EVENTS;
  wclear(screen->win);
  free_img_buf(&img_data);
}

#endif 

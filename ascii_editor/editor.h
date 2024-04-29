#ifndef EDITOR_H
#define EDITOR_H

#include "image.h"
#include <curses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#define MIN_WIN_HEIGHT 20
#define MIN_WIN_WIDTH 20

#define MAIN_MENU_TOPBAR_OFFSET 3
#define TOPBAR_BUTTONS 2

#define BOTTOMBAR_OFFSET 2 //from the bottom

struct window{
  WINDOW* win;
  int width;
  int height;
};

enum brush_mode{
  DRAW,
  ERASE
};

struct brush{
  enum brush_mode mode;
  int x;
  int y;
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
  GREEN,
  RED
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
    printf("window height and width must be both atleast 20px\n");
    sleep(1);
    exit(1);
  }

  initscr();
  cbreak();
  keypad(stdscr,TRUE);
  nodelay(stdscr,TRUE);
  noecho();
  
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
  init_pair(GREEN,COLOR_GREEN,COLOR_BLACK);
  init_pair(RED,COLOR_RED,COLOR_BLACK);
}

void ed_close(struct window* screen){
  delwin(screen->win);
  endwin();
}


void draw_menu_text(struct window* screen,int sel){
  int text_offset = screen->width/TOPBAR_BUTTONS;

  char text[TOPBAR_BUTTONS][64] = {"Create new Image","Edit image","they arent","skill issue","get good","palle"};

  int cnt = 0;

  for(int i = text_offset - text_offset/2; i < screen->width; i += text_offset){
    if(cnt == sel){
          wattron(screen->win,COLOR_PAIR(YELLOW)); 
      }
      mvwprintw(screen->win,1,i - strlen(text[cnt])/2,"%s",text[cnt]);
      cnt++;
      wattroff(screen->win,COLOR_PAIR(YELLOW));
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
  wrefresh(screen->win);
}

void draw_bottom_bar(struct window* screen,int last_ch){
  char ch = (last_ch >= 33 && last_ch <= 126) ? last_ch : ch; 
  mvwhline(screen->win,screen->height - BOTTOMBAR_OFFSET - 1,0,0,screen->width);
  mvwprintw(screen->win,screen->height - 2,2,"%s: x:%d y:%d  | last: %c","Window size",screen->width,screen->height,ch);
}



#endif 

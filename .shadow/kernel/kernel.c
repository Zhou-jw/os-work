#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include "photop3.h"
#define SIDE 1

static int w, h;  // Screen size

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = { AM_KEYS(KEYNAME) };

static inline void puts(const char *s) {
  for (; *s; s++) putch(*s);
}

void print_key() {
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}

static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;

  for (int x = 0; x * SIDE <= w; x++) {
    for (int y = 0; y * SIDE <= h; y++) {
      if ((x & 1) ^ (y & 1)) {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      }
    }
  }
}

int read_pic_wh(char *str, int *pic_w, int *pic_h) {
  // read pic_w
  char *tmp = str;
  *pic_h = atoi(str + 3);
  str = str + 3;
  while(*str != ' ')
    str++;
  *pic_w = atoi(str+1);
  str++;
  while(*str >= '0' && *str <= '9') 
    str++;
  return str-tmp+5;
}

static void draw(int x, int y, int w, int h, uint32_t color){
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  puts("draw func\n");
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

uint32_t get_color(char *str) {
  puts("get_color func \n");
  char hex[10]="\0", tmp[2]="0\0";
  for(int i = 1; i <= 3; i++) {
    for(int j = 1; j <= 2; j++) {
      *tmp = *str;
      strcat(hex, tmp);
      str++;
    }
    str = str + 2;
  }
  printf("%s", hex);
  return atoi(hex);
}
void disp_xy2uv(int pic_w, int pic_h, int width, int height,char *str) {
  // display xy to uv with color (pic_h, pic_w) -> (width, height)
  int u, v, color;
  for(int x=0; x<pic_h; x++) {
    for(int y=0; y<pic_w; y++) {
      printf("display function \n");
      color = get_color(str);
      u = x * height / pic_h;
      v = y * width / pic_w;
      draw(u, v, 1, 1, color);
      str = str + 12;
    }
  }
}

// Operating system is a C program!
int main(const char *args) {
  ioe_init();
  puts("mainargs = \"");
  puts(args);  // make run mainargs=xxx
  puts("\"\n");

  // // splash();
  int width = io_read(AM_GPU_CONFIG).width;
  int height = io_read(AM_GPU_CONFIG).height;
  int pic_w, pic_h;
  int color_start = read_pic_wh(photop3_ppm, &pic_h, &pic_w);
  printf("%d %d %d", pic_w, pic_h, color_start);
  disp_xy2uv(pic_w, pic_h, width, height, photop3_ppm + color_start);
  // printf("%s is string", "absdcds");
  puts("Press any key to see its key code...\n");
  while (1) {
    print_key();
  }
  return 0;
}

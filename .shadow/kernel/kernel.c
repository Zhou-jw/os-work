#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include "photop3.h"
#define SIDE 16

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
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 16777215); // white
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
  // puts("draw func\n");
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

int get_int(char *str, int *len) {
  int num=0;
  while( '0' <= *str  && *str <='9') {
    num = num*10 + (*str - '0');
    str++;
    (*len)++;
  }
  // printf("in get_int() num is %d, len is %d", num, *len);
  return num;
}

uint32_t get_color(char *str, int *step) {
  int rgb = 0, dec = 0, num = 0, power = 5;
  while(rgb < 3) {
    int len = 0;
    num = get_int(str, &len);
    printf("%d", num);
    dec = dec + (num/16) * pow(16, power);
    dec = dec + (num % 16) * pow(16, power-1);
    power -= 2;
    rgb++;
    str = str + len + 2;
    *step += len + 2;
  }
  putch('\n');
  // printf("color is %d, step is %d\n", dec, *step);
  return dec;
}
void disp_xy2uv(int pic_w, int pic_h, int width, int height,char *str) {
  // display xy to uv with color (pic_h, pic_w) -> (width, height)
  int u, v, color, step;
  for(int y=0; y<pic_w; y++) {
     for(int x=0; x<pic_h; x++) {
      // printf("display function \n");
      step = 0;
      color = get_color(str, &step);
      u = x * width / pic_w;
      v = y * height / pic_h;
      draw(u, v, 1, 1, color);
      str = str + step;
      // printf("now str is %c%c", *str, *(str+1));
      // putch(' ');
    }
  }
}

// Operating system is a C program!
int main(const char *args) {
  ioe_init();
  puts("mainargs = \"");
  puts(args);  // make run mainargs=xxx
  puts("\"\n");

  // splash();
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

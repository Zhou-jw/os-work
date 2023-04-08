#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int pow(int x, int y) {
  if(y == 0) return 1;
  return pow(x, y-1) * x; 
}

int printf(const char *fmt, ...) {
  char *str = (char *) fmt;
  va_list ap;
  va_start(ap, fmt);
  int d;
  while(*str != '\0') {
    switch (*str++) {
      case '%':
        str++;
        switch (*str) {
          case 'd':
            d = va_arg(ap, int);
            int len = 0, tmp = d;
            while(tmp / 10) {
              len++;
              tmp = tmp / 10;
            }
            char ch;
            while(len) {
              ch = (d / pow(10, len)) % 10 + '0';
              putch(ch);
              d = d % pow(10, len);
              len--;
            }
            continue;
        }
        default: putch(*str); continue;
      }
      
  }
  va_end(ap);
  // panic("Not implemented");
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif

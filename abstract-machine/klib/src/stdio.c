#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

static inline void puts_debug(const char *s) {
  for (; *s; s++) putch(*s);
}

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int pow(int x, int y) {
  if(y == 0) return 1;
  return pow(x, y-1) * x; 
}

int printf(const char *fmt, ...) {
  // puts_debug(fmt);
  char *str = (char *) fmt;
  // putch(*str);
  // puts_debug(str);
  va_list ap;
  va_start(ap, fmt);
  int d;
  char *s;
  char c;
  while(*str != '\0') {
    // puts_debug("into while here!\n");
    switch (*str) {
      case '%':
        str++;
        switch (*str) {
          case 'd':
            str++;
            d = va_arg(ap, int);
            int len = 1, tmp = d;
            while(tmp / 10) {
              len++;
              tmp = tmp / 10;
            }
            char ch;
            while(len) {
              ch = (char)((d / pow(10, len-1)) % 10) + '0';
              putch(ch);
              d = d % pow(10, len-1);
              len--;
            }
            continue;
          case 's':
            s = va_arg(ap, char*);
            while(*s) {
              putch(*s);
              s++;
            }
            str++;
            continue;
          case 'c':
            c = va_arg(ap, int);
            putch((char)c);
            str++;
            continue;
        }
      default: 
        putch(*str);
        str++;
        continue;
    }
  }
  // puts_debug("end printf here!\n");
  va_end(ap);
  putch('\n');
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

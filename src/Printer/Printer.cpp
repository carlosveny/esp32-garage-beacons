#include <Arduino.h>
#include "Printer.h"

Printer::Printer(int size) {
  _size = size;
}

char* Printer::sprintf(const char *format, ...) {
  char* buffer = new char[_size];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, _size, format, args);
  va_end(args);
  return buffer;
}

#include <Arduino.h>
#include "Printer.h"

char* Printer::sprintf(const char *format, ...){
  const int SIZE = 80;
  char* buffer = new char[SIZE];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, SIZE, format, args);
  va_end(args);
  return buffer;
}
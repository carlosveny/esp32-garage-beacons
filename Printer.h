class Printer { 
public:
  Printer(int size);
  char* sprintf(const char *format, ...);
  private:
    int _size;
};
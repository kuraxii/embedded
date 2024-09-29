#include <fcntl.h>
#include <iostream>
#include <memory>
#include <unistd.h>

class FILE_FD {
  struct FD_DELETER {
    using pointer = int *;
    void operator()(pointer p) {
      if (*p >= 0) {
        close(*p);
        std::cout << "close...\n";
      }
    }
  };

  std::unique_ptr<int, FD_DELETER> uptr;

public:
  FILE_FD(const char *path, int WR) {
    int fd = open(path, WR);
    uptr = std::unique_ptr<int, FD_DELETER>(new int{fd});
    std::cout << "open...\n";
  }
  int *get() const { return uptr.get(); }
};



int main() {
  FILE_FD file("test.cpp", O_RDONLY);

  return 0;
}
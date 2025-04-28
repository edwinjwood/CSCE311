// Copyright 2025 CSCE 311
//
// This example mimics how virtual memory and page tables behave in a demand-paged system.
//
// The call to mmap() reserves a range of virtual memory and sets up page table entries
// for a file — this is analogous to allocating address space without yet assigning
// physical pages.
//
// The truncate() calls simulate the operating system actually backing pages with
// physical storage — this mirrors demand paging, where physical memory is only allocated
// when a page is accessed or written to.
//
// If you attempt to write to a page in the mapped range before the file has been
// truncated to cover it, the system will raise SIGBUS — just like a page fault
// that cannot be resolved because no backing store exists.
//
// Build with:
//   g++ -std=c++17 -Wall -pedantic -o grow-mmap-file grow_mmap_file.cc

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


int main(int argc, char *argv[]) {
  char *addr;
  int fd;

  const int kTotalPages = 10;
  const int kPageSize = 4096;

  if (argc < 2) {
    printf("grow-mmap-file dat/file_name.txt\n");
    return 1;
  }

  // contrast O_<permission>
  printf("O_RDONLY: %d\n", O_RDONLY);
  printf("O_WRONLY: %d\n", O_WRONLY);
  printf("O_RDWR: %d\n", O_RDWR);
  fd = open(argv[1], O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    printf("open err: %s", strerror(errno));
    return 2;
  }
  // with PROT_<permission>
  printf("PROT_READ: %d\n", PROT_READ);
  printf("PROT_WRITE: %d\n", PROT_WRITE);
  printf("PROT_READ | PROT_WRITE: %d\n", PROT_READ | PROT_WRITE);
  addr = static_cast<char *>(mmap(NULL,
                             kTotalPages * kPageSize,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED,
                             fd,
                             0));
  if (addr == MAP_FAILED) {
    printf("mmap err: %s", strerror(errno));
    return 3;
  }

  if (close(fd) < 0) {       /* No longer need 'fd' */
    printf("close err: %s", strerror(errno));
    return 4;
  }

  if (truncate(argv[1], kPageSize) < 0) {
    printf("mmap err: %s", strerror(errno));
    return 5;
  }
  int pages_used = 1;

  for (int i = 0, ii = (int)'A'; i < kTotalPages * kPageSize; ++i, ++ii) {
    if (i >= pages_used * kPageSize) {
      ++pages_used;
      if (truncate(argv[1], pages_used * kPageSize) < 0) {
        printf("mmap err: %s", strerror(errno));
        return 6;
      }
    }
    
    addr[i] = (char)ii;
    if (ii == (int)'Z')
      ii = (int)'a' - 1;  // ++ in for-loop comes after
    if (ii == (int)'z')
      ii = (int)'A' - 1;  // ++ in for-loop comes after
  }

  int msync_sig = msync(addr, kTotalPages * kPageSize, MS_SYNC);
  if (msync_sig < 0) {
    printf("msync err: %s", strerror(errno));
  }

  exit(EXIT_SUCCESS);
}

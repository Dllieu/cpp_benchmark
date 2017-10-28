# Compiler and Standard Libraries

 - How to compile from source : www.linuxfromscratch.org/lfs/view/development/index.html

## Compilers

 - Versions
  - GCC : https://gcc.gnu.org/
  - CLang : http://releases.llvm.org/download.html
 - C++ Support Status
  - GCC : https://gcc.gnu.org/projects/cxx-status.html
  - CLang : https://clang.llvm.org/cxx_status.html


## libstdc++ (GNU)

 - Get source from SVN GCC repo + build it

## libc++ (LLVM)

 - Versions : https://libcxx.llvm.org (Build from the head)

## GLIBC

 - Version : https://www.gnu.org/software/libc/
 - Version installed

```bash
--ldd --version
```

```c++
#include <gnu/libc-version.h>

void foo()
{
    std::cout << "GLIBC Version: " << gnu_get_libc_version() << "\n" << std::endl;
}
```

 - Build

- GLIBC_VERSION = 2.26

```bash
git clone git://sourceware.org/git/glibc.git
cd glibc
git checkout --track -b local_glibc-${GLIBC_VERSION) origin/release/${GLIBC_VERSION)/master

mkdir build && cd build
../configure --prefix=/opt/glibc-${GLIBC_VERSION}
make -j8
sudo make install

# If you wish to use it somehwere else
export LD_LIBRARY_PATH=/opt/glibc-${GLIBC_VERSION}/lib
```

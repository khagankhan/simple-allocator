# Simple Memory Allocator

* It supports old malloc version of `sbrk` and implicit free list

It will support:

* Explicit Free List
* Segregated Free List 
* Comparison of these approaches

## How to build and run
```bash
git clone https://github.com/khagankhan/simple-allocator.git
cd ./simple-allocator
mkdir build && cd build
cmake ..
make -j
```
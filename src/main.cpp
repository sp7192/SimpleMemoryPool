#include "SimpleMemoryPool.h"

#include <cstdio>

#define ALLOCATOR_SIZE 1024 * 1024
#define ALLOCATOR_CHUNCK_SIZE 256

struct Point {
    float x;
    float y;

    Point(float _x, float _y) : x(_x), y(_y) {
        printf("in Point constructor\n");
    }
    ~Point() {
        printf("in point destructor\n");
    }
};

int main() {
    SimpleMemoryPool sMemPool(ALLOCATOR_SIZE, ALLOCATOR_CHUNCK_SIZE);
    logMem(&sMemPool);

    MemoryBlock met = sMemPool.allocateMem();
    logMem(&sMemPool);

    MemoryBlock mem2 = sMemPool.allocateMem();
    MemoryBlock mem3 = sMemPool.allocateMem();
    logMem(&sMemPool);

    sMemPool.freeMem(mem2.ptr);
    logMem(&sMemPool);

    Point* p = sMemPool.construct<Point>(12.0f, 25.0f);
    printf("Point is : %.2f, %.2f\n", p->x, p->y);
    logMem(&sMemPool);
    sMemPool.destruct(p);
    logMem(&sMemPool);

    return 0;
}
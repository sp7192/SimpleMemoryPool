﻿#include "SimpleMemoryPool.h"

#include <cstdio>

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
    sMemPool.init();
    logMem(&sMemPool);

    void* ptr = sMemPool.allocateMem();
    logMem(&sMemPool);

    void* ptr2 = sMemPool.allocateMem();
    void* ptr3 = sMemPool.allocateMem();
    logMem(&sMemPool);

    sMemPool.freeMem(ptr2);
    logMem(&sMemPool);

    Point* p = sMemPool.construct<Point>(12.0f, 25.0f);
    printf("Point is : %.2f, %.2f\n", p->x, p->y);
    logMem(&sMemPool);
    sMemPool.destruct(p);
    logMem(&sMemPool);

    auto leakedMem = sMemPool.getUsingMemories();
    for (const auto & m : leakedMem) {
        printf("leaked mem address : %zd\n", reinterpret_cast<std::uintptr_t>(m));
    }

	return 0;
}

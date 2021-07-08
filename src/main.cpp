#include "SimpleMemoryPool.h"
#include "gtest/gtest.h"

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

TEST(SimpleMemoryPoolTest, IntegrityChecking) {
    SimpleMemoryPool simpleMemoryPool(ALLOCATOR_SIZE, ALLOCATOR_CHUNCK_SIZE);
    logMem(&simpleMemoryPool);
    size_t memoryBlockCount = ALLOCATOR_SIZE / ALLOCATOR_CHUNCK_SIZE;
    EXPECT_EQ(simpleMemoryPool.getMemoryBlockSize(), ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getMemoryTotalSize(), ALLOCATOR_SIZE);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlocksCount(), memoryBlockCount);

    MemoryBlock mem = simpleMemoryPool.allocateMem();
    logMem(&simpleMemoryPool);

    MemoryBlock mem2 = simpleMemoryPool.allocateMem();
    MemoryBlock mem3 = simpleMemoryPool.allocateMem();
    logMem(&simpleMemoryPool);

    simpleMemoryPool.freeMem(mem2.ptr);
    logMem(&simpleMemoryPool);

    Point* p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    printf("Point is : %.2f, %.2f\n", p->x, p->y);
    logMem(&simpleMemoryPool);
    simpleMemoryPool.destruct(p);
    logMem(&simpleMemoryPool);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
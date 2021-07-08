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

TEST(SimpleMemoryPoolTest, CheckConstructor) {
    SimpleMemoryPool simpleMemoryPool(ALLOCATOR_SIZE, ALLOCATOR_CHUNCK_SIZE);
    logMem(&simpleMemoryPool);
    size_t memoryBlockCount = ALLOCATOR_SIZE / ALLOCATOR_CHUNCK_SIZE;
    EXPECT_EQ(simpleMemoryPool.getMemoryBlockSize(), ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getMemoryTotalSize(), ALLOCATOR_SIZE);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlocksCount(), memoryBlockCount);

    MemoryBlock mem = simpleMemoryPool.allocateMem();
    logMem(&simpleMemoryPool);
    EXPECT_NE(mem.ptr, nullptr);
    EXPECT_EQ(mem.size, ALLOCATOR_CHUNCK_SIZE) << "mem size is " << mem.size;
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount -1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);


    MemoryBlock mem2 = simpleMemoryPool.allocateMem();
    MemoryBlock mem3 = simpleMemoryPool.allocateMem();
    logMem(&simpleMemoryPool);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 3 * ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 3);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 3);

    simpleMemoryPool.freeMem(mem2.ptr);
    logMem(&simpleMemoryPool);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 2);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 2);

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
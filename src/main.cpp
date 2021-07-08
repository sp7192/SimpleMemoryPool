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
    SimpleFixedMemoryPool simpleMemoryPool(ALLOCATOR_SIZE, ALLOCATOR_CHUNCK_SIZE);
    logMem(&simpleMemoryPool);
    size_t memoryBlockCount = ALLOCATOR_SIZE / ALLOCATOR_CHUNCK_SIZE;
    EXPECT_EQ(simpleMemoryPool.getMemoryBlockSize(), ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getMemoryTotalSize(), ALLOCATOR_SIZE);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlocksCount(), memoryBlockCount);
}

TEST(SimpleMemoryPoolTest, CheckAllocateMem) {
    SimpleFixedMemoryPool simpleMemoryPool(ALLOCATOR_SIZE, ALLOCATOR_CHUNCK_SIZE);
    size_t memoryBlockCount = ALLOCATOR_SIZE / ALLOCATOR_CHUNCK_SIZE;

    MemoryBlock mem = simpleMemoryPool.allocateMem();
    logMem(&simpleMemoryPool);
    ASSERT_TRUE(mem.ptr);
    EXPECT_EQ(mem.size, ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);


    MemoryBlock mem2 = simpleMemoryPool.allocateMem();
    MemoryBlock mem3 = simpleMemoryPool.allocateMem();
    logMem(&simpleMemoryPool);
    ASSERT_NE(mem2.ptr, nullptr);
    EXPECT_EQ(mem2.size, ALLOCATOR_CHUNCK_SIZE);
    ASSERT_NE(mem3.ptr, nullptr);
    EXPECT_EQ(mem3.size, ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 3 * ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 3);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 3);
}

TEST(SimpleMemoryPoolTest, CheckFreeMem) {
    SimpleFixedMemoryPool simpleMemoryPool(ALLOCATOR_SIZE, ALLOCATOR_CHUNCK_SIZE);
    size_t memoryBlockCount = ALLOCATOR_SIZE / ALLOCATOR_CHUNCK_SIZE;

    MemoryBlock mem = simpleMemoryPool.allocateMem();
    MemoryBlock mem2 = simpleMemoryPool.allocateMem();
    MemoryBlock mem3 = simpleMemoryPool.allocateMem();

    bool res = simpleMemoryPool.freeMem(mem2.ptr);
    logMem(&simpleMemoryPool);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 2);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 2);
}

TEST(SimpleMemoryPoolTest, CheckConstruct) {
    const float epsilon = 0.0001f;

    SimpleFixedMemoryPool simpleMemoryPool(ALLOCATOR_SIZE, ALLOCATOR_CHUNCK_SIZE);
    size_t memoryBlockCount = ALLOCATOR_SIZE / ALLOCATOR_CHUNCK_SIZE;

    Point* p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    ASSERT_TRUE(p);
    EXPECT_TRUE(std::abs(p->x - 12.0f) < epsilon);
    EXPECT_TRUE(std::abs(p->y - 25.0f) < epsilon);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), ALLOCATOR_CHUNCK_SIZE);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);
}

TEST(SimpleMemoryPoolTest, CheckDestruct) {
    const float epsilon = 0.0001f;

    SimpleFixedMemoryPool simpleMemoryPool(ALLOCATOR_SIZE, ALLOCATOR_CHUNCK_SIZE);
    size_t memoryBlockCount = ALLOCATOR_SIZE / ALLOCATOR_CHUNCK_SIZE;

    Point * p   = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    bool    res = simpleMemoryPool.destruct(p);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 0);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 0);
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
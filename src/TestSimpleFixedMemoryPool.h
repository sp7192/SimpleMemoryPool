#include <cstdio>
#include "SimpleFixedMemoryPool.h"
#include "gtest/gtest.h"

namespace smp = SimpleMemoryPool;

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

TEST(SimpleFixedMemoryPoolTest, CheckConstructor) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;
    smp::logMem(&simpleMemoryPool);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlockSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryTotalSize(), totalMemorySize);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlocksCount(), memoryBlockCount);
}

TEST(SimpleFixedMemoryPoolTest, CheckAllocateMem) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock mem = simpleMemoryPool.allocateMem();
    smp::logMem(&simpleMemoryPool);
    ASSERT_TRUE(mem.ptr);
    EXPECT_EQ(mem.size, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);

    smp::MemoryBlock mem2 = simpleMemoryPool.allocateMem();
    smp::MemoryBlock mem3 = simpleMemoryPool.allocateMem();
    smp::logMem(&simpleMemoryPool);
    ASSERT_TRUE(mem2.ptr);
    EXPECT_EQ(mem2.size, memoryBlockSize);
    ASSERT_TRUE(mem3.ptr);
    EXPECT_EQ(mem3.size, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 3 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 3);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 3);
}

TEST(SimpleFixedMemoryPoolTest, CheckFreeMem) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock mem = simpleMemoryPool.allocateMem();
    smp::MemoryBlock mem2 = simpleMemoryPool.allocateMem();
    smp::MemoryBlock mem3 = simpleMemoryPool.allocateMem();

    bool res = simpleMemoryPool.freeMem(mem2.ptr);
    smp::logMem(&simpleMemoryPool);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 2);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 2);
}

TEST(SimpleFixedMemoryPoolTest, CheckConstruct) {
    const float epsilon = 0.0001f;

    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    Point* p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    ASSERT_TRUE(p);
    EXPECT_TRUE(std::abs(p->x - 12.0f) < epsilon);
    EXPECT_TRUE(std::abs(p->y - 25.0f) < epsilon);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);
}

TEST(SimpleFixedMemoryPoolTest, CheckDestruct) {
    const float epsilon = 0.0001f;

    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    Point* p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    bool    res = simpleMemoryPool.destruct(p);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 0);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 0);
}

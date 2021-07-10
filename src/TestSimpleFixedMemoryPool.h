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
        printf("in Point destructor\n");
    }
};

void logMemory(const smp::SimpleFixedMemoryPool * mem) {
    if (mem) {
        printf("================\n");
        printf("Total Memory size : %zu, memoryUsedSize Mem : %zu\n", mem->getMemoryTotalSize(), mem->getMemoryUsedSize());
        printf("Total chuncks : %zu, memoryUsedSize chuncks : %zu\n", mem->getMemoryBlocksCount(),
            mem->getMemoryBlocksCount() - mem->getFreeMemoryBlocksCount());
    }
}

TEST(SimpleFixedMemoryPoolTest, SuccsessfulConstruction) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;
    logMemory(&simpleMemoryPool);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlockSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryTotalSize(), totalMemorySize);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlocksCount(), memoryBlockCount);
}

TEST(SimpleFixedMemoryPoolTest, ConstructionWithIndivisbleBlocks) {
    const size_t totalMemorySize = 64;
    const size_t memoryBlockSize = 50;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlocksCount(), 1);
}

TEST(SimpleFixedMemoryPoolTest, ConstructionWithLowerTotalSize) {
    const size_t totalMemorySize = 32;
    const size_t memoryBlockSize = 50;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlockSize(), simpleMemoryPool.getMemoryTotalSize());
}


TEST(SimpleFixedMemoryPoolTest, SuccessfulAllocateMemory) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory();
    logMemory(&simpleMemoryPool);
    ASSERT_TRUE(mem.ptr);
    EXPECT_EQ(mem.size, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);

    smp::MemoryBlock mem2 = simpleMemoryPool.allocateMemory();
    smp::MemoryBlock mem3 = simpleMemoryPool.allocateMemory();
    logMemory(&simpleMemoryPool);
    ASSERT_TRUE(mem2.ptr);
    EXPECT_EQ(mem2.size, memoryBlockSize);
    ASSERT_TRUE(mem3.ptr);
    EXPECT_EQ(mem3.size, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 3 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 3);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 3);
    printf("Address of mem : 0x%.2x, mem2 : 0x%.2x, mem3 : 0x%.2x\n", mem.ptr, mem2.ptr, mem3.ptr);
}

TEST(SimpleFixedMemoryPoolTest, UnsuccessfulAllocateMemory) {
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    constexpr size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock memories[memoryBlockCount];
    for (int i = 0; i < memoryBlockCount; ++i) {
        memories[i] = simpleMemoryPool.allocateMemory();
        ASSERT_TRUE(memories[i].ptr);
        EXPECT_EQ(memories[i].size, memoryBlockSize);
    }

    auto memory = simpleMemoryPool.allocateMemory();
    ASSERT_FALSE(memory.ptr);
    EXPECT_EQ(memory.size, 0);
}

TEST(SimpleFixedMemoryPoolTest, SuccessfulFreeMemory) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory();
    smp::MemoryBlock mem2 = simpleMemoryPool.allocateMemory();
    smp::MemoryBlock mem3 = simpleMemoryPool.allocateMemory();

    bool res = simpleMemoryPool.freeMemory(&mem2);
    logMemory(&simpleMemoryPool);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 2);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 2);
}

TEST(SimpleFixedMemoryPoolTest, MemoryBlockAfterSuccessfulFreeMemory) {
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory();
    bool res = simpleMemoryPool.freeMemory(&mem);
    EXPECT_FALSE(mem.ptr);
    EXPECT_EQ(mem.size, 0);
}

TEST(SimpleFixedMemoryPoolTest, DoubleFreeMemory) {
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory();
    bool res = simpleMemoryPool.freeMemory(&mem);
    res = simpleMemoryPool.freeMemory(&mem);
    EXPECT_FALSE(res);
}

TEST(SimpleFixedMemoryPoolTest, FreeNullMemory) {
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);

    smp::MemoryBlock mem;
    bool res = simpleMemoryPool.freeMemory(&mem);
    EXPECT_FALSE(res);
}

TEST(SimpleFixedMemoryPoolTest, SuccessfulConstruct) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    Point * p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    ASSERT_TRUE(p);
    EXPECT_FLOAT_EQ(p->x, 12.0f);
    EXPECT_FLOAT_EQ(p->y, 25.0f);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);
}

TEST(SimpleFixedMemoryPoolTest, UnuccessfulExcessiveConstruct) {
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    constexpr size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    Point * points[memoryBlockCount];
    for (int i = 0; i < memoryBlockCount; ++i) {
        points[i] = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
        ASSERT_TRUE(points[i]);
        EXPECT_FLOAT_EQ(points[i]->x, 12.0f);
        EXPECT_FLOAT_EQ(points[i]->y, 25.0f);
        EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize * (i+1) );
        EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - (i+1));
        EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), i+1);
    }
    auto point = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    EXPECT_FALSE(point);
}

TEST(SimpleFixedMemoryPoolTest, UnsuccessfulConstructOnLowSpace) {
    const size_t totalMemorySize = 256;
    const size_t memoryBlockSize = sizeof(Point) -1;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    ASSERT_FALSE(p);
}

TEST(SimpleFixedMemoryPoolTest, SuccessfulDestruct) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    Point * p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    bool res = simpleMemoryPool.destruct(&p);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 0);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 0);
}

TEST(SimpleFixedMemoryPoolTest, UnsuccessfulDestructionOfnullptr) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);

    Point * p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    bool res = simpleMemoryPool.destruct(&p);
    res = simpleMemoryPool.destruct(&p);
    EXPECT_FALSE(res);
}

TEST(SimpleFixedMemoryPoolTest, SuccessfulConstructArrayInOneBlock) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::ArrayBlock points = simpleMemoryPool.constructArray<Point>(10, 0.0f, 0.0f);
    printf("points count : %zd\n", points.count);
    ASSERT_TRUE(points.ptr);
    EXPECT_EQ(points.count, 10);
    for (int i = 0; i < points.count; ++i) {
        points[i].x = i;
        points[i].y = 2 * i;
    }
    for (int i = 0; i < points.count; ++i) {
        EXPECT_EQ(points[i].x, i);
        EXPECT_EQ(points[i].y, 2 * i);
    }
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);
}

TEST(SimpleFixedMemoryPoolTest, SuccessfulDestructArrayInOneBlock) {
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::ArrayBlock points = simpleMemoryPool.constructArray<Point>(10, 0.0f, 0.0f);
    printf("points count : %zd\n", points.count);
    ASSERT_TRUE(points.ptr);

    bool res = simpleMemoryPool.destructArray(&points);
    EXPECT_TRUE(res);
    EXPECT_FALSE(points.ptr);
}


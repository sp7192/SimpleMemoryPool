#include <cstdio>
#include "SimpleFixedMemoryPool.h"
#include "gtest/gtest.h"

namespace smp = SimpleMemoryPool;

void logMemory(const smp::SimpleFixedMemoryPool * mem, int line)
{
    if(mem)
    {
        printf("================\n");
        printf("Line %d : Total Memory size : %zu, usedSize Mem : %zu\n", line, mem->getMemoryTotalSize(), mem->getMemoryUsedSize());
        printf("Line %d : Total chuncks : %zu, usedSize chuncks : %zu\n", line, mem->getMemoryBlocksCount(),
               mem->getMemoryBlocksCount() - mem->getFreeMemoryBlocksCount());
        printf("================\n");
    }
}

//#define LOG_MEMORY(mem) do { logMemory(mem, __LINE__);} while(0)

struct Point
{
    float x;
    float y;

    Point(float _x, float _y) : x(_x), y(_y)
    {
        printf("in Point constructor\n");
    }
    ~Point()
    {
        printf("in Point destructor\n");
    }
};

TEST(SMP_Construct, SuccsessfulConstruction)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;
    EXPECT_EQ(simpleMemoryPool.getMemoryBlockSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryTotalSize(), totalMemorySize);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlocksCount(), memoryBlockCount);
}

TEST(SMP_Construct, ConstructionWithIndivisbleBlocks)
{
    const size_t totalMemorySize = 64;
    const size_t memoryBlockSize = 50;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlocksCount(), 1);
}

TEST(SMP_Construct, ConstructionWithLowerTotalSize)
{
    const size_t totalMemorySize = 32;
    const size_t memoryBlockSize = 50;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryBlockSize(), simpleMemoryPool.getMemoryTotalSize());
}

TEST(SMP_Allocate, SuccessfulAllocateMemory)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory();
    ASSERT_TRUE(mem.ptr);
    EXPECT_EQ(mem.size, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);

    smp::MemoryBlock mem2 = simpleMemoryPool.allocateMemory();
    smp::MemoryBlock mem3 = simpleMemoryPool.allocateMemory();
    ASSERT_TRUE(mem2.ptr);
    EXPECT_EQ(mem2.size, memoryBlockSize);
    ASSERT_TRUE(mem3.ptr);
    EXPECT_EQ(mem3.size, memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 3 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 3);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 3);
}

TEST(SMP_Allocate, SuccessfulAllocateMemoryWithSize)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory(300);
    ASSERT_TRUE(mem.ptr);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 *memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 2);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 2);
    EXPECT_EQ(mem.size, 2 * memoryBlockSize);

    smp::MemoryBlock mem2 = simpleMemoryPool.allocateMemory(10);
    smp::MemoryBlock mem3 = simpleMemoryPool.allocateMemory(1024);
    ASSERT_TRUE(mem2.ptr);
    EXPECT_EQ(mem2.size, memoryBlockSize);
    ASSERT_TRUE(mem3.ptr);
    EXPECT_EQ(mem3.size, 4 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 7 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 7);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 7);
}

TEST(SMP_Allocate, UnsuccessfulAllocateMemory)
{
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    constexpr size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock memories[memoryBlockCount];
    for(int i = 0; i < memoryBlockCount; ++i)
    {
        memories[i] = simpleMemoryPool.allocateMemory();
        ASSERT_TRUE(memories[i].ptr);
        EXPECT_EQ(memories[i].size, memoryBlockSize);
    }

    auto memory = simpleMemoryPool.allocateMemory();
    ASSERT_FALSE(memory.ptr);
    EXPECT_EQ(memory.size, 0);
}

TEST(SMP_Allocate, UnsuccessfulAllocateMemoryWithSize)
{
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory(200);
    ASSERT_TRUE(mem.ptr);
    EXPECT_EQ(mem.size, simpleMemoryPool.getMemoryBlockSize());
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), 3);
    
    smp::MemoryBlock mem2 = simpleMemoryPool.allocateMemory(500);
    ASSERT_TRUE(mem2.ptr);
    EXPECT_EQ(mem2.size, 2 * simpleMemoryPool.getMemoryBlockSize());
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), 1);

    smp::MemoryBlock mem3 = simpleMemoryPool.allocateMemory(256);
    ASSERT_TRUE(mem3.ptr);
    EXPECT_EQ(mem3.size, simpleMemoryPool.getMemoryBlockSize());
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), 0);

    bool res = simpleMemoryPool.freeMemory(&mem);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), 1);

    res = simpleMemoryPool.freeMemory(&mem3);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), 2);

    smp::MemoryBlock mem4 = simpleMemoryPool.allocateMemory(500);
    ASSERT_FALSE(mem4.ptr);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), 2);

    smp::MemoryBlock mem5 = simpleMemoryPool.allocateMemory(150);
    ASSERT_TRUE(mem5.ptr);
    EXPECT_EQ(mem5.size, simpleMemoryPool.getMemoryBlockSize());
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), 1);
    
    res = simpleMemoryPool.freeMemory(&mem2);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), 3);

    smp::MemoryBlock mem6 = simpleMemoryPool.allocateMemory(700);
    ASSERT_TRUE(mem6.ptr);
    EXPECT_EQ(mem6.size, 3 * simpleMemoryPool.getMemoryBlockSize());

}

TEST(SMP_Free, SuccessfulFreeMemory)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory();
    smp::MemoryBlock mem2 = simpleMemoryPool.allocateMemory();
    smp::MemoryBlock mem3 = simpleMemoryPool.allocateMemory();

    bool res = simpleMemoryPool.freeMemory(&mem2);
    EXPECT_TRUE(res);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 2);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 2);
}

TEST(SMP_Free, MemoryBlockAfterSuccessfulFreeMemory)
{
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory();
    bool res = simpleMemoryPool.freeMemory(&mem);
    EXPECT_FALSE(mem.ptr);
    EXPECT_EQ(mem.size, 0);
}

TEST(SMP_Free, DoubleFreeMemory)
{
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);

    smp::MemoryBlock mem = simpleMemoryPool.allocateMemory();
    bool res = simpleMemoryPool.freeMemory(&mem);
    res = simpleMemoryPool.freeMemory(&mem);
    EXPECT_FALSE(res);
}

TEST(SMP_Free, FreeNullMemory)
{
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);

    smp::MemoryBlock mem;
    bool res = simpleMemoryPool.freeMemory(&mem);
    EXPECT_FALSE(res);
}

TEST(SMP_Construct, SuccessfulConstruct)
{
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

TEST(SMP_Construct, UnuccessfulExcessiveConstruct)
{
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    constexpr size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    Point * points[memoryBlockCount];
    for(int i = 0; i < memoryBlockCount; ++i)
    {
        points[i] = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
        ASSERT_TRUE(points[i]);
        EXPECT_FLOAT_EQ(points[i]->x, 12.0f);
        EXPECT_FLOAT_EQ(points[i]->y, 25.0f);
        EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize * (i + 1));
        EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - (i + 1));
        EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), i + 1);
    }
    auto point = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    EXPECT_FALSE(point);
}

TEST(SMP_Construct, SuccessfulConstrucOnMoreBlocks)
{
    const size_t totalMemorySize = 256;
    const size_t memoryBlockSize = sizeof(Point) - 1;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    ASSERT_TRUE(p);
}

TEST(SMP_Destruct, SuccessfulDestruct)
{
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

TEST(SMP_Destruct, UnsuccessfulDestructionOfnullptr)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);

    Point * p = simpleMemoryPool.construct<Point>(12.0f, 25.0f);
    bool res = simpleMemoryPool.destruct(&p);
    res = simpleMemoryPool.destruct(&p);
    EXPECT_FALSE(res);
}

TEST(SMP_ConstructArray, SuccessfulConstructArrayInOneBlock)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 256;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::ArrayBlock points = simpleMemoryPool.constructArray<Point>(10, 0.0f, 0.0f);
    ASSERT_TRUE(points.ptr);
    EXPECT_EQ(points.count, 10);
    for(size_t i = 0; i < points.count; ++i)
    {
        points[i].x = (float)i;
        points[i].y = (float)2 * i;
    }
    for(size_t i = 0; i < points.count; ++i)
    {
        EXPECT_EQ(points[i].x, (float)i);
        EXPECT_EQ(points[i].y, (float)2 * i);
    }
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 1);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 1);
}

TEST(SMP_ConstructArray, SuccessfulConstructArray)
{
    const size_t totalMemorySize = 64;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::ArrayBlock points = simpleMemoryPool.constructArray<Point>(4, 0.0f, 0.0f);
    ASSERT_TRUE(points.ptr);
    EXPECT_EQ(points.count, 4);

    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 2);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 2);
}

TEST(SMP_DestructArray, SuccessfulDestructArrayInOneBlock)
{
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

TEST(SMP_DestructArray, SuccessfulDestructArray)
{
    const size_t totalMemorySize = 64;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    smp::ArrayBlock points = simpleMemoryPool.constructArray<Point>(4, 0.0f, 0.0f);
    ASSERT_TRUE(points.ptr);
    EXPECT_EQ(points.count, 4);

    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 2);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 2);

    bool res = simpleMemoryPool.destructArray(&points);
    EXPECT_TRUE(res);
    EXPECT_FALSE(points.ptr);

    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 0);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 0);

}


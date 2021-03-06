#include <cstdio>
#include "SimpleFixedMemoryPool.h"
#include "SMPString.h"
#include "gtest/gtest.h"
#include <cstring>

namespace smp = SimpleMemoryPool;

#define LOG_MEMORY(MEM) do { printf("in line %d\n", __LINE__);\
                              (MEM).logMemory();\
                            } while(0)

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

    LOG_MEMORY(simpleMemoryPool);
    smp::ArrayBlock points = simpleMemoryPool.constructArray<Point>(4, 0.0f, 0.0f);
    LOG_MEMORY(simpleMemoryPool);
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

    LOG_MEMORY(simpleMemoryPool);
    smp::ArrayBlock points = simpleMemoryPool.constructArray<Point>(4, 0.0f, 0.0f);
    ASSERT_TRUE(points.ptr);
    EXPECT_EQ(points.count, 4);
    LOG_MEMORY(simpleMemoryPool);

    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount - 2);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 2);

    bool res = simpleMemoryPool.destructArray(&points);
    LOG_MEMORY(simpleMemoryPool);

    EXPECT_TRUE(res);
    EXPECT_FALSE(points.ptr);

    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 0);
    EXPECT_EQ(simpleMemoryPool.getFreeMemoryBlocksCount(), memoryBlockCount);
    EXPECT_EQ(simpleMemoryPool.getUsedMemoryBlocksCount(), 0);
}

TEST(SMP_Policy, SUCCESSFUL_ALLOCATE_RANGES_POLICY)
{
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 2, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto mem1 = simpleMemoryPool.allocateMemory(64);
    auto mem2 = simpleMemoryPool.allocateMemory(500);
    auto mem3 = simpleMemoryPool.allocateMemory(64);

    EXPECT_TRUE(mem1.ptr);
    EXPECT_TRUE(mem2.ptr);
    EXPECT_TRUE(mem3.ptr);

    simpleMemoryPool.freeMemory(&mem1);
    simpleMemoryPool.freeMemory(&mem3);

    auto mem4 = simpleMemoryPool.allocateMemory(64);
    auto mem5 = simpleMemoryPool.allocateMemory(64);
    EXPECT_TRUE(mem4.ptr);
    EXPECT_TRUE(mem5.ptr);
}

TEST(SMP_Policy, SUCCESSFUL_ALLOCATE_RANGES_POLICY2)
{
    const size_t memoryBlockSize = 16;
    const size_t totalMemorySize = memoryBlockSize * 1024;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto mem1 = simpleMemoryPool.allocateMemory(250 * memoryBlockSize);
    EXPECT_TRUE(mem1.ptr);
}

TEST(SMP_Policy, SUCCESSFUL_ALLOCATE_CLOSERANGES_POLICY2)
{
    const size_t memoryBlockSize = 16;
    const size_t totalMemorySize = memoryBlockSize * 1024;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto mem1 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_TRUE(mem1.ptr);
    auto mem2 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_TRUE(mem2.ptr);
    auto mem3 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_TRUE(mem3.ptr);
    auto mem4 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_TRUE(mem4.ptr);
    auto mem5 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_FALSE(mem5.ptr);
}

TEST(SMP_Policy, SUCCESSFUL_ALLOCATE_OPENRANGES_POLICY2)
{
    const size_t memoryBlockSize = 16;
    const size_t totalMemorySize = memoryBlockSize * 1024;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::OpenRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto mem1 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_TRUE(mem1.ptr);
    auto mem2 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_TRUE(mem2.ptr);
    auto mem3 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_TRUE(mem3.ptr);
    auto mem4 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_TRUE(mem4.ptr);
    auto mem5 = simpleMemoryPool.allocateMemory(64 * memoryBlockSize);
    EXPECT_TRUE(mem5.ptr);
}

TEST(SMP_Policy, UNSUCCESSFUL_ALLOCATE_RANGES_POLICY)
{
    const size_t totalMemorySize = 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto mem = simpleMemoryPool.allocateMemory(300);
    EXPECT_FALSE(mem.ptr);
    EXPECT_EQ(mem.size, 0);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_DEFAULT_CONSTRUCTION)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool);
    EXPECT_EQ(strcmp(str.getBuffer(), ""), 0);
    EXPECT_EQ(str.getStringSize(), 0);
    EXPECT_EQ(str.getBufferSize(), 32);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_CONSTRUCTOR_WITH_CONST_CHAR_PTR)
{
    const size_t totalMemorySize = 1024*1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    EXPECT_EQ(strcmp(str.getBuffer(), "Sina"), 0);
    EXPECT_EQ(str.getStringSize(), 4);
    EXPECT_EQ(str.getBufferSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_CONSTRUCTOR_WITH_SIZE)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, 5);
    str = "Sina";
    EXPECT_EQ(strcmp(str.getBuffer(), "Sina"), 0);
    EXPECT_EQ(str.getStringSize(), 4);
    EXPECT_EQ(str.getBufferSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
}


TEST(SMP_STRING, SUCCESSFUL_STRING_DESTRUCTOR)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    {
        auto str = smp::SMPString(&simpleMemoryPool, "Sina");
        EXPECT_EQ(strcmp(str.getBuffer(), "Sina"), 0);
        EXPECT_EQ(str.getStringSize(), 4);
        EXPECT_EQ(str.getBufferSize(), memoryBlockSize);
        EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
    }
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 0);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_COPY_CONSTRUCTION)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    smp::SMPString str2 = str;
    EXPECT_EQ(strcmp(str2.getBuffer(), "Sina"), 0);
    EXPECT_EQ(str2.getStringSize(), 4);
    EXPECT_EQ(str2.getBufferSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_COPY_ASSIGNMENT)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    auto str2 = smp::SMPString(&simpleMemoryPool, "Mina");
    str2 = str;
    EXPECT_EQ(strcmp(str2.getBuffer(), "Sina"), 0);
    EXPECT_EQ(str2.getStringSize(), 4);
    EXPECT_EQ(str2.getBufferSize(), memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_MOVE_CONSTRUCTION)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    smp::SMPString str2 = std::move(str);
    EXPECT_EQ(strcmp(str2.getBuffer(), "Sina"), 0);
    EXPECT_EQ(str2.getStringSize(), 4);
    EXPECT_EQ(str2.getBufferSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_MOVE_ASSIGNMENT)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    smp::SMPString str2(&simpleMemoryPool);
    str2 = std::move(str);
    EXPECT_EQ(strcmp(str2.getBuffer(), "Sina"), 0);
    EXPECT_EQ(str2.getStringSize(), 4);
    EXPECT_EQ(str2.getBufferSize(), memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_ASSIGNMENT_WITH_CONST_CHAR_PTR)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    str = "GG";
    EXPECT_EQ(strcmp(str.getBuffer(), "GG"), 0);
    EXPECT_EQ(str.getStringSize(), 2);
    EXPECT_EQ(str.getBufferSize(), memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_ASSIGNMENT_WITH_CONST_CHAR_PTR2)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    str = "Sina-Sina-Sina-Sina-";
    EXPECT_EQ(strcmp(str.getBuffer(), "Sina-Sina-Sina-Sina-"), 0);
    EXPECT_EQ(str.getStringSize(), 20);
    EXPECT_EQ(str.getBufferSize(), 2 * memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_INDEX_OPERATOR)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    EXPECT_EQ(str[0], 'S');
    str[0] = 'M';
    EXPECT_EQ(strcmp(str.getBuffer(), "Mina"), 0);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_EQUAL_OPERATOR)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    EXPECT_TRUE(str == "Sina");
    auto str2 = str;
    EXPECT_TRUE(str == str2);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_PLUS_OPERATOR)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    auto str2 = smp::SMPString(&simpleMemoryPool, "GG");
    auto str3 = str + str2;
    EXPECT_EQ(strcmp(str3.getBuffer(), "SinaGG"), 0);
    EXPECT_EQ(str3.getStringSize(), 6);
    EXPECT_EQ(str3.getBufferSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 3 * memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_PLUS_OPERATOR2)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    auto str2 = str + "GG";
    EXPECT_EQ(strcmp(str2.getBuffer(), "SinaGG"), 0);
    EXPECT_EQ(str2.getStringSize(), 6);
    EXPECT_EQ(str2.getBufferSize(), memoryBlockSize);
    EXPECT_EQ(simpleMemoryPool.getMemoryUsedSize(), 2 * memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_INCREMENT_OPERATOR)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    str += "GG";
    EXPECT_EQ(strcmp(str.getBuffer(), "SinaGG"), 0);
    EXPECT_EQ(str.getStringSize(), 6);
    EXPECT_EQ(str.getBufferSize(), memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_INCREMENT_OPERATOR2)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str = smp::SMPString(&simpleMemoryPool, "Sina");
    str += "Sina-Sina-Sina-Sina-";
    EXPECT_EQ(strcmp(str.getBuffer(), "SinaSina-Sina-Sina-Sina-"), 0);
    EXPECT_EQ(str.getStringSize(), 24);
    EXPECT_EQ(str.getBufferSize(), 2 * memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_INCREMENT_OPERATOR3)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str  = smp::SMPString(&simpleMemoryPool, "Sina");
    auto str2 = smp::SMPString(&simpleMemoryPool, "GG");
    str += str2;
    EXPECT_EQ(strcmp(str.getBuffer(), "SinaGG"), 0);
    EXPECT_EQ(str.getStringSize(), 6);
    EXPECT_EQ(str.getBufferSize(), memoryBlockSize);
}

TEST(SMP_STRING, SUCCESSFUL_STRING_INCREMENT_OPERATOR4)
{
    const size_t totalMemorySize = 1024 * 1024;
    const size_t memoryBlockSize = 16;
    smp::SimpleFixedMemoryPool simpleMemoryPool(totalMemorySize, memoryBlockSize, 4, smp::MemoryDistributionPolicy::CloseRanges);
    size_t memoryBlockCount = totalMemorySize / memoryBlockSize;

    auto str  = smp::SMPString(&simpleMemoryPool, "Sina");
    auto str2 = smp::SMPString(&simpleMemoryPool, "Sina-Sina-Sina-Sina-");
    str += str2;
    EXPECT_EQ(strcmp(str.getBuffer(), "SinaSina-Sina-Sina-Sina-"), 0);
    EXPECT_EQ(str.getStringSize(), 24);
    EXPECT_EQ(str.getBufferSize(), 2 * memoryBlockSize);
}
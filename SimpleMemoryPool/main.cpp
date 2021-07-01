#include "SimpleMemoryPool.h"

class Point {
    int m_x;
    int m_y;
public:
    Point(int x, int y) : m_x(x), m_y(y) {
        printf("in Point constructor\n");
    }

    ~Point() {
        printf("in point destructor\n");
    }

    void log() const {
        printf("Point : (%d,%d)\n", m_x, m_y);
    }
};

int main() {
    SimpleMemoryPool sMemPool(ALLOCATOR_SIZE, ALLOCATOR_CHUNCK_SIZE);
    sMemPool.init();
    sMemPool.logMem();

    void* ptr = sMemPool.allocateMem();
    sMemPool.logMem();

    void* ptr2 = sMemPool.allocateMem();
    void* ptr3 = sMemPool.allocateMem();
    sMemPool.logMem();

    sMemPool.freeMem(ptr2);
    sMemPool.logMem();

    Point* p = sMemPool.construct<Point>(12, 25);
    p->log();
    sMemPool.logMem();
    sMemPool.destruct(p);
    sMemPool.logMem();

    auto leakedMem = sMemPool.getUsingMem();
    for (const auto& m : leakedMem) {
        printf("leaked mem address : %lu\n", (unsigned long)m);
    }

	return 0;
}

#ifndef LRUCACHE_H
#define LRUCACHE_H

#include "./ICacheable.hpp"

class LRUCache : public ICacheable {
public:
    LRUCache();
    
    void get() override;

    void put() override;

    ~LRUCache();
private:
};

#endif
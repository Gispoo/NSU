#ifndef LFUCACHE_H
#define LFUCACHE_H

#include "./ICacheable.hpp"

class LFUCache : public ICacheable {
private:
    
public:
    LFUCache(); 

    void get() override;

    void put() override;

    ~LFUCache();
};

#endif
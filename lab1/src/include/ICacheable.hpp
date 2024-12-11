#ifndef ICACHEABLE_H
#define ICACHEABLE_H

class ICacheable {
public:
    virtual void get() = 0;

    virtual void put() = 0;
    
};

#endif
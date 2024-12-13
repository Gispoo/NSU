#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <map>
#include <list>
#include "./ICacheable.hpp"

template <typename K, typename V>
class LRUCache : public ICacheable<K, V> {
public:
  LRUCache(size_t capacity) : capacity(capacity) {}

  V get(const K& key) override;
  void put(const K& key, const V& value) override;
  V operator[](const K& key) override;

private:
  std::list<K> list;
  std::map<K, std::list<K>::iterator> map;
  size_t capacity;
};

#endif
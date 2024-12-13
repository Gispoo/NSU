#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <map>
#include <list>
#include "./ICache.hpp"

template <typename K, typename V>
class LRUCache : public ICache<K, V> {
public:
  LRUCache(size_t capacity) : capacity(capacity) {}

  V get(const K& key) override;
  void put(const K& key, const V& value) override;
  V operator[](const K& key) override;

  ~LRUCache() {} = default;
private:
  std::list<K> list;
  std::map<K, typename std::list<std::pair<K, V>>::iterator> map;
  size_t capacity;
};

#endif
#ifndef LFUCACHE_H
#define LFUCACHE_H

#include <map>
#include <list>
#include "./ICache.hpp"

template <typename K, typename V>
class LFUCache : public ICache<K, V> {
public:
  LFUCache(size_t capacity) : capacity(capacity) {}

  V get(const K& key) override;
  void put(const K& key, const V& value) override;
  V operator[](const K& key) override;

  ~LRUCache() {} = default;
private:
  std::map<K, V> values;
  std::map<K, int> freq;
  std::map<int, std::list<K>> freqList;
  size_t capacity;
};

#endif
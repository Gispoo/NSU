#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <map>
#include <unordered_map>
#include <list>
#include <iostream>
#include "./ICache.hpp"
#include "../../exception/CacheException.hpp"

template <typename K, typename V>
class LRUCache : public ICache<K, V> {
public:
  LRUCache(size_t capacity) : capacity(capacity) {
    if (capacity < 1) throw SmallSizeCacheException("Cache capacity must be at least 1");
  }

  V get(const K& key) override;
  void put(const K& key, const V& value) override;
  V operator[](const K& key) override;
  void print_cache() override;
  
private:
  std::list<std::pair<K, V>> list_K_V;
  std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map_K_I;
  size_t capacity;
};

template <typename K, typename V>
V LRUCache<K, V>::get(const K& key) {
  auto it = map_K_I.find(key);
  if (it == map_K_I.end()) {
    throw KeyNotFoundException("Key not found");
  }
  V value = it->second->second;
  list_K_V.erase(it->second);
  list_K_V.push_front({key, value});
  map_K_I[key] = list_K_V.begin();
  return value;
}

template <typename K, typename V>
void LRUCache<K, V>::put(const K& key, const V& value) {
  auto it = map_K_I.find(key);
  if (it != map_K_I.end()) {
    list_K_V.erase(it->second);
  } else if (list_K_V.size() == capacity) {
    map_K_I.erase(list_K_V.back().first);
    list_K_V.pop_back();
  }
  list_K_V.push_front({key, value});
  map_K_I[key] = list_K_V.begin();
}

template <typename K, typename V>
V LRUCache<K, V>::operator[](const K& key) {
  return get(key);
}

template <typename K, typename V>
void LRUCache<K, V>::print_cache() {
  std::cout << "LRU Cache contents:\n";
  if (list_K_V.empty()) {
    std::cout << "Cache is empty.\n";
    return;
  }

  for (const auto& pair : list_K_V) {
    std::cout << "Key: " << pair.first << ", Value: " << pair.second << "\n";
  }
}

#endif
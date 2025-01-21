#ifndef LFUCACHE_H
#define LFUCACHE_H

#include <map>
#include <unordered_map>
#include <list>
#include <iostream>
#include "./ICache.hpp"
#include "../../exception/CacheException.hpp"

template <typename K, typename V>
class LFUCache : public ICache<K, V> {
public:
  LFUCache(size_t capacity) : capacity(capacity) {
    if (capacity < 1) throw SmallSizeCacheException("Cache capacity must be at least 1");
  }

  V get(const K& key) override;
  void put(const K& key, const V& value) override;
  V operator[](const K& key) override;
  void print_cache() override;
  
  size_t size() const { return map_K_V.size(); }
  int getFrequency(const K& key) const { return map_K_freq.count(key) ? map_K_freq.at(key) : 0; }

private:
  std::unordered_map<K, V> map_K_V;
  std::unordered_map<K, int> map_K_freq;
  std::map<int, std::list<K>> map_freq_listK;
  size_t capacity;
};

template <typename K, typename V>
V LFUCache<K, V>::get(const K& key) {
  auto it_val = map_K_V.find(key);
  if (it_val == map_K_V.end()) {
    throw KeyNotFoundException("Key not found");
  }

  int freq = map_K_freq[key];
  map_freq_listK[freq].remove(key);
  if (map_freq_listK[freq].empty()){
    map_freq_listK.erase(freq);
  }

  map_K_freq[key]++;
  map_freq_listK[map_K_freq[key]].push_front(key);
  return it_val->second;
}

template <typename K, typename V>
void LFUCache<K, V>::put(const K& key, const V& value) {
  if (map_K_V.count(key)) {
      map_K_V[key] = value;
      get(key);
      return;
  }

  if (map_K_V.size() == capacity) {
    int min_freq = map_freq_listK.begin()->first;
    K key_to_remove = map_freq_listK[min_freq].back();
    map_freq_listK[min_freq].pop_back();
    if (map_freq_listK[min_freq].empty()) {
      map_freq_listK.erase(min_freq);
    }
    map_K_V.erase(key_to_remove);
    map_K_freq.erase(key_to_remove);
  }

  map_K_V[key] = value;
  map_K_freq[key] = 1;
  map_freq_listK[1].push_front(key);
}

template <typename K, typename V>
V LFUCache<K, V>::operator[](const K& key) {
  return get(key);
}

template <typename K, typename V>
void LFUCache<K, V>::print_cache() {
  std::cout << "LFU Cache contents:\n";
  if (map_K_V.empty()) {
    std::cout << "Cache is empty.\n";
    return;
  }

  for (const auto& pair : map_K_V) {
    std::cout << "Key: " << pair.first << ", Value: " << pair.second
              << ", Frequency: " << map_K_freq.at(pair.first) << "\n";
  }
}

#endif
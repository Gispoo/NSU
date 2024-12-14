#ifndef ICACHEABLE_H
#define ICACHEABLE_H

template <typename K, typename V>
class ICache {
public:
  virtual V get(const K& key) = 0;
  virtual void put(const K& key, const V& value) = 0;
  virtual V operator[](const K& key) = 0;
  virtual ~ICache() {}
};

#endif
#include <gtest/gtest.h>
#include "../../src/include/LRUCache.hpp"
#include "../../exception/CacheE.hpp"

TEST(LRUCacheTest, BasicTest) {
  LRUCache<int, int> cache(2);
  cache.put(1, 10);
  cache.put(2, 20);
  EXPECT_EQ(cache.get(1), 10);
  EXPECT_EQ(cache.get(2), 20);
}

TEST(LRUCacheTest, CapacityExceeded) {
  LRUCache<int, int> cache(2);
  cache.put(1, 10);
  cache.put(2, 20);
  cache.put(3, 30);
  EXPECT_EQ(cache.get(2), 20);
  EXPECT_THROW(cache.get(1), KeyNotFoundException);
}

TEST(LRUCacheTest, ReplaceExistingElement) {
  LRUCache<int, int> cache(2);
  cache.put(1, 10);
  cache.put(2, 20);
  cache.put(2, 25);
  EXPECT_EQ(cache.get(2), 25);
}

TEST(LRUCacheTest, GetThrowsException) {
  LRUCache<int, int> cache(1);
  cache.put(1, 10);
  EXPECT_EQ(cache.get(1), 10);
  EXPECT_THROW(cache.get(2), KeyNotFoundException);
}

TEST(LRUCacheTest, OperatorBracketThrowsException) {
  LRUCache<int, int> cache(1);
  cache.put(1, 10);
  EXPECT_EQ(cache.get(1), 10);
  EXPECT_THROW(cache[2], KeyNotFoundException);
}

TEST(LRUCacheTest, EmptyCache) {
  LRUCache<int, int> cache(2);
  EXPECT_THROW(cache.get(1), KeyNotFoundException);
  EXPECT_THROW(cache[1], KeyNotFoundException);
}

TEST(LRUCacheTest, StringKeysAndValues) {
    LRUCache<std::string, std::string> cache(2);
    cache.put("key1", "value1");
    cache.put("key2", "value2");
    EXPECT_EQ(cache.get("key1"), "value1");
    EXPECT_EQ(cache.get("key2"), "value2");
}
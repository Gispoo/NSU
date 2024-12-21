#include <gtest/gtest.h>
#include "../../src/include/LFUCache.hpp"
#include "../../exception/CacheE.hpp"

TEST(LFUCacheTest, Get_KeyNotFound) {
  LFUCache<int, int> cache(2);
  ASSERT_THROW(cache.get(1), KeyNotFoundException);
}

TEST(LFUCacheTest, Get_KeyFound1) {
  LFUCache<int, int> cache(2);
  cache.put(1, 10);
  ASSERT_EQ(cache.get(1), 10);
}

TEST(LFUCacheTest, Get_KeyFound2) {
  LFUCache<int, int> cache(2);
  cache.put(1, 10);
  cache.get(1);
  cache.put(2, 20);
  ASSERT_EQ(cache.get(1), 10);
  ASSERT_EQ(cache.get(2), 20);
}

TEST(LFUCacheTest, Put_WithinCapacity) {
  LFUCache<int, int> cache(2);
  cache.put(1, 10);
  cache.put(2, 20);
  ASSERT_EQ(cache.size(), 2);
  ASSERT_EQ(cache[1], 10);
  ASSERT_EQ(cache[2], 20);
}

TEST(LFUCacheTest, Put_BeyondCapacity) {
  LFUCache<int, int> cache(2);
  cache.put(1, 10);
  cache.put(2, 20);
  cache.put(3, 30);
  ASSERT_EQ(cache.size(), 2);
  ASSERT_THROW(cache[1], KeyNotFoundException);
  ASSERT_EQ(cache[2], 20);
  ASSERT_EQ(cache[3], 30);
}

TEST(LFUCacheTest, Put_UpdateExisting) {
    LFUCache<int, int> cache(2);
    cache.put(1, 10);
    cache.put(1, 100);
    ASSERT_EQ(cache.size(), 1);
    ASSERT_EQ(cache[1], 100);
}

TEST(LFUCacheTest, OperatorBracket_KeyNotFound) {
    LFUCache<int, int> cache(2);
    ASSERT_THROW(cache[1], KeyNotFoundException);
}

TEST(LFUCacheTest, OperatorBracket_KeyFound) {
    LFUCache<int, int> cache(2);
    cache.put(1, 10);
    ASSERT_EQ(cache[1], 10);
}

TEST(LFUCacheTest,ComplexScenario){
    LFUCache<int, int> cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    cache.get(1);
    cache.put(4, 4);
    ASSERT_EQ(cache.size(),3);
    ASSERT_EQ(cache[1], 1);
    ASSERT_THROW(cache[2], KeyNotFoundException);
    ASSERT_EQ(cache[3], 3);
    ASSERT_EQ(cache[4], 4);
}

TEST(LFUCacheTest,SizeMethod){
    LFUCache<int,int> cache(2);
    cache.put(1,10);
    cache.put(2,20);
    ASSERT_EQ(cache.size(),2);
}

TEST(LFUCacheTest,FrequencyMethod){
    LFUCache<int,int> cache(2);
    cache.put(1,10);
    cache.get(1);
    ASSERT_EQ(cache.getFrequency(1),2);
    ASSERT_EQ(cache.getFrequency(2),0);
}
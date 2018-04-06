/*
 * extendible_hash.h : implementation of in-memory hash table using extendible
 * hashing
 *
 * Functionality: The buffer pool manager must maintain a page table to be able
 * to quickly map a PageId to its corresponding memory location; or alternately
 * report that the PageId does not match any currently-buffered page.
 */

#pragma once

#include <cstdlib>
#include <vector>
#include <string>
#include <mutex> 
#include <map>
#include <memory>
#include <thread> 
#include <chrono> 

#include "hash/hash_table.h"

namespace cmudb {

template <typename K, typename V>
class ExtendibleHash : public HashTable<K, V> {
public:
  // constructor
  struct Bucket{
    int id, ld ;                      // 分别是bucket的ID和local global 
    std::map<K, V> m ;                // Bucket中的元素  
    std::shared_ptr<struct Bucket> next ;          // 同一个组中桶（就是overflow了之后，新增的桶） 
    Bucket(int i, int d): id(i), ld(d) {}
  }; 

  ExtendibleHash(size_t size);
  // helper function to generate hash addressing
  size_t HashKey(const K &key);
  // helper function to get global & local depth
  int GetGlobalDepth() const;
  int GetLocalDepth(int bucket_id) const;
  int GetNumBuckets() const;
  // lookup and modifier
  bool Find(const K &key, V &value) override;
  bool Remove(const K &key) override;
  void Insert(const K &key, const V &value) override;
  // 用来查找bucket的index 
  int BucketIndex(const K &key) ;  

private:
  // add your own member variables here
  mutable std::mutex bucket_mutex ;                        // 保护共享数据，同时只有一个读写 
  size_t bucket_size_ ;                    // 每个bucket中的最大数量 
  int gd ;                                          // global depth 
  std::vector<std::shared_ptr<Bucket>> bs ;                          // 桶的数组 
  int bucket_counts ;                               // bucket 的数量 
};
} // namespace cmudb

#include <list>
#include <iostream>
#include <cstdio> 

#include "hash/extendible_hash.h"
#include "page/page.h"

namespace cmudb {

/*
 * constructor
 * array_size: fixed array size for each bucket
 */
template <typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash(size_t size) {
    bucket_size_ = size ; 
    gd = 0 ; 
    bucket_counts = 1 ;
    bs.emplace_back(new Bucket(0,0)) ;
}

/*
 * helper function to calculate the hashing address of input key
 * 对key进行hash，std::hash里面提供了所有基本数据类型的hash方法
 * 这个得到的hash值就是bucket的index
 */
template <typename K, typename V>
size_t ExtendibleHash<K, V>::HashKey(const K &key) {
    return std::hash<K>()(key) ; 
}

/*
 * helper function to return global depth of hash table
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetGlobalDepth() const {
  std::unique_lock<std::mutex> lock(bucket_mutex) ; 
  return gd ;
}

/*
 * helper function to return local depth of one specific bucket
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const {
  std::unique_lock<std::mutex> lock(bucket_mutex) ; 
  if ( bucket_id < 0 || bucket_id >= bs.size() || bs[bucket_id] == NULL ) 
      return -1 ; 
  return bs[bucket_id]->ld ; 
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const {
  std::unique_lock<std::mutex> lock(bucket_mutex) ; 
  return bucket_counts ;
}

/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {
  std::unique_lock<std::mutex> lock(bucket_mutex) ; 
  size_t h = HashKey(key) ; 
  if ( bs[h] == NULL ) 
    return false ;  
  auto b = bs[h] ; 
  do {
    if ( b->m.count(key) != 0 ) {
      value = b->m[key] ; 
      return true ; 
    }
    b = b->next ;  
  } while ( b ) ; 
  return false ; 
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
  std::unique_lock<std::mutex> lock(bucket_mutex) ;  
  size_t h = HashKey(key) ;
  bool flag = false ; 
  if ( bs[h] == NULL ) 
    return false ; 
  auto b = bs[h] ;
  do {
    if ( b->m.count(key) != 0 ) {
      b->m.erase(key) ; 
      flag = true ; 
    }
    b = b->next ; 
  } while ( b ) ; 
  return flag ;
}

/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {
  std::unique_lock<std::mutex> lock(bucket_mutex) ;    
  size_t h = HashKey(key) ; 
  if ( bs[h] == NULL ) {
    bs[h] = std::make_shared<Bucket>(h,1) ; 
    bucket_counts++ ; 
  }
  auto b = bs[h] ; 
  // 已存在，修改value
  if ( b->m.count(key) != 0 ) {
    b->m[key] = value ; 
    return ; 
  }
  
  b->m[key] = value ; 
  if ( b->m.size() > bucket_size_ ) {
    ; 
  } 
}

/*
 * 由hash值生成应存入的bucket的index 
 * 返回的值是 hashkey(key)的右边 gd位中1的个数  
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::BucketIndex(const K &key) {
  return HashKey(key) & (( 1 << gd ) - 1 ) ; 
} 

template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace cmudb

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
    gd = 0 ;                            // 初始化  gd 为0
    bucket_counts = 1 ;                // 刚开始只有一个桶, 桶的数量：有元素的桶的数量 
    bs.push_back(new Bucket(0,0)) ;
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
 * NOTE: you must implement this function in order to pass test --> 检查不符合的index和不存在的index
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
  int h = BucketIndex(key) ;
  if ( bs[h] == NULL )
    return false ;
  auto b = bs[h] ;
  if ( b->m.count(key) != 0 ) {
    value = b->m[key] ;
    return true ;
  }
  return false ;
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
  std::unique_lock<std::mutex> lock(bucket_mutex) ;
  int h = BucketIndex(key) ;
  if ( bs[h] == NULL )
    return false ;
  auto b = bs[h] ;
  if ( b->m.count(key) != 0 ) {
    b->m.erase(key) ;
    return true ; 
  }
  return false ;
}


 
/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {
  std::unique_lock<std::mutex> lock(bucket_mutex) ;
  int h = BucketIndex(key) ;
  if ( bs[h] == NULL ) {                                    
    bs[h] = new Bucket(h,gd) ;
    bucket_counts++ ;
  }
  auto b = bs[h] ;
  // 已存在，修改value
  if ( b->m.find(key) != b->m.end() ) {
    b->m[key] = value ;
    return ;
  }

  b->m.insert({key,value}) ; 
  if ( b->m.size() <= bucket_size_ ) {          // 如果bucket 没满，直接插入后返回  
    return ; 
  } 
                                                  // 接下来的情况都是bucket满了的情况 
  int oldsize = bs.size() ; 

  struct Bucket *newb = new Bucket(0,b->ld) ;       // 127-146 : split桶  
  while ( newb->m.empty() ) {
    newb->ld++ ; 
    b->ld++ ; 
    for ( auto it = b->m.begin() ; it != b->m.end() ; ) {
      if ( HashKey(it->first) & (1 << (newb->ld-1)) ) {              // 判断第ld位是否为1  
        newb->id = HashKey(it->first) & ( (1<< newb->ld) -1 ) ; 
        newb->m.insert(*it) ; 
        it = b->m.erase(it) ; 
      }
      else {
        it++ ; 
      }
    }
    if ( b->m.empty() ) {                            // 如果全部分配到一个桶里面，说明还需要重新分配
      b->m.swap(newb->m) ;                           // 此时，把新桶的ID给旧桶，在下一次循环中，新桶会被分配新的ID， 
      b->id = newb->id ; 
    }
  }  
  bucket_counts++ ; 


  if ( newb->ld > gd ) {                         //  分配之后，localdepth大于globaldepth，要给bs增加容量  
    bs.resize(oldsize * (1 << (newb->ld - gd))) ;          //  bs 的size需要增大的倍数 
    gd = newb->ld ; 
    for ( int i = 0 ; i < oldsize ; i++ ) {
      if ( bs[i] == NULL ) 
        continue ; 
      for ( int j = i ; j < bs.size() ; j+= ( 1 << bs[i]->ld )) {    // 设置没有拓展的bucket和原bucket一样 
        bs[j] = bs[i] ; 
      } 
    }
    bs[b->id] = b ; 
    bs[newb->id] = newb ; 
  }
  else {                                        // split之后，localdepth没超出globaldepth 
    bs[b->id] = b ; 
    bs[newb->id] = newb ; 
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

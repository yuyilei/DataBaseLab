/**
 * LRU implementation
 */
#include "buffer/lru_replacer.h"
#include "page/page.h"

namespace cmudb {

template <typename T> LRUReplacer<T>::LRUReplacer() {
   node_size = 0 ;
}

template <typename T> LRUReplacer<T>::~LRUReplacer() {}

/*
 * Insert value into LRU
 */
template <typename T> void LRUReplacer<T>::Insert(const T &value) {
   std::unique_lock<std::mutex> lock(node_mutex) ;
   for ( auto it = ns.begin() ; it != ns.end() ; ) {                 // 把原本存在的value 删除
    if ( *it == value ) {
        it = ns.erase(it) ;
        node_size-- ;
    }
    else
        it++ ;
   }
   ns.insert(ns.begin(),value) ;                                    // 在开头插入value
   node_size++ ;
}

/* If LRU is non-empty, pop the head member from LRU to argument "value", and
 * return true. If LRU is empty, return false
 */
template <typename T> bool LRUReplacer<T>::Victim(T &value) {
  std::unique_lock<std::mutex> lock(node_mutex) ;
  if ( node_size == 0 ) {
    return false ;
  }
  value = ns[node_size-1] ;
  ns.erase(ns.begin()+node_size-1) ;
  node_size-- ;
  return true ;
}

/*
 * Remove value from LRU. If removal is successful, return true, otherwise
 * return false
 */
template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
  std::unique_lock<std::mutex> lock(node_mutex) ;
  bool res = false ;
  for ( auto it = ns.begin() ; it != ns.end() ; ) {
      if ( *it == value ) {
        it = ns.erase(it) ;
        res = true ;
        node_size-- ;
        break ;
      }
      else
        it++ ;
  }
  return res ;
}

template <typename T> size_t LRUReplacer<T>::Size() {
  std::unique_lock<std::mutex> lock(node_mutex) ;
  return node_size ;
}

template class LRUReplacer<Page *>;
// test only
template class LRUReplacer<int>;

} // namespace cmudb

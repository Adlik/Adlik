#ifndef HEAAB628E_761B_4552_BEFF_EE3AAA1F9A18
#define HEAAB628E_761B_4552_BEFF_EE3AAA1F9A18

#include "cub/list/apr_ring.h"

namespace cub {

template <typename T>
struct List;

template <typename T>
struct ListElem {
  ListElem() {
    link.prev = 0;
    link.next = 0;
  }

  void remove() {
    APR_RING_REMOVE(this, link);
  }

  T* getNext() {
    return link.next;
  }

  const T* getNext() const {
    return link.next;
  }

  T* getPrev() {
    return link.prev;
  }

  const T* getPrev() const {
    return link.prev;
  }

  friend struct List<T>;

  APR_RING_ENTRY(T)
  link;  // __cacheline_aligned;
};

/////////////////////////////////////////////////////////////////

#define DEFINE_LIST_BASED_INTERFACE(INTF) struct INTF : ListElem<INTF>, cub::details::Interface

}  // namespace cub

#endif

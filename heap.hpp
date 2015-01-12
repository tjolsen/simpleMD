#ifndef _HEAP_HPP
#define _HEAP_HPP

#include <vector>
#include <iostream>

template<typename T>
class Heap {
  
private:

  void BubbleUp(unsigned node);
  void BubbleDown(unsigned node);
  inline unsigned childL(unsigned node) {return node*2 + 1;}
  inline unsigned childR(unsigned node) {return node*2 + 2;}
  inline unsigned parent(unsigned node) {return (node-1)/2;}
  
public:
  std::vector<T> array;

  Heap();
  void insert(T item);
  T peek();
  T extract();
  void print();
  T deleteNode(unsigned node);
};

#include "heap_implementation.hpp"

#endif

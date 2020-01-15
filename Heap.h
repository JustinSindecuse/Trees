#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <exception>

using namespace std;

template <typename T>
class MinHeap {
    public:
        MinHeap (int x);
        /* Constructor that builds a d-ary Min Heap
          This should work for any d >= 2,
          but doesn't have to do anything for smaller d.*/

        ~MinHeap ();

        int add (T item, int priority);
         /* adds the item to the heap, with the given priority. 
            multiple identical items can be in the heap simultaneously. 
            Returns the number of times add has been called prior to this
            call (for use with the update function).*/

        const T & peek () const;
         /* returns the element with smallest priority.  
            If two elements have the same priority, use operator< on the 
            T data, and return the one with smaller data.*/

        void remove ();
         /* removes the element with smallest priority, with the same tie-breaker
            as peek. */

        void update (int nth, int priority);
         /* finds the nth item (where nth is 0-based) that has ever been added 
            to the heap (the node that was created on the nth call to add), 
            and updates its priority accordingly. */

        bool isEmpty ();
         /* returns true iff there are no elements on the heap. */

        void trickleup(int pos);

        void trickledown(int pos);

   private:
        // whatever you need to naturally store things.
        // You may also add helper functions here.
        struct data{
            int priority;
            int index;
            int id;
            T value;
        };

        void swap(int pos1, int pos2);

        std::vector<data> heap;
        std::vector<data> id_holder;
        int size;
        int d;
        int count;
};

template <typename T>
MinHeap<T>::MinHeap(int x){
    this->d = x;
    size = 0;
    count = 0;
}

template <typename T>
MinHeap<T>::~MinHeap(){

}

template <typename T>
int MinHeap<T>::add(T item, int priority){

    data next;
    next.value = item;
    next.priority = priority;
    next.id = count++;
    size++;
    heap.push_back(next);
    id_holder.push_back(next);
    trickleup(size-1);
    return count-1;
}

template <typename T>
const T & MinHeap<T>::peek() const{
    if(size == 0){
        throw std::out_of_range("Index is out of range");
    }
    else{
        return heap[0].value;
    }
}

template <typename T>
void MinHeap<T>::remove(){
    if(size == 0){
        throw std::out_of_range("Index is out of range");
    }
    else{
        heap[0] = heap[size-1];
        heap[0].index = 0;
        id_holder[heap[0].id].index = 0;
        heap.pop_back();
        trickledown(0);  
        size--;  
    }
}

template <typename T>
void MinHeap<T>::update(int n, int priority){
    int x = id_holder[n].priority;
    if(x < priority){
        id_holder[n].priority = priority;
        heap[id_holder[n].index].priority = priority;
        trickledown(id_holder[n].index);
    }
    else{
        id_holder[n].priority = priority;
        heap[id_holder[n].index].priority = priority;
        trickleup(id_holder[n].index);
    }
}

template <typename T>
bool MinHeap<T>::isEmpty(){
    if(size>0)
        return false;
    else
        return true;
}

template <typename T>
void MinHeap<T>::trickleup(int pos){
    int parent = (pos-1)/d;
    
    if(heap[pos].priority < heap[parent].priority){
        swap(parent, pos);
        trickleup(parent);
    }
    else if(heap[pos].priority == heap[parent].priority){
        if(heap[pos].value < heap[parent].value){
            swap(parent, pos);
            trickleup(parent);
        }
    }
}

template <typename T>
void MinHeap<T>::trickledown(int pos){
    if(pos >= size-1){
        return;
    }
    if(pos * d + 1 >= size - 1){
        return;
    }

    int initial = d*pos+1;
    int min;
    for(int i = 1; i <= d; i++){
        if(pos * d + i < size){
            min = pos * d + i;
            if(heap[initial].priority > heap[min].priority)
                initial = min;
            else if(heap[initial].priority == heap[min].priority){
                if(heap[initial].value > heap[min].value)
                    initial = min;
            }
        }
    }

    if(initial == pos)
        return;
    else{
        swap(pos, initial);
        trickledown(initial);
    }
}

template <typename T>
void MinHeap<T>::swap(int pos1, int pos2){
    data swap_temp = heap[pos1];
    heap[pos1] = heap[pos2];
    heap[pos2] = swap_temp;

    heap[pos1].index = pos1;
    heap[pos2].index = pos2;

    id_holder[heap[pos1].id].index = pos1;
    id_holder[heap[pos2].id].index = pos2;
}

#include <bits/stdc++.h>
using namespace std;

typedef double T;

struct Queue
{
    T *array;           // for runtime resizing
    int capacity = 2;   // for table doubling
    int back, front;    // indirection markers
    int size_ = 0;

    Queue()
    {
    	this->array = new T[this->capacity];         // dynamic array in heap
	    this->front = this->back = -1;          // initialize indirection markers
	    size_ = 0;
    };
    ~Queue(){ delete [] array; }         
 
    void enqueue(T data)
    {
	    if (this->is_empty() && capacity == 0 && array == nullptr)
	    {
	        capacity = 1;
	        this->resize(capacity);
	    }
	    if (this->is_full())       // if array is completely full
	        this->resize(capacity * 2);
	    
	    back = next(back);         // go to the next index
	    array[back] = data;        // fill index with new element
	    size_++;                   // adjust size_ attribute

	    if (size_ == 1 && front < 0)
	        front = back;          // special case
	}

    T dequeue()
    {
	    if (array == nullptr || this->is_empty())
	    {
	        cerr << "dequeue() method doesn't work because structure is empty\n";
	        return T{};
	    }
	    T rt_value = array[front];  // obtain value of front
	    front = next(front);        // go to the next index
	    size_--;                    // adjust size_ member attribute

	    // estrategia para liberar espacios inutilizados
	    if (size_ < (capacity/2))
	        this->resize(capacity/2);
	    return rt_value;
	}                 
    
    bool is_full(){	return size_ == capacity; }
    bool is_empty(){ return (size_ == 0); }
                  
    int size(){ return size_; }
    void resize(int new_capacity)
    {
	    T* new_array = new T[new_capacity];
	    int traverse, i, aux_sz;
	    
	    // initialize "pointers" and accountant
	    traverse = front, i = traverse % new_capacity, aux_sz = 0;
	    this->front = i;    // reassign front indirection marker

	    // copy numbers in new array
	    while (aux_sz < size_)
	    {
	        new_array[i] = array[traverse];
	        i = (i + 1) % new_capacity;
	        traverse = next(traverse);
	        aux_sz++;
	    }
	    delete[] array;     // release memory

	    // reassing member attributes
	    this->capacity = new_capacity;
	    this->back = prev(i);
	    this->array = new_array;
	}

    int next(int index){ return (index + 1) % capacity; }
    int prev(int index){ return (index <= 0) ? capacity - 1 : index - 1; }
};

int main()
{
#ifndef TEST
  // freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
#endif
  	// int(1e5), 
  	vector<int> vec = {int(1e3), 1500, 500, 250, 200, 150, 90, 50, 20, 10};
  	for (const auto& v : vec)
  	{
  		cout << '\t' << v << '\n';
  		Queue q;
  		for (int i = v; i >= 0; --i)
  			q.enqueue(i);
  		while (!q.is_empty())
  		{
  			cout << q.dequeue() << " ";
  		}
  		cout << '\n';
  		break;
  	}
}
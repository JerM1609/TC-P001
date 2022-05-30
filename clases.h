#ifndef TC_CLASES_H
#define TC_CLASES_H

#include <iostream>
#include <string>
#include <chrono>
using namespace std;

using time_point_t = chrono::time_point<chrono::high_resolution_clock>;

struct Transition;
struct State;
struct Automata;
struct ParBFS;
struct Queue;
struct intArray;

struct intArray
{
    int sz=0, capacity=1;
    int* arr = new int[capacity];


    intArray() = default;
    intArray(int v)
    {
        this->arr[sz++] = v;
    }
    void add(int el);
    void resize();
    void show();
};

class timer_t{
    time_point_t start;
    time_point_t stop;
public:
    timer_t(){ start = chrono::high_resolution_clock::now(); }
    ~timer_t()
    {   // https://en.cppreference.com/w/cpp/chrono/duration
        stop = chrono::high_resolution_clock::now();
        cout << "\t\tduration: "
             << (chrono::duration_cast<chrono::nanoseconds>(stop-start).count())
             << " ns" << endl;
    }
};

struct Transition
{
    char symbol=' ';
    State* next = nullptr;

    Transition() = default;
    Transition(char _symbol, State* _next): symbol{_symbol}, next{_next}{}
    void show();
};

struct State
{
    int sz_delta = 0, cap_delta = 1;
    intArray id;
    Transition** delta = new Transition*[cap_delta]{};
    bool F = false;

    State() = default;
    explicit State(int i, bool F_ = false) : F{F_}
    {
        this->id.add(i);
    }

    State(int i, int c, bool F_ = false) : cap_delta{c}
    {
        id.add(i);
        delta = new Transition*[cap_delta]{};
    }
    State(intArray array, bool F_ = false): id{array}, F{F_}{}

    void add_id(int new_id);
    void add_transition(Transition* t);

    void display();

    void resize_transition();

    intArray get_set_id(char symbol);
    bool compare_ids(intArray arr);
};

struct ParBFS
{
    int index;
    State* state;
    ParBFS() = default;
    ParBFS(int _index, State* _state): index{_index}, state{_state}{}
};

struct Queue
{
    ParBFS *array;      // for runtime resizing
    int capacity = 2;   // for table doubling
    int back, front;    // indirection markers
    int size_ = 0;

    Queue()
    {
        this->array = new ParBFS[this->capacity];         // dynamic array in heap
        this->front = this->back = -1;          // initialize indirection markers
        size_ = 0;
    };
    ~Queue(){ delete [] array; }

    void enqueue(ParBFS data);
    ParBFS dequeue();

    bool is_full() { return size_ == capacity; }
    bool empty(){ return (size_ == 0); }

    int size(){ return size_; }
    void resize(int new_capacity);

    int next(int index){ return (index + 1) % capacity; }
    int prev(int index){ return (index <= 0) ? capacity - 1 : index - 1; }
};

struct Automata
{
    int sz_Q = 1, cap_Q = 2, id_count=0;
    State** Q = new State*[cap_Q]{};
    string E;

    Automata(string& E_): E{E_}
    {
        this->Q[0] = new State(this->id_count++);
    }

    void build_AFN(string*& T, int t);
    Automata* transform_AFD();

    void BFS(string s);

    void resize_Q();
    void adjacency_list();

    void test(string*& S, int q);
    void add_state(State* s);
    State* get_state(intArray id);
};


#endif //TC_CLASES_H

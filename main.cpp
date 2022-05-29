#include <iostream>
#include <string>
#include <chrono>
using namespace std;   


using time_point_t = chrono::time_point<chrono::high_resolution_clock>;
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

// AF -> (Q:estados, E:alfabeto, δ:f_transicion, q0, F)

// template<typename T>
// void print(T* arr, int N)
// {
//     for (size_t i = 0; i < N; i++)
//         cout << arr[i] << " ";
//     cout << endl;
// }

struct Transition;
struct State;
struct Automata;
struct ParBFS;
struct Queue;

struct Transition
{
    char symbol;
    State* next = nullptr;

    Transition() = default;
    Transition(char _symbol, State* _next): symbol{_symbol}, next{_next}{}
    void show();
};

struct State
{
    string id;
    int sz = 0, arr_sz = 1;
    Transition* delta = new Transition[arr_sz];
    bool F = false;

    State() = default;
    State(int _id, bool _F = false):id{to_string(_id)}, F{_F}{}

    void resize();
    void list_transitions();
    
    void add_transition(Transition t)
    {
        if (this->sz == this->arr_sz)
            this->resize();
        delta[this->sz++] = t;
    }
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
    ParBFS *array;           // for runtime resizing
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
 
    void enqueue(ParBFS data)
    {
        if (this->empty() && capacity == 0 && array == nullptr)
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

    ParBFS dequeue()
    {
        if (array == nullptr || this->empty())
            throw std::runtime_error("dequeue() method doesn't work because structure is empty\n");
        
        ParBFS rt_value = array[front];  // obtain value of front
        front = next(front);        // go to the next index
        size_--;                    // adjust size_ member attribute

        // estrategia para liberar espacios inutilizados
        if (size_ < (capacity/2))
            this->resize(capacity/2);
        return rt_value;
    }                 
    
    bool is_full(){ return size_ == capacity; }
    bool empty(){ return (size_ == 0); }
                  
    int size(){ return size_; }
    void resize(int new_capacity)
    {
        ParBFS* new_array = new ParBFS[new_capacity];
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

struct Automata
{
    int sz = 1, arr_sz = 2, id_count=1;
    State** Q = new State*[arr_sz]{};
    string E;

    Automata(string _E): E{_E}
    {
        Q[0] = new State(this->id_count++);
    }

// public
    void build_AFN(string*& T, int t)
    {
        for (int i = 0; i < this->E.size(); ++i)
            Q[0]->add_transition(Transition(E[i], Q[0]));

        printf("\tbuild_AFN\n");
        for (int i=0; i<t; i++)
        {
            string c_word = T[i];
            // cout << "word: " << c_word << '\t';
            // if (c_word == "")
            //     cout << "epsilon\n";
            State* c_state = Q[0];
            for (int i = 0; i < c_word.size(); ++i)
            {
                State* n_state = new State(this->id_count++, (i == c_word.size() - 1));
                c_state->add_transition(Transition(c_word[i], n_state));
                this->add_state(n_state);
                c_state = n_state;
            }
        }
    }
        
    void transform_AFD()
    {
        
    }

    void BFS(string s)
    {
        Queue q;
        q.enqueue(ParBFS(-1, this->Q[0]));
        while (!q.empty())
        {
            ParBFS p = q.dequeue();
            if (p.state->F)
            {
                printf("YES\n");
                return;
            }
            int idx = p.index + 1;
            if (idx == s.size())
                continue;

            for (int i = 0; i < p.state->sz; ++i)
            {
                if (p.state->delta[i].symbol == s[idx])
                {
                    // cout << p.state->delta[i].symbol << " ";
                    q.enqueue(ParBFS(idx, p.state->delta[i].next));
                }
            }
        }
        printf("NO\n");
    }

    void resize();
    void adjacency_list();

    void test(string*& S, int q)
    {
        printf("\ttest\n");
        for (int i = 0; i < q; ++i)
        {
            cout << S[i] << " -> ";
            {
                timer_t timer;
                this->BFS(S[i]);
            }
            // cout << endl;
        }
        // print(S, q);
    }

// private
    void add_state(State* s)
    {
        if (sz == arr_sz)       
            this->resize();
        this->Q[sz++] = s;   
    }

    // void add_state(int id, int F)
    // {
    //     if (sz == arr_sz)       
    //         this->resize();
    //     this->Q[sz++] = new State(id, F);
    // }
};

int main() 
{
/*
    E = abcdefghijklmnopqrstuvwxyz
    t = 2
    T = {web, ebay}
    q = 3
    S = {quebay, eweb, ffwegg}
*/
#ifndef TEST
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
#endif
    int t, q;       
    string E; 
    
    cin >> E;
    Automata* a = new Automata(E);
    
    scanf ("%d", &t);
    string* T = new string[t];
    for (int i=0; i<t; i++)
        cin >> T[i];
    
    {
        timer_t timer;
        a->build_AFN(T, t);
    }

    printf("\tP1\n");
    a->adjacency_list();
    
    scanf ("%d", &q);
    string* S = new string[q];
    for (int i=0; i<q; i++)
        cin >> S[i];

    printf("\tP2\n");
    // {
        // timer_t timer;
    a->test(S, q);
    // }
}


void Transition::show()
{
    printf("(%c , ", symbol); cout << this->next->id << ") ";
    // cout << this->next->id << " ";
}

void State::list_transitions()
{
    for (int i=0; i<sz; i++)
        delta[i].show();
    printf("\n");
}

void State::resize()
{
    this->arr_sz *= 2;
    Transition* aux_delta = new Transition[this->arr_sz];
    for (int i = 0; i<this->sz; i++)
        aux_delta[i] = this->delta[i];
    delete [] this->delta;
    this->delta = aux_delta;
}

void Automata::adjacency_list()
{
    for (int i=0; i<sz; i++)
    {
        cout << Q[i]->id << " -> ";
        Q[i]->list_transitions();
    }
}

void Automata::resize()
{
    this->arr_sz *= 2;
    State** aux_Q = new State*[this->arr_sz];
    for (int i = 0; i<this->sz; i++)
        aux_Q[i] = this->Q[i];
    delete[] this->Q;
    this->Q = aux_Q;
}
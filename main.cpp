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

const int global_E = 100;
// AF -> (Q:estados, E:alfabeto, δ:f_transicion, q0, F)

template<typename T>
void print(T* arr, int N)
{
    for (size_t i = 0; i < N; i++)
        cout << i << ":" << arr[i] << "(" << arr[i].size() << ")\n";
    cout << endl;
}

string insert_sorted(const string& s1, const string& s2) {
    string ss;
    int l1=0, l2=0;
    while (l1 < s1.size() && l2 < s2.size())
    {
        char c1=s1[l1], c2=s2[l2];
        if (c1 < c2)
            ss.push_back(s1[l1]), l1++;
        else if (c2 < c1)
            ss.push_back(s2[l2]), l2++;
        else
        {
            ss.push_back(s2[l2]),
            l1++; l2++;
        }

    }
    while (l1 < s1.size())
        ss.push_back(s1[l1]), l1++;
    while (l2 < s2.size())
        ss.push_back(s2[l2]), l2++;
    return ss;
}

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
    Transition** delta = new Transition*[arr_sz]{};
    bool F = false;

    State() = default;
    State(int _id, bool _F = false) : id{to_string(_id)}, F{_F}{}
    State(string _id, bool _F = false) : id{_id}, F{_F}{}
    State(string _id, int _arr_sz, bool _F = false) : id{_id}, arr_sz{_arr_sz}, F{_F}
    {
        this->delta = new Transition*[arr_sz]{};
    }

    void resize();
    void list_transitions();
    void show();

    void add_transition(Transition* t)
    {
        if (this->sz == this->arr_sz)
            this->resize();
        delta[this->sz++] = t;
    }

    string get_transition_id(char symbol)
    {
        string sep="";
        for (int i = 0; i < this->sz; ++i)
        {
            if (this->delta[i] == nullptr)
                continue;
            if (this->delta[i]->symbol == symbol)
                sep += this->delta[i]->next->id;
        }
        return sep;
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
    int sz = 1, arr_sz = 2, id_count=0;
    State** Q = new State*[arr_sz]{};
    string E;

    Automata(string _E): E{_E}
    {
        this->Q[0] = new State(this->id_count++);
    }

    void build_AFN(string*& T, int t)
    {
        // printf("\tbuild_AFN\n");
        // generate loop
        for (int i = 0; i < this->E.size(); ++i)
            this->Q[0]->add_transition(new Transition(E[i], Q[0]));

        for (int i=0; i<t; i++)
        {
            string c_word = T[i];
            // cout << "word: " << c_word << '\t';
            State* c_state = Q[0];
            if (c_word == "")
                c_state->F = true;

            for (int i = 0; i < c_word.size(); ++i)
            {
                State* n_state = new State(this->id_count++, (i == c_word.size() - 1));
                c_state->add_transition(new Transition(c_word[i], n_state));
                this->add_state(n_state);
                c_state = n_state;
            }
        }
    }

    Automata* transform_AFD()
    {
        Automata* AFD = new Automata(this->E);
        int sz_E = this->E.size();

        for (int i = 0; i < sz_E; ++i)
        {   // por cada elemento del abecedario
            // cout << " (" << this->E[i] << ", ";
            string current_id = this->Q[0]->get_transition_id(this->E[i]);
            // cout << current_id << ") ";

            State* state = AFD->get_state(current_id);      // O(n)
            if (state == nullptr)
            {
                state = new State(current_id);      // , global_E
                AFD->Q[0]->add_transition(new Transition(this->E[i], state)); // O(n)
                AFD->add_state(state);  // O(n)
            }
            else
                AFD->Q[0]->add_transition(new Transition(this->E[i], state)); // O(n)
        }

        for (int i = 1; i < AFD->sz; ++i)       // por cada estado en el AFD generado a partir del estado inicial
        {
            string state_id =  AFD->Q[i]->id;
            // cout << state_id << "\n";
            for (int j = 0; j < sz_E; ++j)      // por cada letra del abecedario
            {
                string new_id = "";
                for (int k = 0; k < state_id.size(); ++k)       // por cada estado en el id de los estados compuestos del AFD
                {
                    // cout << " (" << state_id[k] << " " << this->E[j] << " ";
                    State *state = this->get_state(string(1, state_id[k]));      // O(n)
                    string transition = state->get_transition_id(this->E[j]);
                    // cout << transition << ") ";
                    new_id = insert_sorted(transition, new_id);
                }
                State* find_state = AFD->get_state(new_id);
                if (find_state == nullptr)
                {   // crear nuevo estado y agregar transicion del actual al nuevo estado
                    State* n_state = new State(new_id);
                    AFD->add_state(n_state);
                    AFD->Q[i]->add_transition(new Transition(this->E[j], n_state));
                }
                else
                    AFD->Q[i]->add_transition(new Transition(this->E[j], find_state));
                // cout << "\n";
                // cout << new_id << "\n";
            }
            // cout << endl;
        }

        return AFD;
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
                if (p.state->delta[i]->symbol == s[idx])
                {
                    // cout << p.state->delta[i].symbol << " ";
                    q.enqueue(ParBFS(idx, p.state->delta[i]->next));
                }
            }
        }
        printf("NO\n");
    }

    void resize();
    void adjacency_list();

    void test(string*& S, int q)
    {
        // printf("\ttest\n");
        for (int i = 0; i < q; ++i)
        {
            // cout << S[i] << " -> ";
            // {
                // timer_t timer;
                this->BFS(S[i]);
            // }
            // cout << endl;
        }
    }

    void add_state(State* s)
    {
        if (sz == arr_sz)
            this->resize();
        this->Q[sz++] = s;
    }

    State* get_state(string id)
    {
        for (int i = 0; i < this->sz; ++i)
        {
            State* current = this->Q[i];
            if (current != nullptr && current->id == id)
                return current;
        }
        return nullptr;
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
    // string s = "024", id="235";
    // cout << insert_sorted(s, id) << endl;

    int t, q;
    string E;

    cin >> E;
    Automata* AFN = new Automata(E);

    scanf ("%d", &t);
    string* T = new string[t];

    cin.ignore();
    for (int i=0; i<t; i++)
        getline(cin, T[i]);
    // print(T, t);

    // {
        // timer_t timer;
        AFN->build_AFN(T, t);
    // }

    // printf("\tP1\n");
    // AFN->adjacency_list();

    scanf ("%d", &q);
    cin.ignore();
    string* S = new string[q];
    for (int i=0; i<q; i++)
        getline(cin, S[i]);

    printf("\tP2\n");
    // {
        // timer_t timer;
        AFN->test(S, q);
    // }

    printf("\tP3\n");
    Automata* AFD = AFN->transform_AFD();
    AFD->adjacency_list();
}

void Transition::show()
{
    printf("(%c,", symbol); cout << this->next->id << ") ";
    // cout << this->next->id << " ";
}

void State::list_transitions()
{
    for (int i=0; i<sz; i++)
    {
        Transition* current = this->delta[i];
        if (current != nullptr)
            current->show();
    }
    printf("\n");
}

void State::resize()
{
    this->arr_sz *= 2;
    Transition** aux_delta = new Transition*[this->arr_sz]{};
    for (int i = 0; i<this->sz; i++)
        aux_delta[i] = this->delta[i];
    delete [] this->delta;
    this->delta = aux_delta;
}

void State::show()
{
    cout << "(id:"<< this->id << ") ";
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
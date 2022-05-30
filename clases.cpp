#include "clases.h"

void Transition::show()
{
    printf("(%c ", this->symbol);
    this->next->id.show();
    printf(") ");
}

void State::add_transition(Transition *t)
{
    if (this->sz_delta == this->cap_delta)
        this->resize_transition();
    this->delta[sz_delta++] = t;
}

void State::resize_transition()
{
    this->cap_delta *= 2;
    auto aux_delta = new Transition*[this->cap_delta]{};
    for (int i = 0; i < this->sz_delta; ++i)
        aux_delta[i] = this->delta[i];
    delete [] this->delta;
    this->delta = aux_delta;
}

void State::add_id(int new_id)
{
    this->id.add(new_id);
}

intArray State::get_set_id(char symbol)
{
    intArray arr;
    for (int i = 0; i < this->sz_delta; ++i) {
        if (this->delta[i] == nullptr)
            continue;
        if (this->delta[i]->symbol == symbol) {
            for (int j = 0; j < this->delta[i]->next->id.sz; ++j) {
                // printf("coincidencia array nuevo: ");
                arr.add(this->delta[i]->next->id.arr[j]);
                // arr.show();
            }
        }
    }
    // printf("(sz: %i) ", arr.sz);
    return arr;
}

bool State::compare_ids(intArray arr)
{
    if (this->id.sz != arr.sz)
        return false;
    for (int i = 0; i < this->id.sz; ++i)
        if (this->id.arr[i] != arr.arr[i])
            return false;
    return true;
}

void State::display()
{
    this->id.show();
    printf(" %c -> ", (this->F ? 'F' : ' '));
    for (int i = 0; i < this->sz_delta; ++i)
        this->delta[i]->show();
    printf("\n");
}

void Automata::build_AFN(string *&T, int t)
{
    // loop
    for (int i = 0; i < this->E.size(); ++i)
        this->Q[0]->add_transition(new Transition(E[i], Q[0]));

    // branches for each word
    for (int i = 0; i < t; i++)
    {   // for each word in T[]
        string c_word = T[i];
        State* c_state = Q[0];

        if (c_word.empty())
            c_state->F = true;

        for (int j = 0; j < c_word.size(); ++j)
        {   // for each character in current word add new transition
            auto n_state = new State(this->id_count++, (j == c_word.size() - 1));
            c_state->add_transition(new Transition(c_word[j], n_state));
            this->add_state(n_state);
            c_state = n_state;
        }
    }
}

void Automata::BFS(string s)
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

        for (int i = 0; i < p.state->sz_delta; ++i)
        {
            if (p.state->delta[i]->symbol == s[idx])
                q.enqueue(ParBFS(idx, p.state->delta[i]->next));
        }
    }
    printf("NO\n");
}

Automata *Automata::transform_AFD()
{
    auto AFD = new Automata(this->E);
    int sz_E = this->E.size();
    // printf("---\n");
    for (int i = 0; i < sz_E; ++i)
    {   // por cada elemento del abecedario
        // printf("%c -> ", this->E[i]);
        auto current_id = this->Q[0]->get_set_id(this->E[i]);
        State* state = AFD->get_state(current_id);      // O(n)
        // current_id.show();
        // printf("\n");

        if (state == nullptr)
        {
            // printf("null\n");
            state = new State(current_id);      // , global_E
            AFD->Q[0]->add_transition(new Transition(this->E[i], state)); // O(n)
            AFD->add_state(state);  // O(n)
        }
        else
            AFD->Q[0]->add_transition(new Transition(this->E[i], state)); // O(n)
    }
    // printf("---\n");
    for (int i = 1; i < AFD->sz_Q; ++i)
    {   // por cada estado nuevo en el AFD generado a partir del estado inicial
        intArray state_id =  AFD->Q[i]->id;

        // state_id.show();    printf("\n");
        for (int j = 0; j < sz_E; ++j)
        {   // por cada letra del abecedario
            // printf("%c -> \n\t", E[j]);
            intArray id_to_search;
            for (int k = 0; k < state_id.sz; ++k)
            {   // por cada estado en el id de los estados compuestos del AFD
                // ver a donde van con la letra E[j]
                // printf(" (%i %c) ", state_id.arr[k], E[j]);
                intArray curr_state(state_id.arr[k]);
                State* from = this->get_state(curr_state);
                // from->display();
                intArray to = from->get_set_id(E[j]);
                for (int l = 0; l < to.sz; ++l)
                    id_to_search.add(to.arr[l]);
                // to.show();
                /*printf(" ");


                */
            }
            State* find_state = AFD->get_state(id_to_search);
            if (find_state == nullptr)
            {   // crear nuevo estado y agregar transicion del actual al nuevo estado
                // id_to_search.show();    printf("\n");
                auto n_state = new State(id_to_search);
                AFD->add_state(n_state);
                AFD->Q[i]->add_transition(new Transition(this->E[j], n_state));
            }
            else
                AFD->Q[i]->add_transition(new Transition(this->E[j], find_state));
            // printf("\n");
        }
    }

    return AFD;
}

State *Automata::get_state(intArray id)
{
    for (int i = 0; i < this->sz_Q; ++i)
    {
        if (this->Q[i]->compare_ids(id))
            return this->Q[i];
    }
    return nullptr;
}

void Automata::test(string *&S, int q)
{
    for (int i = 0; i < q; ++i)
        this->BFS(S[i]);
}

void Automata::add_state(State *s)
{
    if (sz_Q == cap_Q)
        this->resize_Q();
    this->Q[sz_Q++] = s;
}

void Automata::resize_Q()
{
    this->cap_Q *=2;
    auto aux_Q = new State*[this->cap_Q]{};
    for (int i = 0; i < this->sz_Q; ++i)
        aux_Q[i] = this->Q[i];
    delete [] this->Q;
    this->Q = aux_Q;
}

void Automata::adjacency_list()
{
    for (int i = 0; i < this->sz_Q; ++i)
        this->Q[i]->display();
}

void Queue::enqueue(ParBFS data)
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

ParBFS Queue::dequeue()
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

void Queue::resize(int new_capacity)
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

void intArray::add(int el)
{
    if (this->sz == this->capacity)
        this->resize();
    // insert unique
    for (int i = 0; i < this->sz; ++i)
        if (this->arr[i] == el)
            return;
    this->arr[sz++] = el;
    // sorting
    for (int i = 0; i < this->sz; ++i)
    {
        int idx = i;
        for (int j = i-1; j >= 0 ; --j)
        {
            if (this->arr[idx] < this->arr[j])
            {   // swapping
                auto s_aux = this->arr[idx];
                this->arr[idx] = this->arr[j];
                this->arr[j] = s_aux;
                idx--;
            }
            else break;
        }
    }
}

void intArray::resize()
{
    this->capacity *=2;
    auto aux_arr = new int[this->capacity]{};
    for (int i = 0; i < this->sz; ++i)
        aux_arr[i] = this->arr[i];
    delete [] this->arr;
    this->arr = aux_arr;
}

void intArray::show()
{
    if (this->sz == 0)
    {
        printf("[]");
        return;
    }

    printf("[");
    for (int i = 0; i < this->sz-1; ++i)
        printf("%i ", this->arr[i]);
    printf("%i]", this->arr[this->sz-1]);
}
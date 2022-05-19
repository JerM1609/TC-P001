#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

struct State {
    int sz = 0, arr_sz = 1;
    char* delta = new char[arr_sz];
    State** next = new State*[arr_sz]{};
    bool F = false;

    State() = default;
    State(bool FF): F{FF}{}

    void show(string sep="")
    {
        cout << endl;
        for (int i = 0; i < this->sz; ++i)
        {
            cout << "\t" + sep + "δ(" << delta[i] << ", " << this << ") = " << next[i] << (next[i]->F?" F":""); 
            next[i]->show(sep+"\t");
        }
        
    }
    
    void word(string str)
    {
        auto len = str.size();
        auto first = new State(true);
        // cout << '\n';
        for (int i = len-1; i >= 1; --i)
        {
            // cout << str[i] << " ";
            auto new_first = new State();
            new_first->add(str[i], first);
            first = new_first;
        }
        this->add(str[0], first);
    }

    void add(char letter, State* state)
    {
        if (sz == arr_sz)
            this->resize();
        this->delta[sz] = letter;
        this->next[sz] = state;
        sz++;
    }

    void resize()
    {
        this->arr_sz *= 2;
        auto aux_delta = new char[this->arr_sz];
        auto aux_next = new State*[this->arr_sz];

        for (int i = 0; i < this->sz; ++i)
            aux_delta[i] = this->delta[i], aux_next[i] = this->next[i];

        delete[] this->delta;
        delete[] this->next;

        this->delta = aux_delta;
        this->next = aux_next;
    }

    void killSelf()
    {
        for (int i = 0; i < this->sz; ++i)
            this->next[i]->killSelf();

        delete[] this->delta;
        delete[] this->next;
    }
};

struct AFN
{
    State* q0 = nullptr;
    string E;
    string* T;
    int t;

    AFN() = default;
    AFN(string EE, int tt, string* TT): E{EE}, t{tt}, T{TT}
    {
        q0 = new State();
        for (int i = 0; i < t; ++i)
        {
            // cout << *(T+i) << " ";
            q0->word(*(this->T+i));
        }
    }

    void DFS()
    {

    }

    void adjacency_list()
    {
        cout << "q0 " << q0;
        q0->show();
    }

    ~AFN()
    {
        q0->killSelf();
    }
};

int main()
{
#ifndef TEST
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
#endif
    int t;
    string s;
    cin >> s >> t;

    string *T = new string[t];
    for (size_t i = 0; i < t; i++)
        cin >> T[i];
    
    // cout << "s -> " << s << "\nt -> " << t << "\nT -> ";
    // for (size_t i = 0; i < t; i++)
    //     cout << T[i] << " ";

    auto afn = new AFN(s, t, T);
    afn->adjacency_list();
}
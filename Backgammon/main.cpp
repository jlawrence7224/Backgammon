
#include <iostream>
#include <vector>
#include <list>
#include <stdlib.h>
#include <algorithm>
#include "endgame.h"

extern     endgame::inner_array endgame::enr;

int usage(char** argv)
{

    std::cerr << "usage: " << std::endl
        << argv[0] << "<start> <n>" << std::endl
        << argv[0] << " <pip1> <pip2> ... <pip6>" << std::endl;
    return -1;
}

int main(int argc, char** argv) {
    using namespace std;
    endgame::init_tables();
    array<int,7> items;
    int cnt = 0;

    float max_diff = -1.0;
    int I = -1, J = -1;
    for (int i = 1; i < endgame::n_exact; ++i)
        for (int j = 1; j < endgame::n_exact; ++j)
            if (max_diff < std::abs(endgame::p_exact[i][j] - endgame::Pwin(i, j)))
            {
                max_diff = std::abs(endgame::p_exact[i][j] - endgame::Pwin(i, j));
                I = i; J = j;
            }

    cout << std::endl << "MAX DIFF@ (" << I << ", " << J << ") : " << max_diff << std::endl;

    switch (argc)
    {
    case 4:
    {
        // print first n compute_enr values.
        int I = atoi(argv[1]);
        int J = atoi(argv[2]);
        int n = atoi(argv[3]);
        cout << "ENR" << std::endl;
        for (int i = I; i < I+n; ++i)
        {
            cout << endgame::enr[i] << std::endl;
        }
        cout << std::endl << "Prob(Win) Exact";
        for (int i = I; i < I+n; ++i)
        {
            cout << std::endl << i << ":";
            for (int j = J; j < J+n; ++j)
            {
                cout << '\t' << endgame::p_exact[i][j];
            }
        }
        cout << std::endl << "------------------------------------------------------------" << std::endl;
        for (int j = J; j < J+n; ++j)
        {
            cout << '\t' << j;
        }
        cout << std::endl << "Prob(Win) ENR strategy";
        for (int i = I; i < I + n; ++i)
        {
            cout << std::endl << i << ":";
            for (int j = J; j < J + n; ++j)
            {
                cout << '\t' << endgame::Pwin(i, j);
            }
        }
        cout << std::endl << "------------------------------------------------------------" << std::endl;
        for (int j = J; j < J + n; ++j)
        {
            cout << '\t' << j;
        }
        return 0;
    }

    case 7:
    {
        for (int i = 1; i < argc; ++i) {
            int c = atoi(argv[i]);
            items[i] = c;
            cnt += c;
        }
        if (cnt <= 15)
        {
            items[0] = 15 - cnt;
            uint64 hash = endgame::position_hash(items);
            cout << "hash: " << hash << " ENR: " << endgame::enr[hash] << std::endl;
            return 0;
        }
        else
            cout << "Checker count exceeds 15" << std::endl;
        // fall through
    }
    default:
        return usage(argv);
    }
}
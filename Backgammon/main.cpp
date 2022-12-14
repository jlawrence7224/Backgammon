
#include <iostream>
#include <vector>
#include <list>
#include <stdlib.h>
#include <algorithm>
// #include "endgame.h"
#include "enr.h"
#include "pwinx.h"

struct eg_hash eg;  // singleton

PNR Pnr;
p_opt exact;  // singleton

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
    // endgame::init_tables();
    array<int,7> items;
    int cnt = 0;
    const int nx = 8;
    std::cout << std::endl << "DBG exact.Pwin(i, j)";
    for (int i = 0; i < nx; ++i)
    {
        std::cout << std::endl << i << ": ";
        for (int j = 0; j < nx; ++j)
            std::cout << " " << exact.Pwin(i, j);
    }
    std::cout << std::endl;
#if 0
    std::cout << std::endl << "DBG p_exact[i][j]";
    for (int i = 0; i < nx; ++i)
    {
        std::cout << std::endl << i << ": ";
        for (int j = 0; j < nx; ++j)
            std::cout << " " << endgame::p_exact[i][j];
    }
    std::cout << std::endl;
#endif
    float max_diff = -1.0;
    int I = -1, J = -1;
#if 0
    for (int i = 1; i < endgame::n_exact; ++i)
        for (int j = 1; j < endgame::n_exact; ++j)
            if (max_diff < std::abs(endgame::p_exact[i][j] - exact.Pwin(i,j)))
            {
                max_diff = std::abs(endgame::p_exact[i][j] - exact.Pwin(i, j));
                I = i; J = j;
            }

    std::cout << std::endl << "MAX DIFF (eg.x - x.Pwin) (" << I << ", " << J << ") : " << max_diff << std::endl;

    max_diff = -1.0;
    I = -1; J = -1;
    for (int i = 1; i < endgame::n_exact; ++i)
        for (int j = 1; j < endgame::n_exact; ++j)
            if (max_diff < std::abs(endgame::p_exact[i][j] - endgame::Pwin(i, j)))
            {
                max_diff = std::abs(endgame::p_exact[i][j] - endgame::Pwin(i, j));
                I = i; J = j;
            }

    std::cout << std::endl << "MAX DIFF (eg.x - eg.Pwin)@ (" << I << ", " << J << ") : " << max_diff << std::endl;
#endif

    max_diff = -1.0;
    I = -1; J = -1;
    for (int i = 1; i < p_opt::N; ++i)
        for (int j = 924; j < p_opt::N; ++j)
            if (max_diff < std::abs(Pnr.Pwin(i,j) - exact.Pwin(i, j)))
            {
                max_diff = std::abs(Pnr.Pwin(i,j) - exact.Pwin(i, j));
                I = i; J = j;
            }

    std::cout << std::endl << "MAX DIFF (Pnr.PWIN - x.PWIN)@ (" << I << ", " << J << ") : " << max_diff << std::endl;
#if 0
    max_diff = -1.0;
    I = -1; J = -1;
    for (int i = 1; i < endgame::n_exact; ++i)
        for (int j = 1; j < endgame::n_exact; ++j)
            if (max_diff < std::abs(endgame::Pwin(i, j) - exact.Pwin(i, j)))
            {
                max_diff = std::abs(endgame::Pwin(i, j) - exact.Pwin(i, j));
                I = i; J = j;
            }

    std::cout << std::endl << "MAX DIFF (eg.PWIN - x.PWIN)@ (" << I << ", " << J << ") : " << max_diff << std::endl;
#endif

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
            cout << "PNR: " << Pnr[i] << std::endl;
            //cout << "PNR: " << Pnr[i] << " ENR: " << endgame::enr[i] << std::endl;
            //cout << endgame::enr[i] << std::endl;
        }
        cout << std::endl << "Prob(Win) Exact";
        for (int i = I; i < I+n; ++i)
        {
            cout << std::endl << i << ":";
            for (int j = J; j < J+n; ++j)
            {
                cout << "  \t" << exact.Pwin(i,j);
                //cout << "  \t" << endgame::p_exact[i][j];
            }
        }
        cout << std::endl << "------------------------------------------------------------" << std::endl;
        for (int j = J; j < J+n; ++j)
        {
            cout << "\t" << j << "\t";
        }
        cout << std::endl << "Prob(Win) ENR strategy";
        for (int i = I; i < I + n; ++i)
        {
            cout << std::endl << i << ":";
            for (int j = J; j < J + n; ++j)
            {
                cout << "  \t" << Pnr.Pwin(i, j);
                //cout << "  \t" << endgame::Pwin(i, j);
            }
        }
        cout << std::endl << "------------------------------------------------------------" << std::endl;
        for (int j = J; j < J + n; ++j)
        {
            cout << "\t" << j << "\t";
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
            uint64 hash = eg.hash(items);
            cout << "hash: " << hash << " ENR: " << Pnr[hash] << std::endl;
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
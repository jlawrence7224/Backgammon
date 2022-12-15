
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

int usage(char** argv)
{

    std::cerr << "usage: " << std::endl
        << argv[0] << "<start> <n>" << std::endl
        << argv[0] << " <pip1> <pip2> ... <pip6>" << std::endl;
    return -1;
}

int main(int argc, char** argv)
{
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

    float max_diff = -1.0;
    int I = -1, J = -1;

    max_diff = -1.0;
    I = -1; J = -1;
    for (int i = 1; i < p_opt::N; ++i)
        for (int j = 1; j < p_opt::N; ++j)
            if (max_diff < abs(Pnr.Pwin(i,j) - exact.Pwin(i, j)))
            {
                max_diff = abs(Pnr.Pwin(i,j) - exact.Pwin(i, j));
                I = i; J = j;
            }

    std::cout << std::endl << "MAX DIFF (Pnr.PWIN - x.PWIN)@ (" << I << ", " << J << ") : " << max_diff << std::endl;
    std::cout << Board(I, J);

    switch (argc)
    {
    case 4:
    {
        // print first n compute_enr values.
        int I = atoi(argv[1]);
        int J = atoi(argv[2]);
        int n = atoi(argv[3]);

        std::cout << Board(I, J);
        
        std::cout << "P(X=n)" << std::endl;
        std::cout << I << ": " << Pnr.X[I] << std::endl;
        std::cout << J << ": " << Pnr.X[J] << std::endl;

        std::cout << std::endl << "ENR" << std::endl;
        for (int i = I-n; i < I+n; ++i)
        {
            std::cout << i << ": " << Pnr[i] << std::endl;
        }
        std::cout << std::endl << "Prob(Win) Exact";
        for (int i = I-n; i < I+n; ++i)
        {
            std::cout << std::endl << i << ":";
            for (int j = J-n; j < J+n; ++j)
            {
                std::cout << "  \t" << exact.Pwin(i,j);
            }
        }
        std::cout << std::endl << "------------------------------------------------------------" << std::endl;
        for (int j = J-n; j < J+n; ++j)
        {
            std::cout << "\t" << j << "\t";
        }
        std::cout << std::endl << "Prob(Win) ENR strategy";
        for (int i = I-n; i < I + n; ++i)
        {
            std::cout << std::endl << i << ":";
            for (int j = J-n; j < J + n; ++j)
            {
                std::cout << "  \t" << Pnr.Pwin(i, j);
            }
        }
        std::cout << std::endl << "------------------------------------------------------------" << std::endl;
        for (int j = J-n; j < J + n; ++j)
        {
            std::cout << "\t" << j << "\t";
        }


        return 0;
    }

    case 7:
    {
        std::array<int, 7> items;
        for (int i = 1; i < argc; ++i) {
            int c = atoi(argv[i]);
            items[i] = c;
            cnt += c;
        }
        if (cnt <= 15)
        {
            items[0] = 15 - cnt;
            uint64 hash = eg.hash(items);
            std::cout << "hash: " << hash << " ENR: " << Pnr[hash] << std::endl;
            return 0;
        }
        else
            std::cout << "Checker count exceeds 15" << std::endl;
        // fall through
    }
    default:
        return usage(argv);
    }
    return -1;
}
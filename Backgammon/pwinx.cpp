#include "pwinx.h"
#include "board.h"


/// <summary>
/// Accumulate the minimum of the P(win) values with hb to move
/// of the boards pushed. (looked up in pwin vector) 
/// </summary>
struct minPexact {
    using Hash = eg_hash::Hash;
    using Pwin_t = p_exact::Pwin_t;
    minPexact(Hash hb, const Pwin_t& pwin) : pwin(pwin), hb(hb), min_p(std::numeric_limits<float>::infinity()) {}

    const Pwin_t& pwin;
    Hash hb;
    float min_p;

    // MoveContainer interface 
    void push_board(const Board& b)
    {
        Hash hw = eg.hash_w(b);
        min_p = std::min(min_p, pwin[hb][hw]);
    }
};

float p_exact::init_p_exact(int hb, BoardInfo& b, const Roll& r)
{
    // find minimum compute_enr over possible moves with roll 'r'
    minPexact min_p(hb,p_win);
    genMoves(min_p, b, r);
    return min_p.min_p;
}

// compute exact win probabilites for (hw,hb) inner boards
// assuming p_exact has been computed for all positions (w,b)
// with (w+b) < (hw+hb).
void p_exact::init_p_exact(Hash hw, Hash hb, BoardInfo& b)
{
    double p = 0;
    for (auto& r : Roll::rolls21)
    {
        p += r.p * init_p_exact(hb, b, r);
    }
    p_win[hw][hb] = 1.0 - p;
}

// compute exact win probabilites of first (n_exact,n_exact) inner boards
void p_exact::init_p_exact()
{
    // Init terminal positions
    p_win[0][0] = 1.0;
    for (int i = 1; i < n_exact; ++i)
    {
        p_win[0][i] = 1.0;
        p_win[i][0] = 0.0;
    }

    // compute array diagonal by diagonal

    // Fill sub diagonals up to main diagonal
    for (int i = 1; i < n_exact; ++i)
    {
        inner_table_iterator it;
        for (int hb = i; hb > 0; --hb)
        {
            BoardInfo B(*++it);
            init_p_exact(it._hash, hb, B);
            Assert(it._hash + hb == i + 1);
        }
    }
    // Fill super diagonals
    for (int i = 2; i < n_exact; ++i)
    {
        inner_table_iterator it(i); Assert(it._hash == i);
        for (int hb = n_exact - 1; it._hash < n_exact; --hb, ++it)
        {
            BoardInfo B(*it);
            init_p_exact(it._hash, hb, B);
        }
    }
}

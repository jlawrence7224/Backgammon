#include "enr.h"
#include "board.h"

std::ostream& operator<<(std::ostream& s, const finite_support_vector& v)
{
    float d = 0.0;
    int i = v.lower();
    for( auto dx : v.support)
    {
        s << i++ << ": " << dx - d << " ";
        d = dx;
    }
    return s;
}
/// <summary>
/// Accumulate the minimum of the compute_enr (looked up in enr vector) 
/// of the boards pushed.
/// </summary>
struct minENR {
    using enrvec = ENR::enrvec;

    minENR(const enrvec& enr) : enr(enr), min_enr(std::numeric_limits<float>::infinity()) {}

    const enrvec& enr;
    float min_enr;

    // MoveContainer interface 
    void push_board(const Board& b)
    {
        min_enr = std::min(min_enr, enr[eg.hash_w(b)]);
    }
};

/// <summary>
/// Store the hash of the least ENR board
/// </summary>
struct minENRhash {
    using Hash = ENR::Hash;

    minENRhash(const ENR& enr) : enr(enr), min_enr(std::numeric_limits<float>::infinity()), hash(0) {}

    const ENR& enr;
    float min_enr;
    Hash hash;

    // MoveContainer interface 
    void push_board(const Board& b)
    {
        Hash h = eg.hash_w(b);

        if (min_enr > enr[h])
        {
            min_enr = enr[h];
            hash = h;
        }
    }
};
/// <summary>
/// Compute compute_enr for Board 'b' having rolled 'r'
/// </summary>
/// <param name="b"></param>
/// <param name="r"></param>
/// <returns></returns>
float ENR::compute_enr(BoardInfo& b, const Roll& r)
{
    // find minimum compute_enr over possible moves with roll 'r'
    minENR minenr(enr);
    //b.genMoves(minenr, b.get_Info(), r.hi, r.lo);
    genMoves(minenr, b, r);
    return 1.0 + minenr.min_enr;;
}

/// <summary>
/// Compute compute_enr for Board 'b'
/// </summary>
/// <param name="b"></param>
/// <returns></returns>
float ENR::compute_enr(BoardInfo& b)
{
    // Compute expectation over all rolls
    double enr = 0.0;
    for (auto& r : Roll::rolls21)
    {
        enr += r.p * compute_enr(b, r);
    }
    return enr;
}

/// <summary>
/// compute_enr -- Expected Number of Rolls required to  
/// bare off from a no-contact inner board position.
/// Compute compute_enr of each inner table position
/// Store results in vector enr;
/// </summary>
void ENR::compute_enr()
{
    inner_table_iterator it;
    enr.clear();
    enr.reserve(it.max_index + 1);
    enr.push_back(0.0);     // finish position: enr = 0.0

    while (it.more())
    {
        BoardInfo B(*++it); // preincrement
        enr.push_back(compute_enr(B));
    }
    Assert(enr.size() == n_inner_table_configurations);
}

/// <summary>
/// Hash of best move using min ENR strategy
/// </summary>
/// <param name="b"></param>
/// <param name="r"></param>
/// <returns></returns>
ENR::Hash ENR::bestMove(BoardInfo& b, const Roll& r) const
{
    minENRhash minhash(*this);
    genMoves(minhash, b, r);
    return minhash.hash;
}

/// <summary>
/// for board b
/// Compute density P(X=n) of random variable X
/// X=n -- Player p bares off in <n> moves 
/// </summary>
/// <param name="b"></param>
void PNR::computeXden(BoardInfo& b)
{
    std::array<Hash, 21> H;     // The hash of the best move for each roll
    size_t lo = 100, hi = 0;    // Low and High bounds for density being computed

    // Find best move for each roll and retrieve X density
    // Determine support (lo,hi) bounds for output density.
    for (auto& r : Roll::rolls21)
    {
        H[r.ordinal] = bestMove(b, r);
        Dist& den = X[H[r.ordinal]];
        lo = std::min(lo, den.lower());
        hi = std::max(hi, den.upper());
    }

    // Append new density vector to end of Xdist
    X.emplace_back(hi, lo);
    Dist& den_b = X.back(); // The density of the X for board b
    Assert(den_b.support[hi - lo - 1] == 0.0);

    for (auto& r : Roll::rolls21)
    {
        Dist& den_r = X[H[r.ordinal]];      // density of best move for roll r
        int j = den_r.lower() - den_b.lower();
        for (float d : den_r.support)
        {
            den_b.support[j++] += r.p * d;
        }
    }
    den_b.shift();   // Shift data by one to count move from this position
}

/// <summary>
/// for each inner board configuration
/// Compute density P(X=n) of random variable X
/// X=n -- Player p bares off in <n> moves 
/// </summary>
void PNR::computeXden()
{
    inner_table_iterator it;
    X.clear();
    X.reserve(inner_table_iterator::max_index + 1);
    X.emplace_back(1.0);   // Emplace the finish position: P(X=0) = 1.0

    while (it.more())
    {
        BoardInfo B(*++it);
        computeXden(B);
    }
}
/// <summary>
/// for each inner board configuration
/// Compute distribution: P(X<=n) of the random variable X
/// X=n -- Player p bares off in <n> moves     
/// </summary>
void PNR::computeXdist()
{
    // Compute the density of X
    computeXden();
    // Convert densities to distributions
    for (auto& dist : X)
        dist.distribution();
}

/// <summary>
/// The Win probability of player to move
/// when both players use min ENR strategy
/// </summary>
/// <param name="hw">inner board hash of player to move</param>
/// <param name="hb">inner board hash of opponent</param>
/// <returns>P(player to move wins)</returns>
float PNR::Pwin(Hash hw, Hash hb)
{
    Dist& Xw = X[hw];   // player to move
    Dist& Xb = X[hb];   // opponent

    size_t lower = std::max(Xw.lower(), Xb.lower());
    size_t upper = std::min(Xw.upper(), Xb.upper());

    float db = Xb.at(lower - 1);
    float pwin = 0.0;

    for (int i = lower; i < upper; ++i)
    {
        pwin += (Xb[i] - db) * Xw[i];
        db = Xb[i];
    }
    pwin += (1.0 - db);
    return pwin;
}
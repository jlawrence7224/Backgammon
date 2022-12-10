#include "endgame.h"
#include "roll.h"
#include "board.h"

namespace endgame {
    
    int64 multi_choose[7][17]; // precalculated values for  n = 1..7 k = -1..15

    int64 binom(int64 n, int64 k)
    {
        k = std::min(k, n - k);

        int64 res = k >= 0; // Ensure correst results for k < 0

        for (int64 i = 0; i < k; ++i)
            res = res * (n - i) / (i + 1);

        return res;
    }

    int64 multichoose(int n, int k) { return binom(n + k - 1, k); }

    void multi_choose_init()
    {
        for (int n = 0; n < DIM(multi_choose); ++n)
            for (int k = 0; k < DIM(multi_choose[0]); ++k)
                multi_choose[n][k] = multichoose(n + 1, k - 1);
    }

    /// <summary>
    /// Multichoose of small frequently used parameters
    /// Looks up precalculated values of these parameters
    /// </summary>
    /// <param name="n">1..7</param>
    /// <param name="k">-1..15</param>
    /// <returns>multichoose(n,k)</returns>
    int64 multichoose_pre(int n, int k)
    {
        Assert(n >= 1 && n <= 7 && k >= -1 && k <= 15);
        int64 res = multi_choose[n - 1][k + 1]; // Adjust the indices
        Assert(res == multichoose(n, k));
        return res;
    }

    /// <summary>
    /// The multichoose coefficient which occurs in
    /// in position hash calculations.
    /// </summary>
    /// <param name="i"></param>
    /// <param name="K">sum of checkers on pips <= i</param>
    /// <returns></returns>
    Hash S(int i, int K) { return multichoose_pre(7 - i, 15 - (K + 1)); }

    Hash position_hash(const inner_board& b)
    {
        int k = 14;
        int64 _hash = 0;
        for (int i = 0; i < 6; ++i)
        {
            int n = 7 - i;
            k -= b[i];
            _hash += multichoose_pre(n, k);
        }
        return _hash;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="hash"></param>
    /// <param name="b"></param>
    /// <returns></returns>
    inner_board& inverse_hash(Hash hash, inner_board& b)
    {
        int K = 0;
        Hash H = hash; // invariant: H >= 0
        for (int i = 0; i < 7; ++i)
        {
            int k = K;
            Hash delta;
            while ( (delta = H - S(i, k)) < 0)
                ++k;
            H = delta;      // delta >= 0
            b[i] = k - K;   // k >= K
            K = k;
        }
        Assert(hash == position_hash(b));
        return b;
    }

    // BareOff hash of White position
    uint64 position_hash_w(const Board& b)
    {
        int k = 14;
        uint64 _hash = 0;
        for (int i = 0; i < 6; ++i)
        {
            int n = 7 - i;
            k -= b.board[25 - i];
            _hash += multichoose_pre(n, k);
        }
        return _hash;
    }

    void position_hash(Board& b, bg_hash& h)
    {
        const int fin = 25;
        int k_w = 14 - b.finished(), k_b = 14 - b._finishedB;
        uint64 hash_w = multichoose_pre(7, k_w), hash_b = multichoose_pre(7, k_b);
        for (int i = 1; i < 6; ++i)
        {
            int n = 7 - i;
            k_w -= b.board[fin - i];    // white checker counts are positive
            k_b += b.board[i];          // black checker counts are negative
            hash_w += multichoose_pre(n, k_w);
            hash_b += multichoose_pre(n, k_b);
        }
        h.w_hash = hash_w;
        h.b_hash = hash_b;
    }

    inner_array enr; // Pre-computed ENR for all inner board positions

    /// <summary>
    /// Accumulate the minimum of the compute_enr (looked up in enr vector) 
    /// of the boards pushed.
    /// </summary>
    struct minENR {
        minENR() : min_enr(std::numeric_limits<float>::infinity()) {}
        float min_enr;

        // MoveContainer interface 
        void push_board(const Board& b)
        {
            min_enr = std::min(min_enr, enr[position_hash_w(b)]);
        }
    };

    /// <summary>
    /// Store the hash of the least ENR board
    /// </summary>
    struct minENRhash {
        minENRhash() : min_enr(std::numeric_limits<float>::infinity()), hash(0) {}
        float min_enr;
        Hash hash;
        // MoveContainer interface 
        void push_board(const Board& b)
        {
            Hash h = position_hash_w(b);

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
    float compute_enr(BoardInfo& b, const Roll& r)
    {
        // find minimum compute_enr over possible moves with roll 'r'
        minENR minenr;
        //b.genMoves(minenr, b.get_Info(), r.hi, r.lo);
        genMoves(minenr, b, r);
        return 1.0 + minenr.min_enr;;
    }

    /// <summary>
    /// Compute compute_enr for Board 'b'
    /// </summary>
    /// <param name="b"></param>
    /// <returns></returns>
    float compute_enr(BoardInfo& b)
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
    void compute_enr()
    {
        inner_table_iterator it;
        enr.clear();
        enr.reserve(it.max_index + 1);
        enr.push_back(0.0);     // finish position: enr = 0.0

        while( it.more() )
        {
            BoardInfo B(*++it); // preincrement
            enr.push_back(compute_enr(B));
        }
        Assert(enr.size() == n_inner_table_configs);
    }

    XDist Xdist;
    
    /// <summary>
    /// Hash of best move as determined by least ENR
    /// </summary>
    /// <param name="b"></param>
    /// <param name="r"></param>
    /// <returns></returns>
    Hash bestMove(BoardInfo& b, const Roll& r)
    {
        minENRhash minhash;
        genMoves(minhash, b, r);
        return minhash.hash;
    }

    /// <summary>
    /// for board b
    /// Compute density P(X=n) of random variable X
    /// X=n -- Player p bares off in <n> moves 
    /// </summary>
    /// <param name="b"></param>
    void computeXden(BoardInfo& b) 
    {
        std::array<Hash, 21> H;     // The hash of the best move for each roll
        size_t lo = 100, hi = 0;    // Low and High bounds for density being computed

        // Find best move for each roll and retrieve X density
        // Determine support (lo,hi) bounds for output density.
        for( auto& r : Roll::rolls21 )
        { 
            H[r.ordinal] = bestMove(b, r);
            Dist& den = Xdist[H[r.ordinal]];
            lo = std::min(lo, den.lower());
            hi = std::max(hi, den.upper());
        }

        // Append new density vector to end of Xdist
        Xdist.emplace_back(hi, lo);
        Dist& den_b = Xdist.back(); // The density of the X for board b
        Assert(den_b.support[hi-lo-1] == 0.0);

        for (auto& r : Roll::rolls21)
        {
            Dist& den_r = Xdist[H[r.ordinal]];      // density of best move for roll r
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
    void computeXden()
    {
        inner_table_iterator it;
        Xdist.clear();
        Xdist.reserve(inner_table_iterator::max_index+1);
        Xdist.emplace_back(1.0);   // Emplace the finish position: P(X=0) = 1.0

        while( it.more() )
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
    void computeXdist()
    {
        // Compute the density of X
        computeXden();
        // Convert densities to distributions
        for (auto& dist : Xdist)
            dist.distribution();
    }

    /// <summary>
    /// Calculate the Win probability of player to move
    /// </summary>
    /// <param name="hw">inner board hash of player to move (White)</param>
    /// <param name="hb">inner board hash of opponent (Black)</param>
    /// <returns>P(player to move wins)</returns>
    float Pwin(Hash hw, Hash hb)
    {
        Dist& Xw = Xdist[hw];
        Dist& Xb = Xdist[hb];

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

    Pexact p_exact;
    /// <summary>
    /// Accumulate the minimum of the p_opt values with hb to move
    /// of the boards pushed. (looked up in p_opt vector) 
    /// </summary>
    struct minPexact {
        minPexact(int hb) : hb(hb), min_p(std::numeric_limits<float>::infinity()) {}
        int hb;
        float min_p;

        // MoveContainer interface 
        void push_board(const Board& b)
        {
            int hw = position_hash_w(b);
            min_p = std::min(min_p, p_exact[hb][hw]);
        }
    };

    float compute_p_exact(int hb, BoardInfo& b, const Roll& r)
    {
        // find minimum compute_enr over possible moves with roll 'r'
        minPexact min_p(hb);
        b.genMoves(min_p, b.get_Info(), r.hi, r.lo);
        return min_p.min_p;
    }
    // compute exact win probabilites for (hw,hb) inner boards
    // assuming p_opt has been computed for all positions (w,b)
    // with (w+b) < (hw+hb).
    void compute_p_exact(int hw, int hb, BoardInfo& b)
    {
        float p = 0;
        for( auto& r: Roll::rolls21)
        { 
            p += r.p * compute_p_exact(hb,b,r);
        }
        p_exact[hw][hb] = 1.0 - p;
    }

    // compute exact win probabilites of first (n_exact,n_exact) inner board 
    void compute_p_exact()
    {
        // Init terminal positions
        p_exact[0][0] = 1.0;
        for (int i = 1; i < n_exact; ++i)
        {
            p_exact[0][i] = 1.0;
            p_exact[i][0] = 0.0;
        }

        // compute array diagonal by diagonal
        
        // Fill sub diagonals up to main diagonal
        for (int i = 1; i < n_exact; ++i)
        {
            inner_table_iterator it;
            for (int hb = i; hb > 0; --hb)
            {
                BoardInfo B(*++it);
                compute_p_exact(it._hash, hb, B);
                Assert(it._hash + hb == i + 1);
            }
        }
        // Fill super diagonals
        for (int i = 2; i < n_exact; ++i)
        {
            inner_table_iterator it(i); Assert(it._hash == i);
            for (int hb = n_exact-1; it._hash < n_exact; --hb, ++it)
            {
                BoardInfo B(*it);
                compute_p_exact(it._hash, hb, B);
            }
        }
    }

    void init_tables()
    {
        multi_choose_init();
        compute_enr();
        compute_p_exact();
        computeXdist();
    }
};
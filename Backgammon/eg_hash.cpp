#include "eg_hash.h"

int64 binom(int64 n, int64 k)
{
    k = std::min(k, n - k);

    int64 res = k >= 0; // Ensure correst results for k < 0

    for (int64 i = 0; i < k; ++i)
        res = res * (n - i) / (i + 1);

    return res;
}

int64 multichoose(int64 n, int64 k) { return binom(n + k - 1, k); }

eg_hash::Hash eg_hash::hash(const inner_table& b)
{
    int k = 14;
    int64 _hash = 0;
    for (int i = 0; i < 6; ++i)
    {
        int n = 7 - i;
        k -= b[i];
        _hash += mc(n, k);
    }
    return _hash;
}

eg_hash::inner_table& eg_hash::inverse_hash(Hash h, inner_table& b)
{
    int K = 0;
    Hash H = h; // invariant: H >= 0
    for (int i = 0; i < 7; ++i)
    {
        int k = K;
        Hash delta;
        while ((delta = H - S(i, k)) < 0)
            ++k;
        H = delta;      // delta >= 0
        b[i] = k - K;   // k >= K
        K = k;
    }
    Assert(h == hash(b));
    return b;
}

eg_hash::Hash eg_hash::hash_w(const Board& b)
{
    int k = 14;
    Hash _hash = 0;
    for (int i = 0; i < 6; ++i)
    {
        int n = 7 - i;
        k -= b.board[25 - i];
        _hash += mc(n, k);
    }
    return _hash;
}

eg_hash::BwHash eg_hash::hash_bw(const Board& b)
{
    const int fin = 25;
    int k_w = 14 - b.finished(), k_b = 14 - b._finishedB;
    uint64 hash_w = mc(7, k_w), hash_b = mc(7, k_b);
    for (int i = 1; i < 6; ++i)
    {
        int n = 7 - i;
        k_w -= b.board[fin - i];    // white checker counts are positive
        k_b += b.board[i];          // black checker counts are negative
        hash_w += mc(n, k_w);
        hash_b += mc(n, k_b);
    }
    return BwHash(hash_b, hash_w);
}


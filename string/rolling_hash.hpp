#pragma once

#include <atcoder/modint>
#include <cassert>
#include <random>
#include <string>
#include <utility>
#include <vector>

// Rolling Hash (double mod). Requires AtCoder Library (ACL), C++17 or later.
// Different rhash instances share the same randomized bases and power tables.
struct rhash {
private:
    using m1 = atcoder::modint998244353;
    using m2 = atcoder::modint1000000007;
    using phash = std::pair<m1, m2>;

    inline static m1 base1;
    inline static m2 base2;
    inline static std::vector<m1> pow_base1;
    inline static std::vector<m2> pow_base2;

    std::vector<m1> val1;
    std::vector<m2> val2;
    int n;

public:
    explicit rhash(const std::string& s) : n(static_cast<int>(s.size())) {
        if (base1.val() == 0) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist1(200, 998244351);
            std::uniform_int_distribution<int> dist2(200, 1000000005);
            base1 = dist1(gen);
            base2 = dist2(gen);
            pow_base1 = {1};
            pow_base2 = {1};
        }

        while (static_cast<int>(pow_base1.size()) < n + 1) {
            pow_base1.push_back(pow_base1.back() * base1);
            pow_base2.push_back(pow_base2.back() * base2);
        }

        val1.resize(n + 1);
        val2.resize(n + 1);
        for (int i = 0; i < n; ++i) {
            int c = static_cast<unsigned char>(s[i]);
            val1[i + 1] = val1[i] * base1 + c;
            val2[i + 1] = val2[i] * base2 + c;
        }
    }

    // Hash of s[l:r] (half-open range). O(1).
    phash get(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        return {
            val1[r] - val1[l] * pow_base1[r - l],
            val2[r] - val2[l] * pow_base2[r - l]
        };
    }
};

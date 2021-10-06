#include <iostream>

#include "matrix.hpp"
#include "timer.hpp"

int main(int, char **)
{
    auto n = 1000, m = 700, k = 900;

    auto mat1 = MatXi::Random(100, n, m);
    auto mat2 = MatXi::Random(100, m, k);

    MatXi::PAR_MUL = false;                 // 1. multiply sequentially
    time(auto prod_seq = mat1 * mat2;)      // elapsed time is 10.3598 seconds
    MatXi::PAR_MUL = true;                  // 2. multiply in parallel
    time(auto prod_par = mat1 * mat2;)      // elapsed time is 2.77139 seconds

    std::cout << "seq == par: "             // calculations are equally correct
              << (prod_seq == prod_par)     // seq == par: 1
              << '\n';
}

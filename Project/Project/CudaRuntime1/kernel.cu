#include "cuda_runtime.h"
#include "thrust/device_vector.h"
#include <thrust/random.h>
#include <thrust/transform.h>

#include <chrono>
#include <iostream>

struct GenRand {
    __device__
        int operator () (int idx) {
        thrust::default_random_engine randEng;
        thrust::uniform_int_distribution<int> uniDist;
        randEng.discard(idx);
        return uniDist(randEng);
    }
};

int main() {
    const size_t N = 3e4;
    const size_t size = N * N;
    std::cout << "elements: " << size << '\n';

    thrust::device_vector<int> a(size);
    thrust::device_vector<int> b(size);
    thrust::device_vector<int> c(size);

    auto start = std::chrono::high_resolution_clock::now();
    thrust::transform(
        thrust::make_counting_iterator(0ULL),
        thrust::make_counting_iterator(size),
        a.begin(),
        GenRand());
    thrust::transform(
        thrust::make_counting_iterator(0ULL),
        thrust::make_counting_iterator(size),
        b.begin(),
        GenRand());
    auto end = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;
    return 0;
}

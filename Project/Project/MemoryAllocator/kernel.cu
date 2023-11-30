#include "cuda_runtime.h"
#include <thrust/device_vector.h>
#include <thrust/functional.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/random.h>
#include <iostream>

struct GenRand {
private:
    int seed;

public:
    GenRand(int seed)
        : seed(seed) {
    }

    __device__ float operator()(int idx) {
        thrust::default_random_engine randEng(seed);
        thrust::uniform_int_distribution<int> uniDist(1, 10);
        randEng.discard(idx);
        return uniDist(randEng);
    }
};

int main(int argc, const char** argv) {
    if (argc != 2) {
        std::cout << "Enter how much bytes to allocate!" << std::endl;
    }
    size_t count = std::atoll(argv[1]);
    thrust::device_vector<std::uint8_t> garbage(count, 0);
    thrust::transform(thrust::make_counting_iterator(0ULL), thrust::make_counting_iterator(garbage.size()),
                      garbage.begin(), GenRand(0));
    std::cout << "Allocated " << count << " bytes" << std::endl;
    std::string input;
    std::cin >> input;
    return 0;
}

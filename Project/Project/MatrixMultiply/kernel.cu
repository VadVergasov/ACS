#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/random.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <sstream>
#include <random>

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

__global__ void matrixMultiplicationKernel(const float* A, const float* B, float* C, size_t N) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < N && col < N) {
        float sum = 0.0f;
        for (int k = 0; k < N; ++k) {
            sum += A[row * N + k] * B[k * N + col];
        }
        C[row * N + col] = sum;
    }
}

void matrixMultiplication(const thrust::device_vector<float>& A, const thrust::device_vector<float>& B,
                          thrust::device_vector<float>& C, int N, bool mp) {
    dim3 threadsPerBlock, blocksPerGrid;
    if (mp) {
        threadsPerBlock = dim3(16, 16, 1);
        blocksPerGrid =
            dim3((N + threadsPerBlock.x - 1) / threadsPerBlock.x, (N + threadsPerBlock.y - 1) / threadsPerBlock.y);
    } else {
        threadsPerBlock = dim3(1, 1, 1);
        blocksPerGrid =
            dim3((N + threadsPerBlock.x - 1) / threadsPerBlock.x, (N + threadsPerBlock.y - 1) / threadsPerBlock.y);
    }

    matrixMultiplicationKernel<<<blocksPerGrid, threadsPerBlock>>>(
        thrust::raw_pointer_cast(A.data()), thrust::raw_pointer_cast(B.data()), thrust::raw_pointer_cast(C.data()), N);
}

int main(int argc, const char** argv) {
    if (argc != 6) {
        std::cout << "You should specify start size, end size, step size of matrix and output filename and use "
                     "parallel or not!"
                  << std::endl;
        return -1;
    }

    srand(time(0));
    std::ostringstream output;
    output << "count,allocation_time,fill_time,calculation_time\n";

    for (size_t N = std::atoi(argv[1]); N < std::atoi(argv[2]); N += std::atoi(argv[3])) {
        auto start = std::chrono::system_clock::now();
        thrust::device_vector<float> A(N * N);
        thrust::device_vector<float> B(N * N);
        thrust::device_vector<float> C(N * N);
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        output << N << ',' << elapsed << ',';

        start = std::chrono::system_clock::now();
        thrust::transform(thrust::make_counting_iterator(0ULL), thrust::make_counting_iterator(N * N), A.begin(),
                          GenRand(rand()));
        thrust::transform(thrust::make_counting_iterator(0ULL), thrust::make_counting_iterator(N * N), B.begin(),
                          GenRand(rand()));
        end = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        output << elapsed << ',';

        bool use_mp = std::atoi(argv[5]);
        start = std::chrono::system_clock::now();
        matrixMultiplication(A, B, C, N, use_mp);
        cudaDeviceSynchronize();
        end = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        output << elapsed << '\n';
    }
    std::ofstream output_file(std::string(argv[4]) + ".csv");
    output_file << output.str();

    return 0;
}

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <omp.h>

void multiplyOMP(const std::vector<std::vector<float>>& A, const std::vector<std::vector<float>>& B,
                 std::vector<std::vector<float>>& C) {
#pragma omp parallel for num_threads(12)
    for (int i = 0; i < A.size(); i++) {
        for (int k = 0; k < B.size(); k++) {
            for (int j = 0; j < A.front().size(); j++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void multiply(const std::vector<std::vector<float>>& A, const std::vector<std::vector<float>>& B,
              std::vector<std::vector<float>>& C) {
    for (int i = 0; i < A.size(); i++) {
        for (int k = 0; k < B.size(); k++) {
            for (int j = 0; j < A.front().size(); j++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void printProgress(double progress) {
    static int barWidth = 70;
    std::cout << "\r[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) {
            std::cout << "=";
        } else if (i == pos) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << int(progress * 100.0) << " %";
    std::cout.flush();
}

int main(int argc, const char** argv) {
    if (argc != 6) {
        std::cout << "You should specify start size, end size, step size of matrix and output filename and use "
                     "parallel or not!"
                  << std::endl;
        return -1;
    }
    std::ostringstream output;
    output << "count,allocation_time,fill_time,calculation_time\n";

    size_t left = std::atoi(argv[1]), right = std::atoi(argv[2]), step = std::atoi(argv[3]);
    size_t block_count = 0, current = 0;
    double progress = 0;

    for (size_t N = left; N <= right; N += step) {
        block_count += N * N;
    }

    for (size_t N = left; N <= right; N += step) {
        progress = static_cast<double>(current) / static_cast<double>(block_count);
        printProgress(progress);
        std::mt19937 random(time(0));
        std::uniform_real_distribution<float> dist(1, 10);
        auto start_serial = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<float>> A(N, std::vector<float>(N));
        std::vector<std::vector<float>> B(N, std::vector<float>(N));
        std::vector<std::vector<float>> C(N, std::vector<float>(N));
        auto end_serial = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_serial - start_serial).count();

        output << N << ',' << elapsed << ',';

        start_serial = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = dist(random);
                B[i][j] = dist(random);
            }
        }
        end_serial = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_serial - start_serial).count();

        output << elapsed << ',';

        bool use_mp = std::atoi(argv[5]);
        start_serial = std::chrono::high_resolution_clock::now();
        if (use_mp) {
            multiplyOMP(A, B, C);
        } else {
            multiply(A, B, C);
        }
        end_serial = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_serial - start_serial).count();

        output << elapsed << '\n';
        current += N * N;
    }
    printProgress(progress);
    std::cout << std::endl;
    std::ofstream output_file(std::string(argv[4]) + ".csv");
    output_file << output.str();
    return 0;
}

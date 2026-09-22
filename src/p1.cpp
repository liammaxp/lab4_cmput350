#include <cstdint>
#include <iostream>
#include <random>
#include "timer.h"

int main() { 
  const std::size_t rows = 4000;
  const std::size_t cols = 4000;
  const std::size_t size = rows * cols;
  uint64_t* rowMajor = new uint64_t[size];
  uint64_t* columnMajor = new uint64_t[size];
  std::mt19937_64 generator(0);
  for (std::size_t row = 0; row < rows; ++row) {
    for (std::size_t col = 0; col < cols; ++col) {
        rowMajor[row * cols + col] = generator();
    }
  }
  generator.seed(0);
  for (std::size_t col = 0; col < cols; ++col) {
      for (std::size_t row = 0; row < rows; ++row) {
          columnMajor[row *cols + col] = generator();
      }
  }
  Timer timer;
  uint64_t rowSum = 0;
  for (std::size_t row = 0; row < rows; ++row) {
      for (std::size_t col = 0; col < cols; ++col) {
          rowSum += rowMajor[row *cols + col];
      }
  }
  auto rowTime = timer.click<Timer::Micros>();
  uint64_t columnSum = 0;
  for (std::size_t col = 0; col < cols; ++col) {
      for (std::size_t row = 0; row < rows; ++row) {
          columnSum += columnMajor[row *cols + col];
      }
  }
  auto columnTime = timer.click<Timer::Micros>();

  std::cout << rowTime << " " << rowSum << '\n';
  std::cout << columnTime << " " << columnSum << '\n';
  delete[] rowMajor;
  delete[] columnMajor;
  return 0; }

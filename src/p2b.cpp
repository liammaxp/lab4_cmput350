#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include "p2a.h"
#include "timer.h"

const std::size_t X = 256;
const std::size_t Y = 256;
const std::size_t Z = 256;
const std::size_t OUT_X = 64;
const std::size_t OUT_Y = 64;
const std::size_t OUT_Z = 64;

std::size_t rowMajorIndexA(std::size_t x, std::size_t y, std::size_t z){ //change 3D position into row major index
  return z * Y * X + y * X + x;
}
std::size_t rowMajorIndexK(std::size_t x, std::size_t y, std::size_t z){//row major index for kernel
  return z * 4 * 4 + y * 4 + x;
}
std::size_t rowMajorIndexConv(std::size_t x, std::size_t y, std::size_t z){//row major index for convolution result
  return z * OUT_Y * OUT_X + y * OUT_X + x;
}
int main() {
  const std::size_t size = X * Y * Z;
  uint64_t* A = new uint64_t[size];
  uint64_t* B = new uint64_t[size];
  std::mt19937_64 generator(0);
  for (std::size_t i = 0; i < size; ++i) {
    A[i] = generator();
  }
  for (std::size_t z = 0; z < Z; ++z) { //copy A into B by morton order
    for (std::size_t y = 0; y < Y; ++y) {
      for (std::size_t x = 0; x < X; ++x) {
        B[morton3d(x, y, z)] = A[rowMajorIndexA(x, y, z)];
      }
    }
  }
  const std::size_t kernelSize = 4 * 4 * 4;
  uint64_t* Ka = new uint64_t[kernelSize];
  uint64_t* Kb = new uint64_t[kernelSize];

  for (std::size_t z = 0; z < 4; ++z) {//make two kernels with same values but different layout
    for (std::size_t y = 0; y < 4; ++y) {
      for (std::size_t x = 0; x < 4; ++x) {
        uint64_t value = x + y + z;
        Ka[rowMajorIndexK(x, y, z)] = value;
        Kb[morton3d(x, y, z)] = value;
      }
    }
  }
  const std::size_t outputSize = OUT_X * OUT_Y * OUT_Z;
  uint64_t* convA = new uint64_t[outputSize];
  uint64_t* convB = new uint64_t[outputSize];
  Timer timer;
  for (std::size_t oz = 0; oz < OUT_Z; ++oz) {
    for (std::size_t oy = 0; oy < OUT_Y; ++oy) {
      for (std::size_t ox = 0; ox < OUT_X; ++ox) {
        uint64_t sum = 0;
        for (std::size_t kz = 0; kz < 4; ++kz) {
          for (std::size_t ky = 0; ky < 4; ++ky) {
            for (std::size_t kx = 0; kx < 4; ++kx) {//convolution for row major array
              std::size_t x = ox * 4 + kx;
              std::size_t y = oy * 4 + ky;
              std::size_t z = oz * 4 + kz;
              sum += A[rowMajorIndexA(x, y, z)] * Ka[rowMajorIndexK(kx, ky, kz)];
            }
          }
        }
        convA[rowMajorIndexConv(ox, oy, oz)] = sum;
      }
    }
  }
  auto rowMajorTime = timer.click<Timer::Micros>();
  for (std::size_t block = 0; block < outputSize; ++block) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < kernelSize; ++i) {
      sum += B[block * kernelSize + i] * Kb[i];
    }
    convB[block] = sum;
  }
  auto mortonTime = timer.click<Timer::Micros>();
  for (std::size_t z = 0; z < OUT_Z; ++z) { //check both convolution results are same
    for (std::size_t y = 0; y < OUT_Y; ++y) {
      for (std::size_t x = 0; x < OUT_X; ++x) {
        assert(convA[rowMajorIndexConv(x, y, z)] == convB[morton3d(x, y, z)]);
      }
    }
  }
  std::cout << rowMajorTime << '\n';
  std::cout << mortonTime << '\n';
  delete[] A;
  delete[] B;
  delete[] Ka;
  delete[] Kb;
  delete[] convA;
  delete[] convB;
  return 0;}
#pragma once
#include <cmath>
#include <vector>

std::vector<int> Factorize(int number) {
  std::vector<int> factors;
  int number_copy = number;
  for (int i = 2; i <= sqrt(number_copy); ++i) {
    while (number % i == 0) {
      number /= i;
      factors.push_back(i);
    }
  }
  if (number != 1) {
    factors.push_back(number);
  }
  return factors;
}
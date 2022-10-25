#pragma once

int* LowerBound(const int* first, const int* last, int value) {
  int dist = last - first;
  int* mid = first + dist / 2;
  while (first != last) {
    if (*mid < value) {
      first = ++mid;
    }
    else {
      last = mid;
    }
    dist = last - first;
    mid = first + dist / 2;
  }
  return first;
}
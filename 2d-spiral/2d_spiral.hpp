int** MakeSpiral(int n) {
  int** spiral = new int*[n];
  for (int i = 0; i < n; ++i) {
    spiral[i] = new int[n];
  }
  int start = 0;
  int finish = n - 1;
  int temp_num = 1;
  while (temp_num <= n * n) {
    for (int i = start; i <= finish; ++i) {
      spiral[start][i] = temp_num++;
    }
    for (int j = start + 1; j <= finish; ++j) {
      spiral[j][finish] = temp_num++;
    }
    for (int i = finish - 1; i >= start; --i) {
      spiral[finish][i] = temp_num++;
    }
    for (int j = finish - 1; j >= start + 1; --j) {
      spiral[j][start] = temp_num++;
    }
    finish--;
    start++;
  }
  return spiral;
}
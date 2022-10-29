int** MakeSpiral(int n) {
  int** spiral = new int* [n];
  for (int i = 0; i < n; ++i) {
    spiral[i] = new int[n];
  }
  int row_start = 0;
  int col_start = 0;
  int row_finish = n - 1;
  int col_finish = n - 1;
  int temp_num = 1;
  while (temp_num <= n * n) {
    for (int i = col_start; i <= col_finish; ++i) {
      spiral[row_start][i] = temp_num++;
    }
    for (int j = row_start + 1; j <= row_finish; ++j) {
      spiral[j][col_finish] = temp_num++;
    }
    for (int i = col_finish - 1; i >= col_start; --i) {
      spiral[row_finish][i] = temp_num++;
    }
    for (int j = row_finish - 1; j >= row_start + 1; --j) {
      spiral[j][col_start] = temp_num++;
    }
    row_start++;
    row_finish--;
    col_start++;
    col_finish--;
  }
  return spiral;
}
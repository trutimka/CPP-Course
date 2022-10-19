int** MakeSpiral(int n) {
  int** spiral = new int* [n];
  for (int i = 0; i < n; ++i) {
    spiral[i] = new int[n];
  }
  int row1 = 0;
  int col1 = 0;
  int row2 = n - 1;
  int col2 = n - 1;
  int iter = 1;
  while (iter <= (n * n)) {
    for (int i = col1; i <= col2; i++) {
      spiral[row1][i] = iter++;
    }
    for (int j = row1 + 1; j <= row2; j++) {
      spiral[j][col2] = iter++;
    }
    for (int i = col2 - 1; i >= col1; i--) {
      spiral[row2][i] = iter++;
    }
    for (int j = row2 - 1; j >= row1 + 1; j--) {
      spiral[j][col1] = iter++;
    }
    row1++;
    row2--;
    col1++;
    col2--;
  }
  return spiral;
}
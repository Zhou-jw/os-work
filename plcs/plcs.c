#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "thread.h"
#include "thread-sync.h"

#define MAXN 10000
int T, N, M;
char A[MAXN + 1], B[MAXN + 1];
int dp[MAXN][MAXN];
int result;

#define DP(x, y) (((x) >= 0 && (y) >= 0) ? dp[x][y] : 0)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MAX3(x, y, z) MAX(MAX(x, y), z)

void Tworker(int id) {
  if (id != 1) {
    // This is a serial implementation
    // Only one worker needs to be activated
    return;
  }

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      // Always try to make DP code more readable
      int skip_a = DP(i - 1, j);
      int skip_b = DP(i, j - 1);
      int take_both = DP(i - 1, j - 1) + (A[i] == B[j]);
      dp[i][j] = MAX3(skip_a, skip_b, take_both);
    }
  }

  result = dp[N - 1][M - 1];
}

int j, k;
void plcs() {
  int skip_a = DP(j - 1, k);
  int skip_b = DP(j, k - 1);
  int take_both = DP(j - 1, k - 1) + (A[j] == B[k]);
  dp[j][k] = MAX3(skip_a, skip_b, take_both);
}

int main(int argc, char *argv[]) {
  // No need to change
  FILE *fp;
  fp = fopen("/home/zjw/os-workbench/plcs/test.txt", "r");
  assert(fscanf(fp, "%s%s", A, B) == 2);
  // printf("%s, %s\n", A, B);
  N = strlen(A);
  M = strlen(B);
  // printf("%d\n", dp[N-1][M-1]);
  T = !argv[1] ? 1 : atoi(argv[1]);
  printf("T is %d", T);
  // Add preprocessing code here

  // for (int i = 0; i < T; i++) {
  //   create(Tworker);
  // }

  for(int i = 0; i < N + M - 1; i++) {
    for(j = 0; j <= i; j++) {
      if(j > N-1) break;
      for(k = i; k >= 0; k--) {
        if(k > M-1) break;
        create(plcs);
      }
    }
    join();
  }
  join();  // Wait for all workers
  result = dp[N-1][M-1];
  printf("your answer is %d\n", result);
  
  create(Tworker);
  join();
  printf("correct answer is %d\n", result);
  return 0;
}

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread-sync.h"
#include "thread.h"

#define MAXN 10000
int T, N, M, Round;
char A[MAXN + 1], B[MAXN + 1];
int dp[MAXN][MAXN], flag[2 * MAXN];
int result;

#define DP(x, y) (((x) >= 0 && (y) >= 0) ? dp[x][y] : 0)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MAX3(x, y, z) MAX(MAX(x, y), z)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

mutex_t lk = MUTEX_INIT();
cond_t cv = COND_INIT();
_Atomic int done = 0;

void test() { return; }

void Ttime(int id) {
  float ms = 0;
  while (1) {
    if (atomic_load(&done) == 1) break;
    usleep(1000);
    ms += 1.0;
  }
  printf("Approximate running time: %.1lfs\n", ms / 1000);
}

void Tworker(int id) {
  float ms = 0;
  // if (id != 2) {
  //   // This is a serial implementation
  //   // Only one worker needs to be activated
  //   return;
  // }

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
  atomic_fetch_add(&done, 1);
}
static inline int belongs(int num, int tid) {
  return (num / 2000) % T == tid - 2;
}

static inline int can_calculate(int r) {
  int ok = 0;
  if (r == 0)
    ok = 1;
  else if (0 < r && r < MIN(N, M) && (r - 1) + 1 == flag[r - 1]) {
    ok = 1;
  } else if (MIN(N, M) <= r && r < MAX(N, M) && MIN(N, M) == flag[r - 1]) {
    ok = 1;
  } else if (MAX(N, M) <= r && flag[r - 1] == Round - (r - 1)) {
    ok = 1;
  }
  return ok;
}

void plcs_thread(int tid) {
  // printf("tid is %d\n\n", tid);
  for (int r = 0; r < Round; r++) {
    mutex_lock(&lk);
    while (!can_calculate(r)) {
      cond_wait(&cv, &lk);
    }
    mutex_unlock(&lk);
    int cnt = 0, num = 0;
    for (int i = 0; i < N; i++) {
      int j = r - i;
      if (j < 0) break;
      if (0 <= j && j < M) cnt++;
      if (0 <= j && j < M && belongs(cnt, tid)) {
        int skip_a = DP(i - 1, j);
        int skip_b = DP(i, j - 1);
        int take_both = DP(i - 1, j - 1) + (A[i] == B[j]);
        dp[i][j] = MAX3(skip_a, skip_b, take_both);
        num++;
        // printf("tid is %d\n", tid);
        // printf("skip_a is %d\n", skip_a);
        // printf("skip_b is %d\n", skip_b);
        // printf("take_both is %d\n", take_both);
        // printf("r is %d, dp[%d][%d] is %d\n", r, i, j, dp[i][j]);
      }
    }
    // assert(r <= Round);
    mutex_lock(&lk);
    flag[r] += num;
    cond_broadcast(&cv);
    mutex_unlock(&lk);
  }
  done = 1;
  result = dp[N - 1][M - 1];
}

int main(int argc, char *argv[]) {
  // No need to change
  FILE *fp;
  fp = fopen("test.txt", "r");
  assert(fscanf(fp, "%s%s", A, B) == 2);
  // printf("%s, %s\n", A, B);
  N = strlen(A);
  M = strlen(B);
  Round = M + N - 1;
  // printf("%d\n", dp[N-1][M-1]);
  T = !argv[1] ? 1 : atoi(argv[1]);
  printf("T is %d\n", T);
  create(Ttime);
  // Add preprocessing code here

  for (int i = 0; i < T; i++) {
    create(plcs_thread);  // plcs_thread's tid begins with 2; Ttime(tid = 1)
  }
  join();
  printf("your answer is %d\n", result);

  // create(Tworker);
  // join();
  // printf("correct answer is %d\n", result);
  return 0;
}
/*
openssl rand --hex [num/2] will produce num of chars
openssl rand --hex [num/2] >> test.txt && openssl rand --hex [num/2] >> test.txt
openssl rand --hex 5000 >> test.txt && openssl rand --hex 5000 >> test.txt
*/
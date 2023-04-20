#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "thread.h"
#include "thread-sync.h"


#define MAXN 10000
int T, N, M, Round;
char A[MAXN + 1], B[MAXN + 1];
int dp[MAXN][MAXN], dp_flag[MAXN][MAXN];
int result;

#define DP(x, y) (((x) >= 0 && (y) >= 0) ? dp[x][y] : 0)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MAX3(x, y, z) MAX(MAX(x, y), z)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

mutex_t lk = MUTEX_INIT();
cond_t cv = COND_INIT();
_Atomic int done = 0;
int last_r_done = 0, this_r_done = 0, r = 0;

void Ttime(int id) {
  float ms = 0;
  while (1) {
    if (atomic_load(&done) == 1) break;
    usleep(1000 / 2);
    ms += 1000.0 / 2;
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
static inline int belongs(int thisr, int tid) { return thisr % T == tid - 2; }

static inline int can_calculate(int r, int r_done) {
  if (r <= MIN(N, M) && r_done == r) {
    return 1;
  } else if (MIN(N, M) < r && r <= Round - MAX(N, M) && r_done == MIN(N, M)) {
    return 1;
  } else if (MAX(M, N) < r && r_done == Round - r + 1) {
    return 1;
  }
  return 0;
}

void plcs_thread(int tid) {
  while (1) {
    mutex_lock(&lk);
    if (done) return;
    while (!can_calculate(r, last_r_done)) {
      cond_wait(&cv, &lk);
    }
    mutex_unlock(&lk);
    int num = 0;
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++) {
        if (i + j == r && belongs(num, tid)) {
          int skip_a = DP(i - 1, j);
          int skip_b = DP(i, j - 1);
          int take_both = DP(i - 1, j - 1) + (A[i] == B[j]);
          dp[i][j] = MAX3(skip_a, skip_b, take_both);
          num++;
          break;
        }
        if (i + j > r) break;
      }
    }
    mutex_lock(&lk);
    this_r_done += num;
    assert(num <= MAX(N, M));
    cond_broadcast(&cv);
    if (can_calculate(r + 1, this_r_done)) {
      last_r_done = this_r_done;
      this_r_done = 0;
      r++;
    }
    if (r == Round) {
      done = 1;
      result = dp[N - 1][M - 1];
    }
    mutex_unlock(&lk);
  }
}

int main(int argc, char *argv[]) {
  // No need to change
  FILE *fp;
  fp = fopen("/home/zjw/os-workbench/plcs/test1.txt", "r");
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

  // for (int i = 0; i < T; i++) {
  //   create(plcs_thread);  // plcs_thread's tid begins with 2; Ttime(tid = 1)
  // }
  create(plcs_thread);
  create(plcs_thread);
  join();
  printf("your answer is %d\n", dp[N - 1][M - 1]);

  create(Tworker);
  join();
  printf("correct answer is %d\n", result);
  return 0;
}
/*
openssl rand --hex [num/2] will produce num of chars
openssl rand --hex [num/2] >> test.txt && openssl rand --hex [num/2] >> test.txt
openssl rand --hex 5000 >> test.txt && openssl rand --hex 5000 >> test.txt
*/
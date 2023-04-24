#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread-sync.h"
#include "thread.h"

#define MAXN 10000
int T, N, M, Round;
char A[MAXN + 1], B[MAXN + 1];
int dp[MAXN][MAXN], dp_flag[MAXN][MAXN], flag[2 * MAXN];
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
    usleep(100000 / 2);
    ms += 100.0 / 2;
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
  int ok = 1;
  if(r == 0) return ok;
  if (!flag[r - 1]) {
    for (int i = 0; i < N; i++) {
      int j = r - i - 1;
      if (0 <= j && j < M) {
        if (!dp_flag[i][j]) {
          ok = 0;
        }  // every dp[i][j] in last round is done
      }
    }
    if(ok) flag[r - 1] = 1;
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
    int num = 0;
    for (int i = 0; i < N; i++) {
      int j = r - i;
      if (0 <= j && j < M) num++;
      if (0 <= j && j < M && belongs(num, tid)) {
        int skip_a = DP(i - 1, j);
        int skip_b = DP(i, j - 1);
        int take_both = DP(i - 1, j - 1) + (A[i] == B[j]);
        dp[i][j] = MAX3(skip_a, skip_b, take_both);
        dp_flag[i][j] = 1;
        // printf("tid is %d\n", tid);
        // printf("skip_a is %d\n", skip_a);
        // printf("skip_b is %d\n", skip_b);
        // printf("take_both is %d\n", take_both);
        // printf("r is %d, dp[%d][%d] is %d\n", r, i, j, dp[i][j]);
      }
    }
    // assert(r <= Round);
    mutex_lock(&lk);
    cond_broadcast(&cv);
    mutex_unlock(&lk);
  }
  done = 1;
  result = dp[N - 1][M - 1];
}

int main(int argc, char *argv[]) {
  // No need to change
  FILE *fp;
  fp = fopen("/home/zjw/os-workbench/plcs/test.txt", "r");
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
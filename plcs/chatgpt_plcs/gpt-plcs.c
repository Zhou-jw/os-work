#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHUNK_SIZE 1000
#define MAX_N 10000

int lcs[MAX_N][MAX_N] = {0};

typedef struct {
  int start, end;
  int tid;
  char* str1;
  char* str2;
} Chunk;

void* lcs_thread(void* arg) {
  Chunk* ch = (Chunk*)arg;
  int start = ch->start, end = ch->end, tid = ch->tid;
  char* str1 = ch->str1;
  char* str2 = ch->str2;
  int n2 = strlen(str2);

  int dp[MAX_CHUNK_SIZE][MAX_N] = {0};
  int dp_prev[MAX_CHUNK_SIZE][MAX_N] = {0};
  for (int j = 0; j <= n2; j++) {
    dp[0][j] = 0;
  }

  for (int i = start; i < end; i++) {
    for (int j = 0; j <= n2; j++) {
      dp_prev[i % 2][j] = dp[i % 2][j];
      if (str1[i] == str2[j]) {
        dp[i % 2][j] = dp_prev[(i - 1) % 2][j - 1] + 1;
      } else {
      }
      dp[i % 2][j] = (dp[i % 2][j - 1] >= dp_prev[i % 2][j])
                         ? dp[i % 2][j - 1]
                         : dp_prev[i % 2][j];
    }
  }

  for (int i = start; i < end; i++) {
    for (int j = 0; j <= n2; j++) {
      lcs[i][j] = dp[i % 2][j];
    }
  }

  pthread_exit(NULL);
}

int main() {
  char str1[MAX_N], str2[MAX_N];
  FILE* fp;
  fp = fopen("test.txt", "r");
  assert(fscanf(fp, "%s%s", str1, str2) == 2);
  int n1 = strlen(str1), n2 = strlen(str2);

  int num_threads = (n1 + MAX_CHUNK_SIZE - 1) / MAX_CHUNK_SIZE;
  Chunk chunks[num_threads];

  // Divide the input string str1 into chunks
  int chunk_size = n1 / num_threads;
  for (int i = 0; i < num_threads; i++) {
    chunks[i].tid = i;
    chunks[i].str1 = str1;
    chunks[i].str2 = str2;
    chunks[i].start = i * chunk_size;
    chunks[i].end =
        (i == (num_threads - 1)) ? n1 : chunks[i].start + chunk_size;
  }

  // Create threads for each chunk and execute the DP algorithm concurrently
  pthread_t threads[num_threads];
  for (int i = 0; i < num_threads; i++) {
    pthread_create(&threads[i], NULL, lcs_thread, &chunks[i]);
  }

  // Wait for all threads to finish
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  // Merge the LCS results from each thread to get the final LCS of the input
  // strings
  int res = lcs[n1 - 1][n2 - 1];
  printf("The length of the longest common subsequence is: %d\n", res);

  return 0;
}
/*
In this code, we have created a Chunk struct that
contains the start and end indices of a chunk of str1 along with the tid, str1
and str2. Later, we have created num_threads threads, and each thread performs
dynamic programming on the corresponding chunk of str1. After that, each thread
stores its computed results in a separate shared memory lcs. Finally, we have
merged the LCS results from each thread and printed the final result. In this
code, we have created a Chunk struct that

*/
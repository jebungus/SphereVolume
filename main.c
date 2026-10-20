#include <pthread.h>

#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

#include <math.h>

#include <time.h>

#include <unistd.h>

#include <threads.h>

#define NUMBER_OF_POINTS 50000000

#define NUMBER_OF_THREADS 4

/* the global variable number of hits in the sphere */

int sphere_count = 0;
// global for total threads
int upper = 1;
int lower = 0;
int counter = 0;
// using lcg64_temper to get random seed for each thread
//  credit: http://stackoverflow.com/a/19083740/2635342

static int32_t temper(int32_t x) {
  x ^= x >> 11;
  x ^= x << 7 & 0x9D2C5680;
  x ^= x << 15 & 0xEFC60000;
  x ^= x >> 18;
  return x;
}

int32_t lcg64_temper(uint64_t *seed) {
  *seed = 6364136223846793005ULL * *seed + 1;
  return temper(*seed >> 32);
}

int threadFunction(void *data) {
  int32_t id = *(int32_t *)data;
  printf("thread %d up\n", id);

  uint64_t threadSeed = time(NULL) ^ id;

  int32_t n = lcg64_temper(&threadSeed) % 20 + 1;

  // divided n by 10 in thrd_sleep to allow for shorter runtime
  thrd_sleep(&(struct timespec){.tv_sec = n / 10},
             NULL); // Sleep for a random number of seconds.
  // random number between 0 and 1
  double x;
  double y;
  double z;

  while (counter < NUMBER_OF_POINTS) {
    // random number between 0 and 1
    x = (double)rand() / (double)RAND_MAX;
    y = (double)rand() / (double)RAND_MAX;
    z = (double)rand() / (double)RAND_MAX;

    // check if in sphere
    if (sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) <= 1) {
      sphere_count++;
    }

    counter++;
  }
  // printf("%f, %f, %f\n", x, y, z);

  printf("thread %d done\n", id);
  return 0;
}

int main(void) {
  int i;

  // clock start time
  clock_t start, end;
  double cpu_time_used;

  start = clock();

  // number of threads
  const size_t NUM_THREADS = NUMBER_OF_THREADS;

  int32_t threadData[NUM_THREADS];
  thrd_t threads[NUM_THREADS];

  // Initialize data for each thread.
  for (size_t i = 0; i < NUM_THREADS; ++i) {
    threadData[i] = i;
  }

  // Create the threads.
  for (size_t i = 0; i < NUM_THREADS; ++i) {
    if (thrd_create(threads + i, threadFunction, threadData + i) !=
        thrd_success) {
      printf("thread: %zu - thread create error\n", i);
      return 1;
    }
  }

  // Wait for all threads to complete.
  for (size_t i = 0; i < NUM_THREADS; ++i)
    thrd_join(threads[i], NULL);
  printf("\nPoints in Sphere : %d\n", sphere_count);
  printf("Total Points : %d\n", NUMBER_OF_POINTS);

  double vol = (8.0 * sphere_count) / NUMBER_OF_POINTS;
  printf("\nEstimated Volume: %f\n", vol);

  double actual = (4.0 / 3.0) * 3.14159265 * 1;
  printf("Actual Volume: %f\n\n", actual);

  // clock end time
  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Time taken to execute in seconds : %f seconds\n", cpu_time_used);
  return 0;
}
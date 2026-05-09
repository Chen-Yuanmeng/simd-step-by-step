#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <smmintrin.h>

#ifdef __MINGW32__
#include <malloc.h>
#define aligned_malloc(size) _aligned_malloc(size, 64)
#define aligned_free(ptr)    _aligned_free(ptr)
#else
#define aligned_malloc(size) aligned_alloc(64,(size))
#define aligned_free(ptr)    free(ptr)
#endif

#define N 4096
#define BLOCK 64
#define VLEN 4

int main(void) {
    int32_t *A = (int32_t *)aligned_malloc(N * N * sizeof(int32_t));
    int32_t *B = (int32_t *)aligned_malloc(N * N * sizeof(int32_t));
    int32_t *C = (int32_t *)aligned_malloc(N * N * sizeof(int32_t));

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i * N + j] = (i + j) % 10;
            B[i * N + j] = (i + j) % 10;
        }

    for (int i = 0; i < N * N; i++)
        C[i] = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int ib = 0; ib < N; ib += BLOCK)
        for (int kb = 0; kb < N; kb += BLOCK)
            for (int jb = 0; jb < N; jb += BLOCK)
                for (int i = ib; i < ib + BLOCK; i++)
                    for (int k = kb; k < kb + BLOCK; k++) {
                        __m128i aik = _mm_set1_epi32(A[i * N + k]);
                        for (int j = jb; j < jb + BLOCK; j += VLEN) {
                            __m128i c = _mm_loadu_si128((__m128i *)(C + i * N + j));
                            __m128i b = _mm_loadu_si128((__m128i *)(B + k * N + j));
                            __m128i mul = _mm_mullo_epi32(b, aik);
                            c = _mm_add_epi32(c, mul);
                            _mm_storeu_si128((__m128i *)(C + i * N + j), c);
                        }
                    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec)
                   + (end.tv_nsec - start.tv_nsec) / 1e9;

    int64_t sum = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            sum += C[i * N + j];

    printf("checksum: %ld\n", (long)(sum % 100000));
    printf("time: %.6f\n", elapsed);

    aligned_free(A);
    aligned_free(B);
    aligned_free(C);
    return 0;
}

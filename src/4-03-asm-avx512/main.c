#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define N 1024
#define BLOCK 64
#define VLEN 16

int main(void) {
    int32_t *A = (int32_t *)aligned_alloc(64, N * N * sizeof(int32_t));
    int32_t *B = (int32_t *)aligned_alloc(64, N * N * sizeof(int32_t));
    int32_t *C = (int32_t *)aligned_alloc(64, N * N * sizeof(int32_t));

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
                        int32_t a_val = A[i * N + k];
                        for (int j = jb; j < jb + BLOCK; j += VLEN) {
                            int32_t *c_ptr = C + i * N + j;
                            int32_t *b_ptr = B + k * N + j;
                            __asm__ __volatile__ (
                                "vmovd          %[a], %%xmm2\n\t"
                                "vpbroadcastd   %%xmm2, %%zmm2\n\t"
                                "vmovdqu32      (%[c]), %%zmm0\n\t"
                                "vmovdqu32      (%[b]), %%zmm1\n\t"
                                "vpmulld        %%zmm2, %%zmm1, %%zmm1\n\t"
                                "vpaddd         %%zmm1, %%zmm0, %%zmm0\n\t"
                                "vmovdqu32      %%zmm0, (%[c])\n\t"
                                :
                                : [c] "r"(c_ptr), [b] "r"(b_ptr), [a] "rm"(a_val)
                                : "xmm0", "xmm1", "xmm2", "memory"
                            );
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

    free(A);
    free(B);
    free(C);
    return 0;
}

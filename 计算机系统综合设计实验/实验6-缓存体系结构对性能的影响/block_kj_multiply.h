
#ifndef __MATMUL_BLOCK_KJ_MULTIPLY_H__
#define __MATMUL_BLOCK_KJ_MULTIPLY_H__

void multiply(double** A, double** B, double** C, int matrix_size, int block_size)
{
    // TODO: Put your matrix multiplication code with blocking over k, j here.
    int i, j, k, kk, jj;
    int en = block_size * (matrix_size / block_size);
    
    for (i = 0; i < matrix_size; i++)
        for (j = 0; j < matrix_size; j++)
            C[i][j] = 0.0;
    
    for (kk = 0; kk < en; kk += block_size) {
        for (jj = 0; jj < en; jj += block_size) {
            for (i = 0; i < matrix_size; i++) {
                for (j = jj; j < jj + block_size; j++) {
                    for (k = kk; k < kk + block_size; k++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
        }
    }
}

#endif // __MATMUL_BLOCK_KJ_MULTIPLY_H__

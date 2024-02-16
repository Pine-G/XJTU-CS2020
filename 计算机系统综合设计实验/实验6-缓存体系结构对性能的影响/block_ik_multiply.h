
#ifndef __MATMUL_BLOCK_IK_MULTIPLY_H__
#define __MATMUL_BLOCK_IK_MULTIPLY_H__

void multiply(double** A, double** B, double** C, int matrix_size, int block_size)
{
    // TODO: Put your matrix multiplication code with blocking over i, k here.
    int i, j, k, ii, kk;
    int en = block_size * (matrix_size / block_size);
    
    for (i = 0; i < matrix_size; i++)
        for (j = 0; j < matrix_size; j++)
            C[i][j] = 0.0;
    
    for (ii = 0; ii < en; ii += block_size) {
        for (kk = 0; kk < en; kk += block_size) {
            for (j = 0; j < matrix_size; j++) {
                for (k = kk; k < kk + block_size; k++) {
                    for (i = ii; i < ii + block_size; i++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
        }
    }
}

#endif // __MATMUL_BLOCK_IK_MULTIPLY_H__

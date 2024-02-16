
#ifndef __MATMUL_BLOCK_IJ_MULTIPLY_H__
#define __MATMUL_BLOCK_IJ_MULTIPLY_H__

void multiply(double** A, double** B, double** C, int matrix_size, int block_size)
{
    // TODO: Put your matrix multiplication code with blocking over i, j here.
    int i, j, k, ii, jj;
    int en = block_size * (matrix_size / block_size);
    
    for (i = 0; i < matrix_size; i++)
        for (j = 0; j < matrix_size; j++)
            C[i][j] = 0.0;
    
    for (ii = 0; ii < en; ii += block_size) {
        for (jj = 0; jj < en; jj += block_size) {
            for (k = 0; k < matrix_size; k++) {
                for (j = jj; j < jj + block_size; j++) {
                    for (i = ii; i < ii + block_size; i++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
        }
    }
}

#endif // __MATMUL_BLOCK_IJ_MULTIPLY_H__

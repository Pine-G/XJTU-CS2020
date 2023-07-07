import numpy as np


def affine_transform(A, x):
    if A.shape != (2, 3):
        raise ValueError('The input A should be in 2x3 format!')

    if x.shape[1] != 2:
        raise ValueError('The input x should be in Nx2 format!')
    else:
        x = np.hstack([x, np.ones((x.shape[0], 1))])

    return np.dot(A, x.T).T

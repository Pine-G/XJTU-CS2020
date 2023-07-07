import numpy as np


def transform(A, x):
    if A.shape != (3, 3):
        raise ValueError('The input A should be in 3x3 format!')

    if x.shape[1] != 2:
        raise ValueError('The input x should be in Nx2 format!')
    else:
        x = np.hstack([x, np.ones((x.shape[0], 1))])

    y = np.dot(A, x.T).T
    y /= y[:, 2].reshape(-1, 1) + 1e-20

    return y[:, :2]

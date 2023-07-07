import numpy as np


def GetHomography(p1, p2):
    if p1.shape != p2.shape:
        raise ValueError('The points should be given in pair!')
    if p1.shape != (4, 2):
        raise ValueError('The points should be given in 4x2 format!')

    A = np.zeros((8, 9))
    for i in range(4):
        x1, y1 = p1[i]
        x2, y2 = p2[i]
        A[2 * i] = [x1, y1, 1, 0, 0, 0, -x1 * x2, -y1 * x2, -x2]
        A[2 * i + 1] = [0, 0, 0, x1, y1, 1, -x1 * y2, -y1 * y2, -y2]

    U, S, V = np.linalg.svd(A)
    H = V[-1, :].reshape((3, 3))

    return H

import numpy as np


def est_optimal_affine(p1, p2):
    # Check the input
    if p1.shape != p2.shape:
        raise ValueError('The points should be given in pair!')
    if p1.shape != (3, 2):
        raise ValueError('The points should be given in 3x2 format!')

    # Compute the affine transformation matrix
    A = np.zeros((2, 3))

    # Build the linear system of equations
    M = np.zeros((6, 6))
    b = np.zeros((6, 1))
    for i in range(3):
        x1, y1 = p1[i, :]
        x2, y2 = p2[i, :]
        M[2 * i, :] = [x1, y1, 1, 0, 0, 0]
        M[2 * i + 1, :] = [0, 0, 0, x1, y1, 1]
        b[2 * i, 0] = x2
        b[2 * i + 1, 0] = y2

    # Solve the linear system of equations
    x = np.linalg.solve(M, b)

    # Extract the affine transformation matrix
    A[0, :] = [x[0, 0], x[1, 0], x[2, 0]]
    A[1, :] = [x[3, 0], x[4, 0], x[5, 0]]

    return A

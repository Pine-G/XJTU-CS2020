import numpy as np
from Homography import GetHomography
from Transform import transform


def ransac(points1, points2, trials):
    # Check the input
    if points1.shape != points2.shape:
        raise ValueError('The points should be given in pair!')
    if points1.shape[1] != 2:
        raise ValueError('The points should be given in Nx2 format!')
    if points1.shape[0] < 4:
        raise ValueError('At least 4 points are required!')

    # Check the number of trials
    trials = 20 if trials < 20 else trials

    # Set the random state
    seed = 1234
    np.random.seed(seed)

    # Do the random experiments
    inliers = []
    num_inliers = np.zeros(trials)
    th_inliner = 20
    for i in range(trials):
        # Randomly picking 4 points
        x = np.random.choice(points1.shape[0], 4)

        # Estimate the Homography matrix
        A = GetHomography(points1[x, :], points2[x, :])

        # Get the inlier indexes
        L12 = transform(A, points1)
        d = np.sqrt(np.sum((L12 - points2) ** 2, axis=1))
        inlier_idxs = np.where(d < th_inliner)[0]
        inliers.append((A, inlier_idxs))

        # Get the number of inliers
        num_inliers[i] = len(inlier_idxs)

    # Get the optimal solution
    idx = np.argmax(num_inliers)
    A, inlier_idxs = inliers[idx]

    return A, inlier_idxs

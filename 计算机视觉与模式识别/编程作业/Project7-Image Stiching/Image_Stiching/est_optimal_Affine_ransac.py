import numpy as np
from affine_transform import affine_transform
from est_optimal_affine import est_optimal_affine


def est_optimal_Affine_ransac(points1, points2, ntrials):
    # Check the input
    if points1.shape != points2.shape:
        raise ValueError('The points should be given in pair!')
    if points1.shape[1] != 2:
        raise ValueError('The points should be given in Nx2 format!')
    if points1.shape[0] < 3:
        raise ValueError('At least 3 points are required!')

    # Check the number of trials for fundamental
    ntrials = 20 if ntrials < 20 else ntrials

    # Set the random state
    seed = 1234
    np.random.seed(seed)

    # Do the random experiments
    inliners = []
    num_inliners = np.zeros(ntrials)
    th_inliner = 20
    for i in range(ntrials):
        # Randomly picking 3 points
        y = np.random.choice(points1.shape[0], 3)

        # Estimate the affine transformation matrix
        try:
            A = est_optimal_affine(points1[y, :], points2[y, :])
        except np.linalg.LinAlgError:
            inliners.append((None, None))
            continue

        # Get the inlier indexes
        L12 = affine_transform(A, points1)
        d = np.sqrt(np.sum((L12 - points2) ** 2, axis=1))
        inlier_idxs = np.where(d < th_inliner)[0]
        inliners.append((A, inlier_idxs))

        # Get the number of inliers
        num_inliners[i] = len(inlier_idxs)

    # Get the optimal solution
    idx = np.argmax(num_inliners)
    A, inlieridxs = inliners[idx]

    return A, inlieridxs

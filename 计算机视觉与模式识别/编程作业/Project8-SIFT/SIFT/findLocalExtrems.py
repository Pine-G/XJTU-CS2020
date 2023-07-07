import numpy as np


# Do the comparison between current points and its 26 neighbors
def findLocalExtrems(DoG, interval, edge):
    sz = DoG[:, :, interval].shape
    minima = np.zeros(sz, dtype=int)
    maxima = np.zeros(sz, dtype=int)

    for i in range(int(edge), int(sz[0] - edge)):
        for j in range(int(edge), int(sz[1] - edge)):
            minima[i, j] = (DoG[i, j, interval] <= DoG[i - 1, j, interval]) & \
                           (DoG[i, j, interval] <= DoG[i + 1, j, interval]) & \
                           (DoG[i, j, interval] <= DoG[i, j - 1, interval]) & \
                           (DoG[i, j, interval] <= DoG[i, j + 1, interval]) & \
                           (DoG[i, j, interval] <= DoG[i - 1, j - 1, interval]) & \
                           (DoG[i, j, interval] <= DoG[i - 1, j + 1, interval]) & \
                           (DoG[i, j, interval] <= DoG[i + 1, j - 1, interval]) & \
                           (DoG[i, j, interval] <= DoG[i + 1, j + 1, interval]) & \
                           (DoG[i, j, interval] <= DoG[i, j, interval - 1]) & \
                           (DoG[i, j, interval] <= DoG[i - 1, j, interval - 1]) & \
                           (DoG[i, j, interval] <= DoG[i + 1, j, interval - 1]) & \
                           (DoG[i, j, interval] <= DoG[i, j - 1, interval - 1]) & \
                           (DoG[i, j, interval] <= DoG[i, j + 1, interval - 1]) & \
                           (DoG[i, j, interval] <= DoG[i - 1, j - 1, interval - 1]) & \
                           (DoG[i, j, interval] <= DoG[i - 1, j + 1, interval - 1]) & \
                           (DoG[i, j, interval] <= DoG[i + 1, j - 1, interval - 1]) & \
                           (DoG[i, j, interval] <= DoG[i + 1, j + 1, interval - 1]) & \
                           (DoG[i, j, interval] <= DoG[i, j, interval + 1]) & \
                           (DoG[i, j, interval] <= DoG[i - 1, j, interval + 1]) & \
                           (DoG[i, j, interval] <= DoG[i + 1, j, interval + 1]) & \
                           (DoG[i, j, interval] <= DoG[i, j - 1, interval + 1]) & \
                           (DoG[i, j, interval] <= DoG[i, j + 1, interval + 1]) & \
                           (DoG[i, j, interval] <= DoG[i - 1, j - 1, interval + 1]) & \
                           (DoG[i, j, interval] <= DoG[i - 1, j + 1, interval + 1]) & \
                           (DoG[i, j, interval] <= DoG[i + 1, j - 1, interval + 1]) & \
                           (DoG[i, j, interval] <= DoG[i + 1, j + 1, interval + 1])

            maxima[i, j] = (DoG[i, j, interval] >= DoG[i - 1, j, interval]) & \
                           (DoG[i, j, interval] >= DoG[i + 1, j, interval]) & \
                           (DoG[i, j, interval] >= DoG[i, j - 1, interval]) & \
                           (DoG[i, j, interval] >= DoG[i, j + 1, interval]) & \
                           (DoG[i, j, interval] >= DoG[i - 1, j - 1, interval]) & \
                           (DoG[i, j, interval] >= DoG[i - 1, j + 1, interval]) & \
                           (DoG[i, j, interval] >= DoG[i + 1, j - 1, interval]) & \
                           (DoG[i, j, interval] >= DoG[i + 1, j + 1, interval]) & \
                           (DoG[i, j, interval] >= DoG[i, j, interval - 1]) & \
                           (DoG[i, j, interval] >= DoG[i - 1, j, interval - 1]) & \
                           (DoG[i, j, interval] >= DoG[i + 1, j, interval - 1]) & \
                           (DoG[i, j, interval] >= DoG[i, j - 1, interval - 1]) & \
                           (DoG[i, j, interval] >= DoG[i, j + 1, interval - 1]) & \
                           (DoG[i, j, interval] >= DoG[i - 1, j - 1, interval - 1]) & \
                           (DoG[i, j, interval] >= DoG[i - 1, j + 1, interval - 1]) & \
                           (DoG[i, j, interval] >= DoG[i + 1, j - 1, interval - 1]) & \
                           (DoG[i, j, interval] >= DoG[i + 1, j + 1, interval - 1]) & \
                           (DoG[i, j, interval] >= DoG[i, j, interval + 1]) & \
                           (DoG[i, j, interval] >= DoG[i - 1, j, interval + 1]) & \
                           (DoG[i, j, interval] >= DoG[i + 1, j, interval + 1]) & \
                           (DoG[i, j, interval] >= DoG[i, j - 1, interval + 1]) & \
                           (DoG[i, j, interval] >= DoG[i, j + 1, interval + 1]) & \
                           (DoG[i, j, interval] >= DoG[i - 1, j - 1, interval + 1]) & \
                           (DoG[i, j, interval] >= DoG[i - 1, j + 1, interval + 1]) & \
                           (DoG[i, j, interval] >= DoG[i + 1, j - 1, interval + 1]) & \
                           (DoG[i, j, interval] >= DoG[i + 1, j + 1, interval + 1])

    return minima, maxima

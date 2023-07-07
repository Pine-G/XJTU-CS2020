import numpy as np
from scipy.signal import gaussian


def gaussian_filter(sigma, size=5):
    g = gaussian(size, sigma)
    g /= np.sum(g)
    return g

import numpy as np


def extractNccFeature(img, Locs, halfsz=None):
    if halfsz is None:
        halfsz = [12, 12]
    elif len(halfsz) <= 1:
        halfsz = [halfsz[0], halfsz[0]]
    else:
        halfsz = halfsz[0:2]
    halfsz = np.round(halfsz).astype(int)
    halfsz[halfsz < 1] = 1

    nc = img.shape[2]
    dim = np.prod(2 * halfsz + 1)
    descps = np.zeros((Locs.shape[0], nc * dim))

    img = img.astype(np.float32)

    for i in range(Locs.shape[0]):
        x, y = Locs[i, :]

        xlo = max([0, x - halfsz[0]])
        xhi = min([img.shape[1] - 1, x + halfsz[0]])
        ylo = max([0, y - halfsz[1]])
        yhi = min([img.shape[0] - 1, y + halfsz[1]])

        subimg = img[ylo: yhi + 1, xlo: xhi + 1, :]
        size = (xhi - xlo + 1) * (yhi - ylo + 1) * 3
        descps[i, 0:size] = subimg.flatten()

    descps -= np.tile(np.mean(descps, axis=1)[:, np.newaxis], (1, nc * dim))
    descps /= np.tile(np.sqrt(np.sum(descps ** 2, axis=1) + 1e-20)[:, np.newaxis], (1, nc * dim))

    return descps

import numpy as np


def extractNccFeature(img, Loca, half_sz=None):
    if half_sz is None:
        half_sz = [12, 12]
    elif len(half_sz) <= 1:
        half_sz = [half_sz[0], half_sz[0]]
    else:
        half_sz = half_sz[0:2]
    half_sz = np.round(half_sz).astype(int)
    half_sz[half_sz < 1] = 1

    nc = img.shape[2]
    dim = np.prod(2 * half_sz + 1)
    description = np.zeros((Loca.shape[0], nc * dim))

    img = img.astype(np.float32)

    for i in range(Loca.shape[0]):
        x, y = Loca[i, :]

        xlo = max([0, x - half_sz[0]])
        xhi = min([img.shape[1] - 1, x + half_sz[0]])
        ylo = max([0, y - half_sz[1]])
        yhi = min([img.shape[0] - 1, y + half_sz[1]])

        sub_img = img[ylo: yhi + 1, xlo: xhi + 1, :]
        size = (xhi - xlo + 1) * (yhi - ylo + 1) * 3
        description[i, 0:size] = sub_img.flatten()

    description -= np.tile(np.mean(description, axis=1)[:, np.newaxis], (1, nc * dim))
    description /= np.tile(np.sqrt(np.sum(description ** 2, axis=1) + 1e-20)[:, np.newaxis], (1, nc * dim))

    return description

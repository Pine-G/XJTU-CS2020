import numpy as np
import cv2
from matplotlib import pyplot


def zigzag(n):
    indexorder = sorted(((x,y) for x in range(n) for y in range(n)),key = lambda p: (p[0]+p[1], -p[1] if (p[0]+p[1]) % 2 else p[1]))
    return dict((index,n) for n,index in enumerate(indexorder))

buffer_size = 8
zorder = np.zeros([buffer_size]*2, dtype=int)
myarray = zigzag(buffer_size)
for i in range(buffer_size):
    for j in range(buffer_size):
        zorder[i,j] = int(myarray[(i,j)])

print(zorder)


def DCTBS(buffer_size):
    bs = np.zeros([buffer_size]*2)

    for q in range(buffer_size):
        if q > 0:
            aq = np.sqrt(2/buffer_size)
        else:
            aq = np.sqrt(1/buffer_size)

        for n in range(buffer_size):
            bs[q,n] = aq * np.cos(np.pi*(2*n+1)*q/2/buffer_size)

    return bs.transpose()

bs = DCTBS(buffer_size)
print(np.matmul(bs.transpose(),bs).round(2))


# Separable version for discrete cosine transform
def dct1(patch):
    return np.matmul(bs.transpose(),np.matmul(patch,bs))


# Inverse discrete cosine transform
def idct1(coefs):
    return np.matmul(bs,np.matmul(coefs,bs.transpose()))


def dct(patch):
    nr, nc = patch.shape
    coefs = np.zeros([nr, nc], dtype=float)

    for p in range(nr):
        for q in range(nc):
            # 计算alpha_p和alpha_q的值
            if q > 0:
                aq = np.sqrt(2 / nc)
            else:
                aq = np.sqrt(1 / nc)

            if p > 0:
                ap = np.sqrt(2 / nr)
            else:
                ap = np.sqrt(1 / nr)

            for m in range(nr):
                for n in range(nc):
                    # 填充计算的代码
                    coefs[p, q] += patch[m, n] * aq * ap * np.cos(np.pi * (2 * n + 1) * q / 2 / nc) * np.cos(np.pi * (2 * m + 1) * p / 2 / nr)

    return coefs


def idct(coefs):
    nr, nc = coefs.shape
    patch = np.zeros([nr, nc], dtype=float)

    for m in range(nr):
        for n in range(nc):

            for p in range(nr):
                for q in range(nc):
                    # 计算alpha_p和alpha_q的值
                    if q > 0:
                        aq = np.sqrt(2 / nc)
                    else:
                        aq = np.sqrt(1 / nc)

                    if p > 0:
                        ap = np.sqrt(2 / nr)
                    else:
                        ap = np.sqrt(1 / nr)

                    # 填充计算的代码
                    patch[m, n] += coefs[p, q] * aq * ap * np.cos(np.pi * (2 * n + 1) * q / 2 / nc) * np.cos(np.pi * (2 * m + 1) * p / 2 / nr)

    return patch


# Read image by using opencv
image = cv2.imread('lena.jpg',flags=cv2.IMREAD_GRAYSCALE)
#print(image)

rows, cols = image.shape

nr, nc = rows // buffer_size, cols // buffer_size

pyplot.figure("Compression with Discrete Cosine Transform")

pyplot.subplot(1,2,1)
pyplot.imshow(image, cmap = 'gray')
pyplot.xticks([])
pyplot.yticks([])
pyplot.title("Original Image")

nkeep = 2

# print(zorder < nkeep)
recon = np.zeros([nr*buffer_size,nc*buffer_size]).astype(np.int32)
for i in range(nr):
    for j in range(nc):
        patch = image[i*buffer_size:(i+1)*buffer_size,j*buffer_size:(j+1)*buffer_size].astype(np.single)
        coefs = dct(patch)
        coefs_keep = coefs * (zorder < nkeep)
        patch_keep = idct(coefs_keep).astype(np.int32)
        recon[i*buffer_size:(i+1)*buffer_size,j*buffer_size:(j+1)*buffer_size] = patch_keep

#print(recon)
pyplot.subplot(1,2,2)
pyplot.imshow(recon, cmap = 'gray')
pyplot.xticks([])
pyplot.yticks([])
pyplot.title("Reconstructed Image")

pyplot.show()

import numpy as np
import cv2
import matplotlib.pyplot as plt
from scipy import signal


def detHarrisCorners(img, corn_thresh=0.01, interactive=False):
    # convert to grayscale
    if img.ndim == 3 and img.shape[2] > 1:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Filter for horizontal and vertical direction
    dx = np.array([[1, 0, -1]])
    dy = np.array([[1], [0], [-1]])

    # Convolution of image with dx and dy
    Ix = signal.convolve(img, dx, mode='same')
    Iy = signal.convolve(img, dy, mode='same')

    if interactive:
        plt.subplot(1,2,1); plt.imshow(Ix, cmap='gray'); plt.axis('off'); plt.title('Ix')
        plt.subplot(1,2,2); plt.imshow(Iy, cmap='gray'); plt.axis('off'); plt.title('Iy')
        plt.show()

    # Raw Hessian Matrix
    Ixx = Ix * Ix
    Ixy = Ix * Iy
    Iyy = Iy * Iy

    if interactive:
        plt.gcf().suptitle('Before Smoothing')
        plt.subplot(2,2,1); plt.imshow(Ixx, cmap='gray'); plt.axis('off'); plt.title('Ixx')
        plt.subplot(2,2,2); plt.imshow(Ixy, cmap='gray'); plt.axis('off'); plt.title('Ixy')
        plt.subplot(2,2,3); plt.imshow(Ixy, cmap='gray'); plt.axis('off'); plt.title('Ixy')
        plt.subplot(2,2,4); plt.imshow(Iyy, cmap='gray'); plt.axis('off'); plt.title('Iyy')
        plt.show()

    # Gaussian filter definition
    G = np.array([[2,4,5,4,2], [4,9,12,9,4], [5,12,15,12,5], [4,9,12,9,4], [2,4,5,4,2]]) / 159.0

    # Convolution with Gaussian filter
    Ixx = signal.convolve(Ixx, G, mode='same')
    Iyy = signal.convolve(Iyy, G, mode='same')
    Ixy = signal.convolve(Ixy, G, mode='same')

    if interactive:
        plt.gcf().suptitle('After Smoothing')
        plt.subplot(2,2,1); plt.imshow(Ixx, cmap='gray'); plt.axis('off'); plt.title('Ixx')
        plt.subplot(2,2,2); plt.imshow(Ixy, cmap='gray'); plt.axis('off'); plt.title('Ixy')
        plt.subplot(2,2,3); plt.imshow(Ixy, cmap='gray'); plt.axis('off'); plt.title('Ixy')
        plt.subplot(2,2,4); plt.imshow(Iyy, cmap='gray'); plt.axis('off'); plt.title('Iyy')
        plt.show()

    # calculate the lambda1 and lambda2
    delta = (Ixx + Iyy) ** 2 - 4 * (Ixx * Iyy - Ixy ** 2)
    delta[delta < 0] = 0
    lambda1 = 0.5 * ((Ixx + Iyy) + np.sqrt(delta))
    lambda2 = 0.5 * ((Ixx + Iyy) - np.sqrt(delta))

    if interactive:
        plt.subplot(1,2,1); plt.imshow(lambda1, cmap='gray'); plt.axis('off'); plt.title('lambda_1')
        plt.subplot(1,2,2); plt.imshow(lambda2, cmap='gray'); plt.axis('off'); plt.title('lambda_2')
        plt.show()

    # Calculate the corner responseling'luan
    k = 0.04    # usually in the range[0.04 0.06]
    R = lambda1 * lambda2 - k * (lambda1 + lambda2) ** 2

    # Apply non-maximum suppression
    nr, nc = img.shape
    corner = np.ones((nr, nc))
    for i in range(0, nr):
        for j in range(0, nc):
            flag = True
            for u in range(-1, 2):
                for v in range(-1, 2):
                    try:
                        if R[i, j] < R[i + u, j + v]:
                            corner[i, j] = 0
                            flag = False
                            break
                    except IndexError:
                        continue
                if not flag:
                    break

    if interactive:
        plt.subplot(1,2,1); plt.imshow(R, cmap='gray'); plt.axis('off'); plt.title('Before NMS')
        plt.subplot(1,2,2); plt.imshow(R * corner, cmap='gray'); plt.axis('off'); plt.title('After NMS')
        plt.show()

    # find corner locations
    iLoc, jLoc = np.where(R * corner > corn_thresh * R.max())

    if interactive:
        plt.imshow(img, cmap='gray')
        plt.axis('off')
        plt.plot(jLoc, iLoc, 'rx', markersize=5)
        plt.show()

    # assign the output variables
    locs = np.column_stack((jLoc, iLoc))
    cornerness = R[iLoc, jLoc]

    return locs, cornerness

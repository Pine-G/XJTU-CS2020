import numpy as np
import cv2
from detHarrisCorners import detHarrisCorners
from extractNccFeature import extractNccFeature
from est_optimal_Affine_ransac import est_optimal_Affine_ransac

if __name__ == '__main__':
    im1 = cv2.imread('img1.jpg')
    im2 = cv2.imread('img2.jpg')

    im_cat = np.concatenate((im1, im2), axis=1)
    cv2.imshow('', im_cat); cv2.waitKey(0); cv2.destroyAllWindows()

    Locs1, cornerness1 = detHarrisCorners(im1)
    Locs2, cornerness2 = detHarrisCorners(im2)

    descps1 = extractNccFeature(im1, Locs1, [20])
    descps2 = extractNccFeature(im2, Locs2, [20])
    dist = np.dot(descps1, descps2.T)

    vals = np.max(dist, axis=1)
    idxs = np.argmax(dist, axis=1)
    threshold = 0.75
    idxs1 = np.where(vals > threshold)[0]
    idxs2 = idxs[idxs1]
    Locs1 = Locs1[idxs1, :]

    for i in range(len(Locs1)):
        cv2.circle(im_cat, (int(Locs1[i, 0]), int(Locs1[i, 1])), 5, (0, 0, 255), 2)
        cv2.circle(im_cat, (int(Locs2[idxs2[i], 0] + im1.shape[1]), int(Locs2[idxs2[i], 1])), 5, (0, 255, 0), 2)
    cv2.imshow('', im_cat); cv2.waitKey(0); cv2.destroyAllWindows()

    ntrials = 10000
    A, inlineridxs = est_optimal_Affine_ransac(Locs2[idxs2, :], Locs1, ntrials)

    nx2, ny2 = im2.shape[1], im2.shape[0]
    xsbound2 = np.array([0, nx2 - 1, nx2 - 1, 0])
    ysbound2 = np.array([0, 0, ny2 - 1, ny2 - 1])
    x2bound_transformed = np.dot(A, np.vstack((xsbound2, ysbound2, np.ones(4))))

    nx1, ny1 = im1.shape[1], im1.shape[0]
    xlo = int(np.floor(np.min([0, np.min(x2bound_transformed[0, :])])))
    xhi = int(np.ceil(np.max([nx1 - 1, np.max(x2bound_transformed[0, :])])))
    ylo = int(np.floor(np.min([0, np.min(x2bound_transformed[1, :])])))
    yhi = int(np.ceil(np.max([ny1 - 1, np.max(x2bound_transformed[1, :])])))

    bounds = np.empty((2, 4), dtype=object)
    bounds[0, 0] = np.array([[0, nx1 - 1, nx1 - 1, 0], [0, 0, ny1 - 1, ny1 - 1]]) + np.tile([[-xlo], [-ylo]], (1, 4))
    bounds[1, 0] = x2bound_transformed + np.tile([[-xlo], [-ylo]], (1, 4))

    bounds[0, 1] = np.array([[1, 0, -xlo], [0, 1, -ylo]])
    bounds[1, 1] = A.copy()
    bounds[1, 1][0, 2] += -xlo
    bounds[1, 1][1, 2] += -ylo

    sigma = 0.75
    xg1, yg1 = np.meshgrid(np.arange(0, nx1), np.arange(0, ny1))
    mask1 = ((xg1 - nx1 / 2.0) ** 2) / ((sigma * nx1) ** 2) + ((yg1 - ny1 / 2.0) ** 2) / ((sigma * ny1) ** 2)
    xg2, yg2 = np.meshgrid(np.arange(0, nx2), np.arange(0, ny2))
    mask2 = ((xg2 - nx2 / 2.0) ** 2) / ((sigma * nx2) ** 2) + ((yg2 - ny2 / 2.0) ** 2) / ((sigma * ny2) ** 2)

    bounds[0, 2] = np.exp(-mask1)
    bounds[1, 2] = np.exp(-mask2)

    bounds[0, 3] = im1
    bounds[1, 3] = im2

    nc = im1.shape[2]
    imTotal = np.zeros((yhi - ylo + 1, xhi - xlo + 1, nc))
    maskTotal = np.zeros((yhi - ylo + 1, xhi - xlo + 1))

    for i in range(2):
        xlo_i = int(np.floor(np.min(bounds[i, 0][0, :])))
        xhi_i = int(np.ceil(np.max(bounds[i, 0][0, :])))
        ylo_i = int(np.floor(np.min(bounds[i, 0][1, :])))
        yhi_i = int(np.ceil(np.max(bounds[i, 0][1, :])))

        xg_i, yg_i = np.meshgrid(np.arange(xlo_i, xhi_i + 1), np.arange(ylo_i, yhi_i + 1))

        Aff = bounds[i, 1]
        coords_i = np.dot(np.linalg.inv(Aff[0:2, 0:2]),
                          np.vstack([xg_i.reshape(-1, order='F'), yg_i.reshape(-1, order='F')])
                          - np.tile(Aff[:, 2].reshape(2, 1), (1, xg_i.size)))
        xcoords_i = coords_i[0, :].reshape(xg_i.shape, order='F')
        ycoords_i = coords_i[1, :].reshape(yg_i.shape, order='F')

        im_i = np.zeros((yhi_i - ylo_i + 1, xhi_i - xlo_i + 1, nc))

        for j in range(nc):
            im_i[:, :, j] = cv2.remap(bounds[i, 3][:, :, j], xcoords_i.astype(np.float32), ycoords_i.astype(np.float32),
                                      cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT, borderValue=0)

        mask_i = cv2.remap(bounds[i, 2], xcoords_i.astype(np.float32), ycoords_i.astype(np.float32),
                           cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT, borderValue=0)

        cv2.imshow('', im_i.astype('uint8')); cv2.waitKey(0); cv2.destroyAllWindows()
        cv2.imshow('', mask_i); cv2.waitKey(0); cv2.destroyAllWindows()

        imTotal[ylo_i: yhi_i + 1, xlo_i: xhi_i + 1, :] += im_i * np.tile(mask_i[:, :, np.newaxis], (1, 1, nc))
        maskTotal[ylo_i: yhi_i + 1, xlo_i: xhi_i + 1] += mask_i

    imTotal /= np.dstack(((maskTotal + 1e-20), (maskTotal + 1e-20), (maskTotal + 1e-20)))
    cv2.imshow('', imTotal.astype('uint8')); cv2.waitKey(0); cv2.destroyAllWindows()

import numpy as np
import cv2
from NccFeature import extractNccFeature
from Ransac import ransac
from Transform import transform


def stiching(im1, im2, interactive=False):
    # show images before stiching
    im_cat = None
    if interactive:
        im_cat = np.concatenate((im1, im2), axis=1)
        cv2.imshow('', im_cat)
        cv2.waitKey(0)
        cv2.destroyAllWindows()

    # get SIFT feature points
    sift = cv2.SIFT_create()
    key_points1, _ = sift.detectAndCompute(im1, None)
    Loca1 = np.array([kp.pt for kp in key_points1]).reshape(-1, 2).astype(int)
    key_points2, _ = sift.detectAndCompute(im2, None)
    Loca2 = np.array([kp.pt for kp in key_points2]).reshape(-1, 2).astype(int)

    # match feature points
    description1 = extractNccFeature(im1, Loca1, [20])
    description2 = extractNccFeature(im2, Loca2, [20])
    dist = np.dot(description1, description2.T)

    # get matching point pairs
    vals = np.max(dist, axis=1)
    indexes = np.argmax(dist, axis=1)
    threshold = 0.75
    index1 = np.where(vals > threshold)[0]
    index2 = indexes[index1]
    Loca1 = Loca1[index1, :]
    Loca2 = Loca2[index2, :]

    # show matching point pairs
    if interactive:
        for i in range(len(Loca1)):
            cv2.circle(im_cat, (int(Loca1[i, 0]), int(Loca1[i, 1])), 5, (0, 0, 255), 2)
            cv2.circle(im_cat, (int(Loca2[i, 0] + im1.shape[1]), int(Loca2[i, 1])), 5, (0, 255, 0), 2)
        cv2.imshow('', im_cat)
        cv2.waitKey(0)
        cv2.destroyAllWindows()

    # use RANSAC to get the best Homography Matrix
    trials = 10000
    H, _ = ransac(Loca2, Loca1, trials)

    # get boundary point after img2 transformation
    nx2, ny2 = im2.shape[1], im2.shape[0]
    x_bound2 = np.array([0, nx2 - 1, nx2 - 1, 0])
    y_bound2 = np.array([0, 0, ny2 - 1, ny2 - 1])
    bound_transformed = transform(H, np.vstack((x_bound2, y_bound2)).T).T

    # get boundary point of stiching image
    nx1, ny1 = im1.shape[1], im1.shape[0]
    xlo = int(np.floor(np.min([0, np.min(bound_transformed[0, :])])))
    xhi = int(np.ceil(np.max([nx1 - 1, np.max(bound_transformed[0, :])])))
    ylo = int(np.floor(np.min([0, np.min(bound_transformed[1, :])])))
    yhi = int(np.ceil(np.max([ny1 - 1, np.max(bound_transformed[1, :])])))

    bounds = np.empty((2, 4), dtype=object)
    # coordinate transformation of boundary point
    bounds[0, 0] = np.array([[0, nx1 - 1, nx1 - 1, 0], [0, 0, ny1 - 1, ny1 - 1]]) + np.array([[-xlo], [-ylo]])
    bounds[1, 0] = bound_transformed + np.array([[-xlo], [-ylo]])

    # transformation matrix to img1
    bounds[0, 1] = np.eye(3)
    bounds[1, 1] = H

    # mask matrix
    sigma = 0.75
    xg1, yg1 = np.meshgrid(np.arange(0, nx1), np.arange(0, ny1))
    mask1 = ((xg1 - nx1 / 2.0) ** 2) / ((sigma * nx1) ** 2) + ((yg1 - ny1 / 2.0) ** 2) / ((sigma * ny1) ** 2)
    xg2, yg2 = np.meshgrid(np.arange(0, nx2), np.arange(0, ny2))
    mask2 = ((xg2 - nx2 / 2.0) ** 2) / ((sigma * nx2) ** 2) + ((yg2 - ny2 / 2.0) ** 2) / ((sigma * ny2) ** 2)
    bounds[0, 2] = np.exp(-mask1)
    bounds[1, 2] = np.exp(-mask2)

    # img matrix
    bounds[0, 3] = im1
    bounds[1, 3] = im2

    nc = im1.shape[2]
    imTotal = np.zeros((yhi - ylo + 1, xhi - xlo + 1, nc))
    maskTotal = np.zeros((yhi - ylo + 1, xhi - xlo + 1))

    # forward transformation
    for i in range(2):
        xlo_i = int(np.floor(np.min(bounds[i, 0][0, :])))
        xhi_i = int(np.ceil(np.max(bounds[i, 0][0, :])))
        ylo_i = int(np.floor(np.min(bounds[i, 0][1, :])))
        yhi_i = int(np.ceil(np.max(bounds[i, 0][1, :])))

        xg_i, yg_i = np.meshgrid(np.arange(xlo_i, xhi_i + 1), np.arange(ylo_i, yhi_i + 1))

        M = bounds[i, 1]
        coord = transform(np.linalg.inv(M),
                          (np.vstack([xg_i.reshape(-1, order='F'), yg_i.reshape(-1, order='F')])
                           + np.array([[xlo], [ylo]])).T).T
        x_coord = coord[0, :].reshape(xg_i.shape, order='F')
        y_coord = coord[1, :].reshape(yg_i.shape, order='F')

        im_i = np.zeros((yhi_i - ylo_i + 1, xhi_i - xlo_i + 1, nc))

        for j in range(nc):
            im_i[:, :, j] = cv2.remap(bounds[i, 3][:, :, j], x_coord.astype(np.float32), y_coord.astype(np.float32),
                                      cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT, borderValue=0)

        mask_i = cv2.remap(bounds[i, 2], x_coord.astype(np.float32), y_coord.astype(np.float32),
                           cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT, borderValue=0)

        # show transformed image and mask
        if interactive:
            cv2.imshow('', im_i.astype('uint8'))
            cv2.waitKey(0)
            cv2.destroyAllWindows()
            cv2.imshow('', mask_i)
            cv2.waitKey(0)
            cv2.destroyAllWindows()

        imTotal[ylo_i: yhi_i + 1, xlo_i: xhi_i + 1, :] += im_i * np.tile(mask_i[:, :, np.newaxis], (1, 1, nc))
        maskTotal[ylo_i: yhi_i + 1, xlo_i: xhi_i + 1] += mask_i

    imTotal /= np.dstack(((maskTotal + 1e-20), (maskTotal + 1e-20), (maskTotal + 1e-20)))
    imTotal = imTotal.astype('uint8')

    # show stiching image
    if interactive:
        cv2.imshow('', imTotal)
        cv2.waitKey(0)
        cv2.destroyAllWindows()

    return imTotal

import numpy as np
import cv2
import matplotlib.pyplot as plt
from scipy import signal
from gaussian_filter import gaussian_filter
from findLocalExtrems import findLocalExtrems


def Sift1Scale(im, name='sift'):
    # define the scales in one octave
    o = 1   # just 1 octave for illustration
    s = 4
    k = 2 ** (1 / s)

    # define the sigma
    octave = o  # clamp the octave to 1
    initial_sigma = 2 ** 0.5
    absolute_sigma = np.zeros((octave, s + 3))
    absolute_sigma[octave - 1, 0] = initial_sigma

    filter_size = np.zeros((octave, s + 3))
    filter_sigma = np.zeros((octave, s + 3))
    gauss_pyr = np.empty((octave, s + 3), dtype=object)
    DOG_pyr = np.empty(octave, dtype=object)

    # do the progressive blurring
    sigma = initial_sigma
    g = gaussian_filter(sigma)
    filter_size[octave-1, 0] = len(g)
    filter_sigma[octave-1, 0] = sigma
    gauss_pyr[octave-1, 0] = signal.convolve2d(im, np.outer(g, g), mode='same')
    DOG_pyr[octave-1] = np.zeros((gauss_pyr[octave-1, 0].shape[0], gauss_pyr[octave-1, 0].shape[1], s+2))

    # Get the DoG and Gaussian Pyramid
    for interval in range(1, s+3):
        # get the kernel size for the progressive blurring
        sigma_f = (k ** 2 - 1) ** 0.5 * sigma
        g = gaussian_filter(sigma_f)
        sigma = k * sigma

        # Keep track of the absolute sigma
        absolute_sigma[octave-1, interval] = sigma

        # Store the size and standard deviation of the filter for later use
        filter_size[octave-1, interval] = len(g)
        filter_sigma[octave-1, interval] = sigma

        # Get the Gaussian Pyramid
        gauss_pyr[octave-1, interval] = signal.convolve2d(gauss_pyr[octave-1, interval-1], np.outer(g, g), mode='same')

        # Get the DoG Pyramid
        DOG_pyr[octave-1][:, :, interval-1] = gauss_pyr[octave-1, interval] - gauss_pyr[octave-1, interval-1]

    # Display the Gaussian and Difference of Gaussian Pyramids
    interactive = True
    if interactive:
        # Display the gaussian pyramid when in interactive mode
        octaves = o
        intervals = s
        sz = np.zeros((1, 2))
        sz[0, 1] = (intervals + 3) * gauss_pyr[0, 0].shape[1]
        for octave in range(0, octaves):
            sz[0, 0] += gauss_pyr[octave, 0].shape[0]
        pic = np.zeros((int(sz[0, 0]), int(sz[0, 1])))
        y = 0
        for octave in range(0, octaves):
            x = 0
            sz[0, 0], sz[0, 1] = gauss_pyr[octave, 0].shape
            for interval in range(0, intervals+3):
                pic[y: y + int(sz[0, 0]), x: x + int(sz[0, 1])] = gauss_pyr[octave, interval]
                x += int(sz[0, 1])
            y += int(sz[0, 0])

        plt.imshow(pic, cmap='gray')
        plt.axis('equal')
        plt.axis('image')
        plt.axis('off')
        plt.show()
        plt.imsave(name + '-gauss_pyr.png', pic, cmap='gray')

        # Display the Difference of Gaussian Pyramid when in active model
        sz = np.zeros((1, 2))
        sz[0, 1] = (intervals + 2) * DOG_pyr[0][:, :, 0].shape[1]
        for octave in range(0, octaves):
            sz[0, 0] += DOG_pyr[octave][:, :, 0].shape[0]
        pic = np.zeros((int(sz[0, 0]), int(sz[0, 1])))
        y = 0
        for octave in range(0, octaves):
            x = 0
            sz[0, 0], sz[0, 1] = DOG_pyr[octave][:, :, 0].shape
            for interval in range(0, intervals+2):
                pic[y: y + int(sz[0, 0]), x: x + int(sz[0, 1])] = DOG_pyr[octave][:, :, interval]
                x += int(sz[0, 1])
            y += int(sz[0, 0])

        plt.imshow(pic, cmap='gray')
        plt.axis('equal')
        plt.axis('image')
        plt.axis('off')
        plt.show()
        plt.imsave(name + '-DoGpyramid.png', pic, cmap='gray')

    # constrast_threshold for eliminate the points with small value in DoG
    contrast_threshold = 0.02

    # curvature_threshold is used for eliminating the points with one big
    # eigenvalue and a smaller one.Like Harris edge detection, we need both
    # eigenvalues be large enough.
    curvature_threshold = 10.0

    dxx = np.array([[1, -2, 1]])
    dyy = dxx.T
    dxy = np.array([[1, 0, -1], [0, 0, 0], [-1, 0, 1]]) / 4

    keypoints = np.empty((0, 4))
    for interval in range(1, s+1):
        # the eliminated margin
        edge = np.ceil((filter_size[octave-1, interval] - 1) / 2)
        edge = 1 if edge < 1 else edge

        # Get the minima and maxima indicator
        minima, maxima = findLocalExtrems(DOG_pyr[octave-1], interval, edge)

        # Threshold with the contrast_threshold
        beContrast = abs(DOG_pyr[octave-1][:, :, interval]) >= contrast_threshold

        # Eliminate the edge points
        Dxx = signal.convolve2d(DOG_pyr[octave-1][:, :, interval], dxx, mode='same')
        Dyy = signal.convolve2d(DOG_pyr[octave-1][:, :, interval], dyy, mode='same')
        Dxy = signal.convolve2d(DOG_pyr[octave-1][:, :, interval], dxy, mode='same')

        # Compute the trace and the determinant of the Hessian
        Tr_H = Dxx + Dyy
        Det_H = Dxx * Dyy - Dxy ** 2
        curvature_ratio = Tr_H ** 2 / (Det_H + 1e-20)

        notEdge = (Det_H > 0) & (curvature_ratio < ((curvature_threshold + 1) ** 2 / curvature_threshold))

        # Get the edge points
        iy, ix = np.where((minima | maxima) & beContrast & notEdge)

        # Store the key points
        new_points = np.concatenate([np.array(ix).reshape(-1, 1), np.array(iy).reshape(-1, 1),
                                     octave * np.ones((len(ix), 1)), interval * np.ones((len(ix), 1))], axis=1)
        keypoints = np.concatenate([keypoints, new_points], axis=0)

    if interactive:
        plt.imshow(im, cmap='gray')
        plt.plot(keypoints[:, 0], keypoints[:, 1], 'rx', markersize=12)
        plt.axis('off')
        plt.savefig(name + '-keypoint.png')
        plt.show()

        ss = np.unique(keypoints[:, 3])
        for i in range(len(ss)):
            plt.imshow(im, cmap='gray')
            plt.plot(keypoints[keypoints[:, 3] == ss[i], 0], keypoints[keypoints[:, 3] == ss[i], 1], 'rx', markersize=12)
            plt.axis('off')
            plt.savefig(f'{name}-keypoint{i+1}.png')
            plt.show()

    # Compute the gradient direction and magnitude of the gaussian pyramid images
    mag_pyr = np.empty(gauss_pyr.shape, dtype=object)
    grad_pyr = np.empty(gauss_pyr.shape, dtype=object)
    dx = np.array([[1, 0, -1]]) / 2.0
    dy = dx.T
    octave = o
    intervals = s
    for interval in range(1, intervals+1):
        # Compute x and y derivatives using pixel differences
        diff_x = signal.convolve2d(gauss_pyr[octave-1, interval], dx, mode='same')
        diff_y = signal.convolve2d(gauss_pyr[octave-1, interval], dy, mode='same')

        # Compute the magnitude of the gradient
        mag = np.sqrt(diff_x ** 2 + diff_y ** 2)

        # Compute the orientation of the gradient
        grad = np.arctan2(diff_y, diff_x)
        grad[grad == np.pi] = -np.pi

        # Store the orientation and magnitude of the gradient
        grad_pyr[octave - 1, interval] = grad
        mag_pyr[octave - 1, interval] = mag

    # Estimate the main direction of the key points
    # Set up the histogram bin centers for a 36 bin histogram
    num_bins = 36
    hist_step = 2 * np.pi / num_bins
    hist_orient = np.arange(-np.pi, np.pi, hist_step)

    # Initialize the positions, orientations, and scale information of the keypoints to emtpy matrices
    pos = np.empty((0, 2))
    orient = np.empty((0, 1))
    scale = np.empty((0, 3))

    # Set the Gaussian Filters for integration
    Ifilters = np.empty((o, s+3), dtype=object)
    Hfilters = np.zeros((o, s+3))
    for interval in range(1, s + 1):
        g = gaussian_filter(1.5 * absolute_sigma[octave-1, interval])
        Ifilters[octave-1, interval] = np.dot(g.reshape((len(g), 1)), g.reshape((1, len(g))))
        Hfilters[octave-1, interval] = np.floor(len(g) / 2)

    # Enumerate the found key points
    numKey = keypoints.shape[0]
    keypoint_count = 0
    for i in range(numKey):
        # Get the position, octave and scale of the key points
        Ix = int(keypoints[i, 0])
        Iy = int(keypoints[i, 1])
        Io = int(keypoints[i, 2])
        Is = int(keypoints[i, 3])

        # Use octave and scale to find the correponding filter and half filter size
        g = Ifilters[Io, Is]
        hf = int(Hfilters[Io, Is])

        # Get the Magnitude and Orientation
        mag = np.zeros((2 * hf + 1, 2 * hf + 1))
        ori = np.zeros((2 * hf + 1, 2 * hf + 1))

        # get the size of grad and mag
        sz = mag_pyr[octave-1, intervals].shape

        # Get the bound around the key point
        xlo = int(np.max([Ix - hf, 0]) - Ix + hf)
        ylo = int(np.max([Iy - hf, 0]) - Iy + hf)
        xhi = int(np.min([Ix + hf, sz[1] - 1]) - Ix + hf)
        yhi = int(np.min([Iy + hf, sz[0] - 1]) - Iy + hf)

        # Crop the region
        mag[ylo: yhi+1, xlo: xhi+1] = mag_pyr[octave-1, intervals][ylo-hf+Iy: yhi-hf+Iy+1, xlo-hf+Ix: xhi-hf+Ix+1]
        ori[ylo: yhi+1, xlo: xhi+1] = grad_pyr[octave-1, intervals][ylo-hf+Iy: yhi-hf+Iy+1, xlo-hf+Ix: xhi-hf+Ix+1]

        # Use the Gaussian Integration filter to reweight the magnitute
        mag *= g

        hist = np.zeros((num_bins, num_bins))
        for m in range(ylo, yhi + 1):
            for n in range(xlo, xhi + 1):
                angle = ori[m, n]
                try:
                    locs = np.where(hist_orient > angle)[0][0]
                    hist[locs % num_bins, locs // num_bins] += (1 - (hist_orient[locs] - angle) / hist_step) * mag[m, n]
                    if locs == 1:
                        locs = 36
                    hist[(locs-1) % num_bins, (locs-1) // num_bins] += (hist_orient[locs] - angle) / hist_step * mag[m, n]
                except IndexError:
                    continue

        # Find peaks in the orientation histogram using nonmax suppression
        peaks = hist.flatten('F')
        rot_right = np.roll(peaks, 1)
        rot_left = np.roll(peaks, -1)
        peaks[peaks < rot_right] = 0
        peaks[peaks < rot_left] = 0

        # Extract the value and index of the largest peak
        ipeak = np.argmax(peaks)
        max_peak_val = peaks[ipeak]

        # Iterate over all peaks within 80% of the largest peak and add keypoints with
        # the orientation corresponding to those peaks to the keypoint list
        peak_val = max_peak_val
        while peak_val > 0.8 * max_peak_val:
            # Interpolate the peak by fitting a parabola to the three histogram values closest to each peak
            A = np.empty((0, 3))
            b = np.empty((0, 1))
            for j in range(-1, 2):
                A = np.vstack([A, np.array([(hist_orient[ipeak] + hist_step * j) ** 2, hist_orient[ipeak] + hist_step * j, 1])])
                bin = (ipeak + j + num_bins) % num_bins
                b = np.vstack([b, hist[bin % num_bins, bin // num_bins]])
            c = np.dot(np.linalg.pinv(A), b)
            max_orient = -c[1] / (2 * c[0])
            while max_orient < -np.pi:
                max_orient += 2 * np.pi
            while max_orient >= np.pi:
                max_orient -= 2 * np.pi

            # Store the keypoint position, orientation, and scale information
            pos = np.vstack([pos, np.array([Ix, Iy])])
            orient = np.vstack([orient, np.array([max_orient])])
            scale = np.vstack([scale, np.array([Io, Is, absolute_sigma[Io, Is]])])
            keypoint_count += 1

            # Get the next peak
            peaks[ipeak] = 0
            ipeak = np.argmax(peaks)
            peak_val = peaks[ipeak]

    # The final of the SIFT algorithm is to extract feature descriptors for the keypoints.
    # The descriptors are a grid of gradient orientation histograms, where the sampling
    # grid for the histograms is rotated to the main orientation of each keypoint.  The
    # grid is a 4x4 array of 4x4 sample cells of 8 bin orientation histograms.  This
    # procduces 128 dimensional feature vectors.

    # The orientation histograms have 8 bins
    orient_bin_spacing = np.pi / 4
    orient_angles = np.arange(-np.pi, np.pi, orient_bin_spacing)

    # The feature grid is has 4x4 cells - feat_grid describes the cell center positions
    grid_spacing = 4
    x_coords, y_coords = np.meshgrid(np.arange(-6, 6 + grid_spacing, grid_spacing),
                                     np.arange(-6, 6 + grid_spacing, grid_spacing))
    feat_grid = np.vstack([x_coords.flatten('F'), y_coords.flatten('F')])
    x_coords, y_coords = np.meshgrid(np.arange(-(2 * grid_spacing - 0.5), (2 * grid_spacing - 0.5) + 1e-8, 1),
                                     np.arange(-(2 * grid_spacing - 0.5), (2 * grid_spacing - 0.5) + 1e-8, 1))
    feat_samples = np.vstack([x_coords.flatten('F'), y_coords.flatten('F')])
    feat_window = 2 * grid_spacing

    # Initialize the descriptor list to the empty matrix
    desc = np.empty((0, 128))

    for k in range(0, pos.shape[0]):
        x, y = pos[k, :]

        # Rotate the grid coordinates
        M = np.array([[np.cos(orient[k, 0]), -np.sin(orient[k, 0])], [np.sin(orient[k, 0]), np.cos(orient[k, 0])]])
        feat_rot_grid = np.dot(M, feat_grid) + np.tile([[x], [y]], (1, feat_grid.shape[1]))
        feat_rot_samples = np.dot(M, feat_samples) + np.tile([[x], [y]], (1, feat_samples.shape[1]))

        # Initialize the feature descriptor
        feat_desc = np.zeros((1, 128))

        # Histogram the gradient orientation samples weighted by the gradient magnitude and
        # a gaussian with a standard deviation of 1/2 the feature window.  To avoid boundary
        # effects, each sample is accumulated into neighbouring bins weighted by 1-d in
        # all dimensions, where d is the distance from the center of the bin measured in
        # units of bin spacing.
        for s in range(0, feat_rot_samples.shape[1]):
            x_sample, y_sample = feat_rot_samples[:, s]

            # Interpolate the gradient at the sample position
            X, Y = np.meshgrid(np.arange(x_sample - 1, x_sample + 2), np.arange(y_sample - 1, y_sample + 2))
            G = cv2.remap(gauss_pyr[int(scale[k, 0]), int(scale[k, 1])], X.astype(np.float32), Y.astype(np.float32),
                          cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT, borderValue=0)
            diff_x = 0.5 * (G[1, 2] - G[1, 0])
            diff_y = 0.5 * (G[2, 1] - G[0, 1])
            mag_sample = np.sqrt(diff_x ** 2 + diff_y ** 2)
            grad_sample = np.arctan2(diff_y, diff_x)
            if grad_sample == np.pi:
                grad_sample = -np.pi

            # Compute the weighting for the x and y dimensions
            x_wght = np.maximum(1 - (np.abs(feat_rot_grid[0, :] - x_sample) / grid_spacing), 0)
            y_wght = np.maximum(1 - (np.abs(feat_rot_grid[1, :] - y_sample) / grid_spacing), 0)
            pos_wght = np.reshape(np.repeat(x_wght * y_wght, 8), (1, -1))

            # Compute the weighting for the orientation, rotating the gradient to the
            # main orientation to of the keypoint first, and then computing the difference
            # in angle to the histogram bin mod pi
            diff = np.mod(grad_sample - orient[k, 0] - orient_angles + np.pi, 2 * np.pi) - np.pi
            orient_wght = np.maximum(1 - np.abs(diff) / orient_bin_spacing, 0)
            orient_wght = np.tile(orient_wght, (1, 16))

            # Compute the gaussian weighting
            g = np.exp(-((x_sample - x) ** 2 + (y_sample - y) ** 2) / (2 * feat_window ** 2)) \
                / (2 * np.pi * feat_window ** 2)

            # Accumulate the histogram bins
            feat_desc += pos_wght * orient_wght * g * mag_sample

        # Normalize the feature descriptor to a unit vector to make the descriptor invariant
        # to affine changes in illumination
        feat_desc /= np.linalg.norm(feat_desc)

        # Threshold the large components in the descriptor to 0.2 and then renormalize
        # to reduce the influence of large gradient magnitudes on the descriptor
        feat_desc[feat_desc > 0.2] = 0.2
        feat_desc /= np.linalg.norm(feat_desc)

        # Store the descriptor
        desc = np.vstack([desc, feat_desc])

    return pos, orient, scale, desc

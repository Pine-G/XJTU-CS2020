import numpy as np
import cv2
import matplotlib.pyplot as plt
from Sift1Scale import Sift1Scale

if __name__ == '__main__':
    im = cv2.imread('einstein.png')
    if im.ndim == 3 and im.shape[2] == 3:
        im = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
    im = im.astype(np.float64) / 255.0
    cols, rows = im.shape

    pos, orient, scale, desc = Sift1Scale(im, 'einstein')
    plt.imshow(im, cmap='gray')
    plt.quiver(pos[:, 0], pos[:, 1], np.cos(orient), np.sin(orient), angles='xy', color='blue')
    plt.plot(pos[:, 0], pos[:, 1], 'r+', markersize=12)
    plt.axis('off')
    plt.savefig('einstein-direction.png')
    plt.show()

    M45 = cv2.getRotationMatrix2D((cols / 2, rows / 2), angle=45, scale=1)
    im45 = cv2.warpAffine(im, M45, (cols, rows))
    plt.imsave('einstein45.png', im45, cmap='gray')

    pos45, orient45, scale45, desc45 = Sift1Scale(im45, 'einstein45')
    plt.imshow(im45, cmap='gray')
    plt.quiver(pos45[:, 0], pos45[:, 1], np.cos(orient45), np.sin(orient45), angles='xy', color='blue')
    plt.plot(pos45[:, 0], pos45[:, 1], 'r+', markersize=12)
    plt.axis('off')
    plt.savefig('einstein45-direction.png')
    plt.show()

    M90 = cv2.getRotationMatrix2D((cols / 2, rows / 2), angle=90, scale=1)
    im90 = cv2.warpAffine(im, M90, (cols, rows))
    plt.imsave('einstein90.png', im90, cmap='gray')

    pos90, orient90, scale90, desc90 = Sift1Scale(im90, 'einstein90')
    plt.imshow(im90, cmap='gray')
    plt.quiver(pos90[:, 0], pos90[:, 1], np.cos(orient90), np.sin(orient90), angles='xy', color='blue')
    plt.plot(pos90[:, 0], pos90[:, 1], 'r+', markersize=12)
    plt.axis('off')
    plt.savefig('einstein90-direction.png')
    plt.show()

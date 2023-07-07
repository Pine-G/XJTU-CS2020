import cv2
import numpy as np


def crop(img):
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) if len(img.shape) == 3 else img
    _, gray = cv2.threshold(gray, 1, 255, cv2.THRESH_BINARY)

    height, width = img.shape[:2]

    up = -1
    count = width
    while count > width / 4 and up < height - 1:
        up += 1
        count = np.count_nonzero(gray[up, :] == 0)

    down = height
    count = width
    while count > width / 4 and down > up:
        down -= 1
        count = np.count_nonzero(gray[down, :] == 0)

    height = down - up + 1

    left = -1
    count = height
    while count > height / 4 and left < width - 1:
        left += 1
        count = np.count_nonzero(gray[up: down+1, left] == 0)

    right = width
    count = height
    while count > height / 4 and right > left:
        right -= 1
        count = np.count_nonzero(gray[up: down+1, right] == 0)

    cropped_img = img[up: down+1, left: right+1]

    return cropped_img

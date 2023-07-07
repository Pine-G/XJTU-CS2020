import os
import cv2
from Image_stiching import stiching
from Crop_image import crop

if __name__ == '__main__':
    image_path = './images1'
    image_files = os.listdir(image_path)

    '''
    # left perspective
    im1 = cv2.imread(os.path.join(image_path, image_files[0]))
    for i in range(1, len(image_files)):
        im2 = cv2.imread(os.path.join(image_path, image_files[i]))
        im1 = crop(stiching(im1, im2))
        print(i)
    '''

    # center perspective
    center = len(image_files) // 2
    im1 = cv2.imread(os.path.join(image_path, image_files[center]))

    i = center
    while i > 0:
        i -= 1
        im2 = cv2.imread(os.path.join(image_path, image_files[i]))
        im1 = crop(stiching(im1, im2))
        print(i)

    i = center
    while i < len(image_files) - 1:
        i += 1
        im2 = cv2.imread(os.path.join(image_path, image_files[i]))
        im1 = crop(stiching(im1, im2))
        print(i)

    cv2.imwrite('panorama_image.jpg', im1)

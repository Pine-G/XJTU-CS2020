'''
  File name: cannyEdge.py
  Author: Tarmily Wen
  Date created: Dec. 8, 2019
'''

'''
  File clarification:
    Canny edge detector 
    - Input: A color image I = uint8(H, W, 3), where H, W are two dimensions of the image
    - Output: An edge map E = logical(H, W)

    - TO DO: Complete three main functions findDerivatives, nonMaxSup and edgeLink 
             to make your own canny edge detector work
'''

import numpy as np
import matplotlib.pyplot as plt
import os
from scipy import signal
from PIL import Image
import argparse

# import functions
from findDerivatives import findDerivatives
from nonMaxSup import nonMaxSup
from edgeLink import edgeLink
# from Test_script import Test_script
import utils, helpers


# cannyEdge detector
def cannyEdge(I):
  # convert RGB image to gray color space
  im_gray = utils.rgb2gray(I)

  Mag, Magx, Magy, Ori = findDerivatives(im_gray)
  grad_Ori, edge_Ori = helpers.get_discrete_orientation(Ori)
  M = nonMaxSup(Mag, Ori, grad_Ori)
  E = edgeLink(M, Mag, edge_Ori)

  # only when test passed that can show all results
  # if Test_script(im_gray, E):
    # visualization results
    # utils.visDerivatives(im_gray, Mag, Magx, Magy)
    # utils.visCannyEdge(I, M, E)

    # plt.show()

  return E


if __name__ == "__main__":
  # the folder name that stores all images
  # please make sure that this file has the same directory as image folder

  parser = argparse.ArgumentParser()
  parser.add_argument("--image_folder", type=str, help="folder that contains images to compute the Canny Edge over", required=True)
  parser.add_argument("--save_folder", type=str, help="folder to save the Canny Edge to", required=True)
  opt = parser.parse_args()

  image_folder = os.path.abspath(opt.image_folder)
  save_folder = os.path.abspath(opt.save_folder)

  # read images one by one
  for filename in os.listdir(image_folder):
    # read in image and convert color space for better visualization
    im_path = os.path.join(image_folder, filename)
    I = np.array(Image.open(im_path).convert('RGB'))

    ## TO DO: Complete 'cannyEdge' function
    E = cannyEdge(I)

    pil_image = Image.fromarray(E.astype(bool)).convert('1')

    pil_image.save(os.path.join(save_folder, "{}_Result.png".format(filename.split(".")[0])))

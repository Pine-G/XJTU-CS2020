'''
  File name: nonMaxSup.py
  Author: Tarmily Wen
  Date created: Dec. 8, 2019
'''

import numpy as np

'''
  File clarification:
    Find local maximum edge pixel using NMS along the line of the gradient
    - Input Mag: H x W matrix represents the magnitude of derivatives
    - Input Ori: H x W matrix represents the orientation of derivatives
    - Output M: H x W binary matrix represents the edge map after non-maximum suppression
'''



def nonMaxSup(Mag, Ori, grad_Ori):
    ###############################################################################
    # Your code here: do the non maximum suppression
    ###############################################################################
    suppressed = np.copy(Mag)
    suppressed.fill(0)

    H, W = Mag.shape
    for i in range(1, H - 1):
        for j in range(1, W - 1):
            if grad_Ori[i, j] == 0 or grad_Ori[i, j] == 180 or grad_Ori[i, j] == -180:
                suppressed[i, j] = 1 if Mag[i, j] > max([Mag[i, j - 1], Mag[i, j + 1]]) else 0
            elif grad_Ori[i, j] == 45 or grad_Ori[i, j] == -135:
                suppressed[i, j] = 1 if Mag[i, j] > max([Mag[i - 1, j + 1], Mag[i + 1, j - 1]]) else 0
            elif grad_Ori[i, j] == 90 or grad_Ori[i, j] == -90:
                suppressed[i, j] = 1 if Mag[i, j] > max([Mag[i - 1, j], Mag[i + 1, j]]) else 0
            elif grad_Ori[i, j] == 135 or grad_Ori[i, j] == -45:
                suppressed[i, j] = 1 if Mag[i, j] > max([Mag[i - 1, j - 1], Mag[i + 1, j + 1]]) else 0

    return suppressed

# 频域分析
import numpy as np
from matplotlib import pyplot
import cv2


'''
傅里叶变换
1）读入图像
2）采用numpy的傅里叶变换对原始图像进行变换
3）采用numpy的傅里叶变换平移函数对傅里叶变换结果进行平移，将中心放到图像的中点
'''
# Read image by using opencv
image = cv2.imread('lena.jpg', flags=cv2.IMREAD_GRAYSCALE)
print(image)

pyplot.figure('Fourier transform demo')

# Show the original image
pyplot.subplot(2, 2, 1)
pyplot.imshow(image, cmap='gray')
pyplot.title('Original Image')
pyplot.xticks([])
pyplot.yticks([])

# 获取傅里叶变换
fft_image = np.fft.fft2(image)
magnitude_spectrum = 20 * np.log(np.abs(fft_image))
# Show the result of Fourier transform
pyplot.subplot(2, 2, 2)
pyplot.imshow(magnitude_spectrum, cmap='gray')
pyplot.title('magnitude_spectrum')
pyplot.xticks([])
pyplot.yticks([])

fft_image_shift = np.fft.fftshift(fft_image)
magnitude_spectrum_shift = 20 * np.log(np.abs(fft_image_shift))
# Show the result of Fourier transform
pyplot.subplot(2, 2, 3)
pyplot.imshow(magnitude_spectrum_shift, cmap='gray')
pyplot.title('magnitude_spectrum_shift')
pyplot.xticks([])
pyplot.yticks([])

angle_shift = np.angle(fft_image_shift)
# Show the result of Fourier transform
pyplot.subplot(2, 2, 4)
pyplot.imshow(angle_shift, cmap='gray')
pyplot.title('angle_shift')
pyplot.xticks([])
pyplot.yticks([])

pyplot.show()


'''高通滤波器'''
rows, cols = image.shape
crow, ccol = rows // 2, cols // 2
brow, bcol = rows // 8, cols // 8
# print("size=({},{})".format(rows,cols))
mask_hi = np.ones([rows, cols])
mask_hi[crow-brow:crow+brow, ccol-bcol:ccol+bcol] = 0

fft_image_shift_hi = fft_image_shift * mask_hi
image_hi = np.fft.ifft2(np.fft.ifftshift(fft_image_shift_hi))

pyplot.figure('High Pass Filter')

# Show the original image
pyplot.subplot(1, 2, 1)
pyplot.imshow(image, cmap='gray')
pyplot.title('Original Image')
pyplot.xticks([])
pyplot.yticks([])

# Show the high-passed image
pyplot.subplot(1, 2, 2)
pyplot.imshow(np.abs(image_hi), norm=pyplot.autoscale(), cmap='gray')
pyplot.title('High-passed Image')
pyplot.xticks([])
pyplot.yticks([])

pyplot.show()


'''低通滤波器'''
mask_lo = np.zeros([rows, cols])
mask_lo[crow-brow:crow+brow, ccol-bcol:ccol+bcol] = 1

fft_image_shift_lo = fft_image_shift * mask_lo
image_lo = np.fft.ifft2(np.fft.ifftshift(fft_image_shift_lo))

pyplot.figure('High Pass Filter')

# Show the original image
pyplot.subplot(1, 2, 1)
pyplot.imshow(image, cmap='gray')
pyplot.title('Original Image')
pyplot.xticks([])
pyplot.yticks([])

# Show the high-passed image
pyplot.subplot(1, 2, 2)
pyplot.imshow(np.real(image_lo), cmap='gray')
pyplot.title('Low-passed Image')
pyplot.xticks([])
pyplot.yticks([])

pyplot.show()


'''带通滤波器'''
mask_bd = np.zeros([rows, cols])
mask_bd[crow-2 * brow:crow+2 * brow, ccol-2 * bcol:ccol+2 * bcol] = 1
mask_bd[crow - brow: crow + brow, ccol-bcol:ccol+bcol] = 0

fft_image_shift_bd = fft_image_shift * mask_bd
image_bd = np.fft.ifft2(np.fft.ifftshift(fft_image_shift_bd))

pyplot.figure('Band Pass Filter')

# Show the original image
pyplot.subplot(1, 2, 1)
pyplot.imshow(image, cmap='gray')
pyplot.title('Original Image')
pyplot.xticks([])
pyplot.yticks([])

# Show the high-passed image
pyplot.subplot(1, 2, 2)
pyplot.imshow(np.abs(image_bd), cmap='gray')
pyplot.title('Band-passed Image')
pyplot.xticks([])
pyplot.yticks([])

pyplot.show()

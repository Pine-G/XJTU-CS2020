import numpy as np
from matplotlib import pyplot
import cv2


# construct a 11x11 averaging template
f1 = np.ones([11,11]) / 121
print(f1)

# construct a 11x11 impulse template
f2 = np.zeros([11,11])
f2[5,5] = 1
print(f2)


# Read image by using opencv
image = cv2.imread('lena.jpg',flags=cv2.IMREAD_GRAYSCALE)
print(image)

pyplot.figure('Fourier transform demo')

# Show the original image
pyplot.subplot(2, 2, 1)
pyplot.imshow(image, cmap='gray')
pyplot.title('Original Image')
pyplot.xticks([])
pyplot.yticks([])

# using filter f1 to the original image
image1 = cv2.filter2D(image,-1, f1)

# Show the filtered image
pyplot.subplot(2, 2, 2)
pyplot.imshow(image1, cmap='gray')
pyplot.title('Filtered Image')
pyplot.xticks([])
pyplot.yticks([])

# using filter f2 to the original image
image2 = cv2.filter2D(image, -1, f2)

# Show the filtered image
pyplot.subplot(2, 2, 3)
pyplot.imshow(image2, cmap='gray')
pyplot.title('Filtered Image')
pyplot.xticks([])
pyplot.yticks([])

# calculate the difference between image2 and image1
image3 = image2 - image1

# Show the filtered image
pyplot.subplot(2, 2, 4)
pyplot.imshow(image3, cmap='gray')
pyplot.title('Filtered Image')
pyplot.xticks([])
pyplot.yticks([])

pyplot.show()

##
# 1-1）尝试分析F1，F2，F1-F2的频谱信息
# 1-2）是否和我们先前讲解的例子对应上，观察得到的图像特性？
# 1-3）如果频谱信息和我们所说的对应不上，为什么？


#################################################
# 这部分需要你填充                               #
################################################
f1_fft = np.fft.fft2(f1)
f1_fft = np.fft.fftshift(f1_fft)
f2_fft = np.fft.fft2(f2)
f2_fft = np.fft.fftshift(f2_fft)

print(np.abs(f1_fft).round(1))
print(np.abs(f2_fft).round(1))
print(np.abs(f2_fft-f1_fft).round(1))


from scipy.fftpack import dct

# 2-1）尝试利用离散余弦变换分析f1，f2，f1-f2的频谱信息
# 2-2）如果频谱信息和我们所说的对应不上，为什么？

# 在这个地方填充你的代码
f1_dct = dct(f1, axis=1, norm='ortho')
f1_dct = dct(f1_dct, axis=0, norm='ortho')
f2_dct = dct(f2, axis=1, norm='ortho')
f2_dct = dct(f2_dct, axis=0, norm='ortho')

print("dct for f1:")
print(np.sign(f1_dct) * np.abs(f1_dct).round(2))

print("dct for f2")
print(np.sign(f2_dct) * np.abs(f2_dct).round(2))

print("dct for f1-f2")
print(np.sign(f2_dct-f1_dct) * np.abs(f2_dct-f1_dct).round(2))

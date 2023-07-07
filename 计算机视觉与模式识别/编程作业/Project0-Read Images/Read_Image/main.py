from PIL import Image
import cv2
import matplotlib.pyplot as plt

pillow_images = [Image.open(f'img{i+1}.jpg') for i in range(5)]
opencv_images = [cv2.imread(f'img{i+1}.jpg') for i in range(5)]

fig, axs = plt.subplots(2, 5, figsize=(10, 4))

for i, ax in enumerate(axs[0].flat):
    if i < len(pillow_images):
        ax.imshow(pillow_images[i])
    ax.axis('off')

for i, ax in enumerate(axs[1].flat):
    if i < len(opencv_images):
        # ax.imshow(opencv_images[i])
        ax.imshow(cv2.cvtColor(opencv_images[i], cv2.COLOR_BGR2RGB))
    ax.axis('off')

plt.show()

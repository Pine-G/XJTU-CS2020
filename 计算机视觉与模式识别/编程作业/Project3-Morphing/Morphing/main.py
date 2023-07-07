import matplotlib.pyplot as plt
from PIL import Image
import numpy as np
from scipy.spatial import Delaunay
import os
import imageio.v2 as imageio
import multiprocessing


def click_correspondences(im1, im2):
    # 判断两张图片是否为RGB三通道且大小相同
    assert im1.mode == 'RGB' and im2.mode == 'RGB'
    assert im1.size == im2.size

    # 创建窗口和两个子图
    fig, (ax1, ax2) = plt.subplots(1, 2)
    # 显示图像
    ax1.imshow(im1)
    ax2.imshow(im2)
    # 设置子图坐标轴不可见
    ax1.axis('off')
    ax2.axis('off')

    # 创建两个列表，用于存放所选特征点坐标
    im1_pts = []
    im2_pts = []

    # 定义函数，获取鼠标点击位置
    def on_press(event):
        ix, iy = event.xdata, event.ydata
        if ix is not None and iy is not None:
            if event.inaxes == ax1:
                im1_pts.append((int(ix), int(iy)))
                ax1.scatter(ix, iy, s=10, color='red')
            elif event.inaxes == ax2:
                im2_pts.append((int(ix), int(iy)))
                ax2.scatter(ix, iy, s=10, color='red')
            fig.canvas.draw()  # 更新图像

    # 绑定鼠标点击事件
    cid = fig.canvas.mpl_connect('button_press_event', on_press)

    while True:
        # 按下空格键退出程序
        if plt.waitforbuttonpress(0):
            if len(im1_pts) >= 3 and len(im1_pts) == len(im2_pts):
                break
            else:
                print("ERROR: The number of feature points in both images must be the same and greater than 3!")

    # 解绑鼠标点击事件
    fig.canvas.mpl_disconnect(cid)
    # 关闭图像
    plt.close(fig)
    # 返回选取的特征点
    return im1_pts, im2_pts


def morph(im1, im2, im1_pts, im2_pts, warp_frac, dissolve_frac):
    # 判断两张图片是否为RGB三通道且大小相同
    assert im1.mode == 'RGB' and im2.mode == 'RGB'
    assert im1.size == im2.size

    # 获取特征点的x坐标和y坐标
    im1_pts_x, im1_pts_y = np.stack(im1_pts, axis=1)
    im2_pts_x, im2_pts_y = np.stack(im2_pts, axis=1)

    # 对两张图片的特性点进行插值
    x_mean = (1.0 - warp_frac) * im1_pts_x + warp_frac * im2_pts_x
    y_mean = (1.0 - warp_frac) * im1_pts_y + warp_frac * im2_pts_y
    pts_mean = np.column_stack((x_mean, y_mean))

    # 创建正则化网格点坐标
    width, height = im1.size
    im_mesh = np.zeros((width * height, 2))
    for i in range(height):
        for j in range(width):
            im_mesh[i * width + j, 0] = i
            im_mesh[i * width + j, 1] = j

    # 三角剖分
    tri = Delaunay(im1_pts)
    # 寻找每个像素点所属三角形索引
    t = tri.find_simplex(im_mesh)
    # tri_vertex的每一行表示一个三角形的三个顶点的索引
    tri_vertex = tri.simplices

    # 计算重心坐标
    barycentric = np.zeros((width * height, 3))
    for i in range(width * height):
        if t[i] < 0:
            barycentric[i, :] = [np.nan, np.nan, np.nan]
            continue
        ax = pts_mean[tri_vertex[t[i], 0], 0]
        bx = pts_mean[tri_vertex[t[i], 1], 0]
        cx = pts_mean[tri_vertex[t[i], 2], 0]
        ay = pts_mean[tri_vertex[t[i], 0], 1]
        by = pts_mean[tri_vertex[t[i], 1], 1]
        cy = pts_mean[tri_vertex[t[i], 2], 1]
        A = np.array([[ax, bx, cx], [ay, by, cy], [1, 1, 1]])
        barycentric[i, :] = np.transpose(np.linalg.inv(A) @ np.array([[im_mesh[i, 0]], [im_mesh[i, 1]], [1]]))

    # 计算每个像素点对应于第一张图片的位置
    im1_crsp = np.zeros((width * height, 2))
    for i in range(width * height):
        if t[i] < 0:
            im1_crsp[i, :] = [np.nan, np.nan]
            continue
        ax = im1_pts_x[tri_vertex[t[i], 0]]
        bx = im1_pts_x[tri_vertex[t[i], 1]]
        cx = im1_pts_x[tri_vertex[t[i], 2]]
        ay = im1_pts_y[tri_vertex[t[i], 0]]
        by = im1_pts_y[tri_vertex[t[i], 1]]
        cy = im1_pts_y[tri_vertex[t[i], 2]]
        A = np.array([[ax, bx, cx], [ay, by, cy], [1, 1, 1]])
        X = A @ np.transpose(barycentric[i, :])
        im1_crsp[i, :] = [X[0], X[1]]

    # 计算每个像素点对应于第二张图片的位置
    im2_crsp = np.zeros((width * height, 2))
    for i in range(width * height):
        if t[i] < 0:
            im2_crsp[i, :] = [np.nan, np.nan]
            continue
        ax = im2_pts_x[tri_vertex[t[i], 0]]
        bx = im2_pts_x[tri_vertex[t[i], 1]]
        cx = im2_pts_x[tri_vertex[t[i], 2]]
        ay = im2_pts_y[tri_vertex[t[i], 0]]
        by = im2_pts_y[tri_vertex[t[i], 1]]
        cy = im2_pts_y[tri_vertex[t[i], 2]]
        A = np.array([[ax, bx, cx], [ay, by, cy], [1, 1, 1]])
        X = A @ np.transpose(barycentric[i, :])
        im2_crsp[i, :] = [X[0], X[1]]

    # 计算第一张图片变换后的结果
    im1_morph = []
    for i in range(height):
        for j in range(width):
            x = im1_crsp[i * width + j, 0]
            y = im1_crsp[i * width + j, 1]
            if np.isnan(x) or np.isnan(y):
                pixel = im1.getpixel((j, i))
            else:
                try:
                    pixel = im1.getpixel((y, x))
                except IndexError:
                    pixel = im1.getpixel((j, i))
            im1_morph.append(pixel)

    # 计算第二张图片变换后的结果
    im2_morph = []
    for i in range(height):
        for j in range(width):
            x = im2_crsp[i * width + j, 0]
            y = im2_crsp[i * width + j, 1]
            if np.isnan(x) or np.isnan(y):
                pixel = im2.getpixel((j, i))
            else:
                try:
                    pixel = im2.getpixel((y, x))
                except IndexError:
                    pixel = im2.getpixel((j, i))
            im2_morph.append(pixel)

    # 对变换后的两张图片进行插值
    morphed_im = Image.new('RGB', (width, height), 'white')
    for i in range(height):
        for j in range(width):
            r1, g1, b1 = im1_morph[i * width + j]
            r2, g2, b2 = im2_morph[i * width + j]
            r = (1.0 - dissolve_frac) * r1 + dissolve_frac * r2
            g = (1.0 - dissolve_frac) * g1 + dissolve_frac * g2
            b = (1.0 - dissolve_frac) * b1 + dissolve_frac * b2
            morphed_im.putpixel((j, i), (int(r), int(g), int(b)))

    return morphed_im


def create_gif(image_folder):
    gif_name = 'morph.gif'
    # 当GIF文件已存在时将其删除
    if os.path.exists(gif_name):
        os.remove(gif_name)
    # 获取图片路径
    file_name = [os.path.join(image_folder, f) for f in os.listdir(image_folder)]
    # 对图片进行排序
    images = sorted(file_name, key=lambda x: int(x[len(image_folder)+1:-4]))
    with imageio.get_writer(gif_name, mode='I') as writer:
        for image in images:
            writer.append_data(imageio.imread(image))
        # 最后一帧重复5次
        for i in range(5):
            writer.append_data(imageio.imread(images[-1]))


# 子进程函数
def function(num, images, im1, im2, im1_pts, im2_pts, dir_name, k):
    p = 1.0 / (num * images)
    for i in range(images):
        No = k * images + i
        morphed_img = morph(im1, im2, im1_pts, im2_pts, p * No, p * No)
        morphed_img.save(dir_name + f'/{No}.jpg')


if __name__ == '__main__':
    # 获取两张图片的特征点
    img1 = Image.open('img1.jpg')
    img2 = Image.open('img2.jpg')
    img1_pts, img2_pts = click_correspondences(img1, img2)

    # 新建文件夹以存储morph图片
    directory = 'Morphed_Images'
    if os.path.exists(directory):
        files = [os.path.join(directory, f) for f in os.listdir(directory)]
        for f in files:
            os.remove(f)
    else:
        os.mkdir(directory)

    # 创建多进程执行不同参数的morph函数
    process_num = 4
    img_per_proc = 12
    para_list = [(process_num, img_per_proc, img1, img2, img1_pts, img2_pts, directory, k) for k in range(process_num)]
    pool = multiprocessing.Pool(process_num)
    pool.starmap(function, para_list)
    pool.close()
    pool.join()

    final_morphed_img = morph(img1, img2, img1_pts, img2_pts, 1, 1)
    final_morphed_img.save(directory + f'/{process_num * img_per_proc}.jpg')

    # 将创建的morph图片合成为GIF文件
    create_gif(directory)

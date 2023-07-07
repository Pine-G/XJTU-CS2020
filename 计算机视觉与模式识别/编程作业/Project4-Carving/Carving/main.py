import cv2
import numpy as np


# 求解能量矩阵
def genEngMap(Img):
    if len(Img.shape) == 3:
        # BGR彩色图像转灰度图像
        Ig = np.float64(cv2.cvtColor(Img, cv2.COLOR_BGR2GRAY))
    else:
        Ig = np.float64(Img)

    # 以梯度的L1范数作为能量值
    gx, gy = np.gradient(Ig)
    return np.abs(gx) + np.abs(gy)


# 求解竖直方向上的值函数矩阵与路径矩阵
def cumMinEngVer(E):
    row, col = E.shape
    Mx = np.zeros((row, col))       # 值函数矩阵
    Tbx = np.zeros((row, col))      # 路径矩阵
    Mx[0, :] = E[0, :]

    for i in range(1, row):
        for j in range(col):
            if j == 0:
                Mx[i, j] = E[i, j] + min([Mx[i-1, j], Mx[i-1, j+1]])
                Tbx[i, j] = 0 if Mx[i-1, j] < Mx[i-1, j+1] else 1
            elif j == col-1:
                Mx[i, j] = E[i, j] + min([Mx[i-1, j], Mx[i-1, j-1]])
                Tbx[i, j] = 0 if Mx[i-1, j] < Mx[i-1, j-1] else -1
            else:
                Mx[i, j] = E[i, j] + min([Mx[i-1, j-1], Mx[i-1, j], Mx[i-1, j+1]])
                Tbx[i, j] = -1 if Mx[i-1, j-1] < min([Mx[i-1, j], Mx[i-1, j+1]]) else 0 if Mx[i-1, j] < Mx[i-1, j+1] else 1

    return Mx, Tbx


# 求解水平方向上的值函数矩阵与路径矩阵
def cumMinEngHor(E):
    My, Tby = cumMinEngVer(np.transpose(E))
    return np.transpose(My), np.transpose(Tby)


# 删除竖直seam
def rmVerSeam(Img, Mx, Tbx):
    row, col = Mx.shape
    Mx_min = np.finfo(np.float64).max
    rm_x = 0
    # 计算seam起点
    for i in range(col):
        if Mx[row - 1, i] < Mx_min:
            Mx_min = Mx[row - 1, i]
            rm_x = i

    img_array = np.array(Img)
    # 将竖直seam右侧的像素点左移
    for i in range(row - 1, -1, -1):
        for j in range(rm_x, col - 1):
            img_array[i, j] = img_array[i, j + 1]
        rm_x += int(Tbx[i, rm_x])

    # 删除最后一列像素点
    return np.delete(img_array, -1, axis=1)


# 删除水平seam
def rmHorSeam(Img, My, Tby):
    My = np.transpose(My)
    Tby = np.transpose(Tby)
    Img = cv2.transpose(Img)

    return cv2.transpose(rmVerSeam(Img, My, Tby))


# 增加k个竖直seam
def addVerSeam(Img, Mx, Tbx, k=1):
    row, col = Mx.shape
    add_x = [0] * k
    # 计算k个最小seam起点
    for m in range(k):
        Mx_min = np.finfo(np.float64).max
        for i in range(col):
            if Mx[row - 1, i] < Mx_min:
                Mx_min = Mx[row - 1, i]
                add_x[m] = i
        Mx[row - 1, int(add_x[m])] = np.finfo(np.float64).max
        # 避免seam过于集中
        for i in range(5):
            if int(add_x[m]) - i - 1 > 0:
                Mx[row - 1, int(add_x[m]) - i - 1] = np.finfo(np.float64).max
            if int(add_x[m]) + i + 1 < col:
                Mx[row - 1, int(add_x[m]) + i + 1] = np.finfo(np.float64).max
    add_x.sort(reverse=True)

    img_array = np.array(Img)
    for m in range(k):
        # 插入最后一列像素点
        img_array = np.insert(img_array, -1, img_array[:, -1], axis=1)
        for i in range(row - 1, -1, -1):
            # 将竖直seam右侧的像素点右移
            for j in range(col + m, int(add_x[m]), -1):
                img_array[i, j] = img_array[i, j - 1]
            # 将seam位置的像素值与左侧像素值取平均
            t = int(add_x[m])
            if t > 0:
                img_array[i, t] = np.mean([img_array[i, t-1], img_array[i, t]], axis=0)
            # 更新seam位置
            add_x[m] += Tbx[i, int(add_x[m])]

    return img_array


# 增加k个水平seam
def addHorSeam(Img, My, Tby, k=1):
    My = np.transpose(My)
    Tby = np.transpose(Tby)
    Img = cv2.transpose(Img)

    return cv2.transpose(addVerSeam(Img, My, Tby, k))


# 将图片缩小或者放大
def carving(Img, nr, nc, opType='rm'):
    if opType == 'rm':
        # 缩小图片
        for i in range(nr):
            e = genEngMap(Img)
            A, B = cumMinEngHor(e)
            Img = rmHorSeam(Img, A, B)

        for j in range(nc):
            e = genEngMap(Img)
            A, B = cumMinEngVer(e)
            Img = rmVerSeam(Img, A, B)

    elif opType == 'add':
        # 放大图片
        e = genEngMap(Img)
        A, B = cumMinEngHor(e)
        Img = addHorSeam(Img, A, B, nr)

        e = genEngMap(Img)
        A, B = cumMinEngVer(e)
        Img = addVerSeam(Img, A, B, nc)

    elif opType == 'rm_obj':
        # 目标擦除
        rect = (0, 0, 0, 0)
        while rect == (0, 0, 0, 0):
            rect = cv2.selectROI('', Img)
        cv2.destroyAllWindows()

        H = Img.shape[0]
        W = Img.shape[1]
        mask = np.zeros((H, W))
        for i in range(rect[0], rect[0] + rect[2]):
            for j in range(rect[1], rect[1] + rect[3]):
                mask[j, i] = -100000

        while not np.all(mask == 0):
            e = genEngMap(Img) + mask
            if rect[1] > rect[3]:
                A, B = cumMinEngVer(e)
                Img = rmVerSeam(Img, A, B)
                mask = rmVerSeam(mask, A, B)
            else:
                A, B = cumMinEngHor(e)
                Img = rmHorSeam(Img, A, B)
                mask = rmHorSeam(mask, A, B)

    elif opType == 'rm_save':
        # 目标保留
        rect = (0, 0, 0, 0)
        while rect == (0, 0, 0, 0):
            rect = cv2.selectROI('', Img)
        cv2.destroyAllWindows()

        H = Img.shape[0]
        W = Img.shape[1]
        mask = np.zeros((H, W))
        for i in range(rect[0], rect[0] + rect[2]):
            for j in range(rect[1], rect[1] + rect[3]):
                mask[j, i] = 100000

        for i in range(nr):
            e = genEngMap(Img) + mask
            A, B = cumMinEngHor(e)
            Img = rmHorSeam(Img, A, B)
            mask = rmHorSeam(mask, A, B)

        for j in range(nc):
            e = genEngMap(Img) + mask
            A, B = cumMinEngVer(e)
            Img = rmVerSeam(Img, A, B)
            mask = rmVerSeam(mask, A, B)

    return Img


if __name__ == '__main__':
    img = cv2.imread('img0.jpg')
    new_img = carving(img, 100, 0, opType='rm')
    # new_img = carving(img, 0, 150, opType='rm')
    # new_img = carving(img, 100, 150, opType='rm')
    # new_img = carving(img, 50, 0, opType='add')
    # new_img = carving(img, 0, 30, opType='add')
    # new_img = carving(img, 50, 30, opType='add')
    # new_img = carving(img, 0, 0, opType='rm_obj')
    # new_img = carving(img, 0, 150, opType='rm_save')
    cv2.imshow('original', img)
    cv2.imshow('carving', new_img)
    cv2.waitKey(0)

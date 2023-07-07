import os
import struct
import pickle
import numpy as np
import cupy as cp
import matplotlib.pyplot as plt
from PIL import Image
from torchvision import datasets, transforms
from torch.utils.data import DataLoader, Dataset


def mnist2img(data_path, data_set_name, save_path):
    # Convert MNIST data_set to image
    data_set = datasets.mnist.read_image_file(data_path + data_set_name)
    data_path += save_path + '/'
    if not os.path.exists(data_path):
        os.mkdir(data_path)
    for i, img in enumerate(data_set):
        img_path = data_path + str(i) + '.jpg'
        img = Image.fromarray(img.numpy()).convert('RGB')
        img.save(img_path)


def mnist2label(data_path, data_set_name, save_file_name):
    # Convert MNIST data_set to label
    data_set = datasets.mnist.read_label_file(data_path + data_set_name)
    with open(data_path + save_file_name, 'w') as f:
        for i, label in enumerate(data_set):
            img_path = data_path + str(i) + '.jpg'
            f.write(img_path + ' ' + str(label) + '\n')


# mnist2img('./data/MNIST/raw/', 'train-images-idx3-ubyte', 'train_images')
# mnist2label('./data/MNIST/raw/', 'train-labels-idx1-ubyte', 'train_labels.txt')


def img2mnist(img_path, img_file, label_file):
    image_files = [jpg for jpg in os.listdir(img_path) if jpg.endswith('.jpg')]
    n = len(image_files)
    mnist_data = np.zeros((n, 28, 28), dtype=np.uint8)
    mnist_labels = np.zeros((n,), dtype=np.uint8)
    for i, image_file in enumerate(image_files):
        img = Image.open(os.path.join(img_path, image_file))
        img = img.resize((28, 28))
        img = img.convert('L')
        img_arr = np.array(img)
        img_arr = 255.0 - img_arr  # source image: black text on white background
        mnist_data[i] = img_arr
        # image name is like '123.1.jpg', 123 means image.No, 1 means label
        mnist_labels[i] = int(image_file.split('.')[1])
    with open(os.path.join(img_path, img_file), 'wb') as f:
        f.write(struct.pack('>IIII', 2051, n, 28, 28))
        for i in range(n):
            f.write(mnist_data[i].tobytes())
    with open(os.path.join(img_path, label_file), 'wb') as f:
        f.write(struct.pack('>II', 2049, n))
        f.write(mnist_labels.tobytes())


# img2mnist('./test_images', 'images-ubyte', 'labels-ubyte')
# mnist2img('./test_images/', 'images-ubyte', 'images')
# mnist2label('./test_images/', 'labels-ubyte', 'labels.txt')


class MyDataset(Dataset):
    def __init__(self, folder, data_name, label_name, transform=None):
        (data_set, data_labels) = load_data(folder, data_name, label_name)
        self.data_set = data_set.copy()
        self.data_labels = data_labels
        self.transform = transform

    def __getitem__(self, index):
        img, target = self.data_set[index], int(self.data_labels[index])
        if self.transform is not None:
            img = self.transform(img)
        return img, target

    def __len__(self):
        return len(self.data_set)


def load_data(data_folder, data_name, label_name):
    with open(os.path.join(data_folder, label_name), 'rb') as lb_path:
        y_train = np.frombuffer(lb_path.read(), np.uint8, offset=8)
    with open(os.path.join(data_folder, data_name), 'rb') as img_path:
        x_train = np.frombuffer(img_path.read(), np.uint8, offset=16).reshape((len(y_train), 28, 28))
    return x_train, y_train


MODEL_PATH = './Model_ResNet18'
IMAGE_PATH = './test_images'
trans = transforms.Compose([transforms.ToTensor(), transforms.Resize((224, 224)), transforms.Normalize(0.5, 0.5)])

if os.path.exists(MODEL_PATH):
    with open(MODEL_PATH, 'rb') as file:
        model = pickle.load(file)

    img2mnist(IMAGE_PATH, 'images-ubyte', 'labels-ubyte')
    test_set = MyDataset(IMAGE_PATH, 'images-ubyte', 'labels-ubyte', transform=trans)
    test_loader = DataLoader(test_set, batch_size=64, shuffle=False)

    for data in test_loader:
        images, labels = cp.asarray(data[0].numpy()), cp.asarray(data[1].numpy())
        prob, _ = model.forward(images, labels, train_mode=False)
        top_class = cp.argmax(prob, axis=1)
        print('result:')
        print(top_class)

    jpg_files = [pic for pic in os.listdir(IMAGE_PATH) if pic.endswith('.jpg')]
    for m in range(0, len(jpg_files), 5):
        pics = jpg_files[m: m+5]
        fig, axes = plt.subplots(nrows=1, ncols=5, figsize=(15, 3))
        for k, image_name in enumerate(pics):
            image_path = os.path.join(IMAGE_PATH, image_name)
            image = plt.imread(image_path)
            axes[k].imshow(image)
            axes[k].axis('off')
        plt.show()

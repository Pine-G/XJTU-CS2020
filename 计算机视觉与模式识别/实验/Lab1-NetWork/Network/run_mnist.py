import cupy as cp
import numpy as np
import json
import time
import os
import pickle
import matplotlib.pyplot as plt
from tqdm import tqdm
from torch.utils.data import DataLoader
from torchvision import datasets, transforms
from alexnet import AlexNet
from lenet import LeNet
from resnet import resnet18
from mobilenet import MobileNetV1
from vggnet import VGG16
from optim import SGD, AdaGrad

# %% Preprocess and download datasets

# Variables
MODEL = 'ResNet18'
OPTIMIZER = 'AdaGrad'

DATASET_PATH = './data/'
RESULT_PATH = f'./result_{MODEL}_{OPTIMIZER}/'
MODEL_PATH = f'./Model_{MODEL}_{OPTIMIZER}'

EPOCHS = 15
BATCH_SIZE = 64

LR = None
MOMENTUM = 0.9

SAVE_MODEL = False

# Define a transform to normalize the data
if MODEL == 'LeNet':  # 28x28
    transform = transforms.Compose([transforms.ToTensor(),
                                    transforms.Normalize(0.5, 0.5)
                                    ])
else:  # 224x224
    transform = transforms.Compose([transforms.ToTensor(),
                                    transforms.Resize((224, 224)),
                                    transforms.Normalize(0.5, 0.5)
                                    ])

# Define the model and the optimizer
if MODEL == 'LeNet':
    LR = 1e-6 if OPTIMIZER == 'SGD' else 1e-8
    model = LeNet(input_channel=1, output_class=10)
elif MODEL == 'AlexNet':
    LR = 1e-8 if OPTIMIZER == 'SGD' else 1e-11
    model = AlexNet(input_channel=1, output_class=10)
elif MODEL == 'ResNet18':
    LR = 5e-5 if OPTIMIZER == 'SGD' else 1e-5
    model = resnet18(input_channel=1, output_class=10)
elif MODEL == 'VGG16':
    BATCH_SIZE = 8
    LR = 5e-4 if OPTIMIZER == 'SGD' else 1e-6
    model = VGG16(input_channel=1, output_class=10)
elif MODEL == 'MobileNet':
    BATCH_SIZE = 32
    LR = 1e-2 if OPTIMIZER == 'SGD' else 1e-5
    model = MobileNetV1(input_channel=1, output_class=10)
else:
    raise ValueError('No such model {}, please indicate another'.format(MODEL))

if OPTIMIZER == 'SGD':
    optimizer = SGD(model, lr=LR, momentum=MOMENTUM)
elif OPTIMIZER == 'AdaGrad':
    optimizer = AdaGrad(model, lr=LR)
else:
    raise ValueError('No such optimizer {}, please indicate another'.format(OPTIMIZER))

# Download and load the training data
train_set = datasets.MNIST(DATASET_PATH, download=True, train=True, transform=transform)
test_set = datasets.MNIST(DATASET_PATH, download=True, train=False, transform=transform)
train_loader = DataLoader(train_set, batch_size=BATCH_SIZE, shuffle=True)
test_loader = DataLoader(test_set, batch_size=BATCH_SIZE, shuffle=True)

# %% Training and evaluating process

# Initiate the timer to instrument the performance
timer_start = time.process_time_ns()
epoch_times = [timer_start]

train_losses, test_losses, accuracies = [], [], []

for e in range(EPOCHS):
    running_loss = 0
    print("Epoch: {:03d}/{:03d}..".format(e + 1, EPOCHS))

    # Training pass
    print("Training pass:")
    for data in (tbar := tqdm(train_loader, total=len(train_loader))):
        images, labels = cp.asarray(data[0].numpy()), cp.asarray(data[1].numpy())

        prob, loss = model.forward(images, labels, train_mode=True)
        model.backward(loss)
        optimizer.step()

        total_loss = sum(loss)
        running_loss += total_loss

        tbar.set_description("Running loss {:.2f}".format(total_loss))

    # Testing pass
    print("Validation pass:")
    test_loss = 0
    accuracy = 0
    for data in tqdm(test_loader, total=len(test_loader)):
        images, labels = cp.asarray(data[0].numpy()), cp.asarray(data[1].numpy())

        prob, loss = model.forward(images, labels, train_mode=False)
        test_loss += cp.sum(loss)

        top_class = cp.argmax(prob, axis=1)
        equals = (top_class == labels)
        accuracy += cp.mean(equals.astype(float))

    train_losses.append(running_loss / len(train_loader))
    test_losses.append(test_loss / len(test_loader))
    accuracies.append(accuracy / len(test_loader))

    epoch_times.append(time.process_time_ns())
    print("Train loss: {:.3f}..".format(running_loss / len(train_loader)),
          "Test loss: {:.3f}..".format(test_loss / len(test_loader)),
          "Test Accuracy: {:.3f}".format(accuracy / len(test_loader)),
          "Cur time(ns): {}".format(epoch_times[-1]))

if SAVE_MODEL:
    with open(MODEL_PATH, 'wb') as f:
        pickle.dump(model, f)

# %% Evaluation

train_losses = [cp.asnumpy(x) for x in train_losses]
test_losses = [cp.asnumpy(x) for x in test_losses]
epoch_times = [cp.asnumpy(x) for x in epoch_times]
accuracies = [cp.asnumpy(x) for x in accuracies]

if not os.path.exists(RESULT_PATH[2:-1]):
    os.mkdir(RESULT_PATH[2:-1])

fig, ax = plt.subplots(figsize=(10, 8))
ax.plot(train_losses, label="Train loss")
ax.plot(test_losses, label="Validation loss")
ax.set_xlabel("Epoch")
ax.set_ylabel("Cross Entropy Loss")
ax.legend()
ax2 = ax.twinx()
ax2.plot(np.array(accuracies) * 100, label="Accuracy", color='g')
ax2.set_ylabel("Accuracy (%)")
plt.title("Training procedure")
plt.savefig(RESULT_PATH + 'training_proc.png', dpi=100)

train_losses = [x.tolist() for x in train_losses]
test_losses = [x.tolist() for x in test_losses]
epoch_times = [x.tolist() for x in epoch_times]
accuracies = [x.tolist() for x in accuracies]

proc_results = {
    'train_losses': train_losses,
    'test_losses': test_losses,
    'epoch_times': epoch_times,
    'accuracies': accuracies,
}

with open(RESULT_PATH + 'torch_results.json', 'w+') as f:
    json.dump(proc_results, f)

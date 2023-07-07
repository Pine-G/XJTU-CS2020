from sequential import Sequential
import pylayer as L


class VGG16(Sequential):
    def __init__(self, input_channel=3, output_class=1000):
        super().__init__([
            L.Conv2d(input_channel, 64, (3, 3), padding=1, stride=1),
            L.Conv2d(64, 64, (3, 3), padding=1, stride=1),
            L.BatchNorm2d(64),
            L.ReLU(),
            L.MaxPool2d((2, 2), padding=0, stride=2),
            L.Conv2d(64, 128, (3, 3), padding=1, stride=1),
            L.Conv2d(128, 128, (3, 3), padding=1, stride=1),
            L.BatchNorm2d(128),
            L.ReLU(),
            L.MaxPool2d((2, 2), padding=0, stride=2),
            L.Conv2d(128, 256, (3, 3), padding=1, stride=1),
            L.Conv2d(256, 256, (3, 3), padding=1, stride=1),
            L.Conv2d(256, 256, (3, 3), padding=1, stride=1),
            L.BatchNorm2d(256),
            L.ReLU(),
            L.MaxPool2d((2, 2), padding=0, stride=2),
            L.Conv2d(256, 512, (3, 3), padding=1, stride=1),
            L.Conv2d(512, 512, (3, 3), padding=1, stride=1),
            L.Conv2d(512, 512, (3, 3), padding=1, stride=1),
            L.BatchNorm2d(512),
            L.ReLU(),
            L.MaxPool2d((2, 2), padding=0, stride=2),
            L.Conv2d(512, 512, (3, 3), padding=1, stride=1),
            L.Conv2d(512, 512, (3, 3), padding=1, stride=1),
            L.Conv2d(512, 512, (3, 3), padding=1, stride=1),
            L.BatchNorm2d(512),
            L.ReLU(),
            L.MaxPool2d((2, 2), padding=0, stride=2),
            L.Flatten(),
            L.Linear(7*7*512, 4096),
            L.Linear(4096, 4096),
            L.Linear(4096, output_class),
            L.CrossEntropyLossWithSoftmax(),
        ])

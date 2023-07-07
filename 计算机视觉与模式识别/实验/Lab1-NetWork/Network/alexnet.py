from sequential import Sequential
import pylayer as L


class AlexNet(Sequential):
    def __init__(self, input_channel=3, output_class=1000):
        super().__init__([
            # 224x224xinput_channel
            L.Conv2d(input_channel, 96, (11, 11), padding=0, stride=4),
            L.ReLU(),
            # 54x54x96
            L.MaxPool2d((3, 3), padding=0, stride=2),
            # 26x26x96
            L.Conv2d(96, 256, (5, 5), padding=2, stride=1),
            L.ReLU(),
            # 26x26x256
            L.MaxPool2d((3, 3), padding=0, stride=2),
            # 12x12x256
            L.Conv2d(256, 384, (3, 3), padding=1, stride=1),
            # 12x12x384
            L.ReLU(),
            L.Conv2d(384, 384, (3, 3), padding=1, stride=1),
            # 12x12x256
            L.ReLU(),
            L.Conv2d(384, 256, (3, 3), padding=1, stride=1),
            # 12x12x256
            L.ReLU(),
            L.MaxPool2d((3, 3), padding=0, stride=2),
            # 5x5x256
            L.Flatten(),
            L.Linear(5*5*256, 4096),
            L.ReLU(),
            L.Linear(4096, 4096),
            L.ReLU(),
            L.Linear(4096, output_class),
            L.CrossEntropyLossWithSoftmax(),
        ])

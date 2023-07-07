from sequential import Sequential
import pylayer as L


class LeNet(Sequential):
    def __init__(self, input_channel=1, output_class=10):
        super().__init__([
            # 28x28xinput_channel
            L.Conv2d(input_channel, 6, (5, 5), padding=2, stride=1),
            L.ReLU(),
            # 28x28x6
            L.MaxPool2d((2, 2), padding=0, stride=2),
            # 14x14x6
            L.Conv2d(6, 16, (5, 5), padding=0, stride=1),
            L.ReLU(),
            # 10x10x16
            L.MaxPool2d((2, 2), padding=0, stride=2),
            # 5x5x16
            L.Flatten(),
            L.Linear(5*5*16, 120),
            L.ReLU(),
            L.Linear(120, 84),
            L.ReLU(),
            L.Linear(84, output_class),
            L.CrossEntropyLossWithSoftmax(),
        ])

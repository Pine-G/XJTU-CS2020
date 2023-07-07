from sequential import Sequential
import pylayer as L


class MobileNetV1(Sequential):
    def __init__(self, input_channel=3, output_class=1000):
        super().__init__([
            L.Conv2d(input_channel, 32, (3, 3), padding=1, stride=2),
            L.BatchNorm2d(32),
            L.ReLU6(),
            *self.DepthSeparableConv2d(32, 64, 1),
            *self.DepthSeparableConv2d(64, 128, 2),
            *self.DepthSeparableConv2d(128, 128, 1),
            *self.DepthSeparableConv2d(128, 256, 2),
            *self.DepthSeparableConv2d(256, 256, 1),
            *self.DepthSeparableConv2d(256, 512, 2),
            *self.DepthSeparableConv2d(512, 512, 1),
            *self.DepthSeparableConv2d(512, 512, 1),
            *self.DepthSeparableConv2d(512, 512, 1),
            *self.DepthSeparableConv2d(512, 512, 1),
            *self.DepthSeparableConv2d(512, 512, 1),
            *self.DepthSeparableConv2d(512, 1024, 2),
            *self.DepthSeparableConv2d(1024, 1024, 1),
            L.AvgPool2d((7, 7), padding=0, stride=7),
            L.Flatten(),
            L.Linear(1024, output_class),
            L.CrossEntropyLossWithSoftmax(),
        ])

    @staticmethod
    def DepthSeparableConv2d(input_channels, output_channels, stride):
        layer = [
            L.Conv2d(input_channels, input_channels, (3, 3), padding=1, stride=stride),
            L.BatchNorm2d(input_channels),
            L.ReLU6(),
            L.Conv2d(input_channels, output_channels, (1, 1), padding=0, stride=1),
            L.BatchNorm2d(output_channels),
            L.ReLU6()
        ]
        return layer

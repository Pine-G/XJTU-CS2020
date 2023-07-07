from sequential import Sequential
import pylayer as L


class ResNet(Sequential):
    # 224*224
    def __init__(self, block, num_layer, input_channel=3, output_class=1000):
        self.in_channels = 64
        super().__init__([
            L.Conv2d(input_channel, 64, (7, 7), padding=3, stride=2),
            L.BatchNorm2d(64),
            # L.MaxPool2d((3, 3), padding=1, stride=2), # original implementation
            L.MaxPool2d((2, 2), padding=0, stride=2),
            L.ReLU(),
            *self._make_layer(block, 64, num_layer[0]),
            *self._make_layer(block, 128, num_layer[1], 2),
            *self._make_layer(block, 256, num_layer[2], 2),
            *self._make_layer(block, 512, num_layer[3], 2),
            L.MaxPool2d((7, 7), padding=0, stride=1),
            L.Flatten(),
            L.Linear(block.expansion*512, output_class),
            L.CrossEntropyLossWithSoftmax(),
        ])

    def _make_layer(self, block, out_channels, num_block, stride=1):
        down_sample = None
        if stride != 1 or self.in_channels != out_channels*block.expansion:
            down_sample = [
                L.Conv2d(self.in_channels, out_channels*block.expansion, (1, 1), stride=stride),
                L.BatchNorm2d(out_channels*block.expansion)
            ]
        layers = [block(self.in_channels, out_channels, stride, down_sample)]
        self.in_channels = out_channels*block.expansion
        for _ in range(1, num_block):
            layers.append(block(self.in_channels, out_channels))
        return layers


def resnet18(**kwargs):
    return ResNet(L.BasicBlock, [2, 2, 2, 2], **kwargs)


def resnet34(**kwargs):
    return ResNet(L.BasicBlock, [3, 4, 6, 3], **kwargs)


def resnet50(**kwargs):
    return ResNet(L.BottleNeck, [3, 4, 6, 3], **kwargs)


def resnet101(**kwargs):
    return ResNet(L.BottleNeck, [3, 4, 23, 3], **kwargs)


def resnet152(**kwargs):
    return ResNet(L.BottleNeck, [3, 8, 36, 3], **kwargs)

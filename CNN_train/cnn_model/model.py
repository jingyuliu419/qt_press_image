import torch
import torch.nn as nn
import torch.nn.functional as F

class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        
        # Convolutional layers
        self.conv1 = nn.Conv2d(3, 128, kernel_size=3, padding=1)  # 3x800x800 -> 128x800x800
        self.conv2 = nn.Conv2d(128, 128, kernel_size=3, padding=1)  # 128x800x800 -> 128x800x800
        self.pool = nn.MaxPool2d(2, 2)  # Max pooling layer 2x2 -> 128x400x400
        
        self.conv3 = nn.Conv2d(128, 128, kernel_size=3, padding=1)  # 128x400x400 -> 128x400x400
        self.conv4 = nn.Conv2d(128, 128, kernel_size=3, padding=1)  # 128x400x400 -> 128x400x400
        self.pool2 = nn.MaxPool2d(2, 2)  # Max pooling layer 2x2 -> 128x200x200

        # Additional layers for more downsampling
        self.conv5 = nn.Conv2d(128, 128, kernel_size=3, padding=1)  # 128x200x200 -> 128x200x200
        self.pool3 = nn.MaxPool2d(2, 2)  # 128x200x200 -> 128x100x100
        
        # Fully connected layers
        self.fc1 = nn.Linear(128 * 8 * 8, 5)  # Adjust this based on the new expected input size
        
        # Dropout layer
        self.dropout = nn.Dropout(p=0.5)

    def convs(self, x):
        x = F.relu(self.conv1(x))
        x = F.relu(self.conv2(x))
        x = self.pool(x)
        
        x = F.relu(self.conv3(x))
        x = F.relu(self.conv4(x))
        x = self.pool2(x)

        x = F.relu(self.conv5(x))
        x = self.pool3(x)

        # Print the shape to verify it
        # print("Shape before flattening:", x.shape)

        return x

    def forward(self, x):
        x = self.convs(x)
        
        # Calculate the correct flattened size dynamically
        self._to_linear = x[0].numel()
        # print("Flatten size calculated:", self._to_linear)

        x = x.view(-1, self._to_linear)  # Flattening to the correct size
        # print("Shape after flattening:", x.shape)
        
        x = self.dropout(x)
        x = self.fc1(x)
        return F.log_softmax(x, dim=1)

# Instantiate the model
net = Net()

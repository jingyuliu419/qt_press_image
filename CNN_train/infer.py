import torch
import torch.nn as nn
from torchvision import transforms
from PIL import Image
from cnn_model.model import net

# Define the classes
classes = ('class1', 'class2', 'class3', 'class4', 'class5')

# Load the model
PATH = './pressCNN_net.pth'
net.load_state_dict(torch.load(PATH))
net.eval()  # Set the model to evaluation mode

# Define the image transformation pipeline to resize to 64x64
transform = transforms.Compose([
    transforms.Resize((64, 64)),  # Resize the image to 64x64 to match training size
    transforms.ToTensor(),  # Convert the image to a PyTorch tensor
    transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))  # Normalize the image
])

# Load the image
image_path = '/home/ljy/Doc1/mycode/5/appdesign_5/images_times/dst_image_15_3.png'
image = Image.open(image_path).convert('RGB')  # Ensure the image is in RGB format

# Apply the transformations
image = transform(image)

# Add a batch dimension
image = image.unsqueeze(0)

# Move the image to the same device as the model (if GPU is used)
device = torch.device('cuda:0' if torch.cuda.is_available() else 'cpu')
image = image.to(device)
net.to(device)

# Perform the prediction
outputs = net(image)

# Get the predicted class
_, predicted = torch.max(outputs, 1)

# Print the predicted class
print('Predicted: ', classes[predicted.item()])

import os
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

# Specify the directory containing images
image_directory = '/home/liu/Doc/Doc1/mycode/appdesign_3/images'

# Move the model to the appropriate device
device = torch.device('cuda:0' if torch.cuda.is_available() else 'cpu')
net.to(device)

# Loop through all files in the directory
for filename in os.listdir(image_directory):
    if filename.endswith('.png'):  # Check if the file is a .png image
        image_path = os.path.join(image_directory, filename)
        
        # Load the image
        image = Image.open(image_path).convert('RGB')  # Ensure the image is in RGB format

        # Apply the transformations
        image = transform(image)

        # Add a batch dimension
        image = image.unsqueeze(0)

        # Move the image to the same device as the model
        image = image.to(device)

        # Perform the prediction
        outputs = net(image)

        # Get the predicted class
        _, predicted = torch.max(outputs, 1)

        # Print the predicted class for the current image
        print(f'Image: {filename} - Predicted: {classes[predicted.item()]}')

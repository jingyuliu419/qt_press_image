import os
import onnxruntime
import numpy as np
from PIL import Image
from torchvision import transforms

# Define the classes
classes = ('class1', 'class2', 'class3', 'class4', 'class5')

# Load the ONNX model
onnx_model_path = "pressCNN_net.onnx"
ort_session = onnxruntime.InferenceSession(onnx_model_path)

# Define the image transformation pipeline to resize to 64x64
transform = transforms.Compose([
    transforms.Resize((64, 64)),  # Resize the image to 64x64 to match training size
    transforms.ToTensor(),  # Convert the image to a PyTorch tensor
    transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))  # Normalize the image
])

# Specify the directory containing images
image_directory = '/home/liu/Doc/Doc1/mycode/appdesign_3/images'

# Loop through all files in the directory
for filename in os.listdir(image_directory):
    if filename.endswith('.png'):  # Check if the file is a .png image
        image_path = os.path.join(image_directory, filename)
        
        # Load the image
        image = Image.open(image_path).convert('RGB')  # Ensure the image is in RGB format

        # Apply the transformations
        image = transform(image)

        # Add a batch dimension and convert to numpy
        image = image.unsqueeze(0).numpy()

        # Perform the prediction using ONNX Runtime
        ort_inputs = {ort_session.get_inputs()[0].name: image}
        ort_outputs = ort_session.run(None, ort_inputs)
        outputs = ort_outputs[0]

        # Get the predicted class
        predicted_class = np.argmax(outputs)
        print(f'Image: {filename} - Predicted: {classes[predicted_class]}')

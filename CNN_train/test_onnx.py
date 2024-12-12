import numpy as np
from PIL import Image
from torchvision import transforms

# Define the preprocessing pipeline
transform = transforms.Compose([
    transforms.Resize((64, 64)),
    transforms.ToTensor(),
    transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
])

# Load and preprocess the image
image_path = "/home/liu/Doc/Doc1/mycode/appdesign_3/images/image_2334.png"
image = Image.open(image_path).convert("RGB")
image = transform(image)

# Convert to numpy and save
image_numpy = image.numpy()
np.save("preprocessed_image.npy", image_numpy)

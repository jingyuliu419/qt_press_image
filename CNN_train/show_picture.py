import matplotlib.pyplot as plt
import numpy as np
import torch
import torchvision
import data_prepare 

# Function to show an image
def imshow(img):
    img = img / 2 + 0.5  # Unnormalize
    npimg = img.numpy()
    plt.imshow(np.transpose(npimg, (1, 2, 0)))
    plt.show()

# # Initialize a dictionary to store one image per class
# images_per_class = {}
# class_names = data_prepare.classes  # Assuming classes are stored in data_prepare

# # Loop through the entire dataset to ensure we get one image per class
# for imgs, labels in data_prepare.trainloader:  # `imgs` and `labels` are batched tensors
#     for img, label in zip(imgs, labels):  # Process each image-label pair in the batch
#         class_name = class_names[label.item()]  # Convert tensor label to Python integer
#         if class_name not in images_per_class:
#             images_per_class[class_name] = img
#         # Check if we have one image per class
#         if len(images_per_class) == len(class_names):
#             break
#     if len(images_per_class) == len(class_names):
#         break

# # Combine the images into a single grid
# images = torch.stack(list(images_per_class.values()))

# # Show images
# imshow(torchvision.utils.make_grid(images))

# # Print labels
# print(' '.join(f'{class_name:5s}' for class_name in images_per_class.keys()))

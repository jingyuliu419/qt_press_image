from torchvision.datasets import ImageFolder
from torch.utils.data import DataLoader, Subset
from sklearn.model_selection import StratifiedKFold
import torch
import numpy as np
from collections import Counter
import torchvision.transforms as transforms

def prepare_data():
    print("**********开始处理数据***************")

    # Transformations
    transform = transforms.Compose([
        transforms.Resize((64, 64)),  # Resize images to 64x64
        transforms.ToTensor(),
        transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
    ])

    # Path to the dataset
    dataset_path = '/home/liu/Doc/Doc1/mycode/time4_5class'
    batch_size = 5

    # Load dataset
    dataset = ImageFolder(root=dataset_path, transform=transform)
    classes = ('class1', 'class2', 'class3', 'class4', 'class5')

    # Extract targets (labels) to ensure stratification
    targets = [label for _, label in dataset]

    # Define StratifiedKFold
    n_splits = 5
    skf = StratifiedKFold(n_splits=n_splits, shuffle=True, random_state=42)

    # Split the dataset into folds
    folds = []
    for fold, (train_val_idx, test_idx) in enumerate(skf.split(np.zeros(len(targets)), targets)):
        # Split the train_val indices into training and validation sets (80% training, 20% validation)
        train_val_targets = np.array(targets)[train_val_idx]
        skf_train_val = StratifiedKFold(n_splits=5, shuffle=True, random_state=42)
        for train_sub_idx, val_sub_idx in skf_train_val.split(np.zeros(len(train_val_targets)), train_val_targets):
            train_idx = train_val_idx[train_sub_idx]
            val_idx = train_val_idx[val_sub_idx]
            break

        # Create subsets
        train_subset = Subset(dataset, train_idx)
        val_subset = Subset(dataset, val_idx)
        test_subset = Subset(dataset, test_idx)

        # Create DataLoaders
        trainloader = DataLoader(train_subset, batch_size=batch_size, shuffle=True, num_workers=2)
        valloader = DataLoader(val_subset, batch_size=batch_size, shuffle=False, num_workers=2)
        testloader = DataLoader(test_subset, batch_size=batch_size, shuffle=False, num_workers=2)

        folds.append((trainloader, valloader, testloader))

    print("**********数据处理完成***************")
    return folds, classes

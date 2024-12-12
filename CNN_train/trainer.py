# import torch.optim as optim
# import torch.nn as nn
# from cnn_model.model import net
# import data_prepare 
# import torch
# import show_picture
# import torchvision
# import time
# import numpy as np
# from sklearn.metrics import precision_score, recall_score, confusion_matrix, classification_report
# import matplotlib.pyplot as plt
# import seaborn as sns

# # Prepare data and classes using the modified data_prepare.py
# folds, classes = data_prepare.prepare_data()

# # Define loss function and optimizer
# criterion = nn.CrossEntropyLoss()
# optimizer = optim.SGD(net.parameters(), lr=0.001, momentum=0.9)
# device = torch.device('cuda:0' if torch.cuda.is_available() else 'cpu')
# print(f'Using device: {device}')

# # Move model to the correct device
# net.to(device)

# # 5-Fold Cross Validation
# for fold, (trainloader, valloader, testloader) in enumerate(folds):
#     print(f'Fold {fold + 1}/{len(folds)}')

#     # Training loop for each fold
#     for epoch in range(2):  # loop over the dataset multiple times
#         net.train()  # Set the model to training mode
#         running_loss = 0.0
#         start_time = time.time()  # Track time at the start of each epoch
#         for i, data in enumerate(trainloader, 0):
#             inputs, labels = data[0].to(device), data[1].to(device)

#             # zero the parameter gradients
#             optimizer.zero_grad()

#             # forward + backward + optimize
#             outputs = net(inputs)
#             loss = criterion(outputs, labels)
#             loss.backward()
#             optimizer.step()

#             # print statistics
#             running_loss += loss.item()
#             if i % 2000 == 1999:  # print every 2000 mini-batches
#                 print(f'[{epoch + 1}, {i + 1:5d}] loss: {running_loss / 2000:.3f}')
#                 running_loss = 0.0

#         # Calculate and print training time for each epoch
#         end_time = time.time()
#         epoch_time = end_time - start_time
#         print(f'Epoch {epoch + 1} training time: {epoch_time:.2f} seconds')

#     # Validation loop for each fold
#     net.eval()  # Set the model to evaluation mode
#     val_loss = 0.0
#     correct = 0
#     total = 0
#     all_labels = []
#     all_preds = []
#     with torch.no_grad():
#         for data in valloader:
#             images, labels = data[0].to(device), data[1].to(device)
#             outputs = net(images)
#             loss = criterion(outputs, labels)
#             val_loss += loss.item()
#             _, predicted = torch.max(outputs.data, 1)
#             total += labels.size(0)
#             correct += (predicted == labels).sum().item()
            
#             # Collect all labels and predictions for precision/recall
#             all_labels.extend(labels.cpu().numpy())
#             all_preds.extend(predicted.cpu().numpy())

#     val_accuracy = 100.0 * correct / total
#     print(f'Validation Loss: {val_loss / len(valloader):.3f}, Accuracy: {val_accuracy:.2f} %')

#     # Calculate precision, recall, and F1-score
#     precision = precision_score(all_labels, all_preds, average='macro')
#     recall = recall_score(all_labels, all_preds, average='macro')
#     print(f'Precision: {precision:.3f}, Recall: {recall:.3f}')
#     print(classification_report(all_labels, all_preds, target_names=classes))

# # Save the model after training
# PATH = './pressCNN_net.pth'
# torch.save(net.state_dict(), PATH)

# # Final evaluation on the test sets of all folds
# correct = 0
# total = 0
# correct_pred = {classname: 0 for classname in classes}
# total_pred = {classname: 0 for classname in classes}
# all_labels = []
# all_preds = []

# net.eval()  # Ensure the model is in evaluation mode
# with torch.no_grad():
#     for fold, (_, _, testloader) in enumerate(folds):
#         for data in testloader:
#             images, labels = data[0].to(device), data[1].to(device)
#             outputs = net(images)
#             _, predicted = torch.max(outputs.data, 1)
#             total += labels.size(0)
#             correct += (predicted == labels).sum().item()

#             # Count correct predictions per class
#             for label, prediction in zip(labels, predicted):
#                 if label == prediction:
#                     correct_pred[classes[label.item()]] += 1
#                 total_pred[classes[label.item()]] += 1
            
#             # Collect all labels and predictions for confusion matrix
#             all_labels.extend(labels.cpu().numpy())
#             all_preds.extend(predicted.cpu().numpy())

# # Calculate overall accuracy
# overall_accuracy = 100.0 * correct / total
# print(f'Overall Accuracy on test sets: {overall_accuracy:.2f} %')

# # Print accuracy for each class
# for classname, correct_count in correct_pred.items():
#     accuracy = 100.0 * float(correct_count) / total_pred[classname]
#     print(f'Accuracy for class: {classname:5s} is {accuracy:.1f} %')

# # Compute and plot confusion matrix
# cm = confusion_matrix(all_labels, all_preds)
# plt.figure(figsize=(10, 8))
# sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', xticklabels=classes, yticklabels=classes)
# plt.title('Confusion Matrix')
# plt.xlabel('Predicted')
# plt.ylabel('True')
# plt.savefig('confusion_matrix.png')
# plt.show()

import torch.optim as optim
import torch.nn as nn
from cnn_model.model import net
import data_prepare 
import torch
import show_picture
import torchvision
import time
import numpy as np
from sklearn.metrics import precision_score, recall_score, confusion_matrix, classification_report
import matplotlib.pyplot as plt
import seaborn as sns

# Prepare data and classes using the modified data_prepare.py
folds, classes = data_prepare.prepare_data()

# Define loss function and optimizer
criterion = nn.CrossEntropyLoss()
optimizer = optim.SGD(net.parameters(), lr=0.001, momentum=0.9)
device = torch.device('cuda:0' if torch.cuda.is_available() else 'cpu')
print(f'Using device: {device}')

# Move model to the correct device
net.to(device)

# To track metrics during training
train_losses = []
val_losses = []
val_accuracies = []

# 5-Fold Cross Validation
for fold, (trainloader, valloader, testloader) in enumerate(folds):
    print(f'Fold {fold + 1}/{len(folds)}')

    # Training loop for each fold
    for epoch in range(5):  # Change to the number of epochs you want
        net.train()  # Set the model to training mode
        running_loss = 0.0
        start_time = time.time()  # Track time at the start of each epoch
        correct_train = 0
        total_train = 0

        for i, data in enumerate(trainloader, 0):
            inputs, labels = data[0].to(device), data[1].to(device)

            # zero the parameter gradients
            optimizer.zero_grad()

            # forward + backward + optimize
            outputs = net(inputs)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()

            # print statistics
            running_loss += loss.item()
            _, predicted = torch.max(outputs.data, 1)
            total_train += labels.size(0)
            correct_train += (predicted == labels).sum().item()

        # Calculate and print training time for each epoch
        end_time = time.time()
        epoch_time = end_time - start_time
        train_accuracy = 100.0 * correct_train / total_train
        train_losses.append(running_loss / len(trainloader))
        print(f'Epoch {epoch + 1} training time: {epoch_time:.2f} seconds, Loss: {running_loss / len(trainloader):.3f}, Accuracy: {train_accuracy:.2f} %')

        # Validation loop for each fold
        net.eval()  # Set the model to evaluation mode
        val_loss = 0.0
        correct = 0
        total = 0
        all_labels = []
        all_preds = []
        with torch.no_grad():
            for data in valloader:
                images, labels = data[0].to(device), data[1].to(device)
                outputs = net(images)
                loss = criterion(outputs, labels)
                val_loss += loss.item()
                _, predicted = torch.max(outputs.data, 1)
                total += labels.size(0)
                correct += (predicted == labels).sum().item()
                
                # Collect all labels and predictions for precision/recall
                all_labels.extend(labels.cpu().numpy())
                all_preds.extend(predicted.cpu().numpy())

        val_accuracy = 100.0 * correct / total
        val_losses.append(val_loss / len(valloader))
        val_accuracies.append(val_accuracy)
        print(f'Validation Loss: {val_loss / len(valloader):.3f}, Accuracy: {val_accuracy:.2f} %')

# Plot training and validation loss over epochs
plt.figure()
plt.plot(train_losses, label='Training Loss')
plt.plot(val_losses, label='Validation Loss')
plt.title('Training and Validation Loss over Epochs')
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.legend()
plt.savefig('loss_over_epochs.png')
plt.show()

# Plot validation accuracy over epochs
plt.figure()
plt.plot(val_accuracies, label='Validation Accuracy')
plt.title('Validation Accuracy over Epochs')
plt.xlabel('Epoch')
plt.ylabel('Accuracy (%)')
plt.legend()
plt.savefig('accuracy_over_epochs.png')
plt.show()

# Final evaluation on the test sets of all folds
correct = 0
total = 0
correct_pred = {classname: 0 for classname in classes}
total_pred = {classname: 0 for classname in classes}
all_labels = []
all_preds = []

net.eval()  # Ensure the model is in evaluation mode
with torch.no_grad():
    for fold, (_, _, testloader) in enumerate(folds):
        for data in testloader:
            images, labels = data[0].to(device), data[1].to(device)
            outputs = net(images)
            _, predicted = torch.max(outputs.data, 1)
            total += labels.size(0)
            correct += (predicted == labels).sum().item()

            # Count correct predictions per class
            for label, prediction in zip(labels, predicted):
                if label == prediction:
                    correct_pred[classes[label.item()]] += 1
                total_pred[classes[label.item()]] += 1
            
            # Collect all labels and predictions for confusion matrix
            all_labels.extend(labels.cpu().numpy())
            all_preds.extend(predicted.cpu().numpy())

# Calculate overall accuracy
overall_accuracy = 100.0 * correct / total
print(f'Overall Accuracy on test sets: {overall_accuracy:.2f} %')

# Print accuracy for each class
for classname, correct_count in correct_pred.items():
    accuracy = 100.0 * float(correct_count) / total_pred[classname]
    print(f'Accuracy for class: {classname:5s} is {accuracy:.1f} %')

# Compute and plot confusion matrix
cm = confusion_matrix(all_labels, all_preds)
plt.figure(figsize=(10, 8))
sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', xticklabels=classes, yticklabels=classes)
plt.title('Confusion Matrix')
plt.xlabel('Predicted')
plt.ylabel('True')
plt.savefig('confusion_matrix.png')
plt.show()

# Plot precision and recall per class
precision_per_class = precision_score(all_labels, all_preds, average=None)
recall_per_class = recall_score(all_labels, all_preds, average=None)

x = np.arange(len(classes))

plt.figure(figsize=(12, 6))
plt.bar(x - 0.2, precision_per_class, width=0.4, label='Precision')
plt.bar(x + 0.2, recall_per_class, width=0.4, label='Recall')
plt.xticks(x, classes, rotation=45)
plt.xlabel('Classes')
plt.ylabel('Score')
plt.title('Precision and Recall per Class')
plt.legend()
plt.tight_layout()
plt.savefig('precision_recall_per_class.png')
plt.show()

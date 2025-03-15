import numpy as np
from keras.datasets import cifar10
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
import os
from tqdm import tqdm
import imageio

# Load CIFAR-10 data
(x_train, y_train), (x_test, y_test) = cifar10.load_data()

# Preprocess the data
x_train = x_train.astype('float32') / 255
x_test = x_test.astype('float32') / 255
x_train = x_train.reshape(x_train.shape[0], -1)
x_test = x_test.reshape(x_test.shape[0], -1)

# Reduce dimensionality using PCA
num_components = 50  # You can adjust this number based on your requirements
pca = PCA(n_components=num_components)
x_test_pca = pca.fit_transform(x_test)

# Apply K-means clustering on reduced data
num_clusters = 10000  # You can change this number based on the number of desired clusters
kmeans = KMeans(n_clusters=num_clusters)

cluster_labels = kmeans.fit_predict(x_test_pca)

clusters_dir='results/pca_cluster'

# Create directories for each cluster label
for i in range(num_clusters):
    cluster_dir = f'{clusters_dir}_{i}'
    os.makedirs(cluster_dir, exist_ok=True)

# Save images to respective cluster directories
for idx, label in tqdm(enumerate(cluster_labels), total=len(cluster_labels)):
    image = x_test[idx].reshape(32, 32, 3) * 255  # Reshape the original image data
    image = image.astype(np.uint8)  # Convert to unsigned integer (0 to 255)
    cluster_dir = f'{clusters_dir}_{label}'
    image_filename = f'{idx}_{y_test[idx][0]}.png'
    image_path = os.path.join(cluster_dir, image_filename)
    # Save the image
    imageio.imwrite(image_path, image)

print("Images saved in respective cluster directories.")


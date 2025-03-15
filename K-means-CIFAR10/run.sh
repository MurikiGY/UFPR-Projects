#!/bin/bash

NUM_CLUSTER=10000

python pixel.py
python pca.py
python tsne.py

bash test_clusters.sh pixel_cluster > runs/$NUM_CLUSTER/pixel$NUM_CLUSTER.run
bash test_clusters.sh pca_cluster > runs/$NUM_CLUSTER/pca$NUM_CLUSTER.run
bash test_clusters.sh tsne_cluster > runs/$NUM_CLUSTER/tsne$NUM_CLUSTER.run

rm -rf ./results

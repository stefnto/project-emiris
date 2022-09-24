# project-emiris
The first part of the project implements the LSH and Hypercube algorithms using the Euclidean metric for searching the n Neareset neighbors of a vector in a d-dimensional space
and also algorithms for the clustering of vectors in a d-dimensional space.
The second part of the project is about projecting timeseries to vectors through filtering and some hash functions and then finding the Nearest neighbors with the LSH and Hyperubce algorithms.
The third part is about the clustering of timeseries, initializing them with the K-means algorithm and assigning them with the variations:
  1. Lloyd's assignment (Using the Euclidean metric)
  2. Assignment by Range search with LSH(Using the Discrete Frechet metric)
  3. Assignment by Range search with LSH(Using the Euclidean metric)
  4. Assignment by Range search with Hypercube(Using the Euclidean metric)
  
Code for Discrete Frechet used from repo https://github.com/derohde/Fred

//
//  NeuralNetwork.cpp
//  NeuralNetwork
//
//  Created by Anton Logunov on 6/5/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include "NeuralNetwork.h"


static mat sigmoid(const mat& X) {
  return 1./(1.+exp(-X));
}

vector<mat> NeuralNetwork::forwardPropagation(const mat& X) {
  vector<mat> A(numberOfLayers);
  int i = 0;
  int m = X.n_cols;
  int n = X.n_rows;
  A[i].set_size(n+1, m);
  A[i].row(0).fill(1.);
  A[i].rows(1, n) = X;
  for (i = 1; i < numberOfLayers-1; i++) {
    n = theta[i-1].n_rows;
    A[i].set_size(n+1, m);
    A[i].row(0).fill(1.);
    A[i].rows(1, n) = sigmoid(theta[i-1]*A[i-1]);
  }
  // don't need bias node in last layer
  A[i].set_size(theta[i-1].n_rows, m);
  A[i] = sigmoid(theta[i-1]*A[i-1]);
  return A;
}

double NeuralNetwork::cost(const mat& A, const mat& Y) {
  int m = Y.n_cols;
  // big term
  mat U = zeros(Y.n_rows, Y.n_cols);
  U = Y%log(A) + (1.-Y)%log(1.-A);
  double res = -1./m*accu(U);
  if (!is_finite(res)) res = MAXFLOAT;
  else {
    // regularization term
    double s = 0.;
    for (int i = 0; i < numberOfLayers-1; i++) {
      s += accu(square(theta[i].cols(1, theta[i].n_cols-1)));
    }
    s *= delta/(2.*m);
    res += s;
  }
  return res;
}

double NeuralNetwork::efficiency(const mat& A, const mat& Y) {
  uword maxK;
  uword good = 0;
  for (int i = 0; i < Y.n_cols; i++) {
    vec(A.col(i)).max(maxK);
    if (Y.at(maxK, i) != 0.) good++;
  }
  return good/(double)Y.n_cols;
}

vector<vector<int>> NeuralNetwork::report(const mat& A, const mat& Y) {
  vector<vector<int>> res(10, vector<int>(10, 0));
  uword maxA, maxY;
  for (int i = 0; i < Y.n_cols; i++) {
    vec(A.col(i)).max(maxA);
    vec(Y.col(i)).max(maxY);
    res[maxY][maxA]++;
  } 
  return res;
}

vector<mat> NeuralNetwork::gradient(const mat& X, const mat& Y) {
  int n = numberOfLayers;
  int m = X.n_cols;
  vector<mat> A = forwardPropagation(X);
  vector<mat> d(n-1);
  vector<mat> D(n-1);
  d[n-2] = A[n-1]-Y;
  D[n-2] = d[n-2]*A[n-2].t()/m;
  // no bias node in last layer, but D relate to theta
  D[n-2].cols(1, D[n-2].n_cols-1) += delta*theta[n-2].cols(1, theta[n-2].n_cols-1)/m; 
  for (int i = n-3; i >= 0; i--) {
    //int nR = A[i+1].n_rows;
    d[i] = theta[i+1].t()*d[i+1] % A[i+1] % (1.-A[i+1]);
    d[i] = d[i].rows(1, d[i].n_rows-1);
    D[i] = d[i]*A[i].t()/m;
    D[i].cols(1, D[i].n_cols-1) += delta*theta[i].cols(1, theta[i].n_cols-1)/m;
  }
  return D;
}

vector<mat> NeuralNetwork::gradientApproximation(const mat& X, const mat& Y, double eps) {
  double j1, j2;
  vector<mat> G(numberOfLayers-1);
  for (int i = 0; i < numberOfLayers-1; i++) {
    mat &t = theta[i];
    mat &g = G[i];
    g.set_size(t.n_rows, t.n_cols);
    for (int r = 0; r < t.n_rows; r++) {
      for (int c = 0; c < t.n_cols; c++) {
        t.at(r, c) += eps;
        j1 = cost(activate(X), Y);
        t.at(r, c) -= eps+eps;
        j2 = cost(activate(X), Y);
        t.at(r, c) += eps;
        g.at(r, c) = (j1-j2)/(2.*eps);
      }
    }
  }
  return G;
}


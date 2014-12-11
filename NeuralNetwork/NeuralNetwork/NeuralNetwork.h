//
//  NeuralNetwork.h
//  NeuralNetwork
//
//  Created by Anton Logunov on 6/5/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

//  solving classification problems 

#ifndef __NeuralNetwork__NeuralNetwork__
#define __NeuralNetwork__NeuralNetwork__

#include <iostream>
#include <vector>

#include <armadillo>

using namespace std;
using namespace arma;

// for X each test case is in the separate vector
class NeuralNetwork {
public:
  // bias coefficient
  double delta;
  int numberOfLayers;
  vector<int> layerSizes;
  vector<mat> theta;
  NeuralNetwork(const vector<int>& layerSizes, double delta) : layerSizes(layerSizes), delta(delta) {
    numberOfLayers = (int)layerSizes.size();
    theta.resize(numberOfLayers-1);
    for (int i = 0; i < numberOfLayers-1; i++) {
      // +1 for bias node
      theta[i].set_size(layerSizes[i+1], layerSizes[i]+1);
      // randu - uniform distribution in the [0,1] interval
      theta[i].randu();
      theta[i]-=0.5;
      theta[i]*=2.;
    }
  }
  mat activate(const mat& X) {
    return forwardPropagation(X)[numberOfLayers-1];
  }
  vector<mat> forwardPropagation(const mat& X);
  // back propagation
  vector<mat> gradient(const mat& X, const mat& Y);
  vector<mat> gradientApproximation(const mat& X, const mat& Y, double eps);
  // A - last matrix after forward Propagation
  double cost(const mat& A, const mat& Y);
  double efficiency(const mat& A, const mat& Y);
  vector<vector<int>> report(const mat& A, const mat& Y);
};

#endif /* defined(__NeuralNetwork__NeuralNetwork__) */

//
//  Trainer.cpp
//  NeuralNetwork
//
//  Created by Anton Logunov on 6/6/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include "Trainer.h"

/*
void gradientDescentConstStep(NeuralNetwork& nn, const mat& X, const mat& Y, double lambda) {
  double c1 = cost(nn, X, Y), c2;
  while (true) {
    vector<mat> D = backPropagation(nn, X, Y);
    for (int i = 0; i < nn.numberOfLayers-1; i++) {
      nn.theta[i] -= lambda*D[i]; // maybe start dividing on 2
    }
    c2 = cost(nn, X, Y);
    if (c1 < c2) {
      break;
    }
    c1 = c2;
  }
}


void gradientDescentCrushingStep(NeuralNetwork&& nn, const mat& X, const mat& Y, 
                                 double lambda, double beta) {
  double c1 = cost(nn, X, Y), c2;
  while (true) {
    vector<mat> D = backPropagation(nn, X, Y);
    while (true) {
      NeuralNetwork nn_delta(nn);
      double s = 0.;
      for (int i = 0; i < nn_delta.numberOfLayers-1; i++) {
        nn_delta.theta[i] -= lambda*D[i]; // maybe start dividing on 2
        s += accu(D[i]);
      }
      if ((c2 = cost(nn_delta, X, Y)) > c1 - lambda*sqrt(s)/2.) {
        lambda /= beta;
      }
      else {
        nn = nn_delta;
        break;
      } 
    }
    if (c1 < c2) {
      break;
    }
    c1 = c2;
  }
}
*/


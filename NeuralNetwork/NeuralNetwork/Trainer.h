//
//  Trainer.h
//  NeuralNetwork
//
//  Created by Anton Logunov on 6/6/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __NeuralNetwork__Trainer__
#define __NeuralNetwork__Trainer__

#include <iostream>

#include "NeuralNetwork.h"
/*
class GradientDescentTrainer {
  NeuralNetwork &nn;
  const mat &X;
  const mat &Y;
public:
  GradientDescentTrainer(NeuralNetwork& nn, const mat& X, const mat& Y) : nn(nn), X(X), Y(Y){}
  
  double train(double lambda) {
    vector<mat> D = backPropagation(nn, X, Y);
    for (int i = 0; i < nn.numberOfLayers-1; i++) {
      nn.theta[i] -= lambda*D[i]; 
    }
    return cost(nn, X, Y);
  }
  
};

void gradientDescentConstStep(NeuralNetwork& nn, const mat& X, const mat& Y, double lambda);
void gradientDescentCrushingStep(NeuralNetwork&& nn, const mat& X, const mat& Y, 
                                 double lambda, double beta);
*/
#endif /* defined(__NeuralNetwork__Trainer__) */


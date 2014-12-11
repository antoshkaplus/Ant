//
//  main.cpp
//  NeuralNetwork
//
//  Created by Anton Logunov on 6/5/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <vector>

#include <armadillo>

#include "NeuralNetwork.h"
#include "Trainer.h"
#include "Support.h"

using namespace std;
using namespace arma;

const string root = "/Users/antoshkaplus/Documents/Programming/Projects/NeuralNetwork/Scripts/";


int go(Mat<char>& A, int i, int j) {
  const static int di[] = {0, 0, 1, -1};
  const static int dj[] = {-1, 1, 0, 0};
  int ki, kj, r = 0;
  for (int k = 0; k < 4; k++) {
    ki = i+di[k];
    kj = j+dj[k];
    if (A.in_range(ki, kj) && A.at(ki, kj) == 0){
      A.at(ki, kj) = 1;
      go(A, ki, kj);
      r++;
    }  
  }
  return r;
}

int numberOfAreas(const mat& digit) {
  Mat<char> A(digit.n_rows, digit.n_cols);
  for (int i = 0; i < A.n_rows; i++) {
    for (int j = 0; j < A.n_cols; j++) {
      A.at(i, j) = int(digit.at(i, j));
    }
  }
  int v = 0;
  for (int i = 0; i < A.n_rows; i++) {
    for (int j = 0; j < A.n_cols; j++) {
      if (A.at(i, j) == 0) {
        A.at(i, j) = 1;
        go(A, i, j);
        v++;
      }
    }
  }
  return v;
}

// can close opened ofstreams
class SmallTest {
public:
  const string dir = root + "digits.dataset.small/";
  const string dataFile = dir + "semeion.data.txt";
  bool isOutCost = true;
  bool isOutFails = true;
  ofstream outCost;
  ofstream outFails;
  double trainProportion = 0.7;
  // gradient step coefficient, how many iterations with each lambda
  vector<pair<int, double>> steps = {pair<int, double>(30000, 0.01), 
                                     pair<int, double>(10000, 0.005), 
                                     pair<int, double>(10000, 0.001)}; 
  double delta = 5.;
  int nCases;
  int nTrain;
  int nTest;
  int nInput;
  int nOutput;
  // each test case is in separate vector
  mat trainX, trainY,
      testX, testY;
      
  SmallTest() {
    mat X, Y; // something like buffers
    readData(X, Y);
    constructSets(X, Y);
    NeuralNetwork nn({nInput, nInput+10, nOutput}, delta);
    vector<mat> D; mat trainH, testH;
    if (isOutCost) { 
      outCost.open(dir+"cost.csv");
      outCost << "train_cost,test_cost,train_effeciency,test_efficiency" << endl;
    }
    for (int i = 0; i < steps.size(); i++) {
      int numberOfIterations = steps[i].first;
      double lambda = steps[i].second;
      for (int j = 0; j < numberOfIterations; j++) {
        D = nn.gradient(trainX, trainY);
        for (int i = 0; i < nn.numberOfLayers-1; i++) {
          nn.theta[i] -= lambda*D[i]; 
        }
        if (isOutCost) {
          outCost << nn.cost(trainH=nn.activate(trainX), trainY) << "," 
                  << nn.cost(testH=nn.activate(testX), testY) << ","
                  << nn.efficiency(trainH, trainY) << "," 
                  << nn.efficiency(testH, testY) << endl;
        }
      }
    }
    if (isOutFails) {
      outFails.open(dir+"fails.txt");
      vector<vector<int>> report = nn.report(nn.activate(testX), testY);
      for (int i = 0; i < nOutput; i++) {
        outFails << i << endl;
        for (int j = 0; j < nOutput; j++) {
          outFails << j << " : " << report[i][j] << endl;
        }
        outFails << endl; 
      }
    }
  }
  
  // should init train and test Xs and Ys here
  void readData(mat& X, mat& Y) {
    ifstream in;
    in.open(dataFile);
    mat G;
    G.load(in, raw_ascii);
    shuffle(G, 0);
    nCases = G.n_rows;
    nOutput = 10;
    int nInput = G.n_cols-nOutput;
    X = G.cols(0, nInput-1).t(); 
    Y = G.cols(nInput, G.n_cols-1).t();
  }
  
  void constructSets(mat& X, mat& Y) {
    // you can change X and Y as you want
    // don't forget normalize if you need 
    X.resize(X.n_rows+1, X.n_cols);
    for (int i = 0; i < X.n_cols; i++) {
      mat R = X.submat(0, i, 255, i);
      R.reshape(16, 16);
      mat RR(18, 18);
      RR.fill(0.);
      RR.submat(1, 1, 16, 16) = R;
      X.at(X.n_rows-1, i) = numberOfAreas(RR);
    }
    normalize(X);
    nTrain = trainProportion*nCases;
    nTest = nCases-nTrain;
    nInput = X.n_rows;
    trainX = X.cols(0, nTrain-1);
    trainY = Y.cols(0, nTrain-1);
    testX = X.cols(nTrain, X.n_cols-1);
    testY = Y.cols(nTrain, Y.n_cols-1);
  }
  
};


int main(int argc, const char * argv[])
{
  SmallTest st;
  return 0;
}












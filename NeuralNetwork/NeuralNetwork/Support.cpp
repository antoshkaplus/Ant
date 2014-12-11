//
//  Support.cpp
//  NeuralNetwork
//
//  Created by Anton Logunov on 6/7/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include "Support.h"


void normalize(mat& X) {
  double minEl, maxEl;
  for (int i = 0; i < X.n_rows; i++) {
    minEl = min(X.row(i));
    maxEl = max(X.row(i));
    if (minEl != maxEl) {
      X.row(i) -= minEl;
      X.row(i) /= maxEl-minEl;
    }
  }
}

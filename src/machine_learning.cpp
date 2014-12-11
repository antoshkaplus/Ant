
#include "ant/machine_learning.h"


namespace ant {

namespace ml {

std::vector<Matrix<double>> ant::ml::NeuralNetwork::forward_propagation(const Matrix<double>& X) {
    std::vector<Matrix<double>> A(layer_sizes_.size());
    int i = 0;
    int m = X.col_count();
    int n = X.row_count();
    A[i].set_size(n+1, m);
    A[i].row(0).fill(1.);
    A[i].rows(1, n) = X;
    for (i = 1; i < layer_sizes_.size()-1; i++) {
        n = theta_[i-1].row_count();
        A[i].set_size(n+1, m);
        A[i].row(0).fill(1.);
        A[i].rows(1, n) = sigmoid(theta_[i-1]*A[i-1]);
    }
    // don't need bias node in last layer
    A[i].set_size(theta_[i-1].row_count(), m);
    A[i] = sigmoid(theta_[i-1]*A[i-1]);
    return A;
}

double NeuralNetwork::cost(const Matrix<double>& A, const Matrix<double>& Y) {
    int m = Y.col_count();
    // big term
    Matrix<double> U(Y.size());
    U = Y%log(A) + (1.-Y)%log(1.-A);
    double res = -1./m*sum(U);
    if (!std::isfinite(res)) res = MAXFLOAT;
    else {
        // regularization term
        double s = 0.;
        for (int i = 0; i < layer_sizes_.size()-1; i++) {
            s += sum(square(theta_[i].cols(1, theta_[i].col_count()-1)));
        }
        s *= delta_/(2.*m);
        res += s;
    }
    return res;
}

double NeuralNetwork::efficiency(const Matrix<double>& A, const Matrix<double>& Y) {
    Matrix<Index> m = A.each_col_argmax();
    int good = 0;
    for (int c = 0; c < Y.col_count(); ++c) {
        if (Y(m(0, c), c) != 0.) ++good;
    }
    return good/(double)Y.col_count();
}

std::vector<std::vector<int>> NeuralNetwork::report(const Matrix<double>& A, const Matrix<double>& Y) {
    std::vector<std::vector<int>> res(10, std::vector<int>(10, 0));
    Matrix<Index> A_m = A.each_col_argmax();
    Matrix<Index> Y_m = Y.each_col_argmax();
    for (int c = 0; c < Y.col_count(); c++) {
        ++res[Y_m(0, c)][A_m(0, c)];
    } 
    return res;
}

std::vector<Matrix<double>> NeuralNetwork::gradient(const Matrix<double>& X, const Matrix<double>& Y) {
    int n = (int)layer_sizes_.size();
    int m = X.col_count();
    std::vector<Matrix<double>> A = forward_propagation(X);
    std::vector<Matrix<double>> d(n-1);
    std::vector<Matrix<double>> D(n-1);
    d[n-2] = A[n-1]-Y;
    D[n-2] = d[n-2] * transposed(A[n-2])/m;
    // no bias node in last layer, but D relate to theta
    D[n-2].cols(1, D[n-2].col_count()-1) += delta_*theta_[n-2].cols(1, theta_[n-2].col_count()-1)/m; 
    for (int i = n-3; i >= 0; i--) {
        //int nR = A[i+1].n_rows;
        d[i] = transposed(theta_[i+1])*d[i+1] % A[i+1] % (1.-A[i+1]);
        d[i] = d[i].rows(1, d[i].row_count()-1);
        D[i] = d[i]*transposed(A[i])/m;
        D[i].cols(1, D[i].col_count()-1) += delta_*theta_[i].cols(1, theta_[i].col_count()-1)/m;
    }
    return D;
}

std::vector<Matrix<double>> NeuralNetwork::gradient_approximation(const Matrix<double>& X, const Matrix<double>& Y, double eps) {
    double j1, j2;
    std::vector<Matrix<double>> G(layer_sizes_.size()-1);
    for (int i = 0; i < layer_sizes_.size()-1; i++) {
        Matrix<double> &t = theta_[i];
        Matrix<double> &g = G[i];
        g.set_size(t.size());
        for (int r = 0; r < t.row_count(); r++) {
            for (int c = 0; c < t.col_count(); c++) {
                t(r, c) += eps;
                j1 = cost(activate(X), Y);
                t(r, c) -= eps+eps;
                j2 = cost(activate(X), Y);
                t(r, c) += eps;
                g(r, c) = (j1-j2)/(2.*eps);
            }
        }
    }
    return G;
}



} // end namespace ml

} // end namespace ant

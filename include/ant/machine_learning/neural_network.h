//
//
//class NeuralNetwork {
//public:
//    // bias coefficient
//    NeuralNetwork(const std::vector<int>& layer_sizes, double delta)
//        :  delta_(delta), layer_sizes_(layer_sizes) {
//    
//        theta_.resize(layer_sizes_.size()-1);
//        for (int i = 0; i < layer_sizes_.size()-1; i++) {
//            // +1 for bias node
//            theta_[i].set_size(layer_sizes_[i+1], layer_sizes_[i]+1);
//            // randu - uniform distribution in the [0,1] interval
//            theta_[i].random();
//            theta_[i]-=0.5;
//            theta_[i]*=2.;
//        }
//    }
//    
//    BackPropagation()
//    
//    template<typename X, typename Y>
//    void Train(const X& x, const Y& y) {
//        // making good theta
//        
//        
//        
//        
//    }
//    
//    template<typename X>
//    void Predict()
//    
//    
//    Matrix<double> activate(const Matrix<double>& X) {
//        return forward_propagation(X)[layer_sizes_.size()-1];
//    }
//    std::vector<Matrix<double>> forward_propagation(const Matrix<double>& X);
//    // back propagation
//    std::vector<Matrix<double>> gradient(const Matrix<double>& X, const Matrix<double>& Y);
//    std::vector<Matrix<double>> gradient_approximation(const Matrix<double>& X, const Matrix<double>& Y, double eps);
//    // A - last matrix after forward Propagation
//    double cost(const Matrix<double>& A, const Matrix<double>& Y);
//    double efficiency(const Matrix<double>& A, const Matrix<double>& Y);
//    std::vector<std::vector<int>> report(const Matrix<double>& A, const Matrix<double>& Y);
//    
//private:
//    double delta_;
//    // size is equal to layer count
//    std::vector<int> layer_sizes_;
//    std::vector<Matrix<float>> theta_;
//    
//};

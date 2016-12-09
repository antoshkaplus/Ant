
// lets implement set like skip list

template <class T>
class SkipList {
    
public:

    
    struct HeightGen {
        HeightGen() {}
    
        HeightGen(int max, float prob) {
            std::vector<double> probs(max);
            for (auto& p : probs) {
                p = prob;
                prob *= 0.5;
            }
            distr = std::discrete_distribution<int>(probs.begin(), probs.end());
        }
        ~HeightGen() {}
        
        int operator()() {
            return distr(rng) + 1;
        }
        
    private:
        std::default_random_engine rng;
        std::discrete_distribution<int> distr;
    };


    struct Node
    {
        // number of those is equal to height
        std::vector<std::shared_ptr<Node>> next;
        T value;
        
        Node() {}
        
        Node(int h, const T& t) 
            : next(h), value(t) {}
            
        Node(int h) 
            : next(h) {}
        
        int height() const {
            return next.size();
        }
    };    
    
    
    CounterSkipList(int maxNumElems) : count(0), curHeight(0), heightGen(std::log2(maxNumElems), 0.5) {
        int maxHeight = std::log2(maxNumElems);
        
        head = std::make_shared<Node>(maxHeight);
        tail = std::make_shared<Node>(maxHeight);
    
        for (Index i = 0; i < maxHeight; ++i) {
            head->next[i] = tail;
        }
    }
    
    ~CounterSkipList() {
        auto cur = head;
        // tail items are empty
        while (cur != tail) {
            auto next = cur->next[0];
            for (auto& sh : cur->next) {
                sh.reset();
            }
            cur = next;
        }
    
    }
    
    void insert(std::shared_ptr<Node> prev, std::shared_ptr<Node> newNode, int i) {
        newNode->next[i] = prev->next[i];
        prev->next[i] = newNode;
    }
    
    void remove(std::shared_ptr<Node> prev, std::shared_ptr<Node> cur, int i) {
        prev->next[i] = cur->next[i];
    }
    
	// we don't check if already exists or not
    void insert(const T& val) {

        auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, val);
        
		curHeight = max(curHeight, height);
        
        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
			while (cur->next[i] != tail && cur->next[i]->val < val) {
                cur = cur->next[i];
            }
            if (i < height) {
                insert(cur, newNode, i);
            }
        }
        ++count;
    }
    
    void remove(const T& val) {
		
        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] != tail && cur->next[i]->val < val) {
                cur = cur->next[i];
            }
			
			if (cur->next[i]->val == val) {
                remove(cur, cur->next[i], i);
            } 
            cur = prev; 
        }
        --count;
    }
    
	// we should terminate if not found
    const T& operator[](const T& val) const {
        
        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] != tail && cur->next[i]->val[i] < val) {
                cur = cur->next[i];
            }
			
            if (cur->next[i]->val == val) {
                return cur->value;
            } 
        }
        throw std::runtime_error("out of range");
    }
    
	T* at(const T val) const {
		
		auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] != tail && cur->next[i]->val[i] < val) {
                cur = cur->next[i];
            }
			
            if (cur->next[i]->val == val) {
                return &cur->value;
            } 
        }
		return nullptr;
	}
	
    int count;
    int curHeight;
    std::shared_ptr<Node> head;
    std::shared_ptr<Node> tail;
    HeightGen heightGen;
};

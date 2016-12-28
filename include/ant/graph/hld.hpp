//
//  hld.hpp
//  Ant
//
//  Created by Anton Logunov on 12/27/16.
//
//  Heavy light decmposition


template<class DirGraph>
class HeavyLightDecomp {
    
    struct Node {
        Index parent;
        Index head;
        Index depth;
		// one child is heavy
		// used to decompose
		Index heavy;
    };

public:

    HeavyLightDecomp(const DirGraph& g, Index root) {
		// could do it in initializer
		nodes_.resize(g.nodeCount());
		// here we want to just dfs from root to all other nodes
		
		dfs1(g, root);
		nodes_[root].head = root;
		dfs2(g, root);
    }
    
    // lowest common ancestor
    Index lca(Index u, Index v) {
        auto& ns = nodes_;
		for (; ns[u].head != ns[v].head; v = ns[ns[v].head].parent) {
            if (ns[ns[u].head].depth > ns[ns[v].head].depth) {
                swap(u, v);
            }
        }
		// we have two guys.
		// we pick one that is highest
		// that's why we keep positions 
		return pos[u] < pos[v] ? u : v;
    }
	
private:
	
	// returns number of vertices in the tree
	Count dfs1(const DirGraph& g, Index root) {
		auto& ns = nodes_; 
		Count size = 1;
		Count maxSubtree = 0;
		for (auto v : g.nextNodes(root)) {
			ns[v].parent = root;
			ns[v].depth = ns[root].depth + 1;
			auto subtree = dfs(g, v);
			if (maxSubtree < subtree) {
				maxSubtree = subtree;
				ns[root].heavy = v;
			}
			size += subtree;
		}
		
		return size;
	}

	void dfs2(const DirGraph& g, Index root) {
		auto& ns = nodes_; 
		auto h = ns[root].heavy;
		for (auto v : g.nextNodes(root)) {
			ns[v].head = (v == h ? ns[root].head : v);
			dfs(g, v);
		}
	}

	std::vector<Node> nodes_;
};

Query pairs:

Currently exploring two variants.
* Dijkstra (DJ): 
    * Complexity: 
        Q * N * log N (Dijkstra per each pair)

* Dijkstra Components (DJC): 
    * Use divide and conquer approach: split graph into small subgraphs, clusters.
    For each compute distance matrix. Also compute minimum distance between those
    clusters using border elements.  
    
    * Complexity: 
        sqrt N * sqrt N * log sqrt N (Precomputation) 
        Q * sqrt N * 1/2 log N (Answer Query)  
        
    * As precomputation dominates complexity can be improved by
    disallowing cycles in the subgraph. Then when traversing the nodes
    easy to add distances for each pair between current and list of visited:
    <explored edge + distance from previous node to target>. That means that 
    precomputation drops logarithm in complexity and the whole thing 
    becomes linear. 
    
Current "Dijkstra Components" on sparse graphs with parameters:
* nodes: 100'000
* edges: nodes + 50
* max edge val: 1000
* number of queries: 50'000

improved from several minutes using brute force Dijkstra to => 10-50 seconds using Precomputation
to => 1-5 seconds using no cycles in subgraph property. 

We divide graphs:
* directed/undirected
* index type for edge/vertex
* deleting edges and vertices
* keep edges
* sparse/dense

####Definitions:

##### Graph
A graph is a mathematical structure consisting of a set of points called VERTICES and 
a set (possibly empty) of lines linking some pair of vertices. It is possible for the edges 
to oriented; i.e. to be directed edges. The lines are called EDGES if they are undirected, 
and or ARCS if they are directed.

##### Loop and Multiple edges
A loop is an edge that connects a vertex to itself. If a graph has more than one edge 
joining some pair of vertices then these edges are called multiple edges.

##### Simple Graph
A simple graph is a graph that does not have more than one edge between any two vertices 
and no edge starts and ends at the same vertex. In other words a simple graph is a graph 
without loops and multiple edges.

##### Adjacent Vertices
Two vertices are said to be adjacent if there is an edge (arc) connecting them.

##### Adjacent Edges
Adjacent edges are edges that share a common vertex.

##### Degree of a Vertex
The degree of a vertex is the number of edges incident with that vertex.

##### Path
A path is a sequence of vertices with the property that each vertex in the sequence is adjacent to the vertex next to it. A path that does not repeat vertices is called a simple path.

##### Circuit
A circuit is path that begins and ends at the same vertex.

##### Cycle
A circuit that doesn't repeat vertices is called a cycle.

##### A Connected Graph
A graph is said to be connected if any two of its vertices are joined by a path. A graph that is not connected is a disconnected graph. A disconnected graph is made up of connected subgraphs that are called components.

##### Bridge
A bridge is an edge whose deletion from a graph increases the number of components in the graph. If a graph was a connected graph then the removal of a bridge-edge disconnects it.

##### Euler Path
An Euler path is a path that travels through all edges of a connected graph.

##### Euler Circuit
An Euler circuit is a circuit that visits all edges of a connected graph.

#### Why Constness Prohibited
To have constness you have to avoid putting "model type" as template argument 
for "service types" or make user to provide it himself. That's how it's currently done.

Inside object it's not known if it's const or not instance/reference/pointer. 
Only by calling const methods we get to know.

That means that instead of "using" we should use decltype(method call) to determine 
what kind service type to use based on constness. It requires empty constructors for 
"service types" and additional nonconventional semantics.  

In reality "using" declaration has to have "const" and "non-const" just like methods
have.

**what about tree wrapper to know that current graph satisfies tree conditions:**  
It's user's responsibility to verify somewhere on data structure entry into the
workflow that it's a tree. 

**returned types has to be stadartized:**  
As the library is pretty new, it's hard to choose a common set of structs 
that will satisfy every need. That's why for now we just either have custom 
struct for each function mixed with std containers.

**in some cases there may not be vertex and/or edge descriptors, how
to store references to certain vertices and edges. it's impossible to 
mark anything then:**  
While those graph representations are possible it's clear algorithms 
won't be able to run on them. So those representations are just theory 
without practical application.  

**multilevel graphs, each new level changes view of underlying graph: 
add/remove vertices/edges**
In LEMON they a notion of SubGraph, where you can hide certain Vertices and Edges of the graph.
It's possible to implement the same for my graphs. The way they do it is that each operation is call
on the graph itself, which is not ideal. The cost of those adapters is that I will have to 
wrap everything. Nothing can be reused from the underlying graph.

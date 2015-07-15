

import sys
from sets import Set
from matplotlib import pyplot as plt


filename = sys.argv[1]


triangles_file = open("../temp/" + filename)
n_triangles = int(triangles_file.readline())
edges = Set()
for i in range(n_triangles):
    vs = map(int, triangles_file.readline().split())
    vs.sort()
    edges.add((vs[0], vs[1]))
    edges.add((vs[1], vs[2]))
    edges.add((vs[0], vs[2]))

points_file = open("../data/" + filename)
n_points = int(points_file.readline())
X, Y = [], []
for i in range(n_points):
    x, y = map(int, points_file.readline().split())
    X.append(x)
    Y.append(y)
    
XL, YL = [], []
for e in edges:
    XL.append(X[e[0]])
    YL.append(Y[e[0]])
    XL.append(X[e[1]])
    YL.append(Y[e[1]])
    XL.append(None)
    YL.append(None)
    
plt.plot(XL, YL)
plt.show()
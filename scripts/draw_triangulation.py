# has to be moved to Py modules

import sys
from sets import Set
from matplotlib import pyplot as plt


def ReadTriangleEdges(file):
    n_triangles = int(file.readline())
    edges = Set()
    for i in range(n_triangles):
        vs = map(int, file.readline().split())
        vs.sort()
        edges.add((vs[0], vs[1]))
        edges.add((vs[1], vs[2]))
        edges.add((vs[0], vs[2]))
    return edges

def ReadProblem(file):
    n_points = int(file.readline())
    X, Y = [], []
    for i in range(n_points):
        x, y = map(int, file.readline().split())
        X.append(x)
        Y.append(y)
    return X, Y


filename = sys.argv[1]

file = open("../temp/" + filename)

X, Y = ReadProblem(file)
XL, YL = [], []
for e in ReadTriangleEdges(file):
    XL.append(X[e[0]])
    YL.append(Y[e[0]])
    XL.append(X[e[1]])
    YL.append(Y[e[1]])
    XL.append(None)
    YL.append(None)
    
plt.plot(XL, YL)
plt.show()
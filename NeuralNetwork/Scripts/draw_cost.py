

from matplotlib import pyplot as plt

cost = "digits.dataset.small/cost.csv"

y1, y2, y3, y4 = zip(*map(lambda x: map(float, x.split(",")), open(cost).read().split("\n")[1:-1]))
x = range(1, len(y1)+1)

plt.subplot(121)
p1, = plt.plot(x, y1)
p2, = plt.plot(x, y2)
plt.legend((p1, p2), ("train", "test"))
plt.title("cost")
plt.subplot(122)
p3, = plt.plot(x, y3)
p4, = plt.plot(x, y4)
plt.legend((p3, p4), ("train", "test"))
plt.title("efficiency")
plt.show()
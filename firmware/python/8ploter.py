import numpy as np
import matplotlib.pyplot as plt


x, y = [], []
for i in range(100):
    k = 20
    x.append(np.sin(i/k))
    y.append(np.sin(2*i/k))

    if i == 43:
        plt.plot([x[-1], x[-1]+np.cos(i/k)/k], [y[-1], y[-1]+np.cos(2*i/k)*2/k], linewidth=7)

plt.plot(x, y)
plt.show()
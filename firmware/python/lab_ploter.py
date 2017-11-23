import matplotlib.pyplot as plt
import numpy as np


def draw_rect(axe, acx, acy, labels):
    cell_size = 0.18
    if isinstance(acx, int):
        acx = [acx]
    if isinstance(acy, int):
        acy = [acy]
    for cx, cy in zip(acx, acy):
        axe.plot(np.array([cx, cx + 1, cx + 1, cx, cx]) * cell_size, np.array([cy, cy, cy + 1, cy + 1, cy]) * cell_size,
                 'orange', linewidth=4)
    off = 0.12
    axe.text((cx-off-0.05)*cell_size, (cy+0.5)*cell_size, labels[0], horizontalalignment='center', verticalalignment='center')
    axe.text((cx+0.5)*cell_size, (cy+off)*cell_size, labels[1], horizontalalignment='center', verticalalignment='center')

f = plt.figure()
ax = f.add_subplot(111)

size = 4
for x in range(size):
    for y in range(size):
        draw_rect(ax, x, y, ['{},{}'.format(x, 2*y+1), '{},{}'.format(x, 2*y)])
for x in range(size):
    for y in range(size):
        if x != 0 or y != 0:
            ax.text((x+0.5)*0.18, (y+0.5)*0.18, '{},{}'.format(x, y), horizontalalignment='center', verticalalignment='center', color='grey')
i = np.arange(360)
ax.plot(0.04*np.sin(i) + 0.09, 0.04*np.cos(i) + 0.09, 'r')
ax.plot([0.09, 0.13], [0.09, 0.09], 'r')

ax.axis('equal')
plt.axis('off')
plt.show()
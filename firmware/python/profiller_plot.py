''' wyswietlanie trajektorii ruchu modelu bez dynamiki ruchu '''
import matplotlib.pyplot as plt
import numpy as np
import math
from SLAM import Robot


def draw_rect(axe, acx, acy):
    cell_size = 0.18
    for cx, cy in zip(acx, acy):
        axe.plot(np.array([cx, cx + 1, cx + 1, cx, cx]) * cell_size, np.array([cy, cy, cy + 1, cy + 1, cy]) * cell_size,
                 'orange')

def draw_targets(targets):
    for t in targets:
        ax.plot([t['x'], t['x']+0.07*math.cos(t['alpha'])], [t['y'], t['y'] + 0.07*math.sin(t['alpha'])], 'k')
        ax.plot(t['x'], t['y'], 'ko')


if __name__=="__main__":
    sampling_time = 0.1
    profiler = Robot.Profiler(sampling_time)
    profiler.k = [1, 2, 1]
    x = [0]
    y = [0]
    angles = [0]
    lomega = []
    lv = []
    targets = [{'x': 0.18, 'y': 0.09, 'alpha': 0},
               {'x': 0.36, 'y': 0.27, 'alpha': 0},
               {'x': 0.45, 'y': 0.36, 'alpha': 3.14/2},
               {'x': 0.36, 'y': 0.45, 'alpha': 3.14}]

    plt.ion()
    f = plt.figure()
    ax = f.add_subplot(121)
    axx = f.add_subplot(122)
    draw_rect(ax, [0, 1, 1, 2, 2, 2, 1], [0, 0, 1, 1, 1, 2, 2])
    draw_targets(targets)
    ax.axis('equal')
    ax.set_xlabel('pozycja [m]')
    ax.set_ylabel('pozycja [m]')
    ax.set_title('trajektoria')

    for i in range(100):
        current = {'x': x[-1], 'y': y[-1], 'alpha': angles[-1]}
        v, omega = profiler._get_VW(current, targets[int(i/25)], sampling_time)
        x.append(x[-1] + v*sampling_time*math.cos(angles[-1]))
        y.append(y[-1] + v*sampling_time*math.sin(angles[-1]))
        angles.append(angles[-1] + omega*sampling_time)
        lomega.append(omega)
        lv.append(v)
        l = 0.05
        ax.plot([x[-1], x[-1]+l*math.cos(angles[-1])], [y[-1], y[-1]+l*math.sin(angles[-1])], 'b')
        ax.plot([x[-2], x[-1]], [y[-2], y[-1]], 'ro')
    axx.plot(lomega, label='zadana prędkość obrotowa')
    axx.plot(lv, label='zadana prędkość liniowa')
    axx.set_xlabel('iteracja')
    axx.set_ylabel('sterowanie [rad/s] lub [m/s]')
    axx.legend()
    axx.grid()
    draw_targets(targets)
    plt.title('sterowanie  k = ' + str(profiler.k))
    plt.ioff()
    plt.show()

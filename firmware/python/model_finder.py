import numpy as np
import matplotlib.pyplot as plt
import math
from scipy.optimize import minimize

from model import Model

sampling_time = 0.001

def read_data(filename, Tsampling):
    WHEEL_DIAMETER = 37 * 1e-3
    TICKS_PER_REVOLUTION = 3520.0
    vel = []
    u = []
    with open(filename) as file:
        for l in file.readlines()[3:]:
            sp = l.split()
            ticks = int(sp[0])
            v = ticks*math.pi*WHEEL_DIAMETER/Tsampling/TICKS_PER_REVOLUTION
            vel.append(v)
            pwm = int(sp[1])
            voltage = pwm/1000*4  # 4V supply
            u.append(np.mat([[voltage], [voltage]]))
    return vel, u

def plot_step(name, style):
    p, _ = read_data(name, sampling_time)
    p = np.convolve(p, [1/5 for x in range(5)], mode='valid')
    T90 = np.argmax(p>0.9*p[-1])
    T10 = np.argmin(p<0.1*p[-1])
    plt.plot(p, style, label=name+'  T_n:'+str((T90-T10))+"ms")
    plt.xlabel('czas [ms]')
    plt.ylabel('prędkość [m/s]')
    plt.grid()
    plt.legend()
    plt.plot([T90, T90], [0, p[T90]])
    return T90 - T10


def get_trajectory(m, U, Tsampling):
    y = []
    for u in U:
        m.Y = m.measure(m.X)  # Y = C*X
        y.append(m.Y[0, 0])
        m.X = m.f(m.X, u, Tsampling)
    return y


def find_model(U, Y):
    def fun(params):
        m = Model(*params)
        y = get_trajectory(m, U, sampling_time)
        err = np.linalg.norm(np.array(y) - np.array(Y))
        print((params, err))
        return err
    X0 = [3.01742388, 2., 0.45632968]  # [0.5, 0.5, 0.5]
    res = minimize(fun, X0, bounds=([0, 1e3], [0, 2], [0, 10]))
    return res


m = Model(31, 0, 0)
u = [np.mat([[250], [250]]) for x in range(100)]
y = get_trajectory(m, u, sampling_time)
err = np.linalg.norm(np.array(y))

V250, U250 = read_data('pwm250.txt', sampling_time)
V150, U150 = read_data('pwm150.txt', sampling_time)
res250 = find_model(U250, V250)
res150 = find_model(U150, V150)
print(res250)
print(res150)
print(res250[0])
print(res150[0])
VS250 = get_trajectory(Model(res250[0]), U250)
VS150 = get_trajectory(Model(res150[0]), U150)

T0 = plot_step('pwm250.txt', 'x')
T1 = plot_step('pwm150.txt', 'd')
plt.plot(VS250)
plt.plot(VS150)
print((T0, T1))
plt.grid()
plt.show()
import numpy as np
import matplotlib.pyplot as plt
import math
import control
from scipy.optimize import minimize

from model import Model

sampling_time = 0.001


def read_data(filename, Tsampling):
    ''' read raw data from file and convert it to SI units'''
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
            u.append(voltage)
    return vel, u


def build_lin_model2(params):
    '''create model based on params'''
    A = np.mat([[0.0, 1.0],
                [params[0], params[1]]])
    B = [[0.0],
         [params[2]]]
    C = [1.0, 0.0]
    D = [0]
    return control.ss(A, B, C, D)


def build_lin_model1(params):
    '''create model based on params'''
    A = [[params[0]]]
    B = [[params[1]]]
    C = [1.0]
    D = [0]
    return control.ss(A, B, C, D)


def function(params, model_rank, oryg, u, t):
    '''cost function'''
    if model_rank == 1:
        system = build_lin_model1(params)
    else:
        system = build_lin_model2(params)
    y, _, _ = control.lsim(system, u, t)
    return np.linalg.norm(np.array(y) - np.array(oryg))


def discover_model(model_rank, Y, U, T):
    '''liniowo'''
    plt.plot(T, Y, 'kx', label='pomiary')
    # find linear model
    par = [1, 1, 1]
    par = minimize(function, par, args=(model_rank, Y, U, T))
    # plot to compare
    if model_rank == 1:
        system = build_lin_model1(par.x)
    else:
        system = build_lin_model2(par.x)
    y, _, _ = control.lsim(system, U, T)
    stat = control.matlab.damp(system)
    # plt.plot(T, y, 'o', color='gray', label='model')
    is_controllable = np.ndim(system.A) == np.ndim(control.ctrb(system.A, system.B))
    is_observable = np.ndim(system.A) == np.ndim(control.obsv(system.A, system.C))
    w = [w*(z+(1-z**2)**0.5) for w, z, e in zip(*stat)]
    k = system.B[system.B.shape[0]-1, 0]
    for _w in w:
        k /= _w
    print(stat)
    print("Obserevovable: {}  Controlable:  {}".format(is_observable, is_controllable))
    print("w: {} Hz  z: {}  K: {} m/s/s/V".format(w, stat[1], k))
    return system


if __name__ == "__main__":
    # read data and find linear models
    systems_data = []
    systems = []
    for f in ('pwm150.txt', ):  #, 'pwm250.txt'):
        print("model from file: " + f)
        V, U = read_data(f, sampling_time)
        T = np.arange(0, len(V))*sampling_time
        systems_data.append({'U': U, 'T': T, 'Y': V})
        sys = discover_model(1, **systems_data[-1])
        print(sys)

    # plot model - omit regulator
    sampling_time = 0.004
    ommits_elements = 10
    # second order
    m = Model(k=0.86, zeta=(1, 1), w_n=(2.93, 442), sampling_time=sampling_time)
    y = []
    for u in systems_data[0]["U"]:
        U = np.mat([[u], [u]])
        m.X = m.f(U, sampling_time)  # (A*dt + I)*X + B*U*dt
        y.append(m.measure(m.X)[0, 0])  # Y = C*X
    y = y[0::ommits_elements]
    plt.plot(np.arange(len(y))*sampling_time*ommits_elements, y, 'r--', label='model 2 rzędu')

    # first order
    m = Model(k=0.86, zeta=(1,), w_n=(2.93,), sampling_time=sampling_time)
    y1 = []
    for u in systems_data[0]["U"]:
        U = np.mat([[u], [u]])
        m.X = m.f(U, sampling_time)
        y1.append(m.measure(m.X)[0, 0])
    y1 = y1[0::ommits_elements]
    plt.plot(np.arange(len(y1))*sampling_time*ommits_elements, y1, 'y:', label='model 1 rzędu')

    plt.title("Odpowiedź skokowa: U="+str(systems_data[0]['U'][-1])+'V')
    plt.xlabel("czas [s]")
    plt.ylabel("prędkość [m/s]")
    plt.grid()
    plt.legend()
    plt.show()
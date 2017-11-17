import matplotlib.pyplot as plt
import numpy as np
import math
from model import Model


def saturate(x):
    max_val = 8.0
    return max_val if x > max_val else -max_val if x < -max_val else x

saturate_vector = np.vectorize(saturate)


def open_loop(r, Y):
    return r


def PID(r, Y, reset=False):
    errVP = r[0, 0] - Y[0, 0]
    errWP = r[1, 0] - Y[1, 0]
    PID.errI += np.array([errVP, errWP])
    errVD = errVP - PID.last_errP[0]
    errWD = errWP - PID.last_errP[1]
    PID.last_errP = [errVP, errWP]

    UV = 100 * errVP + 4 * errVD
    UW = 100 * errWP + 10 * errWD

    U = np.mat([[UV - UW], [UV + UW]])
    if reset:
        PID.last_error *= 0
        PID.errI *= 0
    return U
PID.last_errP = np.array([0, 0], dtype=float)
PID.errI = np.array([0, 0], dtype=float)



def simulate(regulator_fun, R, sample_time, arg_model=False):
    result = []
    model = Model(sample_time)
    for r in R:
        if not isinstance(r, (type(np.array([])), type(np.mat([[]])))):
            r = np.mat([r]).transpose()
        Y = model.measure()  # Y = C*X
        if arg_model:
            U = regulator_fun(model, r, Y)
        else:
            U = regulator_fun(r, Y)
        U = saturate_vector(U)
        model.X = model.f(U, sample_time)  # (A*dt + I)*X + B*U*d
        result.append(Y)
    V, W = [y[0, 0] for y in result], [y[1, 0] for y in result]
    a, E = V[50]/timeline[50], W[50]/timeline[50]
    return V, W, a, E


if __name__ == "__main__":
    sample_time = 0.001
    sample_len = int(0.4/sample_time)
    timeline = [i*sample_time for i in range(sample_len)]
    r = [[1, 1] for i in range(sample_len)]

    f = plt.figure()
    ax = [f.add_subplot(211), f.add_subplot(212)]

    V_open, W_open, a, E = simulate(open_loop, r, sample_time)
    ax[0].plot(timeline, V_open, 'g', label='open, a={:.1f}m/s/s'.format(a))
    ax[1].plot(timeline, W_open, 'g', label='open, E={:.1f}rad/s/s'.format(E))

    V_pid, W_pid, a, E = simulate(PID, r, sample_time)
    ax[0].plot(timeline, V_pid, 'b', label='PID, a={:.1f}m/s/s'.format(a))
    ax[1].plot(timeline, W_pid, 'b', label='PID, E={:.1f}rad/s/s'.format(E))

    V_oryg, W_oryg, a, E = simulate(Model.regulator, r, sample_time, arg_model=True)
    ax[0].plot(timeline, V_oryg, 'k', label='oryginal, a={:.1f}m/s/s'.format(a))
    ax[1].plot(timeline, W_oryg, 'k', label='oryginal, E={:.1f}rad/s/s'.format(E))

    ax[0].plot(timeline, r, 'r')
    ax[1].plot(timeline, r, 'r')
    ax[0].grid()
    ax[1].grid()
    ax[0].set_ylabel('prędkość liniowa [m/s]')
    ax[1].set_ylabel('prędkość kątowa [rad/s]')
    ax[1].set_xlabel('czas [s]')
    ax[0].legend()
    ax[1].legend()
    plt.show()

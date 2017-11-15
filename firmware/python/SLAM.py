import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
from matplotlib import gridspec
import numpy as np
import math
import scipy.linalg

import model


def lqr(A, B, Q, R):
    """Solve the continuous time lqr controller.

    dx/dt = A x + B u

    cost = integral x.T*Q*x + u.T*R*u
    """
    # ref Bertsekas, p.151

    # first, try to solve the ricatti equation
    P = np.matrix(scipy.linalg.solve_continuous_are(A, B, Q, R))

    # compute the LQR gain
    K = np.matrix(scipy.linalg.inv(R) * (B.T * P))

    eigVals, eigVecs = scipy.linalg.eig(A - B * K)

    return K, P, eigVals


def trim_angle(angle):
    while angle > math.pi:
        angle -= 2 * math.pi
    while angle < -math.pi:
        angle += 2 * math.pi
    return angle


class Robot:
    h = 0.07  # half wheelbase [m]

    class SLAM:
        def __init__(self):
            self.pos = np.array([0, 0], dtype=type(0.0))  # x, y
            self.alpha = 0.0
            self.gamma = 1.0
            self.map = np.mat([[]])

        def odometry(self, encoders):
            self.pos += 0.5*(encoders[0, 0] + encoders[1, 0])*np.array([math.cos(self.alpha), math.sin(self.alpha)])
            dalpha = self.gamma * (encoders[1, 0] - encoders[0, 0]) / (2*Robot.h)
            self.alpha = trim_angle(self.alpha + dalpha)

        def IMU(self, dalpha):
            self.alpha += (1.0-self.gamma) * dalpha

        def LED(self, reads):
            pass

        def generate_callbacks(self):
            pass

        # def new_cell_callback(self):

    class Profiler:
        def __init__(self):
            self.t = 0
            self.loc_target_pos = np.array([0, 0])
            self.loc_target_alpha = 0
            self.angles = []
            self.vel = []
            self.k = np.array([2, 6, 3], dtype=float)
            pass

        def _get_delta_to_target(self, current_pos, current_alpha):
            delta_pos = self.loc_target_pos - current_pos
            dist = math.sqrt(sum(delta_pos**2))
            angle = trim_angle(math.atan2(delta_pos[1], delta_pos[0]) - current_alpha)
            return dist, angle

        def _get_VW(self, current_pos, current_alpha):
            dist, angle = self._get_delta_to_target(current_pos, current_alpha)
            gamma = trim_angle(current_alpha - self.loc_target_alpha)
            v = self.k[0]*dist*math.cos(angle)
            sinc = 1 if angle == 0 else math.sin(angle)/angle  # sinc angle
            omega = self.k[1]*angle + self.k[0]*math.cos(angle)*sinc*(angle + self.k[2]*gamma)
            return v, omega

        def get_target_vel(self, current_pos, current_alpha):
            v, omega = self._get_VW(current_pos, current_alpha)
            vl = v - omega*Robot.h
            vr = v + omega*Robot.h
            self.angles.append(omega)
            self.vel.append(v)
            return np.mat([[vl],
                           [vr]])

    def __init__(self):
        self.model = model.Model()
        self.slam = Robot.SLAM()
        self.profiler = Robot.Profiler()

    def iterate(self, delta_time):
        pos = self.slam.pos
        r = self.profiler.get_target_vel(self.slam.pos, self.slam.alpha)
        # r = np.mat([[1], [1.1]])
        self.model.iterate(r, delta_time)
        self.slam.odometry(self.model.Y)
        return pos

    def reset(self):
        self.model.X *= 0
        self.profiler.angles = []
        self.profiler.vel = []
        self.slam.pos *= 0
        self.slam.alpha *= 0

    def cost(self, param):
        target = (100, 50, 1)
        self.reset()
        self.profiler.loc_target_pos = target[0:2]
        self.profiler.loc_target_alpha = target[2]
        cost = 0
        for i in range(300):
            robot.iterate(dt)

mm = Robot()

def trace_to(target, robot, delta_time):
    pos = []
    alpha = []
    robot.profiler.loc_target_pos = target[0:2]
    robot.profiler.loc_target_alpha = target[2]
    for i in range(30):
        robot.iterate(delta_time)
        pos.append(robot.slam.pos.tolist())
        alpha.append(robot.slam.alpha)
    return pos, alpha

dt = 0.005
tar = (10, -20, 0)
f = plt.figure()
gs = gridspec.GridSpec(2, 2)
ax = [f.add_subplot(gs[:, 1]), f.add_subplot(gs[0, 0])]


def repaint():
    l = 50
    mm.profiler.angles = []
    mm.profiler.vel = []
    mm.reset()
    #pos, angle = trace_to(tar, mm, dt)
    # narysuj target
    ax[0].clear()
    ax[0].plot(tar[0], tar[1], 'rx')
    ax[0].plot([tar[0], tar[0]+1.5*l*math.cos(tar[2])], [tar[1], tar[1]+1.5*l*math.sin(tar[2])], 'r')
    ax[0].axis('equal')
    ax[0].set_title('trajektoria')
    ax[0].set_xlabel('[m]')
    # oraz wykresy pomocnicze
    ax[1].clear()
    ax[1].legend()
    ax[1].set_title('sterowanie')
    ax[1].set_xlabel('czas [s]')
    ax[1].set_ylabel('[rad/s, m/s]')
    # rysuj trajektorie
    mm.profiler.loc_target_pos = tar[0:2]
    mm.profiler.loc_target_alpha = tar[2]
    lastPos = np.array([0, 0])
    for i in range(30):
        mm.iterate(dt)
        x, y = mm.slam.pos.tolist()
        angle = mm.slam.alpha
        w = mm.profiler.angles[-1]
        v = mm.profiler.vel[-1]
        ax[0].plot([x, x+l*math.cos(angle)], [y, y+l*math.sin(angle)], 'g')
        ax[0].plot([lastPos[0], x], [lastPos[1], y], 'b')
        ax[1].plot(i*dt, angle, 'ro', label='w_u')
        ax[1].plot(i*dt, v, 'gx', label='v_u')
        lastPos = [x, y]


axes = [plt.axes([0.05, 0.2, 0.4, 0.03]),
          plt.axes([0.05, 0.15, 0.4, 0.03]),
          plt.axes([0.05, 0.1, 0.4, 0.03])]
sliders = [Slider(a, 'k'+str(i), 0, 8, valinit=v) for i, (a, v) in enumerate(zip(axes, mm.profiler.k))]


# sliders
def update(val):
    mm.profiler.k = [s.val for s in sliders]
    repaint()
[s.on_changed(update) for s in sliders]


# clickers
def onclick(event):
    global cid
    f.canvas.mpl_disconnect(cid)
    if event.inaxes == ax[0] and event.dblclick:
        global tar
        tar = (event.xdata, event.ydata, 0)
        print(tar)
        repaint()
    cid = f.canvas.mpl_connect('button_press_event', onclick)

cid = f.canvas.mpl_connect('button_press_event', onclick)

repaint()
plt.show()







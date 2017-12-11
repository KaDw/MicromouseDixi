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
    cell_size = 0.18

    class SLAM:
        ''' bada pozycje X/Y z rozdzielczością co do mm lub większą'''
        def __init__(self, h):
            self.pos = np.array([0, 0], dtype=type(0.0))  # x, y
            self.alpha = 0.0  # aktualny kat w rad, os OX to 0, OY to pi/2
            self.gamma = 1.0  # poziom waznosci enkoderow (1) vs zyroskopu (0)
            self.h = h  # polowa rozstawu osi
            self.LAB_CELLS = 4
            self.lab = [False for i in range((self.LAB_CELLS+1)**2)]  # mapa labiryntu
            self.glob_deltax = {'N': 0, 'E': 1, 'S': 0, 'W': -1}
            self.glob_deltay = {'N': 1, 'E': 0, 'S': -1, 'W': 0}
            self.last_zone = [0, 0]

        def odometry(self, encoders):
            self.pos += 0.5*(encoders[0, 0] + encoders[1, 0])*np.array([math.cos(self.alpha), math.sin(self.alpha)])
            dalpha = self.gamma * (encoders[1, 0] - encoders[0, 0]) / (2*self.h)
            self.alpha = trim_angle(self.alpha + dalpha)

        def IMU(self, dalpha):
            self.alpha += (1.0-self.gamma) * dalpha

        def LED(self, reads):
            pass

        def _wall_bit(self, x, y, glob_dir):
            '''zwraca numer bitu odpowiedzialnego za dana sciane'''
            return x+self.glob_deltax[glob_dir] + self.LAB_CELLS*(2*y + self.glob_deltay[glob_dir]+1)

        def _set_wall(self, x, y, glob_dir):
            bit = self._wall_bit(x, y, glob_dir)
            self.lab[bit] = True

        def is_wall(self, x, y, glob_dir):
            bit = self._wall_bit(x, y, glob_dir)
            return self.lab[bit]

        def check_new_cell(self):
            '''sprawdza, czy przekroczylismy linie rozdzielajaca 2 komorki labiryntu'''
            local_pos = [p % Robot.cell_size for p in self.pos]
            zone_size = 0.05 * Robot.cell_size  # relative to cell size
            zone = [1 if p < zone_size else -1 if p > Robot.cell_size - zone_size else 0 for p in local_pos]
            for i in range(2):
                if zone[i] != self.last_zone[i]:
                    pass
            self.last_zone = zone

        def generate_callbacks(self):
            pass

    class Profiler:
        def __init__(self, h):
            self.t = 0
            self.angles = []
            self.vel = []
            self.k = np.array([0.08, 0.23, 0.03], dtype=float)
            max_lin_acc = 5  # m/s/s
            max_rot_acc = 100  # rad/s/s
            self.max_acceleration = min(max_lin_acc, max_rot_acc*h)
            # zmienne stanu
            self.last_vw = (0, 0)
            pass

        def _get_delta_to_target(self, current_pos, current_target):
            delta_pos = np.array([current_target['x']-current_pos['x'], current_target['y'] - current_pos['y']])
            dist = math.sqrt(sum(delta_pos**2))
            angle = trim_angle(math.atan2(delta_pos[1], delta_pos[0]) - current_pos['alpha'])
            gamma = trim_angle(current_pos['alpha'] - current_target['alpha'])
            return dist, angle, gamma

        def get_VW(self, current_pos, current_target, dT):
            dist, angle, gamma = self._get_delta_to_target(current_pos, current_target)
            sinc = 1 if angle == 0 else math.sin(angle)/angle  # sinc angle
            omega = self.k[1]*angle + self.k[0]*math.cos(angle)*sinc*(angle + self.k[2]*gamma)
            v = self.k[0]*dist*math.cos(angle) #* (1/(omega*omega+1))
            max_A = 8*dT  # fizycznie podobno do 20 m/s/s, ale dochodzi ruch obrotowy
            max_E = max_A/Robot.h  # pomnóż przez h rad/s/s
            v = max(self.last_vw[0]-max_A, min(v, self.last_vw[0]+max_A))
            omega = max(self.last_vw[1]-max_E, min(omega, self.last_vw[1]+max_E))
            self.last_vw = [v, omega]
            self.angles.append(omega)
            self.vel.append(v)
            return v, omega

        def get_target_vel(self, current_pos, current_target, dT):
            v, omega = self.get_VW(current_pos, current_target, dT)
            vv = [v - omega*Robot.h, v + omega*Robot.h]
            return np.mat([[vv[0]],
                           [vv[1]]])

    class Controller:
        def __init__(self, start_angle):
            self.max_lead_dist = 0.18  # maksymalne oddalenie celu od nas
            self.min_lead_dist = 0.01  # dystans przy którym zmieniamy cell na kolejny
            self.target = np.array([5.5, 5.5]) * Robot.cell_size
            self.trace_iterator = 0
            self.trace = ['N', 'W', 'E', 'E', 'W', 'W', 'N', 'N']  # from Djikstry
            self.delta = {'N': [1, 0], 'E': [0, 1], 'S': [-1, 0], 'W': [0, -1]}
            self.last_cell = [0, 0]
            self.wait_for_last_cell = False
            self.current_target = {'x': Robot.cell_size*2.5, 'y': Robot.cell_size*0.5, 'alpha': 0}
            self.next_target = {'x': Robot.cell_size/2, 'y': Robot.cell_size/2, 'alpha': 0}

        def new_cell_callback(self, new_cell, pslam):
            ''' aktualizuje punkt docelowy (z rozdzielczoscia komorek)
            wywolywane gdy kolejna komorka zostanie juz odkryta i zapisana w mapie'''
            global_dir = self.trace[self.trace_iterator]
            if pslam.is_wall():  # nie ma sciany do nastepnej komorki
                pass
            new_alpha = math.atan2(self.delta[global_dir][1], self.delta[global_dir][0])
            new_pos = (new_cell[0]+0.5+0.5*self.delta[global_dir][0])*Robot.cell_size,\
                      (new_cell[1]+0.5+0.5*self.delta[global_dir][1])*Robot.cell_size
            self.current_target = {'x': new_pos[0],
                                   'y': new_pos[1],
                                   'alpha': new_alpha}
            self.next_target = {'x': new_pos[0]+0.5*Robot.cell_size*self.delta[global_dir][0],
                                'y': new_pos[1]+0.5*Robot.cell_size*self.delta[global_dir][1],
                                'alpha': math.atan2(self.delta[global_dir][1], self.delta[global_dir][0])}
            self.trace_iterator += 1

        def get_new_target(self, current_position):
            ''' zwraca punkt za ktorym podazamy '''
            if self.current_target['x'] < 0:  # gdy nie wiadomo gdzie jechac
                return self.current_target
            else:
                d = np.array([self.current_target['x']-current_position['x'],\
                             self.current_target['y']-current_position['y'],\
                             (self.current_target['alpha']-current_position['alpha'])])
                d[2] = trim_angle(d[2])*Robot.h
                dist = np.linalg.norm(d)
                if dist > self.max_lead_dist:  # gdy cel jest zbyt daleko, to go przeskaluj
                    d *= self.max_lead_dist / dist
                # elif dist < self.min_lead_dist:  # gdy jest zbyt blisko
                #     if self.next_target['x'] > 0 and self.next_target['y'] > 0:  # zamien na kolejny
                #         self.current_target = self.next_target.copy()
                #         self.next_target['x'] = self.next_target['y'] = -1
                #         return self.get_new_target(current_position)
            return {'x': current_position['x']+d[0],
                    'y': current_position['y']+d[1],
                    'alpha': current_position['alpha']+d[2]}

    def __init__(self, sampling_time):
        self.model = model.Model(sampling_time)
        self.slam = Robot.SLAM(self.h)
        self.profiler = Robot.Profiler(self.h)
        self.controller = Robot.Controller(self.slam.alpha)

    def get_controller_inputs(self):
        pos = np.array([self.slam.pos[0], self.slam.pos[1], self.slam.alpha])
        current_position = {'x': pos[0], 'y': pos[1], 'alpha': pos[2]}
        current_target = self.controller.get_new_target(current_position)
        delta = self.profiler._get_delta_to_target(current_position, current_target)
        state = self.model.Y
        return [*delta, *state.flatten().tolist()[0], self.slam.alpha]

    def iterate(self, delta_time):
        pos = np.array([self.slam.pos[0], self.slam.pos[1], self.slam.alpha])
        current_position = {'x': pos[0], 'y': pos[1], 'alpha': pos[2]}
        current_target = self.controller.get_new_target(current_position)
        r = self.profiler.get_VW(current_position, current_target, delta_time)
        self.model.iterate(r, delta_time)
        self.slam.odometry(self.model.Y)
        return pos

    def reset(self):
        self.model.X *= 0
        self.profiler.angles = []
        self.profiler.vel = []
        self.slam.pos *= 0
        self.slam.alpha *= 0

if __name__=="__main__":
    dt = 0.001
    mm = Robot(dt)

    tar = (10, -20, 0)
    f = plt.figure()
    gs = gridspec.GridSpec(2, 2)
    ax = [f.add_subplot(gs[:, 1]), f.add_subplot(gs[0, 0])]

    def draw_rect(axe, acx, acy):
        cell_size = 0.18
        for cx, cy in zip(acx, acy):
            axe.plot(np.array([cx, cx+1, cx+1, cx, cx])*cell_size, np.array([cy, cy, cy+1, cy+1, cy])*cell_size, 'orange')

    def repaint():
        l = 0.1
        ax[0].clear()
        ax[1].clear()
        plt.ion()
        mm.profiler.angles = []
        mm.profiler.vel = []
        mm.reset()
        # narysuj komorke
        draw_rect(ax[0], [0, 1, 1, 2, 2], [0, 0, 1, 1, 2])
        # rysuj trajektorie
        mm.profiler.loc_target_pos = tar[0:2]
        mm.profiler.loc_target_alpha = tar[2]
        lastPos = np.array([0, 0])
        for i in range(60):
            mm.iterate(dt)
            x, y = mm.slam.pos.tolist()
            angle = mm.slam.alpha
            w = mm.profiler.angles[-1]
            v = mm.profiler.vel[-1]
            ax[0].plot([x, x+l*math.cos(angle)], [y, y+l*math.sin(angle)], 'g')
            ax[0].plot(x, y, 'bo')
            ax[1].plot(i*dt, w, 'ro', label='w_u')
            ax[1].plot(i*dt, v, 'gx', label='v_u')
            lastPos = [x, y]
            plt.draw()
        plt.ioff()


    axes = [plt.axes([0.05, 0.2, 0.4, 0.03]),
              plt.axes([0.05, 0.15, 0.4, 0.03]),
              plt.axes([0.05, 0.1, 0.4, 0.03])]
    sliders = [Slider(a, 'k'+str(i), 0, 1, valinit=v) for i, (a, v) in enumerate(zip(axes, mm.profiler.k))]


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

    ax[0].axis('equal')
    ax[0].set_title('trajektoria')
    ax[0].set_xlabel('[m]')
    ax[1].set_title('sterowanie')
    ax[1].set_xlabel('czas [s]')
    ax[1].set_ylabel('[rad/s, m/s]')
    ax[1].legend()
    repaint()
    plt.show()






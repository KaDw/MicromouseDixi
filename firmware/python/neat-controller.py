import os

import scipy.optimize as opt
import numpy as np
import math
import matplotlib.pyplot as plt

import neat

import visualize
import SLAM

delta_time = 0.001
k = 1000
s = 0.3
targets = [np.array([np.sin(x/k)*s, np.sin(2*x/k)*s, math.atan2(2/k*np.cos(2*x/k)*s, np.cos(x/k)/k)*s]) + [0.09, 0.09, 0] for x in range(5000)]
split = 2000
targets = [(x*s/split, 0.09, 0) if x < split else (s, 0.09+(x-split)*s/split, 3.14/2) for x in range(4000)]


def trim_angle(angle):
    while angle > math.pi:
        angle -= 2 * math.pi
    while angle < -math.pi:
        angle += 2 * math.pi
    return angle


class Robot:
    h = 0.05
    def __init__(self):
        self.sampling_time = 0.001
        k = 0.86
        zeta = (1,)
        w_n = (2.93,)
        w = [w*(z+(1-z**2)**0.5) for w, z in zip(w_n, zeta)]
        if len(w) == 1:
            self.A = np.mat([
                [-w[0], 0],
                [0, -w[0]]
            ], dtype=type(0.0))
            self.B = np.mat([
                [k * w[0], 0],
                [0, k * w[0]]
            ], dtype=type(0.0))
            self.C = np.mat([
                [1, 0],
                [0, 1]
            ], dtype=type(0.0))
        I = np.mat(np.eye(self.A.shape[0]))
        self.dA = (I + self.A*self.sampling_time/2)*(I - self.A*self.sampling_time/2)**(-1)
        self.X = np.zeros((self.A.shape[0], 1))
        self.pos = np.zeros([2, 1], dtype=float)
        self.alpha = 0

    def velocity(self):
        return np.array((self.C*self.X).flatten())[0]

    def position(self):
        return np.array(self.pos.flatten())

    def iterate(self, r):
        ''' transfer function '''
        # dynamic
        max_val = 8
        _r = np.mat([-max_val if rr < -max_val else max_val if rr > max_val else rr for rr in r]).transpose()
        self.X += (self.dA * self.X + self.B * _r) * self.sampling_time

        # odometry
        vel = self.C*self.X
        dpos = 0.5 * self.sampling_time * (vel[0, 0] + vel[1, 0]) * np.mat([math.cos(self.alpha), math.sin(self.alpha)])
        dalpha = self.sampling_time * (vel[1, 0] - vel[0, 0]) / (2*self.h)
        self.pos += dpos.transpose()
        self.alpha = trim_angle(self.alpha + dalpha)


def get_controller_input(robot, target):
    current = np.array(robot.pos.flatten())
    delta = np.array(target)-[*current, robot.alpha]
    delta_alpha = trim_angle(target[2] - robot.alpha)
    delta_gamma = trim_angle(math.atan2(delta[1], delta[0]) - robot.alpha)
    vel = robot.velocity()
    return [math.sqrt(delta[0]**2+delta[1]**2), delta_gamma, delta_alpha, (vel[0]+vel[1])/2, (vel[1]-vel[0])/2/Robot.h]


def get_voltage(v, omega):
    return [max(-8, min(8, v-omega*SLAM.Robot.h)), max(-8, min(8, v+omega*SLAM.Robot.h))]


#def simulate_(k=[8.50370370e-02,   2.34259259e-01,   1.06481482e-04]):
#def simulate_(k=[8.50370370e-02,   2.34259259e-01,   1.06481482e-04]):
def simulate_(k=[0.08, 0.23, 0.00]):
    k = [5, 12, 0]
    robot = Robot()
    pos = []
    r_hist = []
    y_hist = []
    u_hist = []
    last_delta = np.array([0, 0, 0])
    for target in targets:
        dist, dgamma, dalpha, c_v, c_w = get_controller_input(robot, target)

        # sterownik kinetyczny
        sinc_dalpha = 1 if dalpha == 0 else math.sin(dalpha) / dalpha  # sinc angle
        v = k[0]*math.cos(dgamma) * dist
        omega = k[1] * dgamma + k[0]*math.cos(dalpha)*sinc_dalpha * (dgamma + k[2] * dalpha)

        # sterownik dynamiczny
        r = get_voltage(10*(v-c_v), 2*(omega-c_w))

        # model
        robot.iterate(r)

        # zapis trajektorii
        pos.append([*robot.pos.flatten().tolist(), robot.alpha])
        r_hist.append(r)  # sterowanie silnikami
        y_hist.append(robot.velocity())  # prędkości silników
        u_hist.append([dist, dalpha, dgamma])  # delty do celu
    return pos, r_hist, y_hist, u_hist


def rate_simulate_(k):
    _, r_hist, y_hist, u_hist = simulate_(k)
    #err = np.linalg.norm(r_hist) + np.linalg.norm(u_hist)**2
    err = np.linalg.norm(u_hist)**2
    return err


def simulate_findBestK():
    #res = opt.fmin(rate_simulate_, [5.08, 7, 2], disp=3)
    res = opt.fmin_slsqp(rate_simulate_, [5, 4, 2], disp=3, bounds=[(0,100), (0, 100), (0, 100)])
    return res


def simulate(genome, config):
    global delta_time
    robot = Robot()
    # build controller
    net = neat.nn.FeedForwardNetwork.create(genome, config)
    pos = []
    r_hist = []
    y_hist = []
    u_hist = []
    for target in targets:
        controler_input = get_controller_input(robot, target)
        g = controler_input[1]
        controler_input[1] = 1 if g == 0 else math.sin(g)/g

        _v, _omega = net.activate(controler_input)
        r = get_voltage(_v, _omega)

        robot.iterate(r)
        pos.append(robot.position())
        r_hist.append(r)  # sterowanie silnikami
        y_hist.append(robot.velocity())  # prędkości silników
        u_hist.append(controler_input[0:2])  # delty do celu
    return pos, r_hist, y_hist, u_hist


def rate_genome(genome, config):
    try:
        _, r_hist, y_hist, u_hist = simulate(genome, config)
        err = 0
        # err += np.linalg.norm(r_hist)
        # err += np.linalg.norm(y_hist)
        err += np.linalg.norm(u_hist)**2
        #print(np.linalg.norm(r_hist), err)
        return -err
    except Exception as e:
        print(e)
        return -10000.0


def eval_genomes(genomes, config):
    for genome_id, genome in genomes:
        genome.fitness = rate_genome(genome, config)


def plot_rect(acx, acy):
    cell_size = 0.18
    for cx, cy in zip(acx, acy):
        plt.plot(np.array([cx, cx+1, cx+1, cx, cx])*cell_size, np.array([cy, cy, cy+1, cy+1, cy])*cell_size, '--', color='orange')


def plot_robot(pos):
    phi = pos[2]
    R = np.mat([[math.cos(phi), -math.sin(phi)], [math.sin(phi), math.cos(phi)]])
    X = [R*np.mat([[0], [Robot.h]]), R*np.mat([[0], [-Robot.h]])]
    p = np.array([pos[0], pos[1]])
    p1 = [(p + X[0].transpose()).tolist()[0], (p+X[1].transpose()).tolist()[0]]
    p1 = list(zip(*p1))
    plt.plot(p1[0], p1[1], '-r')
    plt.plot(p1[0], p1[1], 'or')


def plot_target():
    targ = list(zip(*targets))
    plt.plot(targ[0], targ[1], '-g', label='trajektoria zadana')


def plot_classic():
    posc, r_hist, y_hist, u_hist = simulate_()
    plot_robot(posc[0])
    plot_robot(posc[-1])
    #for p in posc[::1000]:
    #    plot_robot(p)
    posc = list(zip(*posc))
    plt.plot(posc[0], posc[1], '-.', label='sterownik liniowy', color='k')
    # print('delty, sterowanie')  # sterowanie silnikami, predkosci silnikow, delty odl alhpa i gamma
    # for r, y, u in zip(r_hist, y_hist, u_hist):
    #     print('{}\t{}\t{}'.format(u, r, y))


def run(config_file):
    # Load configuration.
    config = neat.Config(neat.DefaultGenome, neat.DefaultReproduction,
                         neat.DefaultSpeciesSet, neat.DefaultStagnation,
                         config_file)
    #config.save('test_save_config.txt')

    # Create the population, which is the top-level object for a NEAT run.
    p = neat.Population(config)

    # Add a stdout reporter to show progress in the terminal.
    p.add_reporter(neat.StdOutReporter(True))
    stats = neat.StatisticsReporter()
    p.add_reporter(stats)

    # Run for up to 1000 generations.
    pe = neat.ParallelEvaluator(4, rate_genome)
    p.run(pe.evaluate, 15)

    # Write run statistics to file.
    stats.save()

    # Display the winning genome.
    winner = stats.best_genome()
    print('\n\bNajlepszy genom:\ndopasowanie {!s}\n{!s}'.format(winner.fitness, winner))

    posn, rn, yn, un = simulate(winner, config)
    print('r: ', np.linalg.norm(rn))
    print('y: ', np.linalg.norm(yn))
    print('u: ', np.linalg.norm(un))
    print('max u: ', np.max(un))
    posn = list(zip(*posn))
    plot_rect([0], [0])
    plot_classic()
    plot_target()
    plt.plot(posn[0], posn[1], ':', label='NEAT')
    plt.legend()
    plt.title('Trajektoria')
    plt.axis('equal')
    plt.xlabel('[m]')
    plt.ylabel('[m]')
    plt.show()

    visualize.plot_stats(stats, ylog=False, view=True)
    visualize.plot_species(stats, view=False)


def plot_robo(pos, alpha):
    phi = pos[2]
    R = np.mat([[math.cos(phi), -math.sin(phi)], [math.sin(phi), math.cos(phi)]])
    X = [R*np.mat([[0], [Robot.h]]), R*np.mat([[0], [-Robot.h]])]
    p = np.array([pos[0], pos[1]])
    p1 = [(p + X[0].transpose()).tolist()[0], (p+X[1].transpose()).tolist()[0]]
    p1 = list(zip(*p1))
    plt.plot(p1[0], p1[1], '-r')
    plt.plot(p1[0], p1[1], 'or')


if __name__ == '__main__':
    #simulate_findBestK()
    local_dir = os.path.dirname(__file__)
    print(local_dir, os.getcwd())
    config_path = os.path.join(local_dir, 'config')
    run(config_path)
    plot_rect([0], [0])
    plot_classic()
    plot_target()
    plt.legend()
    plt.title('Trajektoria')
    plt.axis('equal')
    #plt.show()

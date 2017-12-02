import simulator
import modules
import matplotlib.pyplot as plt
import numpy as np
import random
import time
import RPN
import model_finder


class Optimizer:
    def __init__(self, pop_size, args):
        self.o_t = None
        self.o_u = []
        self.o_y = []
        self.avg_y = 0
        self.population = []
        self.rates = []
        self.evol_counter = 0
        for i in range(pop_size):
            s = simulator.Simulator(*args)  # RPN.SimulatorRPN(*args)
            s.mutate()
            self.population.append(s)
            self.rates.append(0)

    def prepare(self, x, y, t):
        self.o_t = x
        self.o_y = y
        self.o_t = t
        self.avg_y = sum([sum(i) for i in y])/len(y)/len(y[0])

    def _get_simulator_error(self, s, iter_range):
        assert len(self.o_u) == len(self.o_y)
        assert self.o_t is None or len(self.o_t) == len(self.o_u)
        s.reset_states()
        error = 0.0
        #try:
        y = s.simulate(self.o_u, self.o_t)
        error = np.array(self.o_y) - np.array(y)
        #except Exception as e:
            # print("exception: ", e)
            # error = self.avg_y*len(self.o_y)
        if iter_range != None:
            error = error[int(iter_range[0]):min(int(iter_range[1]), len(self.o_t))]
        return np.sum(error*error)

    def rate_simulator(self, s, max_error, iter_range=None):
        err = self._get_simulator_error(s, iter_range)
        if isinstance(s, simulator.Simulator):
            err += (len(s.modules) + len(s.nodes))**2 * self.avg_y
        elif isinstance(s, RPN.SimulatorRPN):
            err += (len(s.state_equations)-s.inputs_num)**2 * self.avg_y
        if err != err or err == float('inf'):  # is nan
            err = max_error
        if err > max_error:
            err = max_error
        return err

    def tourney_select(self):
        ''' random 'size' individuals
        select best one with probability p
        or second best with probavility p*(1-p) and so on'''
        tourney_size, tourney_p = 3, 0.6
        pop_len = len(self.population)
        participants = [random.randint(0, pop_len-1) for i in range(tourney_size)]
        rates = [self.rates[p] for p in participants]
        rates, participants = (list(t) for t in zip(*sorted(zip(rates, participants))))  # sort
        for part in participants:
            if random.random() < tourney_p:
                return self.population[part]
        return self.population[participants[-1]]

    def rigid_selection(self):
        '''select random parent from best part of population
        use rigid parent/non parent border'''
        reproductive = 0.7 * len(self.population)
        i = random.randint(0, reproductive)
        return self.population[i]

    def new_population(self, elite):
        ''' create new generation basing on current one'''
        selector = self.tourney_select
        cross_fun = self.population[0].cross
        self.population[elite:] = [cross_fun(selector(), selector()) for i in range(len(self.population[elite:]))]

    def evololution(self):
        elite = 2
        section_len = len(self.o_t) / 2
        # cross individuals with score worse than survived
        # parents are reproductive individuals
        self.new_population(elite)
        # mutate individuals with score worse than survived
        for p in self.population[elite:]:
            p.mutate()
            if isinstance(self.population[0], RPN.SimulatorRPN):
                p.simplify()
            #p.validate()
        # rate simulators
        i = self.evol_counter % int(len(self.o_t)-section_len)
        err_window = None  #(i, i+section_len)
        max_err = sum([sum(y)**2 for y in self.o_y])
        self.rates = [self.rate_simulator(p, max_err, err_window) for p in self.population]
        # sort them
        self.rates, self.population = [list(x) for x in zip(*sorted(zip(self.rates, self.population), key=lambda pair: pair[0]))]
        self.evol_counter += 1


def redraw(ax, o):
    best = o.population[0]
    ax[0].clear()
    ax[1].clear()
    ax[2].clear()
    #ax[3].clear()
    best.reset_states()
    y = best.simulate(o.o_u, o.o_t)
    ax[0].plot(o.o_t, o.o_y, '--', label='oryginal')
    plt.pause(0.0001)
    ax[0].plot(o.o_t, y, label='symulator')
    plt.pause(0.0001)
    ax[1].semilogy(range(len(err)), err, '--', label='minimalny')
    ax[2].semilogy(range(len(err_avg)), err_avg, '-', label='średni')
    # ax[1].set_ylim([0, 2*err[0]])
    #ax[3].plot(dist_hamming)
    plt.pause(0.0001)
    ax[0].legend()
    #ax[1].legend()
    ax[2].legend()
    ax[0].set_title('Odpowiedź')
    ax[0].set_xlabel('czas')
    ax[0].set_ylabel('odpowiedz')
    ax[1].set_ylabel('min błąd')
    ax[2].set_ylabel('śr błąd')
    ax[2].set_xlabel('pokolenie')
    #ax[3].set_ylabel('odl. Hammminga')
    plt.legend()
    plt.pause(0.001)
    plt.draw()

if __name__ == "__main__":
    os = simulator.Simulator(1, 1)
    os.add_node()
    os.add_module(modules.dynAmplifier([5]), 0, 2)
    os.add_module(modules.dynSaturation([0, 4]), 2, 1)

    o = Optimizer(pop_size=300, args=(1, 1))
    # cnt = 200
    # o.o_u = [[np.sin(x/10)] for x in range(cnt)]
    # o.o_t = [t+0.01 for t in range(cnt)]
    # o.o_y = os.simulate(o.o_u, o.o_t)

    T_sampling = 0.001
    ly, lu = model_finder.read_data('pwm150.txt', T_sampling)
    o.o_y = [[y] for y in ly]
    o.o_u = [[u] for u in lu]
    o.o_t = [T_sampling*x for x in range(len(o.o_y))]


    plt.ion()
    f, ax = plt.subplots(2, sharex=False)
    ax = np.insert(ax, 1, ax[1].twinx())
    for a in ax:
        a.plot([1], [1])

    ts_start = time.clock()
    iterations = 80
    err = []
    err_avg = []
    dist_hamming = []
    for n in range(iterations):
        o.evololution()
        s1 = o.population[0]
        s2 = o.population[5]
        mod = ()  #(len(s1.modules), len(s2.modules))
        nod = ()  #(len(s1.nodes), len(s2.nodes))
        err.append(o.rates[0])
        err_avg.append(np.mean(o.rates))
        dist_hamming.append(np.mean([p.hamming_distance_to(o.population[0]) for p in o.population[1:]]))
        print("iter {} fittest {:.2e} mod:{} node:{} time:{}s".format(n, int(err[-1]), mod, nod, (int)(time.clock()-ts_start)))
        if n % 10 == 0:
            best = o.population[0]
            print(best)
            redraw(ax, o)
    t = time.clock()-ts_start
    redraw(ax, o)
    model = o.population[0]

    print('')
    print('-----------------------------------------------------------------------------------------')
    print(model)
    print("czas przeszukiwania: {} ({} per individual)".format(t, t/iterations/len(o.population)))
    print("error {}".format(o.rates[0]))
    print("population size: ", len(o.population))

    model.reset_states()

    y = model.simulate(o.o_u, o.o_t)

    plt.ioff()
    if isinstance(model, simulator.Simulator):
        plt.figure('model')
        model.plot()
    plt.show()
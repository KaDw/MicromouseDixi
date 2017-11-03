import simulator
import modules
import matplotlib.pyplot as plt
import numpy as np
import random
import copy
import time


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
            s = simulator.Simulator(*args)
            s.mutate()
            self.population.append(s)
            self.rates.append(self.rate_simulator(s))

    def prepare(self, x, y, t):
        self.o_t = x
        self.o_y = y
        self.o_t = t
        self.avg_y = sum([sum(i) for i in y])/len(y)/len(y[0])

    def _get_simulator_error(self, s, iter_range):
        assert len(self.o_u) == len(self.o_y)
        assert self.o_t is None or len(self.o_t) == len(self.o_u)
        s.reset_states()
        y = s.simulate(self.o_u, self.o_t)
        error = np.array(self.o_y) - np.array(y)
        if iter_range != None:
            error = error[int(iter_range[0]):min(int(iter_range[1]), len(self.o_t))]
        return np.sum(error*error)

    def rate_simulator(self, s, iter_range=None):
        err = self._get_simulator_error(s, iter_range)
        err += (len(s.modules) + len(s.nodes)) * self.avg_y
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

    def new_population(self):
        ''' create new generation basing on current one'''
        elite = 1
        selector = self.tourney_select
        self.population[elite:] = [selector().cross(selector()) for i in range(len(self.population[elite:]))]

    def evololution(self):
        section_len = len(self.o_t) / 2
        pop_len = len(self.population)

        # cross individuals with score worse than survived
        # parents are reproductive individuals
        self.new_population()
        # mutate individuals with score worse than survived
        [p.mutate() for p in self.population]
        # rate simulators
        self.rates = [self.rate_simulator(p) for p in self.population]
        # sort them
        self.rates, self.population = [list(x) for x in zip(*sorted(zip(self.rates, self.population), key=lambda pair: pair[0]))]
        self.evol_counter += 1


if __name__ == "__main__":
    o = Optimizer(pop_size=200, args=(1, 1))

    os = simulator.Simulator(1, 1)
    os.add_node()
    os.add_module(modules.dynAmplifier([10]), 0, 2)
    os.add_module(modules.dynSaturation([0, 40]), 2, 1)

    cnt = 200
    o.o_u = [[1] for x in range(cnt)]
    o.o_t = [t+0.01 for t in range(cnt)]
    o.o_y = os.simulate(o.o_u, o.o_t)

    ts_start = time.clock()
    iterations = 50
    err = []
    for n in range(iterations):
        #try:
            o.evololution()
            s1 = o.population[4]
            s2 = o.population[20]
            mod = (len(s1.modules), len(s2.modules))
            nod = (len(s1.nodes), len(s2.nodes))
            err.append(o.rate_simulator(s1))
            print("iter {} fittest {:.1e} mod:{} node:{} time:{}s".format(n, int(err[-1]), mod, nod, (int)(time.clock()-ts_start)))
            if n % 10 == 0:
                print(o.population[0])
        #except Exception as e:
         #   print('Exception {}'.format(str(e)))
    t = time.clock()-ts_start
    model = o.population[0]

    print("czas przeszukiwania: {} ({})".format(t, t/iterations/len(o.population)))
    print("error {}".format(o.rate_simulator(model)))
    print(model)

    model.reset_states()

    y = []
    lastT = 2*o.o_t[0] - o.o_t[1]
    for u, T in zip(o.o_u, o.o_t):
        model.set_inputs(u)
        model.make_step(T-lastT)
        y.append(model.get_outputs())
        lastT = T
    model.plot()
    f, ax = plt.subplots(2, sharex=False)
    ax[0].plot(o.o_t, o.o_y, label='oryginal')
    ax[0].plot(o.o_t, y, label='symulator')
    ax[0].legend()
    ax[0].set_title('Odpowiedź')
    ax[0].set_xlabel('czas')
    ax[0].set_ylabel('odpowiedz')
    ax[1].semilogy(err)
    #ax[1].set_ylim([0, np.mean(err)])
    ax[1].set_xlabel('pokolenie')
    plt.legend()
    plt.show()
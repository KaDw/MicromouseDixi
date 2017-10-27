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
        for i in range(pop_size):
            s = simulator.Simulator(*args)
            self.mutate(s)
            self.population.append(s)

    def prepare(self, x, y, t):
        self.o_t = x
        self.o_y = y
        self.o_t = t
        self.avg_y = sum([sum(i) for i in y])/len(y)/len(y[0])

    def _get_simulator_error(self, s):
        assert len(self.o_u) == len(self.o_y)
        assert self.o_t is None or len(self.o_t) == len(self.o_u)
        s.reset_states()
        y = s.simulate(self.o_u, self.o_t)
        error = np.array(self.o_y) - np.array(y)
        return np.sum(error*error)

    def rate_simulator(self, s):
        err = self._get_simulator_error(s)
        err += (len(s.modules) + len(s.nodes)) * self.avg_y
        return err

    @staticmethod
    def _random_module():
        r = random.randint(0, 3)
        m = None
        if r == 0:
            m = modules.dynAmplifier()
        elif r == 1:
            m = modules.dynIntegrator()
        elif r == 2:
            m = modules.dynDifferentiator()
        elif r == 3:
            m = modules.dynSaturation()
        else:
            assert AssertionError()
        return m

    def mutate(self, s):
        # add new module
        if random.random() < 0.01:
            m = self._random_module()
            node_count = len(s.nodes)
            s.add_module(m, random.randint(0, node_count-1), random.randint(0, node_count-1))
        # add new node with modules
        if random.random() < 0.01:
            n = s.add_node()
            node_count = len(s.nodes)
            s.add_module(self._random_module(), random.randint(0, node_count-1), n)
            s.add_module(self._random_module(), n, random.randint(0, node_count-1))
        # delete module
        if random.random() < 0.03:
            mi = random.randint(0, len(s.modules))
            if mi < len(s.modules):
                s.del_module(mi)
        # mutate param
        if random.random() < 0.3:
            mi = random.randint(0, len(s.modules))
            if mi < len(s.modules):
                for p in s.modules[mi].param:
                    p += 10*(random.random()-0.5)

    @staticmethod
    def cross(s1, s2):
        nmodules = int(min(len(s1.modules), len(s2.modules)) * random.random())
        newsim = copy.deepcopy(s2)
        newsim.modules[0:nmodules] = copy.deepcopy(s1.modules[0:nmodules])
        newsim.modInNodeNum[0:nmodules] = copy.deepcopy(s1.modInNodeNum[0:nmodules])
        newsim.modOutNodeNum[0:nmodules] = copy.deepcopy(s1.modOutNodeNum[0:nmodules])
        return newsim

    def evololution(self):
        pop_len = len(self.population)
        best = int(0.01*pop_len)
        reproductive = int(0.5*pop_len)

        # cross individuals with score worse than survived
        # parents are reproductive individuals
        self.population[best:] = [self.cross(p, self.population[random.randint(0, reproductive)])
                                  for p in self.population[best:]]

        # mutate individuals with score worse than survived
        [self.mutate(p) for p in self.population[best:]]

        # rate simulators
        rates = [self.rate_simulator(p) for p in self.population]

        # sort them
        rates, self.population = [list(x) for x in zip(*sorted(zip(rates, self.population), key=lambda pair: pair[0]))]


if __name__ == "__main__":
    o = Optimizer(pop_size=200, args=(1, 1))

    cnt = 300
    o.o_y = [[x if x < 30 else 30] for x in range(cnt)]
    o.o_u = [[1] for x in range(cnt)]
    o.o_t = [t+0.01 for t in range(cnt)]

    ts_start = time.clock()
    iterations = 50
    for n in range(iterations):
        o.evololution()
        s1 = o.population[4]
        s2 = o.population[20]
        mod = (len(s1.modules), len(s2.modules))
        nod = (len(s1.nodes), len(s2.nodes))
        print("iter {} fittest {} mod:{} node:{}".format(n, o._get_simulator_error(s1), mod, nod))
        if n % 10 == 0:
            print(o.population[0])
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
    f1 = plt.figure()
    plt.plot(o.o_t, o.o_y, label='oryginal')
    plt.plot(o.o_t, y, label='sumulator')
    plt.xlabel('czas')
    plt.ylabel('odpowiedz')
    plt.legend()
    plt.show()
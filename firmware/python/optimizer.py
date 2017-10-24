import simulator
import modules
import matplotlib.pyplot as plt
import numpy as np
import random
import copy

u = [1 for x in range(100)]
y = [1.5*x for x in range(100)]

s = simulator.Simulator(1, 1)  # 1 in 1 out


class Optimizer:
    def __init__(self, pop_size, args):
        self.o_t = None
        self.o_u = []
        self.o_y = []
        self.population = []
        self.population = []
        for i in range(pop_size):
            s = simulator.Simulator(*args)
            self.mutate(s)
            self.population.append(s)

    def _get_simulator_error(self, s):
        assert len(self.o_u) == len(self.o_y)
        assert self.o_t is None or len(self.o_t) == len(self.o_u)
        s.reset_states()
        y = s.simulate(self.o_u, self.o_t)
        error = np.array(self.o_y) - np.array(y)
        return np.sum(error*error)

    def rate_simulator(self, s):
        err = self._get_simulator_error(s)
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
        for p in self.population[best:]:
            p = self.cross(p, self.population[random.randint(0, reproductive)])

        # mutate individuals with score worse than survived
        for p in self.population[best:]:
            self.mutate(p)

        # rate simulators
        rates = []
        for p in self.population:
            rates.append(self.rate_simulator(p))

        # sort them
        rates, self.population = [list(x) for x in zip(*sorted(zip(rates, self.population), key=lambda pair: pair[0]))]


if __name__ == "__main__":
    o = Optimizer(1000, args=(1, 1))

    cnt = 100
    o.o_y = [[x] for x in range(cnt)]
    o.o_u = [[1] for x in range(cnt)]
    o.o_t = None

    iterations = 2
    for n in range(iterations):
        o.evololution()
        s1 = o.population[4]
        s2 = o.population[20]
        mod = (len(s1.modules), len(s2.modules))
        nod = (len(s1.nodes), len(s2.nodes))
        print("iter {} fittest {} mod:{} node:{}".format(n, o._get_simulator_error(s), mod, nod))
    o.population[0].plot()
    plt.show()
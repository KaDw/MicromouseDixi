import random

class dyn:

    def __init__(self):
        self.input = 0.0
        self.output = 0.0
        self.param = []

    def update(self, dt):
        pass

    def reset(self):
        pass

    def description(self):
        return ('',[])


class dynAmplifier(dyn):
    def __init__(self, params=None):
        super(dynAmplifier, self).__init__()
        if params is None:
            r = random.random()
            self.param = [1.0/(r-0.5) if r != 0.5 else 0]
        else:
            self.param = params

    def update(self, dt):
        self.output = self.param[0]*self.input

    def description(self):
        return ('A', self.param)


class dynIntegrator(dyn):
    def __init__(self, params=None):
        super(dynIntegrator, self).__init__()
        self.sum = 0.0
        if params is None:
            r = random.random()
            self.param = [1.0/(r-0.5) if r != 0.5 else 0]
        else:
            self.param = params

    def update(self, dt):
        self.sum += self.input * dt * self.param[0]
        self.output = self.sum

    def reset(self):
        self.sum = 0.0

    def description(self):
        return ('I', self.param)


class dynDifferentiator(dyn):
    def __init__(self, params=None):
        super(dynDifferentiator, self).__init__()
        self.lastValue = 0.0
        if params is None:
            r = random.random()
            self.param = [1.0/(r-0.5) if r != 0.5 else 0]
        else:
            self.param = params

    def update(self, dt):
        if self.lastValue is not None:
            self.output = (self.input - self.lastValue) * self.param[0] / dt
        else:
            self.output = 0.0
        self.lastValue = self.input

    def reset(self):
        self.lastValue = None

    def description(self):
        return ('D', self.param)


class dynSaturation(dyn):
    def __init__(self, params=None):
        super(dynSaturation, self).__init__()
        self.lastValue = 0.0
        if params is None:
            r = random.random()
            v = 1.0/(r-0.5) if r != 0.5 else 0
            self.param = [-v, v]
        else:
            self.param = params

    def update(self, dt):
        # validacja parametrow
        if self.param[0] > self.param[1]:
            self.param[0], self.param[1] = self.param[1], self.param[0]
        # dzialanie
        if self.input < self.param[0]:
            self.output = self.param[0]
        elif self.input > self.param[1]:
            self.output = self.param[1]
        else:
            self.output = self.input

    def description(self):
        return ('S', self.param)

def new_random_module():
    mod_list = [dynAmplifier, dynDifferentiator, dynIntegrator, dynSaturation]
    r = random.randint(0, len(mod_list)-1)
    return mod_list[r]()

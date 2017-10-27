import random

class dyn:
    input = 0.0
    output = 0.0
    param = []

    def update(self, dt):
        pass

    def reset(self):
        pass

    def description(self):
        return ('',[])


class dynAmplifier(dyn):
    param = [random.random()]

    def update(self, dt):
        self.output = self.param[0]*self.input

    def description(self):
        return ('A', self.param)


class dynIntegrator(dyn):
    param = [random.random()]
    sum = 0.0

    def update(self, dt):
        self.sum += self.input * dt * self.param[0]
        self.output = self.sum

    def reset(self):
        self.sum = 0.0

    def description(self):
        return ('I', self.param)


class dynDifferentiator(dyn):
    param = [random.random()]
    lastValue = None

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
    param = [-random.random(), random.random()]

    def update(self, dt):
        # validacja parametrow
        if self.param[0] < self.param[1]:
            self.param[0], self.param[1] = self.param[1], self.param[0]
        # dzialanie
        if self.input < self.param[0]:
            self.output = self.param[0]
        elif self.param[1] > self.input:
            self.output = self.param[1]
        else:
            self.output = self.input

    def description(self):
        return ('S', self.param)




class dyn:
    input = 0.0
    output = 0.0
    param = []

    def update(self, dt):
        pass


class dynAmplifier(dyn):
    param = [1.0]

    def update(self, dt):
        self.output = self.param[0]*self.input


class dynIntegerator(dyn):
    param = [1.0]

    def update(self, dt):
        self.output += self.input * dt * self.param[1]


class dynDifferentiator(dyn):
    param = [1.0]
    lastValue = 0.0

    def update(self, dt):
        return (self.input - self.lastValue) * self.param[0]

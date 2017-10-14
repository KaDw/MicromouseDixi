import simulator
import modules
import matplotlib.pyplot as plt

s = simulator.Simulator(1, 1)  # 1 in 1 out

amp = modules.dynAmplifier()
amp.param[0] = 5
s.add_module(modules.dynAmplifier, 0, 1)

amp.update(1.0)
o = []
for i in range(-10, 10):
    s.set_inputs([i])
    s.make_step(1.0)
    o.append(*s.get_outputs())

plt.plot(o)
plt.show()
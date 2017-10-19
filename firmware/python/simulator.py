import modules


class Simulator:
    """create list of connected modules
    and then run simulation to see what happen"""
    modules = []  # list of modules with in/out nodes
    modInNodeNum, modOutNodeNum = [], []  # index of in/out node for each module
    nodes = []
    inputs, outputs = [], []  #
    deltaTime = 1.0

    def __init__(self, inputs, outputs):
        for i in range(inputs):
            self.add_input()
        for o in range(outputs):
            self.add_output()

    def reset_states(self):
        for n in self.nodes:
            n = 0.0
        for m in modules:
            m.reset()

    def get_output_index(self, numberofout):
        assert numberofout < len(self.outputs)
        return numberofout + len(self.inputs)

    def add_module(self, module, inNodeNum, outNodeNum):
        assert isinstance(module, modules.dyn)
        assert inNodeNum < len(self.nodes)
        assert outNodeNum < len(self.nodes)
        self.modules.append(module)
        self.modInNodeNum.append(inNodeNum)
        self.modOutNodeNum.append(outNodeNum)

    def del_module(self, index):
        assert index < len(self.modules)
        del self.modules[index]
        del self.modInNodeNum[index]
        del self.modOutNodeNum[index]

    def add_node(self):
        self.nodes.append(0.0)
        return len(self.nodes)-1

    def set_hidden_node_count(self, count):
        assert count >= len(self.inputs) + len(self.outputs)
        while count > len(self.nodes):
            self.add_node()
        while count < len(self.nodes):
            del self.nodes[len(self.nodes)-1]

    def add_input(self):
        index = self.add_node()
        self.inputs.append(index)
        return index

    def add_output(self):
        index = self.add_node()
        self.outputs.append(index)
        return index

    def _update_module(self, i):
        assert i < len(self.modules)
        self.modules[i].input = self.nodes[self.modInNodeNum[i]]
        dt = self.deltaTime
        m = self.modules[i]
        m.update(dt)

    def _update_node(self, i):
        ''' update node[i] value based on modules with output connected to them'''
        assert i < len(self.nodes)
        # lista obiektow z wyjsciem podlaczonym do wezla i
        listOfConnectedModules = [n for n in range(len(self.modOutNodeNum)) if self.modOutNodeNum[n] == i]
        self.nodes[i] = sum([self.modules[n].output for n in listOfConnectedModules])

    def set_inputs(self, lis):
        assert isinstance(lis, list)
        assert len(lis) == len(self.inputs)
        for i in range(len(lis)):
            self.nodes[self.inputs[i]] = lis[i]

    def make_step(self, dt):
        assert dt > 0
        self.deltaTime = dt
        list(map(self._update_module, range(len(self.modules))))  # update each module
        list(map(self._update_node, range(len(self.nodes))))  # update each node

    def get_outputs(self):
        return [self.nodes[n] for n in self.outputs]

    def simulate(self, u, t = None):
        y = []
        last_time = 0.0
        if t is not None:
            last_time = t[0] - (t[1]-t[0])
        for i in range(0, len(u)):
            self.set_inputs(u[i])
            if t is not None:
                self.make_step(t[i]-last_time)
                last_time = t[i]
            else:
                self.make_step(1.0)
            y.append(self.get_outputs())
        return y


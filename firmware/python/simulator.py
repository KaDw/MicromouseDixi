import modules


class Simulator:
    modules = []  # list of modules with in/out nodes
    modInNodeNum, modOutNodeNum = [], []
    nodes, inputs, outputs = [], [], []
    deltaTime = 1.0

    def __init__(self, inputs, outputs):
        for i in range(inputs):
            self.add_input()
        for o in range(outputs):
            self.add_output()

    def add_module(self, module, inNodeNum, outNodeNum):
        assert issubclass(module, modules.dyn)
        assert inNodeNum < len(self.nodes)
        assert outNodeNum < len(self.nodes)
        self.modules.append(module)
        self.modInNodeNum.append(inNodeNum)
        self.modOutNodeNum.append(outNodeNum)

    def add_node(self):
        self.nodes.append(0.0)

    def add_input(self):
        self.add_node()
        self.inputs.append(len(self.nodes)-1)

    def add_output(self):
        self.add_node()
        self.outputs.append(len(self.nodes)-1)

    def _update_module(self, i):
        assert i < len(self.modules)
        self.modules[i].input = self.nodes[self.modInNodeNum[i]]
        dt = self.deltaTime
        m = self.modules[i]
        m.update(m, dt)

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


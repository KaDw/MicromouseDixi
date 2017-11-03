import modules
import numbers
import networkx as nx
import matplotlib.pyplot as plt


class Simulator:
    """create list of connected modules
    and then run simulation to see what happen"""

    def __init__(self, inputs, outputs):
        self.modules = []  # list of modules with in/out nodes
        self.modInNodeNum, self.modOutNodeNum = [], []  # index of in/out node for each module
        self.nodes = []
        self.input_nodes, self.output_nodes = [], []  #
        self.deltaTime = 1.0
        for i in range(inputs):
            self.add_input()
        for o in range(outputs):
            self.add_output()

    def reset_states(self):
        for n in self.nodes:
            n = 0.0
        for m in self.modules:
            m.reset()

    def get_output_index(self, numberofout):
        assert numberofout < len(self.output_nodes)
        return numberofout + len(self.input_nodes)

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
        current_count = len(self.nodes)-len(self.input_nodes)-len(self.output_nodes)
        while count > current_count:
            self.add_node()
        while count < current_count:
            del self.nodes[len(self.nodes)-1]

    def add_input(self):
        index = self.add_node()
        self.input_nodes.append(index)
        return index

    def add_output(self):
        index = self.add_node()
        self.output_nodes.append(index)
        return index

    def _update_module(self, i):
        assert i < len(self.modules)
        assert len(self.modules) > i
        assert len(self.modInNodeNum) > i
        assert len(self.nodes) > self.modInNodeNum[i]
        #print(i, len(self.modInNodeNum), len(self.modules), len(self.nodes))
        #print(self.modInNodeNum[i])
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
        if isinstance(lis, numbers.Number):
            lis = [lis]
        assert isinstance(lis, list)
        assert len(lis) == len(self.input_nodes)
        for i in range(len(lis)):
            self.nodes[self.input_nodes[i]] = lis[i]

    def make_step(self, dt):
        assert dt > 0
        self.deltaTime = dt
        try:
            list(map(self._update_module, range(len(self.modules))))  # update each module
            list(map(self._update_node, range(len(self.nodes))))  # update each node
        except Exception as e:
            print('Exception occured: {}'.format(str(e)))

    def get_outputs(self):
        return [self.nodes[n] for n in self.output_nodes]

    def _correct_module(self, index):
        fail = False
        if self.modInNodeNum[index] > len(self.nodes) or \
            self.modOutNodeNum[index] > len(self.nodes):
            fail = True
        if fail:
            del self.modules[index]
            del self.modInNodeNum[index]
            del self.modOutNodeNum[index]

    def correct(self):
        for m in self.modInNodeNum:
            if m > len(self.nodes):
                self.del_module(self.mo)

    def simulate(self, u, t=None):
        y = []
        last_time = 0.0
        if t is not None:
            assert len(u) == len(t)
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

    def _get_graph(self):
        g = nx.MultiGraph()
        g.add_nodes_from(list(range(len(self.nodes))))
        for i in range(len(self.modules)):
            m = self.modules[i]
            g.add_edge(self.modInNodeNum[i], self.modOutNodeNum[i], object=m.description())
        return g

    def plot(self, where = None):
        g = self._get_graph()
        pos = nx.spring_layout(g)
        nx.draw(g, pos)
        nx.draw_networkx_labels(g, pos, {i:str(i) for i in range(len(g.nodes))}, font_size=16)
        nx.draw_networkx_edge_labels(g, pos)

    def __str__(self):
        g = self._get_graph()
        s = '\n'.join(str(v) for v in g.edges(data=True))
        return s


import modules
import numbers
import networkx as nx
import random
import copy
import numpy as np
import math
import matplotlib.pyplot as plt
import scipy


class SimulatorRPN:
    operands_1 = ['abs', 'relay', 'sin', 'cos']
    operands_2 = ['-', '+', '*', '/', '**', 'log']

    def __init__(self):
        self.variables = [0 for x in random.randint(0, 5)]
        self.equations = [self.random_equation(len(self.variables))]

    def simulate(self, u, t=None):
        pass

    @staticmethod
    def random_param(len_variables):
        if random.random() < 0.5:
            r = random.random()
            v = 0.5 / (r - 0.5) if r != 0.5 else 0.0
            return 'p' + str(v)
        else:
            return 'v' + str(random.randint(0, len_variables))

    @staticmethod
    def random_operand(operands):
        if operands == 1:
            return SimulatorRPN.operands_1[random.randint(0, len(SimulatorRPN.operands_1)-1)]
        elif operands == 2:
            return SimulatorRPN.operands_2[random.randint(0, len(SimulatorRPN.operands_2)-1)]
        else:
            raise Exception("bad operators number ({})".format(operands))


    @staticmethod
    def random_equation(len_variables, max_value_of_subequations=5):
        if max_value_of_subequations == 0:
            return ' '.join([SimulatorRPN.random_param(len_variables),
                             SimulatorRPN.random_operand(1)])
        elif max_value_of_subequations == 1:
            return ' '.join([SimulatorRPN.random_param(len_variables),
                             SimulatorRPN.random_param(len_variables),
                             SimulatorRPN.random_operand(2)])
        else:
            return ' '.join([SimulatorRPN.random_equation(len_variables, random.randint(max_value_of_subequations-1)),
                             SimulatorRPN.random_equation(len_variables, random.randint(max_value_of_subequations-1)),
                             SimulatorRPN.random_operand(2)])

    @staticmethod
    def reverse_polish_notation(expression, variables):
        stack = []
        for val in expression.split(' '):
            if val in SimulatorRPN.operands_2:
                op1 = stack.pop()
                op2 = stack.pop()
                if val == '-': result = op2 - op1
                elif val == '+': result = op2 + op1
                elif val == '*': result = op2 * op1
                elif val == '/': result = op2 / op1
                elif val == '**': result = op2 ** op1
                elif val == 'log': result = math.log(abs(op2), abs(op1))
                else: raise "operand {} isn't fully supported".format(val)
                stack.append(result)
            elif val in SimulatorRPN.operands_1:
                op1 = stack.pop()
                if val == 'abs': result = math.fabs(op1)
                elif val == 'relay': result = 1 if op1 >= 0 else -1
                elif val == 'sin': result = math.sin(op1)
                elif val == 'cos': result = math.cos(op1)
                else: raise "operand {} isn't fully supported".format(val)
                stack.append(result)
            elif val[0] == 'p':
                stack.append(float(val[1:]))
            elif val[0] == 'v':
                stack.append(variables[int(val[1:])])
            else:
                stack.append(float(val))
        return stack.pop()


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
        for i in range(random.randint(0, 3)):
            self.add_node()
        for i in range(random.randint(0, 5)):
            self.add_module(modules.new_random_module(), random.randint(0, len(self.nodes)-1), random.randint(0, len(self.nodes)-1))

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
        if len(self.nodes) > self.modInNodeNum[i]:
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

    def mutate(self):
        # add new module
        if random.random() < 0.03:
            m = modules.new_random_module()
            node_count = len(self.nodes)
            n1, n2 = random.randint(0, node_count - 1), random.randint(0, node_count - 1)
            if n1 != n2:
                self.add_module(m, n1, n2)
        # add new node with modules
        if random.random() < 0.01:
            node_count = len(self.nodes)
            ni = random.randint(0, len(self.nodes)-1)
            no = random.randint(0, len(self.nodes)-1)
            new_node = self.add_node()
            self.add_module(modules.new_random_module(), ni, new_node)
            self.add_module(modules.new_random_module(), new_node, no)
        # delete module
        if random.random() < 0.06 and len(self.modules) > 1:
            mi = random.randint(0, len(self.modules)-1)
            self.del_module(mi)
        # delete node -- trouble with input/output nodes
        if random.random() < 0.05 and len(self.nodes) > len(self.input_nodes) + len(self.output_nodes):
            ni = random.randint(len(self.output_nodes)+len(self.input_nodes), len(self.nodes)-1)
            if ni not in self.modInNodeNum and ni not in self.modOutNodeNum:
                del self.nodes[ni]
                for i, _ in enumerate(self.modules):
                    if self.modInNodeNum[i] == ni or self.modOutNodeNum[i] == ni:
                        del self.modOutNodeNum[i]
                        del self.modInNodeNum[i]
                        del self.modules[i]
                        break
        # mutate param
        if random.random() < 0.05:
            mi = random.randint(0, len(self.modules))
            if mi < len(self.modules):
                for p in self.modules[mi].param:
                    p *= 2 * (random.random() - 0.5)

    def hamming_distance_to(self, second):
        mx, sum = 0, 0
        m1_len, m2_len = len(self.modules), len(second.modules)
        mx, sum = mx+1, (sum + 1) if m1_len != m2_len else sum
        for m in range(min(m1_len, m2_len)):
            mx += 1
            if type(self.modules[m]) != type(second.modules[m])\
                    or np.linalg.norm(np.array(self.modules[m].param)-np.array(second.modules[m].param)) > 0.1:
                sum += 1
        return sum/mx

    @staticmethod
    def cross(first, seccond):
        nmodules = int(min(len(seccond.modules), len(first.modules)) * random.random())
        newsim = copy.deepcopy(first)
        newsim.modules[0:nmodules] = copy.deepcopy(seccond.modules[0:nmodules])
        newsim.modInNodeNum[0:nmodules] = copy.deepcopy(seccond.modInNodeNum[0:nmodules])
        newsim.modOutNodeNum[0:nmodules] = copy.deepcopy(seccond.modOutNodeNum[0:nmodules])
        n_nodes = len(newsim.nodes)
        for i in range(len(newsim.modules)):
            if newsim.modOutNodeNum[i] > n_nodes:
                newsim.modOutNodeNum[i] = random.randint(0, n_nodes-1)
            elif newsim.modInNodeNum[i] > n_nodes:
                newsim.modInNodeNum[i] = random.randint(0, n_nodes-1)
        while newsim.hamming_distance_to(first) < 0.25:
            newsim.mutate()
            newsim.mutate()
        return newsim

    def make_step(self, dt):
        assert dt > 0
        self.deltaTime = dt
        list(map(self._update_module, range(len(self.modules))))  # update each module
        list(map(self._update_node, range(len(self.nodes))))  # update each node

    def get_outputs(self):
        if [x for x in self.output_nodes if x >= len(self.nodes)]:
            print('Error: ', self.output_nodes, len(self.nodes), self.nodes)
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
        nx.draw_networkx(g, pos)
        nx.draw_networkx_labels(g, pos, {i:str(i) for i in range(len(g.nodes()))}, font_size=16)
        nx.draw_networkx_edge_labels(g, pos)

    def __str__(self):
        g = self._get_graph()
        s = '\n'.join(str(v) for v in g.edges(data=True))
        return s

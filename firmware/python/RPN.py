import random
import math
import time
import copy
from scipy.integrate import ode, odeint
import matplotlib.pyplot as plt
import numpy as np


class RPN:
    ''' extended Reverse Polish Notation calculator'''
    operands = (('abs', 'relay', 'sin', 'cos'),  # 1 arg
                ('-', '+', '*'),  #, '/', '**'),  #, 'log'],  # 2 arg
                ('saturate',))  # 3 arg
    all_operands = (*operands[0], *operands[1], *operands[2])
    default_value_of_subequations = 2

    def __init__(self, len_variables, max_value_of_subequations=default_value_of_subequations):
        self.len_variables = len_variables
        self.expression = self.random_equation(max_value_of_subequations)

    def __str__(self):
        ''' konwersja do stringa '''
        return ' '.join([str(e) for e in self.expression])

    def __call__(self, *args):
        return self.calc(*args)

    def random_param(self):
        ''' zwraca wartosci - parametr liczbowy (p) albo numer zmiennej (v) '''
        if random.random() < 0.5:
            r = random.random()
            v = 0.5 / (r - 0.5) if r != 0.5 else 0.0
            #return 'p' + str(v)
            return v
        else:
            return 'x' + str(random.randint(0, self.len_variables - 1))

    @staticmethod
    def random_operand(number):
        ''' zwraca losowy operand do laczenia danych (*, /, sin, cos, ...) '''
        index = number - 1
        if 0 <= index < len(RPN.operands):
            return RPN.operands[index][random.randint(0, len(RPN.operands[index]) - 1)]
        else:
            raise Exception("bad operators number ({})".format(number))

    @staticmethod
    def get_needed_operands(operation):
        needed_operands = 1 if operation in RPN.operands[0] \
                            else 2 if operation in RPN.operands[1]\
                            else 3 if operation in RPN.operands[2]\
                            else 0
        return needed_operands

    @staticmethod
    def validate(expression):
        s = sum([RPN.get_needed_operands(e)-1 for e in expression]) + 1
        assert s == 0

    def get_branch(self, b_end = None):
        ''' znajdz indeks początku (inclusive) i końca (exclusive) brancha który kończy się operatorem b_end lub losowego '''
        # znajdz koniec brancha (dojedz do operatora)
        if b_end is None:
            b_end = random.randint(0, len(self.expression)-1)
        assert b_end < len(self.expression)
        while self.expression[b_end] not in RPN.all_operands:
            b_end += 1
        # znajdz indeks poczatku
        b_start = b_end
        needed_operands = self.get_needed_operands(self.expression[b_end])
        while needed_operands > 0:
            b_start -= 1
            needed_operands += self.get_needed_operands(self.expression[b_start])-1
        assert b_start >= 0
        self.validate(self.expression)
        self.validate(self.expression[b_start:b_end+1])
        return b_start, b_end+1

    def translate_branch(self, new_expression):
        ''' randomly tanslate too big variable numbers'''
        exp = copy.deepcopy(new_expression)
        for i in range(len(exp)):
            if isinstance(exp[i], str) and exp[i][0] == 'x':
                if int(exp[i][1:]) >= self.len_variables:
                    exp[i] = 'x' + str(random.randint(0, self.len_variables-1))
        return exp

    def delete_variable(self, index_variable):
        ''' decrement reference number for variables with number bigger than indec_variable'''
        self.len_variables -= 1
        for i in range(len(self.expression)):
            e = self.expression[i]
            if isinstance(e, float) == False and e[0] == 'x':
                reference = int(e[1:])
                if reference > index_variable:
                    self.expression[i] = 'x' + str(reference - 1)
                elif reference == index_variable:
                    self.expression[i] = 'x' + str(random.randint(0, self.len_variables)-1)

    def replace_branch(self, index_operand, new_expression):
        ''' zastepuje branch ktorego rootem jest element spod indeksu index_operand na nowy branch'''
        ind_start, ind_stop = self.get_branch(index_operand)
        del self.expression[ind_start:ind_stop]
        self.expression[ind_start:ind_start] = self.translate_branch(new_expression)

    def random_equation(self, max_value_of_subequations):
        ''' losowanie rownania zlozonego maksymalnie z max_value_of_subequations! operandow'''
        if max_value_of_subequations == 0:  # rownanie proste (paramtery + 1 operand)
            r = random.random()
            sum_thres = [len(RPN.operands[i])/len(RPN.all_operands) for i in range(len(RPN.operands))]
            if r <sum_thres[0]:
                return [self.random_param(),
                        self.random_operand(1)]
            elif r < sum_thres[0] + sum_thres[1]:
                return [self.random_param(),
                        self.random_param(),
                        self.random_operand(2)]
            elif r < sum_thres[0] + sum_thres[1] + sum_thres[2]:
                return [self.random_param(),
                        self.random_param(),
                        self.random_param(),
                        self.random_operand(3)]
        else:  # rownanie zlozone z rownan prostych/zlozonych polaczone 1 parametrem
            return [*self.random_equation(random.randint(0, max_value_of_subequations - 1)),
                    *self.random_equation(random.randint(0, max_value_of_subequations - 1)),
                    self.random_operand(2)]

    def calc(self, variables):
        stack = []
        for val in self.expression:
            if isinstance(val, float):
                stack.append(val)
            elif val[0] == 'p':
                stack.append(float(val[1:]))
            elif val[0] == 'x':
                num = int(val[1:])
                if num >= len(variables):
                    print("error", "mam podac x{}, a dostalem size(in)={}, a uwazam ze size(in)={}".format(num, len(variables), self.len_variables))
                    print(self)
                    print(variables)
                stack.append(variables[int(val[1:])])
            elif val in RPN.operands[0]:
                op1 = stack.pop()
                if val == 'abs':
                    result = math.fabs(op1)
                elif val == 'relay':
                    result = 1 if op1 >= 0 else -1
                elif val == 'sin':
                    result = math.sin(op1)
                elif val == 'cos':
                    result = math.cos(op1)
                else:
                    raise Exception("operand {} isn't fully supported".format(val))
                stack.append(result)
            elif val in RPN.operands[1]:
                op1 = stack.pop()
                op2 = stack.pop()
                if val == '-':
                    result = op2 - op1
                elif val == '+':
                    result = op2 + op1
                elif val == '*':
                    result = op2 * op1
                elif val == '/':
                    result = op2 / op1
                elif val == '**':
                    result = abs(op2) ** op1
                elif val == 'log':
                    print('log val: {} {}'.format(op1, op2))
                    result = math.log(abs(op2), abs(op1) + 1)
                else:
                    raise Exception("operand {} isn't fully supported".format(val))
                stack.append(result)
            elif val in RPN.operands[2]:
                op1, op2, op3 = stack.pop(), stack.pop(), stack.pop()
                if val == 'saturate':
                    result = max(op1, min(op3, op1+abs(op2)))  # in range [op1, op1+abs(op2)]
                else:
                    raise Exception("operand {} isn't fully supported".format(val))
                stack.append(result)
            else:
                raise Exception("operand {} isn't fully supported".format(val))
        return stack.pop()

    def simplify(self):
        """ zastepuje wyrazenia stale na liczby """
        r = RPN(1, 0)
        for i in range(len(self.expression)-1):
            if self.expression[i] in self.operands[0]:
                if isinstance(self.expression[i-1], float):
                    r.expression = self.expression[i-1:i+1]
                    val = [float(r.calc([]))]
                    self.replace_branch(i, val)
                    break
            elif self.expression[i] in self.operands[1]:
                if isinstance(self.expression[i-1], float) and isinstance(self.expression[i-2], float):
                    r.expression = self.expression[i-2:i+1]
                    val = [float(r.calc([]))]
                    self.replace_branch(i, val)
                    break


class SimulatorRPN:
    def __init__(self, inputs_num, outputs_num, max_start_variables_num=5):
        num_state_space_variables = random.randint(1, max_start_variables_num)
        num_state_equations = inputs_num + num_state_space_variables
        num_out_equations = num_state_space_variables
        self.state_equations = [RPN(num_state_equations) for x in range(num_state_space_variables)]  # input at begin
        self.out_equations = [RPN(num_out_equations) for x in range(outputs_num)]  # output at beginning
        self.inputs_num, self.outputs_num = inputs_num, outputs_num

    def __str__(self):
        S = "state ({}in:{}st): [\n".format(self.inputs_num, len(self.state_equations)) +\
            "\n".join([str(e) for e in self.state_equations])
        O = "\n]\nout ({}): [\n".format(self.outputs_num) + "\n".join([str(e) for e in self.out_equations])
        return S + O + "\n]"

    def reset_states(self):
        pass

    @staticmethod
    def calc_dxdt(x, t, u, sim):
        var = [*u, *x]
        return [e(var) for e in sim.state_equations]

    @staticmethod
    def calc_dxdt2(t, x, u, sim):
        var = [*u, *x]
        return [e(var) for e in sim.state_equations]

    def prepare_to_simulate(self, u, t, x0):
        assert len(u[0]) == self.inputs_num
        if x0 is None:
            x0 = [0 for x in range(len(self.state_equations))]
        if t is None:
            t = tuple(range(len(u)))
        last_t = t[0]
        y = [e(x0) for e in self.out_equations]
        return t, x0, y

    def get_y(self, x):
        return [e(x) for e in self.out_equations]

    def simulate(self, *args):
        #print("symuluje: {} in - {} st - {} out".format(self.inputs_num, len(self.state_equations), self.outputs_num))
        assert [e.len_variables == len(self.state_equations)+self.inputs_num for e in self.state_equations]
        assert [e.len_variables == len(self.state_equations) for e in self.out_equations]
        return self.simulateEuler(*args)

    def simulateOdeint(self, u, t, y0 = None):
        assert len(u[0]) == self.inputs_num
        if y0 is None:
            y0 = [0 for x in range(len(self.state_equations))]
        if t is None:
            t = tuple(range(len(u)))
        last_t = t[0]
        y = [e(y0) for e in self.out_equations]
        try:
            for ct, cu in zip(t[1:], u[:-1]):
                sol = odeint(self.calc_dxdt, y0, [last_t, ct], args=(cu, self))
                y0 = sol[-1]
                y.extend([e(sol[-1]) for e in self.out_equations])
                last_t = ct
        except Exception as error:
            print(error)
            y = [float('inf') for ct in t]
        print("simulation end")
        return y

    def simulate2(self, u, t=None, y0=None):
        assert len(u[0]) == self.inputs_num
        r = ode(SimulatorRPN.calc_dxdt).set_integrator('vode', method='adam', with_jacobian='false')
        y = []
        if y0 is None:
            y0 = [0 for x in range(len(self.state_equations))]
        if t is None:
            t = tuple(range(len(u)))
        try:
            r.set_initial_value(y0, 0)
            for ct, cu in zip(t, u):
                r.set_f_params((cu, self))
                state = r.integrate(ct)
                var = [*cu, *state]
                cy = [e(var) for e in self.out_equations]
                print(cy)
                y.append(cy)
        except Exception as error:
            print(error)
            y = [float('inf')]
        return y

    def simulateEuler(self, u, t=None, x0=None):
        t, x0, y = self.prepare_to_simulate(u, t, x0)
        y = [y]
        state = np.array(x0, dtype='float64')
        last_t = t[0]
        for ct, cu in zip(t[1:], u[:-1]):
            prim = np.array(self.calc_dxdt(x0, ct, cu, self), dtype=state.dtype)
            state += prim * (ct-last_t)
            last_t = ct
            y.append(self.get_y(state))
        return y

    @staticmethod
    def mutate_rpn(rpn):
        ''' dodawanie nowych wezłów, usuwanie starych, modyfikacja operandów i parametrów'''
        if random.random() < 0.01:  # szansa na mutacje parametru/operacji
            e = rpn.expression
            n = random.randint(0, len(e)-1)
            if isinstance(e[n], float) or e[n][0] == 'p':
                r = random.random()
                val = e[n] if isinstance(e[n], float) else float(e[n][1:])
                if r < 0.33:  # zastapienie
                    e[n] = rpn.random_param()
                elif r < 0.66:  # mnozenie
                    e[n] = 'p' + str(math.e*random.random()*val)
                else:  # dodawanie
                    e[n] = 'p' + str(10*(random.random()-0.5) + val)
            elif e[n][0] == 'x':
                e[n] = rpn.random_param()
            elif e[n] in RPN.operands[0]:
                e[n] = RPN.operands[0][random.randint(0, len(RPN.operands[0])-1)]
            elif e[n] in RPN.operands[1]:
                e[n] = RPN.operands[1][random.randint(0, len(RPN.operands[1])-1)]
            elif e[n] in RPN.operands[2]:
                e[n] = RPN.operands[2][random.randint(0, len(RPN.operands[2])-1)]
        if random.random() < 0.05:  # szansa na modyfikacje brancha (powiększenie, uszczuplenie)
            new_expression = rpn.random_equation(RPN.default_value_of_subequations)
            rpn.replace_branch(random.randint(0, len(rpn.expression)-1), new_expression)

    @staticmethod
    def cross_rpn(rpn1, rpn2, new_variable_len = None):
        """ krzyżowanie równań w odwróconej notacji polskiej """
        new_rpn = copy.deepcopy(rpn1)
        if new_variable_len is not None:
            new_rpn.len_variables = new_variable_len
        # wylosuj i przetlumacz brancha z rpn2
        rpn2_start, rpn2_stop = rpn2.get_branch()
        new_expression = rpn2.expression[rpn2_start:rpn2_stop]
        new_expression = new_rpn.translate_branch(new_expression)
        # podmien wylosowanego brancha z rpn1
        branch_n_to_replace = random.randint(0, len(new_rpn.expression)-1)
        new_rpn.replace_branch(branch_n_to_replace, new_expression)
        # i new_rpn jest gotowy
        return new_rpn

    def mutate(self):
        """ mutate whole simulator """
        for r in [*self.state_equations, *self.out_equations]:  # mutacja rownan
            self.mutate_rpn(r)
        len_var = len(self.state_equations)
        if random.random() < 0.05:  # dodanie nowej zmiennej stanu
            len_var += 1
            for e in self.state_equations:
                e.len_variables = len_var+self.inputs_num
            for e in self.out_equations:
                e.len_variables = len_var
            self.state_equations.append(RPN(len_var+self.inputs_num))
        if random.random() < 0.05 and len_var > 1:  # usuniecie zmiennej stanu
            n = random.randint(0, len_var-1)
            del self.state_equations[n]
            [e.delete_variable(n + self.inputs_num) for e in self.state_equations]
            [e.delete_variable(n) for e in self.out_equations]
        self.validate()

    @staticmethod
    def cross(sim1, sim2):
        """ stwórz nowego sobnika będącego kopią sim1 i zastąp jedną gałąź gałęzią z sim2 w każdym z równań """
        assert isinstance(sim1, SimulatorRPN)
        assert isinstance(sim2, SimulatorRPN)
        new_sim = copy.deepcopy(sim1)
        num_st_eq_to_cross = min([len(sim1.state_equations), len(sim2.state_equations)])
        new_sim.state_equations[0:num_st_eq_to_cross] = [SimulatorRPN.cross_rpn(r1, r2) for
                                   r1, r2 in zip(sim1.state_equations[:num_st_eq_to_cross], sim2.state_equations[:num_st_eq_to_cross])]
        new_sim.out_equations = [SimulatorRPN.cross_rpn(r1, r2, len(new_sim.state_equations)) for
                                 r1, r2 in zip(sim1.out_equations, sim2.out_equations)]
        assert [r.len_variables == len(new_sim.state_equations) for r in new_sim.state_equations]
        assert [r.len_variables == len(new_sim.state_equations) for r in new_sim.out_equations]
        return new_sim

    def validate(self):
        for r in self.state_equations:
            r.validate(r.expression)
            assert r.len_variables == len(self.state_equations) + self.inputs_num
        for r in self.out_equations:
            r.validate(r.expression)
            assert r.len_variables == len(self.state_equations)

    def simplify(self):
        for e in [*self.state_equations, *self.out_equations]:
            e.simplify()

    def hamming_distance_to(self, s2):
        """ powinno zwracan przyblizona odleglosc w sensie Hamminga pomiedzy para symulatorow """
        return 0

    @staticmethod
    def test():
        s = SimulatorRPN(1, 1)
        t = list(range(100))
        u = [[1] for x in range(len(t))]
        y = s.simulate(u, t)
        print(y)
        print(s)
        plt.plot(y)
        plt.show()
        RR = RPN(3, 5)
        for j in range(3):
            for i in range(5):
                r = RPN(3, i)
                s.mutate_rpn(r)
                s.mutate_rpn(r)
                s.mutate_rpn(r)
                s.mutate_rpn(r)
                #r = s.cross_rpn(r, RR)
                print(j, r)
                print((r.calc([1, 2, 3]), r.get_branch()))


if __name__=="__main__":
    SimulatorRPN.test()
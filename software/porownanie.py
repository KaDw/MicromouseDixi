import simulator
import modules
import model_finder
import model
import RPN
import matplotlib.pyplot as plt
import numpy as np



if __name__=="__main__":
    T_sampling = 0.001
    ly, lu = model_finder.read_data('pwm250.txt', T_sampling)
    o_y = [[y] for y in ly]
    o_u = [[u] for u in lu]
    o_t = [T_sampling*x for x in range(len(o_y))]
    o_len = len(o_y)

    extra = 700
    us = [*o_u, *[o_u[-1] for _ in range(extra)]]
    ts = [T_sampling*x for x in range(len(us))]
    simulators = []

    st = simulator.Simulator(1, 1)
    st.add_node()
    st.add_module(modules.dynSaturation([-12.897249319394978, 12.897249319394978]), 0, 2)
    st.add_module(modules.dynIntegrator([-2.6721082139980545]), 1, 1)
    st.add_module(modules.dynIntegrator([2.3904308776953007]), 2, 1)
    yt = st.simulate(us, ts)
    et = np.linalg.norm(np.array(o_y) - yt[:o_len])

    sr = simulator.Simulator(1, 1)
    sr.add_node()
    sr.add_module(modules.dynSaturation([-13.240442861116327, 13.240442861116327]), 0, 2)
    sr.add_module(modules.dynIntegrator([-2.0615930676617014]), 1, 1)
    sr.add_module(modules.dynIntegrator([2.0814984932936884]), 2, 1)
    yr = sr.simulate(us, ts)
    er = np.linalg.norm(np.array(o_y) - yr[:o_len])

    srr = RPN.SimulatorRPN(1, 1)
    rpn_st = RPN.RPN(1)
    rpn_st.expression = [-2.99053553466004, -3.1940104860056513, 'log']
    srr.state_equations = [rpn_st]
    rpn_out = RPN.RPN(2)
    rpn_out.expression = ['x0', 1.5809663364117297, -1.1395471772523125, 'saturate']
    srr.out_equations = [rpn_out]
    yrr = srr.simulate(us, ts)
    err = np.linalg.norm(np.array(o_y) - yrr[:o_len])

    srt = RPN.SimulatorRPN(1, 1)
    rpn_st = RPN.RPN(1)
    rpn_st.expression = [1.2361003547843794, 'sin']
    srt.state_equations = [rpn_st]
    rpn_out = RPN.RPN(2)
    rpn_out.expression = ['x0', 1.8328562101127972, -1.3928366115518636, 'saturate']
    srt.out_equations = [rpn_out]
    yrt = srt.simulate(us, ts)
    ert = np.linalg.norm(np.array(o_y) - yrr[:o_len])

    sl = model.Model(T_sampling)
    yl = []
    for cu in us:
        sl.X = sl.f(np.mat([cu, cu]), T_sampling)
        yl.append(sl.measure()[0, 0])
    yel = np.array(o_y) - yl[:o_len]
    el = np.linalg.norm(np.array(o_y).transpose() - np.array(yl[:o_len]).transpose())

    plt.plot(o_t, o_y, 'o', alpha=0.1, label='oryginał')
    plt.plot(ts, yt, '--', label='SM-turniejowa')
    plt.plot(ts, yr, '-.', label='SM-progowa')
    plt.plot(ts, yl, ':', label='model liniowy')
    plt.plot(ts, yrr, '-', label='model ONP')
    plt.plot(ts, yrt, ':', linewidth=5, label='model ONP2')

    errors = [['modułowy-turniejowa', et], ['modułowy-progowa', er], ['liniowy', el], ['ONP-progowy', err], ['ONP-turniejowa', ert]]
    for e in errors:
        print('{:>10}: {:.3f}'.format(e[0], e[1]))

    plt.title('Porównanie modeli - pobudzenie 0.6V')
    plt.xlabel('czas [s]')
    plt.ylabel('Prędkość [m/s]')
    plt.legend(loc='lower right')
    plt.grid()
    plt.axis('equal')
    plt.show()

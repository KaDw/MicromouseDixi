import matplotlib.pyplot as plt
import numpy as np
import math
import scipy.linalg


def lqr(A, B, Q, R):
    """Solve the continuous time lqr controller.

    dx/dt = A x + B u

    cost = integral x.T*Q*x + u.T*R*u
    """
    # ref Bertsekas, p.151

    # first, try to solve the ricatti equation
    P = np.matrix(scipy.linalg.solve_continuous_are(A, B, Q, R))

    # compute the LQR gain
    K = np.matrix(scipy.linalg.inv(R) * (B.T * P))

    eigVals, eigVecs = scipy.linalg.eig(A - B * K)

    return K, P, eigVals


class Robot:
    class Model:
        def __init__(self, k=1, zeta=0.64, t_n=None, w_n=None):
            assert 0 <= zeta <= 1
            self.k = k
            self.zeta = zeta
            if w_n is not None:
                self.t_n = 1.8 / w_n
            elif t_n is not None:
                self.t_n = t_n
            else:
                self.t_n = 1
            self.Q = np.diag([0, 1, 0, 1])
            self.R = np.diag([1, 1])
            self.A, self.B, self.C, self.K, self.L = None, None, None, None, None
            self.Y = None
            self.update()
            self.X = np.zeros((self.A.shape[0], 1))
            self.Y = self.C*self.X

        @staticmethod
        def dlqr(A, B, Q, R):
            """Solve the discrete time lqr controller.
            x[k+1] = A x[k] + B u[k]
            cost = sum x[k].T*Q*x[k] + u[k].T*R*u[k]
            """
            # ref Bertsekas, p.151
            # first, try to solve the ricatti equation
            P = np.matrix(scipy.linalg.solve_discrete_are(A, B, Q, R))
            # compute the LQR gain
            K = np.matrix(scipy.linalg.inv(B.T * P * B + R) * (B.T * P * A))
            eigVals, eigVecs = scipy.linalg.eig(A - B * K)
            return K, P, eigVals

        def update(self):
            zeta = self.zeta
            w_n = 1.8 / self.t_n
            k = self.k
            self.A = np.mat([
                [0, 1, 0, 0],
                [-w_n ** 2, -2 * zeta * w_n, 0, 0],
                [0, 0, 0, 1],
                [0, 0, -w_n ** 2, -2 * zeta * w_n]
            ], dtype=type(0.0))
            self.B = np.mat([
                [0, 0],
                [k*w_n**2, 0],
                [0, 0],
                [0, k*w_n**2]
            ], dtype=type(0.0))
            self.C = np.mat([
                [1, 0, 0, 0],
                [0, 0, 1, 0]
            ], dtype=type(0.0))
            self.K, _, _ = self.dlqr(self.A, self.B, self.Q, self.R)

        def f(self, X, U, dt):
            """
            X = [[vl],
                [al],
                [vr],
                [ar]]
            U = [[dl],
                [dr]]
            """
            I = np.eye(model.A.shape[0])
            return (self.A*dt + I)*X + self.B*U*dt

        def regulator(self, r, Y, Xhat=None):
            if Xhat is None:
                Xhat = self.X
            U = r-Y  # feedback
            # U = r - self.K*Xhat  # LQR
            return U

        def measure(self, X):
            return self.C*X

        def iterate(self, r, dt):
            if not isinstance(r, (type(np.array([])), type(np.mat([[]])))):
                r = np.mat([r]).transpose()
            self.Y = self.measure(self.X)  # Y = C*X
            U = self.regulator(r, self.Y)
            self.X = self.f(self.X, U, dt)  # (A*dt + I)*X + B*U*dt

    class SLAM:
        def __init__(self):
            self.pos = np.array([0, 0], dtype=type(0.0))  # x, y
            self.alpha = 0.0
            self.gamma = 1.0
            self.h = 2
            self.map = np.mat([[]])

        def odometry(self, encoders):
            s = encoders[0, 0] + encoders[1, 0]
            self.pos += 0.5*np.array([math.cos(self.alpha), math.sin(self.alpha)])
            self.alpha += self.gamma * (encoders[0, 0] - encoders[1, 0]) / (2*self.h)

        def IMU(self, dalpha):
            self.alpha += (1.0-self.gamma) * dalpha

        def LED(self, reads):
            pass

        def generate_callbacks(self):
            pass

        #def new_cell_callback(self):

    class Profiler:
        def __init__(self):
            self.t = 0
            self.loc_target_pos = np.array([100, 100])
            self.loc_target_alpha = 90/18*math.pi
            pass

        def _get_delta_to_target(self,current_pos, current_alpha):
            delta_pos = current_pos - self.loc_target_pos
            dist = sum(delta_pos**2)
            angle = math.atan2(delta_pos[1], delta_pos[0]) - self.loc_target_alpha
            angle = angle if angle < math.pi else angle - 2*math.pi  # (-pi, pi>
            return dist, angle

        def get_target_vel(self, current_pos, current_alpha):
            dist, angle = self._get_delta_to_target(current_pos, current_alpha)
            common = 50*(2/(1+math.exp(-dist)) - 1)  # sigmoid from dist * k
            if math.pi*1/4 < math.fabs(angle) < math.pi*3/4:
                common = 0
            elif math.fabs(angle) > math.pi*3/4:
                common = -common
            diff = angle
            vl = common + diff
            vr = common - diff
            self.t += 1
            return np.mat([
                    [10],
                    [math.sin(self.t)]
                ])

    def __init__(self):
        self.model = Robot.Model()
        self.slam = Robot.SLAM()
        self.profiler = Robot.Profiler()


    def iterate(self):
        pos = self.slam.pos
        r = self.profiler.get_target_vel(self.slam.pos, self.slam.alpha)
        #r = np.mat([[10], [30]])
        self.model.iterate(r, 0.001)
        self.slam.odometry(self.model.Y)
        return pos

def trace_to(target, robot):
    pos = []
    alpha = []
    robot.profiler.loc_target_pos = target[0:1]
    robot.profiler.loc_target_alpha = target[2]
    for i in range(100):
        robot.iterate()
        pos.append(robot.slam.pos.tolist())
        alpha.append(robot.slam.alpha)
    return pos, alpha

model = Robot.Model(t_n=2)
mm = Robot()
mm.iterate()

pos, angle = trace_to((100, 50, 1), mm)
l = 15
for (x, y), angle in zip(pos, angle):
    print((x, y, angle))
    plt.plot([x, x+l*math.cos(angle)], [y, y+l*math.sin(angle)], 'g')
    plt.plot([x], [y], 'bo')
plt.axis('equal')
plt.show()







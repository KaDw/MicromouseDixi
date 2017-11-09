import numpy as np
import scipy

class Model:
    def __init__(self, k=0.1, zeta=1, t_n=0.7, w_n=None):
        #  assert 0 <= zeta <= 1
        self.k = k
        self.zeta = zeta
        if w_n is not None and w_n != 0:
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
        w_n = 1.8 / self.t_n if self.t_n != 0 else 1e3
        k = self.k
        self.A = np.mat([
            [0, 1, 0, 0],
            [-(self.k + 1)*w_n ** 2, -2 * zeta * w_n, 0, 0],
            [0, 0, 0, 1],
            [0, 0, -(self.k + 1)*w_n ** 2, -2 * zeta * w_n]
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
        #self.K, _, _ = self.dlqr(self.A, self.B, self.Q, self.R)

    def f(self, X, U, delta_time):
        """
        X = [[vl],
            [al],
            [vr],
            [ar]]
        U = [[dl],
            [dr]]
        """
        assert (X.shape[1] == 1 and X.shape[0] == self.A.shape[1])
        assert self.B.shape[1] == U.shape[0]
        assert self.B.shape[0] == X.shape[0]
        I = np.mat(np.eye(self.A.shape[0]))
        return (self.A*delta_time + I)*X + self.B*U*delta_time

    def regulator(self, r, Y, Xhat=None):
        if Xhat is None:
            Xhat = self.X
        U = r-Y  # feedback
        # U = r - self.K*Xhat  # LQR

        def saturate(x):
            max_val = 8.0
            return max_val if x > max_val else -max_val if x < -max_val else x
        sat = np.vectorize(saturate)
        U = sat(U)  # saturate each element
        return U

    def measure(self, X):
        return self.C*X

    def iterate(self, r, delta_time):
        if not isinstance(r, (type(np.array([])), type(np.mat([[]])))):
            r = np.mat([r]).transpose()
        self.Y = self.measure(self.X)  # Y = C*X
        U = self.regulator(r, self.Y)
        self.X = self.f(self.X, U, delta_time)  # (A*dt + I)*X + B*U*dt
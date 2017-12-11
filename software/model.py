import numpy as np
import scipy
import scipy.linalg as la

class Model:
    #def __init__(self, k=0.86, zeta=(1, 1), w_n=(2.93, 442)):
    def __init__(self, sampling_time, k=0.86, zeta=(1,), w_n=(2.93,)):
        #  assert 0 <= zeta <= 1
        self.WHEEL_DIAMETER = 37 * 1e-3
        self.TICKS_PER_REVOLUTION = 3520.0
        self.k = k
        self.zeta = zeta
        self.w_n = w_n
        self.Q = np.diag([1, 1])
        self.R = np.diag([1, 1])
        self.A, self.B, self.C, self.K, self.L = None, None, None, None, None
        self.Y = None
        self.update(sampling_time)
        self.X = np.zeros((self.A.shape[0], 1))
        self.Y = self.C*self.X
        # regulator
        self.last_errP = np.array([0, 0], dtype=float)
        self.errI = np.array([0, 0], dtype=float)

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

    def update(self, sampling_time):
        ''' build state space, series G:
            1 / w
            -----
            s + w'''
        zeta = self.zeta
        w = [w*(z+(1-z**2)**0.5) for w, z in zip(self.w_n, self.zeta)]
        k = self.k
        if len(w) == 1:
            self.A = np.mat([
                [-w[0], 0],
                [0, -w[0]]
            ], dtype=type(0.0))
            self.B = np.mat([
                [k * w[0], 0],
                [0, k * w[0]]
            ], dtype=type(0.0))
            self.C = np.mat([
                [1, 0],
                [0, 1]
            ], dtype=type(0.0))
        else:
            self.A = np.mat([
                [0, 1, 0, 0],
                [-w[0]*w[1], -(w[0]+w[1]), 0, 0],
                [0, 0, 0, 1],
                [0, 0, -w[0]*w[1], -(w[0]+w[1])]
            ], dtype=type(0.0))
            self.B = np.mat([
                [0, 0],
                [k*w[0]*w[1], 0],
                [0, 0],
                [0, k*w[0]*w[1]]
            ], dtype=type(0.0))
            self.C = np.mat([
                [1, 0, 0, 0],
                [0, 0, 1, 0]
            ], dtype=type(0.0))
        I = np.mat(np.eye(self.A.shape[0]))
        self.dA = (I + self.A*sampling_time/2)*(I - self.A*sampling_time/2)**(-1)
        #self.K, _, _ = self.dlqr(self.A, self.B, self.Q, self.R)

    def f(self, U, delta_time, X = None):
        """
        X = [[vl],
            [al],
            [vr],
            [ar]]
        U = [[dl],
            [dr]]
        """
        if X is None:
            X = self.X
        assert (X.shape[1] == 1 and X.shape[0] == self.A.shape[1])
        assert self.B.shape[1] == U.shape[0]
        assert self.B.shape[0] == X.shape[0]
        X_hat = self.dA*X + self.B*U*delta_time
        return X_hat

    def regulator(self, r, Y, Xhat=None):
        if Xhat is None:
            Xhat = self.X
        errVP = r[0, 0] - Y[0, 0]
        errWP = r[1, 0] - Y[1, 0]
        self.errI += np.array([errVP, errWP])
        errVD = errVP-self.last_errP[0]
        errWD = errWP-self.last_errP[1]
        self.last_errP = [errVP, errWP]

        UV = 100*errVP+4*errVD
        UW = 10*errWP+40*errWD

        U = np.mat([[UV-UW], [UV+UW]])
        #U = 200*(r-Y)  # feedback
        #U = r - self.K*Xhat  # LQR

        def saturate(x):
            max_val = 2.0
            return max_val if x > max_val else -max_val if x < -max_val else x
        sat = np.vectorize(saturate)
        U = sat(U)  # saturate each element
        return U

    def measure(self, X=None):
        if X is None:
            X = self.X
        k = self.TICKS_PER_REVOLUTION/self.WHEEL_DIAMETER/3.1415
        temp = (X*k).astype(int)
        temp += np.random.randint(-10, 10+1, temp.shape)
        temp = temp.astype(float)/k
        return self.C*temp

    def iterate(self, r, delta_time):
        if not isinstance(r, (type(np.array([])), type(np.mat([[]])))):
            r = np.mat([r]).transpose()
        self.Y = self.measure(self.X)  # Y = C*X
        #U = self.regulator(r, self.Y)
        #self.X = self.f(U, delta_time)  # (A*dt + I)*X + B*U*d
        self.X = self.f(r, delta_time)
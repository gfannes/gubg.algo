import numpy as np

class DCT:
    def __init__(self, n, t='dct4'):
        self.n = n
        self.t = t

        # See https://en.wikipedia.org/wiki/Discrete_cosine_transform#Informal_overview
        if False:
            pass
        elif self.t == 'dct4':
            seq = np.arange(self.n)+0.5
            f = np.pi/self.n
            self.fwd_ = np.cos(np.dot(seq.reshape((self.n,1)), seq.reshape((1,self.n)))*f)
            self.inv_ = np.copy(self.fwd_)*(2.0/self.n)
        elif self.t == 'sin':
            k = np.arange(1,self.n+1)
            e = 0.5/self.n
            x = np.linspace(e,1-e,self.n)

            angles = np.dot(x.reshape((self.n,1)), k.reshape((1,self.n)))
            self.inv_ = np.sin(angles*(np.pi))
            self.fwd_ = np.linalg.inv(self.inv_)
        elif self.t == 'sin2':
            k = np.arange(-1,self.n-1)
            e = 0
            x = np.linspace(e,1-e,self.n)

            angles = np.dot(x.reshape((self.n,1)), k.reshape((1,self.n)))
            self.inv_ = np.sin(angles*(np.pi))
            self.inv_[:,0] = np.ones(self.n)
            self.inv_[:,1] = np.linspace(0,1,self.n)
            self.fwd_ = np.linalg.inv(self.inv_)
        else:
            raise ValueError(f'Unknown DCT type {self.t}')
    def __repr__(self):
        return f'DCT(n={self.n}, t={self.t})'
    def fwd(self, samples):
        return np.dot(self.fwd_, samples)
    def inv(self, freqs):
        return np.dot(self.inv_, freqs)

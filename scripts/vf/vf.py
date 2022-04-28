import numpy as np
import matplotlib.pyplot as plt 
import seaborn
import scipy as sp
import matplotlib.cm

class ATMVolFunction(object):

    def __init__(self, V0, V1):
        self.V0 = V0 
        self.V1 = V1
        self.L = np.log(2*self.V1 / (self.V1 + self.V0))

    def atm(self, tau):
        return self.V0 + self.V1*(1 - np.exp(-self.L*tau))

class Gatheral(object):

    def __init__(self, vol, epsilon, rho):
        self.epsilon = epsilon 
        self.rho = rho 
        self.vol = vol

        # these parameters are all fixed
        self.g1 = 0.250
        self.g2 = 0.250
        self.b1 = np.exp(5.18)
        self.b2 = np.exp(-3.0)
        self.n = 2.016048 * np.exp(self.epsilon)

    def phi(self, theta):
        arg0 = np.power(theta, self.g1)
        arg1 = np.power(1 + self.b1*theta, self.g2)
        arg2 = np.power(1 + self.b2*theta, 1.0-self.g1 - self.g2)
        return self.n / ( arg0 * arg1 * arg2 )

    def w_x(self, x, tau):

        # tau = T-t
        # x = log(K/F)
        # returns w = Sigma(x,T)T
        theta = self.vol.atm(tau) * self.vol.atm(tau) * tau 
        phi_theta = self.phi(theta)
        
        # now evaluate the actual gatheral function
        arg_sqrt = (phi_theta*x + self.rho)**2 + (1-self.rho)**2
        return 0.5*theta*(1.0 + self.rho*phi_theta*x + np.sqrt(arg_sqrt))

    def v_x(self, x, tau):
        return np.sqrt(self.w_x(x,tau)/tau)

    def w(self, s,K,r,tau):

        # gatheral function in $ space
        f = s*np.exp(r*tau)
        x = np.log(K/f)
        return self.w_x(x, tau)  

    def v(self, s, K, r, tau):
        return np.sqrt( self.w(s,K,r,tau)/tau )

    def loc(self, s, K, r, tau):

        # use wilmott version of dupire to get local vol from implied
        # we need derivatives in tau and K.  make this more explicit with the 
        # following lambdas
        vT = lambda iTau : self.v(s,K,r,iTau)
        vK = lambda iK : self.v(s,iK,r,tau)
        v_TK = vT(tau)

        epsT, epsK = 0.0001, 0.0001
        
        # estimate the three required derivatives
        dVdT = 0.5*(vT(tau+epsT) - vT(tau-epsT))/epsT
        dVdK = 0.5*(vK(K+epsK) - vK(K-epsK))/epsK 
        ddVdKK = (vK(K+epsK) - 2*v_TK + vK(K-epsK))/(epsK*epsK)

        num = v_TK*v_TK + 2*tau*v_TK*dVdT + 2*r*K*tau*v_TK*dVdK 
        d1  = _d1(s,K,r,tau,v_TK)
        den_arg1 = 1 + K*d1*np.sqrt(tau)*dVdK 
        den_arg2 = ddVdKK - d1*np.sqrt(tau)*dVdK*dVdK 
        den = den_arg1*den_arg1 + K*K*tau*v_TK*den_arg2 
        return np.sqrt(num/den)


def _d1(s,K,r,T,sig):
    
    arg1 = np.log(s/K)
    arg2 = (r + 0.5*sig*sig)*T
    return (arg1 + arg2)/(sig*np.sqrt(T))

def _d2(s,K,r,T,sig):
    return _d1(s,K,r,T,sig) - sig*np.sqrt(T)

def bs(s,K,r,T,sig):

    # black_scholes call prices
    cdf = sp.stats.norm.cdf
    return s*cdf(_d1(s,K,r,T,sig)) - K*np.exp(-r*T)*cdf(_d2(s,K,r,T,sig))

def atfm_k(S,r,T,x):
    return S*np.exp(x + r*T)

if __name__ == '__main__':

    print('Test Gatheral Vol Function')
    v = ATMVolFunction(0.15, 0.3)
    g = Gatheral(v, 0.25, -0.5)
    #tau = np.linspace(0, 2.0, 1001)
    #plt.plot(tau, v.atm(tau))

    T = 0.75
    S = 100.0
    r = 0.02
    x_min, x_max = -1.5, 1.5

    k_min, k_max = atfm_k(S,r,T,x_min), atfm_k(S,r,T,x_max)
    
    xx = np.linspace(x_min, x_max, 1001)
    kk = atfm_k(S,r,T,xx)
    #kk = np.linspace(k_min, k_max, 1001)

    #fig = plt.figure()

    # plot gatheral in x space
    if False:
        plt.subplot(2,1,1)
        plt.plot(xx, g.v_x(xx,T), color='b', label='Gatheral IV')
        plt.xlabel('x'); plt.ylabel('v(x)')
        plt.plot(xx, [g.loc(S,k,r,T) for k in kk], label='Gatheral LV', color='r')
        plt.grid(); plt.title('Gatheral - x space')
        plt.legend()

        plt.subplot(2,1,2)

        # compute the gatheral implied vol v = sqrt(w/T)
        plt.plot(kk, g.v(S,kk,r,T), label='Gatheral IV', color='b')

        # compute the corresponding local vol
        plt.plot(kk, [g.loc(S,k,r,T) for k in kk], label='Gatheral LV', color='r')
        plt.xlabel('k'); plt.ylabel('v(k)')
        plt.grid(); plt.title('Gatheral - k space')
        plt.legend()

    if False:

        # lets plot a local vol surface
        TT = np.linspace(0.1, 2.0, 11)
        locS = np.zeros(shape=(TT.size, xx.size), dtype=float)
        for i in np.arange(TT.size):
            locS[i][:] = g.loc(S,kk,r,TT[i])
        X,Y = np.meshgrid(xx,TT)
        fig = plt.figure()
        ax = fig.gca(projection='3d')
        surf = ax.plot_surface(X,Y,locS,linewidth=0, cmap=matplotlib.cm.RdBu, antialiased=False)
        plt.xlabel('log(K/F)'); plt.ylabel('T'); 
        plt.title('Gatheral Local Volatility')
        

    if True:
        for xi in [-2.0, -1.5, -0.5, 0, 0.5, 1.5, 2.0]:
            ki  = atfm_k(S,r,T,xi)
            w_x = np.sqrt(g.w_x(xi, T)/T)
            w   = np.sqrt(g.w(S,ki, r,T)/T)
            lv  = g.loc(S,ki,r,T)
            print('xi: %.2f | w(x)=%.4f | w(k)=%.4f | lv(k)=%.4f ' % (xi, w_x, w, lv) )

    
    """

    #S = np.linspace(10, 110, 1001)
    K = 80
    T = 1.0 
    sig = 0.25
    r = 0.02
    s = 60.0
    #plt.plot(S, [black_scholes(si, K, r,T,sig) for si in S])
    #plt.grid(); plt.show()
    """
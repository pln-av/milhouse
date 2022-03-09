import numpy as np
import numpy.random
import scipy.stats
import matplotlib.pyplot as plt 
import seaborn
import sys 

class System(object):

    def __init__(self, x_mu, x_sigma, y_sigma):
        self.x_mu = x_mu 
        self.x_sigma = x_sigma
        self.y_sigma = y_sigma

    def generate_observation(self, x):

        rv = scipy.stats.multivariate_normal([0,0], self.y_sigma)
        T = x.shape[1]-1
        eps = rv.rvs(T).T
        y = x[:, 1:] + eps
        return y 

    def generate_state(self, x0, T):
        # model is x_t = mu + x_{t-1} + eta_t
        # x0: initial condition
        # T: number of time periods (ie iterations), convention is this does NOT include the ic

        rv = scipy.stats.multivariate_normal([0,0], self.x_sigma)
    
        eta = rv.rvs(T+1).T # make them rows.  zeroth one is not used but makes things clearer below
        x = np.zeros(shape=(2, T+1), dtype=float)
        x[:,0] = x0 

        # do this in a loop for clarity here instead of vectorised
        for i in np.arange(1,T+1):
            x[0,i] = self.x_mu[0] + x[0,i-1] + eta[0,i]
            x[1,i] = self.x_mu[1] + x[1,i-1] + eta[1,i]
        return x 

    def generate_sample(self, x):

        # return new points from x points according to state model
        N = x.shape[1]
        rv = scipy.stats.multivariate_normal(self.x_mu, self.x_sigma)
        rvs = rv.rvs(N).T
        points = np.zeros(shape=(2,N), dtype=float)
        for i in np.arange(N):
            points[:,i] = x[:,i] + rvs[:,i]
        return points

    def update_weight(self, y, x, w):

        # update weights w using current points x
        # this is the slow part as i need to create new mv normal objects for each sim
        # is there a faster way in python to do this?
        N = x.shape[1]
        weights = np.zeros(N, dtype=float)
        for i in np.arange(N):
            rv = scipy.stats.multivariate_normal(x[:,i], self.y_sigma)
            weights[i] = rv.pdf(y)*w[i]
        return weights/np.sum(weights)

class Filter(object):
    def __init__(self, system):
        self.system = system

    def set_ic(self, x, w):
        
        if x.shape[1] != w.size:
            print('Error: x and w must have same size')
            sys.exit(1)

        self.N = x.size
        self.x0 = x
        self.w0 = w 

        self.x = x
        self.w = w 

    def iterate(self, y, T, verbose):

        # nb here i input a vector of observations in practice we 
        # would be receiving these one at a time
        if verbose:
            print('Iterate system %d times.' % T)
        self.T = T
        for i in np.arange(T-1):
            if verbose:
                print(' -- system at iteration %d.' % i )
            self.x = self.system.generate_sample(self.x)
            self.w = self.system.update_weight(y[:,i], self.x, self.w)

def uniform_ic(x0, scale, N):
    a_x, b_x = x0[0]-0.5*scale, x0[0]+0.5*scale
    a_y, b_y = x0[1]-0.5*scale, x0[1]+0.5*scale
    ic = np.zeros(shape=(2,N), dtype=float)
    ic[0,:] = a_x + (b_x-a_x)*scipy.stats.uniform.rvs(scale=1, loc=0, size=N)
    ic[1,:] = a_y + (b_y-a_y)*scipy.stats.uniform.rvs(scale=1, loc=0, size=N)
    return ic

if __name__ == '__main__':

    # parameters for the state update
    x0 = [1, 2]
    x_mu = [0.25, 0.75]
    x_sigma = [[0.05, 0.01],[0.01, 0.05]]
    y_sigma = [[0.01,0],[0,0.01]]
    system = System(x_mu, x_sigma, y_sigma)

    # generate a sample path {x_t, y_t} using the state object
    T = 30
    x = system.generate_state(x0, T)
    y = system.generate_observation(x)

    # set up filter and initialise with uniform points/weights
    filter = Filter(system)
    N = 1000
    scale = 10
    ic = uniform_ic(x0, scale, N)
    w = np.ones(N, dtype=float)/N
    filter.set_ic(ic, w)

    # use the system object to generate a sample path {x,y}_t we will use
    
    # 
    filter.iterate(y, T, True)
    if False:
        plt.plot(x[0,1:], x[1,1:], marker='o', alpha=0.25, label='x_t')
        plt.plot(y[0,:], y[1,:], color='r', marker='.', alpha=0.5, label='y_t')
        plt.plot()
        plt.xlabel('x'); 
        plt.ylabel('y')
        plt.grid(); 
        plt.legend()
        plt.title('Example 2 SIS for Random Walk Measured with Noise')
        plt.show()

    
    
    # begin filter
    """
    xk = ic
    wk = w 

    # generate new points
    xk1 = system.generate_sample(xk)
    wk1 = system.update_weight(y[:,0], xk1, wk)

    if True:
        # plot the IC and first iteration of the filter
        plt.scatter(ic[0,:], ic[1,:], marker='.', color='g', linestyle='None', alpha=0.1, label='ic')
        plt.scatter(xk1[0,:], xk1[1,:], marker='o', s=1E6*wk1, alpha=0.5, label='iteration 1')
        plt.scatter(y[0,0], y[1,0], marker='o', color='r')
        plt.xlabel('x'); plt.ylabel('y')
        plt.grid()
        plt.legend(loc='upper left')
        
        plt.xlim((np.min(ic[0,:]), np.max(ic[0,:])))
        plt.ylim(np.min(ic[1,:]), np.max(ic[1,:]))
        plt.title('SIS Filter First Re-Weighting')
        plt.show()
    """
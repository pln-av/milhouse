import numpy as np
import numpy.random
import scipy.stats
import matplotlib.pyplot as plt 
import seaborn

if __name__ == '__main__':

    N = 1000

    # generate uncorrelated bivariate uniform(-4,4) as our importance
    # distribution q
    u1, u2 = numpy.random.uniform(-4,4, N), numpy.random.uniform(-4,4, N)
    u = np.zeros(shape=(2,N), dtype=float)
    u[0,:] = u1 
    u[1,:] = u2
    plt.scatter(u[0,:], u[1,:], marker='.', label='importance', color='r', alpha=0.1)
    
    # compute weights w ~ p/q
    #                   ~ p 
    # where p ~ mv normal
    rv = scipy.stats.multivariate_normal([0,0], [[1.0, 0.5], [0.5, 1.0]])
    w = np.array([rv.pdf(u[:,i]) for i in np.arange(N)])
    w = w/np.sum(w)  # scale the weights

    # plot_scale_factor is just for aesthetics in the plot!
    plot_scale_factor = 5.0E4
    plt.scatter(u[0,:], u[1,:], marker='o', alpha=0.15, s=plot_scale_factor*w, label='weighted', color='b')
    plt.legend(loc='upper')
    plt.grid()

    plt.title('importance and weighted distribution')
    plt.show()
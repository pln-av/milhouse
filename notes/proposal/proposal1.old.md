
# Proposal 1 

# Parameter Learning in a Local Volatility Model Using a Particle Filter

In this project we use underlying prices and observed option data to make 
*online* inference about a local volatility model.  We use a parametric
model for the local volatility function, and apply a particle filter to 
construct iterative approximations of the posterior distribution of the
parameters on arrival of new underlying and option data.  This work is
perhaps closest in spirit to Hamida \& Cont (2005), but it differs in the
sense that they solve a pure calibration problem rather than approximate
the unknown parameter's posterior.  

## Problem Outline

### Part 1: Background Fluff

Assume that the risk neutral dynamics of an underlying process $S_t$ are 
given by 
\eq{ \label{eq:lvol} dS/S = r dt + \lvol (S,t; \theta) dB }
where $r$ is the risk free rate and we assume no dividends.  Here 
$\lvol (S,t;\theta)$ is the local volatility function, which we assume 
depends explicitely on the parameter set $\theta$.  Under the usual no
arbitrage assumptions options $V(S,t)$ on $S$ with expiry $T$ satisfy a
Black-Scholes equation
\eq{\pwrt{V}{t} + rS \pwrt{V}{S} + \frac{1}{2} \lvol^2(S,t;\theta) 
    \ppwrt{V}{s} = rV
}
with appropriate boundary and initial conditions.  In this paper we focus
on European call options $V_t(S,t)=c_t(S,T)$, for which the boundary and
initial conditions are
\eqa{
    V(S,T) &= \mrt{max}(S_T-K,0) \nonumber \\
    V(0,t) &= 0 \; \forall t<T \nonumber \\
    V(S,t) &= S - K \, \mrt{exp}(-r(T-t)) \; \forall t<T \nonumber
}

Given a set of $N$ observed call prices
$\left\{c_t(K_i,T_i)\right\}_{i=1}^{N}$ at time $t$, where $K_i$ and
$T_i$ are the strike and expiry of each call, it is possible to infer
the value of $\lvol(K_i, T_i; \theta)$ at each point in the grid of observed prices
via the Dupire Equation
\eq{ \label{eq:dupire}
    \lvol^2 (K,T; \theta) =  \frac{\pwrt{C}{T} + r K \pwrt{C}{K}}{\frac{1}{2} K^2 \ppwrt{C}{K}}
}
(Dupire, 1994).  The Dupire Equation above is a rearrangement of the Dupire
PDE
\eq{
    \pwrt{C(T,K)}{T} = -rK \pwrt{C(T,K)}{K} + \frac{1}{2}K^2 \lvol^2 (K,T; \theta) 
    \ppwrt{C}{K}
}
which has initial condition $C(t,K)=\mrt{max}(S-K,0)$ for all $K>0$.  Its solution
provides prices of all call options on underlying $S$ with strikes $K$ and $T$, 
though the process assumes an arbitrage free market of European call prices.  

### Part 2: Implementation Details

To make inference about the parameters $\theta$ in $\lvol(S,t;\theta)$, we apply
a particle filter to iteratively construct an approximation for posterior 
distribution of the parameters on arrival of new underlying and options data.

Define $c_{t}^{M}(K,T;\theta)$ to be a model generated call price at time $t$ with
strike $K$ and expiry $T$ given a particular set of local volatility parameters
$\theta$.  These model prices can be computed by solving either the Dupire Equation, 
or the Black-Scholes Equation, and are comparable to market observed prices $c_t(K,T)$.
To construct a dynamic system define $s_{1:t}=\left\{s_1, s_2, \hdots, s_t \right\}$
to be a set of observed underlying prices which are assumed to have been generated
from $\lvol(S,t;\theta)$.  This explicit dependance of $s_t$ on $\theta$ is hereafter ommitted
for clarity.  At each $t$ we have a set of $N$ observed call prices 
$\left\{c_t(K_i,T_i)\right\}_{i=1}^{N}$, where $K_i$ and $T_i$ are the strike and expiry 
of each call in the market (ie $N=K \times T$ where we have $K$ strikes and $T$ expiries).

It is conveniant to package the observed quantities into the set $x_{1:t}$, where 
at each time $t$ 
\eq{ x_t = \left[s_t, c_t(K_1,T_1;\theta), \hdots, c_t(K_N,T_N;\theta) \right] }
The objective of the project is to construct a model for the posterior 
$p(\theta | x_{1:t})$.  We construct a particle approximation for $p(\theta | x_{1:t+1})$
where the weight $\pi^{j}_{t+1}=p(\theta^j | x_{1:t+1})$ is updated according to the
standard rule
\eq{ \pi^{j}_{t+1} = \frac{p(x_{t+1}|x_t,\theta^j) \pi_{t}^{j} }{\sum_j 
p(x_{t+1}|x_t,\theta^j) \pi_{t}^{j}}. }
and $j=1,2,\hdots,J$ denotes a suitably large number of particles in
the approximation.  The design of the filter can follow Gellert \& Schlogl 2018.

To compute $p(x_{t+1}|x_t,\theta^j)$ we follow Hurn Et Al 2012 and note that the 
likelihood $p(x_{t+1}|x_t, \theta)$ can be written 
\eq{p(x_{t+1}|x_t, \theta)=f(s_{t+1}|s_t,\theta) \prod_{i=1}^{N} 
g\left(c_{t+1}(K_i,T_i)|c_{t+1}^{M}(K_i,T_i), \theta \right)}
as $x_t$ is fully observed and each option privides independant piece of
information.  The distribution $g(.)$ is discussed below.

Note that $s_{t+1}$ is conditional on $\theta$ via the local volatility function
$\lvol(S,t;\theta)$ and is distributed according to Equation (\ref{eq:lvol}).
The transition pdf required to compute the likelihood $f(s_{t+1}|s_t;\theta)$
satisfies a Fokker-Planck Equation
\eq{\pwrt{f}{t} = \frac{\partial}{\partial s} \left[ \frac{1}{2} 
\frac{\partial}{\partial s} \left( \lvol^2(s,t;\theta) f\right) - rsf \right] }
with initial condition $f(s,t_0;\theta) = \delta(s-s_0)$ and relevant boundary
conditions. In general this PDE will not have an analytic solution but can be
approximated to high accuracy (insert econometric references here).

The distribution $g\left(c_{t+1}(K,T)|c_{t+1}^{M}(K,T), \theta \right)$ is
problematic.  Since all option prices are known when conditioned on the underlying
and local volatility parameters $\theta$, the problem is overspecified.  This 
motivates the use of a *pricing error* model, which might be either multiplicative
or additive (insert references here).  For example we might take 
\eq{ g\left(c_{t+1}(K,T)|c_{t+1}^{M}(K,T), \theta \right) \sim 
\mb{N}(c_{t+1}^{M}(K,T),\sigma^{2}_M) 
}
where $\sigma_M$ is the spread for each option.  

This fully specifies a procedure for computing $p(\theta|x_{1:t})$ via a 
particle filter, where $x_{1:t}$ includes both underlying and option prices.

### Part 3: Reformulation Using Arbitrage Free Implied Volatility Surfaces

A slight reformulation of the problem allows us to volatility surfaces that 
are guaranteed to be free from static arbitrage.  Previously, we have discussed 
inference about the parameters $\theta$ in a local volatility function 
$\lvol(S,t; \theta)$.  Instead we can make inference about the implied 
volatility surface while restricting attention to surfaces free from 
static arbitrage.  From such a surface, we can compute $\lvol(K,T)$ via
Dupire's Formula (in the form for implied volatility rather than prices).

Define $\Sigma_t(K_i,T_i)$ to be the Black-Scholes implied volatility of a
European call option with price $c_t(K_i,T_i)$ at time $t$.  The collection
of $\Sigma_t(K,T)$ for all options in the market defines the implied 
volatility surface.  An equivalent formula to Dupire's Equation (\ref{eq:dupire})
is available so that local volatilities can be compute from implied volatilities.

Previously we assumed that $\lvol(S,t;\theta)$ was parameterised by a
set $\theta$, which we made inference about using a particle filter.  Here,
assume the the implied volatility surface can be parameterised by a set $\alpha$
in $\Sigma(K,T;\alpha)$.  We will restrict attention to $\Sigma(K,T;\alpha)$
guaranteed to be free from static arbitrage by construction, such as those 
introduced in (insert Gatheral references).  This is an easier way to 
enforce no-arbitrage requirements than more procedural methods (eg Fengler 2009).
This formulation has a secondary benefit of exploiting rapid closed form
solutions for model prices using the standard Black-Scholes formula.

Here I will outline the new procedure more clearly.  Define $c_{t}^{M}(K,T;\alpha)$ to
be the Black-Scholes price at time $t$ of a European call with strike $K$
and expiry $T$, whose Black-Scholes volatility $\sigma$ is given by
$\sigma=\Sigma(K,T;\alpha)$.  These call prices are free of static arbitrage,
since $\Sigma(K,T;\alpha)$ is free of static arbitrage by construction.  As
above, we construct the filter
\eq{ \pi^{j}_{t+1} = \frac{p(x_{t+1}|x_t,\alpha^j) \pi_{t}^{j} }{\sum_j 
p(x_{t+1}|x_t,\alpha^j) \pi_{t}^{j}} }
following Gellert \& Schlogl (2018).  A secondary advantage is that by definition
$c_{t}^{M}(K,T;\alpha)$ is available in closed form via 
\eq{ c_{t}^{M}(K,T;\alpha) = \mrt{Call}_{\mrt{BS}}\left(K,T,\sigma=\Sigma(K,T;\alpha)\right) }
when computing the option observation term in the likelihood
\eq{p(x_{t+1}|x_t, \alpha)=f(s_{t+1}|s_t,\alpha) \prod_{i=1}^{N} 
g\left(c_{t+1}(K_i,T_i)|c_{t+1}^{M}(K_i,T_i), \alpha \right)}

Some care is required when computing $f(s_{t+1}|s_t;\alpha^j)$ for the 
$j^{\mrt{th}}$ particle.  Remember that the $\alpha$ refers to parameters
defining the implied volatility surface, which does not factor directly in the 
underlying process
\eq{ dS/S = rdt + \lvol(S,t) \, dB}
The way to handle this will depend on the approximation techique used, but I don't foresee
a problem here.  For example, if we exploit the Euler approximation
\eq{ S_{t+1} = S_t + r S_t \Delta + \lvol(S_t,t) \Delta \varepsilon_t }
we only need a single value for $\lvol(S_t,t)$ and this value can be computed using 
Dupire's equation applied $\Sigma(K=S_t,T=t;\alpha^j)$.

# References 

R Cont, S Ben Hamida: Recovering volatility from option prices by evolutionary 
optimization. Journal of Computational Finance, Vol. 8, No 3, 43-76.

Dupire, B.: Pricing With A Smile. Risk Magazine, January, pp 18-20, 1994.

Fengler, M.: Arbitrage-free smoothing of the implied volatility surface, 
Quantitative Finance, 2009

Gellert \& Schlogl: Parameter Learning and Change Detection Using a Particle
Filter With Accelerated Adaption, 2018.

Hurn Et Al.: Estimating the Parameters of Stochastic Volatility Models using Option 
Price Data, Working Paper #87 October 2012
 
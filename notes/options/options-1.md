\newpage

# Local Volatility

## Introduction 

The Black-Scholes model assumes the underlying asset evolves 
according to the geometric brownian motion
\eq{ dS = \mu S dt + \sigma S dW }
where $\mu$ and more importantly $\sigma$ are constants.  A 
minimal deviation from this which may generate implied volatility
smiles is a **local volatility model**, where we allow the 
diffusion term to be a function of both $S$ and $t$ 
\eq{ dS = \mu S dt + S \sigma(S,t) dW }
where $\sigma(S,t)$ is a deterministic function of the stochastic
variable $S$.  A benefit here over stochastic volatility models
is that we still only have one underlying source of randomness
(which we can try to hedge).  The task remains to determine the
local volatility function $\sigma(S,t)$ which might generate the 
implied vol curve $\Sigma(k,t)$ observed in the market.

An example is the *Constant Elasticity of Variance* model
\eq{ dS = \mu S dt + \sigma S^\beta dW }
where $\sigma(S,t)=\sigma S^{\beta-1}$.  Two special cases are 
$\beta=1$ (ie the Black Scholes log-normal model) and $\beta=0$
(normal returns model).  Generally $\beta<<0$ in practice, but
this model does not generate plausible implied curves.

It is probably more intuitive to think of local volatility 
in terms of underlying returns, ie 
\eq{
    \frac{dS}{S} = \mu dt + \sigma(S,t) dW
}
where $\sigma(S,t)=\sigma S$ in the log-normal black scholes
model.

## Implied Distributions

Local volatility models, and other complex models are essentially
modelling the (risk-neutral) distribution of the underlying at expiry.
We can use observed prices to infer what that distribution is.  This
then informs our choice of which stochastic model to choose for the 
underlying, since we know what distribution we want this choice to achieve.
Remember, the price of a European option with payoff $v(S, T)$ at 
expiry is 
\eq{v_t = exp(-r \tau ) \int v(S,T) f(S_T,T) dS_t }
where $f(S_T,T)$ is the risk-neutral distribution of $S_T$ at expiry
and $\tau=T-t$ the time to expiry.  In the case of a call $c(k)$ 
\eq{v_t = exp(-r \tau ) \int_{k}^{\infty} (S_T - k)  f(S_T,T) dS_T. } 
Can we use observed prices to infer what $f(S_T,T)$ inside the integral 
must have been?  The answer, due to Breeden \& Litzenberger
is that we can!  Define
\eqa{
    G & = \int s f (s,t)ds \nonumber \\ 
    F & = \int f(s,t) ds \nonumber
}
and write the expression for $v_t$ as 
\eqa{
    exp(r \tau) v_t &= \int_{k}^{\infty} S_T f(S_T,T)dS_T - k \int_{k}^{\infty}f(S_T,T)dS_T \nonumber \\ 
    &= G(\infty) - G(k) - k \left( F(\infty) - F(k) \right) \nonumber 
}
Taking a derivative with respect to $k$ gives
\eqa{
    exp(r \tau) \pwrt{c}{k} &= 0 - \pwrt{G}{k} - \left[ k \left(0 - \pwrt{F}{k}\right) + \left( F(\infty) - F(k) \right)\right] \nonumber \\
    &= - \pwrt{G(k)}{k} + k \pwrt{F(k)}{k} - F(\infty) + F(k) \nonumber \\
    &= -k f(k,t) K kf(k,t) - F(\infty) + F(k) \nonumber \\
    &= -F(\infty) + F(k)
}
and after differentiating again
\eqa{
    exp(r \tau) c_{kk} &= 0 + \pwrt{F(k)}{k} \nonumber \\
    &= f(k,t) \nonumber \\
    \textrm{ie}\; \; c_{kk} = exp(-r\tau)f(k,t)
}
This relates the risk neutral underlying distribution $f(s, t)$ to the 
derivative $c_kk$.  This derivative can be approximated in various ways, 
including by using the standard finite difference approximation
\eq{
    c_{kk} = \frac{c(k+\Delta k)-2c(k) + c(k- \Delta k)}{\Delta k^2} \nonumber 
}  
using call prices above and below the strike of interest, or by fitting
some smooth function $c(k)$ to observed call prices (as a function of 
strike), the twice differentiating $c(k)$.

## Local Volatility and Implied Volatility

Implied volatility $\Sigma(S,t,k,T)$ is a two dimensional surface:
it varies cross sectionally in $k$ and forward $t\rightarrow T$ towards
longer dated options.  The relationship between local and implied 
volatilities can be thought of as:

* In the case that local volatility is a function $\sigma(S,t)=\sigma(S)$ 
of underlying alone, then the implied volatility $\Sigma(S,k,\tau)$ of an 
option with strike $k$, underlying $S$ and time to expiry (tte) $\tau$ is a 
cross-sectional average of implied volatilities between $k$ and $S$.
* In the case the local volatility is a function $\sigma(S,t)=\sigma(\tau)$ 
of tte $\tau$ only, then the implied volatility $\Sigma(S,k,\tau)$ of an 
option with strike $k$, underlying $S$ and time to expiry (tte) $\tau$ is a 
forward average over time of implied volatilities between $t$ and expiry $T$.
* The conjecture is that when local volatility is a function of underlying 
and tte ie $\sigma(S,t)=\sigma(S,\tau)$ then implied volatiility $\Sigma(S,k,\tau)$
is an average across *both* strikes and tte.  

We will investiage this final point more carefully now.  

## Dupire Equation

Note that previously
we inferred local volatility from option prices.  Now we wish to infer
local volatility from black-scholes implied volatilities $\Sigma(k,t,T,S)$.
I will leave out some maths here as it is a nice exercise in chain rule
but I don't think it is particular enlightening.  The Dupire equation is 
derived clearly in Gatheral pg 10, where he relies on the Breeden \& 
Litzenberger formula relating the terminal risk neutral distribution to 
$c_{kk}$, and the fact that such a distribution will satisfy a Fokker-Planck 
distribution to derive $\sigma(k,T)$, the local volatility at expiry $T$
when the stock is $S_T=k$, evaluated at the current (earlier) time $t$
when the underlying is $S=S_t$.  I prefer the derivation in Gatheral but
the exposition in Derman \& Miller so I will use their notation from here.

The Dupire equation is 
\eq{
    \frac{\sigma(K,T)}{2} = \frac{1}{k^2}\frac{ \pwrt{c}{T} |_k + r k \; 
    \pwrt{c}{k} |_T}{  \ppwrt{c}{k} |_T}
}
where all partial derivatives can be approximated from market prices.  The
more interesting step however is to derive a *similar* equation for local volatility,
but instead of writing in terms of prices use Black-Scholes implied 
volatilities $\Sigma$ instead.  Since market prices are usually quoted in 
terms of Black-Scholes vols ie 
\eq{
    c(S,k,\sigma,t,T) = c_{BS}(S,k,t,T,\Sigma(S,t,k,T))
}
we can transform the derivatives of prices to derivatives of implied
vols to derive
\eq{
   \sigma(K,T) = \frac{2\pwrt{c}{\Sigma} + \frac{\Sigma}{\tau}}{k^2 \left[ 
       \ppwrt{\Sigma}{k}-d_1\sqrt{\tau}\left(\pwrt{\Sigma}{k}\right)^2 + 
       \frac{1}{\Sigma}\left(\frac{1}{k\sqrt{\tau}} + d_1 \pwrt{\Sigma}{k}\right)^2 \right]}
}
where 
\eq{
    d_1 = \frac{\mrt{log}(S/k)}{\Sigma \sqrt{\tau}} + \frac{1}{2}\Sigma \tau
}
This formulation gives us some nice results, including the statements earlier
that when implied volatility is a function of $\tau$ only, then the total implied
variance is a sum of local volatility forward in time.   

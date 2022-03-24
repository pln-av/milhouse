# Milhouse Project Literature

* **Aihara Et Al 2008.  Estimating volatility and model parameters of stochastic volatility
  models with jumps using particle filter**.

Very clearly written paper with easy to follow results.  Discrete Heston model using Euler
scheme, and fit a clearly described filter to (log) stock prices.  Doucet et al. (2000) appears
to be main reference for the filter.  Good candidate to reproduce.

* **Hurn Et Al 2012. Estimating the Parameters of Stochastic
Volatility Models using Option Price Data**.

Very clearly written paper.  The introduction has a good summary of literature at least to
publication date (ie to 2012).  Their method appears to generate a colosal computational
load requiring gpu acceleration and QUT super computing facilities.  I'm not really sure
what they are doing.  The important thing is that *this paper uses option prices as well 
as underlying prices in volatility estimation*.  See pages 2 and 3 for this.  I think the 
idea is there is a two step procedure - use the underlying prices to model real world 
parameters and options to measure risk neutral ones.  The particle filter used is similar 
to Johannes, Polson and Stroud (2009).  This Johannes Et Al 2009 paper seems to be quoted 
regularly so I think we should read it.

* **Johannes, M.S., Polson, N.G., and Stroud, J.R. 2009. Optimal filtering of jump diffusions:
Extracting latent states from asset prices. Review of Financial Studies, 22(2759-2799)**.

Seems like this is the OG filtering paper for diffusions (this paper also considers jumps).  
At a first glance this paper very clearly describes different filters, how and when they
might work.  A most important feature is that they filter with and without option prices,
and show the huge performance benefits of using options.  I need to learn this paper and 
there is probably value in trying to reproduce.

* **Pather 2020. Parameter Learning with Particle Filters** 

This is a nicely written masters thesis.  It is very short, has a reasonable literature
review and implements a simple example of a filter for the Heston model using a Milstein
discretisation on stock price data.  This is very similar to the Aihara Et Al 2008 paper,
there must be some implementation details different in the filter.

* **Jacobs Et Al 2019.  Estimation and Filtering with Big Option Data:
Implications for Asset Pricing**

This is a helpful paper very similar to the Hurn Et Al 2015 paper.  This is a paper 
developing filters for stochastic vol models using both underlying and options.  It 
discusses some of the computational challenge, and proposes a work around.  I need to 
understand what that work around is.  This paper is also quite up to date, which is 
useful.

* **Muguruza 2019. Not so Particular about Calibration: Smile Problem Resolved**

This paper presents a predictor-corrector MC method to calibrate the LV component of a
SLV model.  It includes a large amount of literature needed on LSV models which is
helpful.  I don't see how the SV part is dealt with - this will be good to learn about.
Close attention is paid to Guyon \& Henry-Labordere which seems like another important
paper in the field.  I have a copy of what look like a presentation from Guyon's 
website, but the original is in Risk magazine and looks very technical...

* **Pascucci 2017. Particle Methods in Option Pricing**

This is a *Alma Mater Studiorum* at Bologna, though I don't know what that means.  This
is a great paper with lots of background.  Most of the first half is relatively simple
but the second half looks like it has a lot of useful material on deriving pricing
PDEs and characteristic functions in SLV models.  Looking on Pascucci's website he has
a heap of papers that look really interesting.  When I can get journal access I should
look through some of these.  His work looks very technical but explained well.

This develops a model which prime facie seems *way* more complex.  His pricing PDE 
is non-linear with coefficients computed from expected values.  He shows how to solve
this using mean-field techniques and a CUDA implementation.  Very cool.
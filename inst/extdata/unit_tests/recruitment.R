#############################
## Recruitment unit tests
#############################

# Sample an initial value for each parameter -----------------------------------------------------------------------

R0 <- runif(1, min=100, max=10000)
h <- runif(1, min=0.2001, max=0.75)
sigma_r <- runif(1, min=0.1, max=4)
deviations <- rnorm(1, mean=0, sd=sigma_r)
phi0 <- runif(1, min=0.001, max=10)
SB0 <- R0*phi0
ssb <- runif(1, min=100, max=SB0)
c <- runif(1, min=-1, max=1)
alpha <- runif(1, min=0.001, max=4)
beta <- runif(1, min=0.001, max=4)


# Calculate expect values for various spawner-recruit models --------------------------------

expect_Ricker <- ssb/phi0*exp(h*(1-ssb/(R0*phi0)))
expect_RickerAlt <- R0*exp(h * (1.0 - ssb /SB0)) #Check equation from MAS source code
expect_BevertonHolt <- (4 * h * R0 * ssb)/(SB0 * (1 - h) + ssb * (5 * h -1))
expect_BevertonHoltAlt <- (0.8*R0*h*ssb)/(0.2*R0*phi0*(1-h) + ssb*(h-0.2)) #Check equation from MAS source code
expect_BevertonHoltDep <- 4.0 * h * ssb / (ssb * (1.0 - h) + ssb * (5.0 * h - 1.0))
expect_Shepherd <- (alpha*ssb)/(1+(ssb/beta)^c)

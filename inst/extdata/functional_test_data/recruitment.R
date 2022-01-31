#############################
## Recruitment functional tests
#############################

# Set up true values -----------------------------------------------------------------------
set.seed(9999)
n <- 3 # Low, medium, and high levels
r0 <- 1000000
phi0 <- 0.01
sb0 <- r0 * phi0
ssb <- sb0 / 2
sigma_r <- seq(from = 0.21, to = 3, length.out = n)


# Beverton-Holt data ------------------------------------------------------

BevertonHolt_h <- seq(from = 0.21, to = 1, length.out = n)

BevertonHolt_data <- expand.grid(r0, phi0, sb0, ssb, BevertonHolt_h, sigma_r)
colnames(BevertonHolt_data) <- c("r0", "phi0", "sb0", "ssb", "h", "sigma_r")
BevertonHolt_data$deviation <- sapply(seq_along(BevertonHolt_data$sigma_r), function(x) rnorm(1, mean = 0, sd = BevertonHolt_data$sigma_r[x]))

BevertonHolt_data$recruit_expect <- (4 * BevertonHolt_data$h * BevertonHolt_data$r0 * BevertonHolt_data$ssb) / (BevertonHolt_data$sb0 * (1 - BevertonHolt_data$h) + BevertonHolt_data$ssb * (5 * BevertonHolt_data$h - 1))
BevertonHolt_data$recruit_deviation <- BevertonHolt_data$recruit_expect * exp(BevertonHolt_data$deviation)
BevertonHolt_data$recruit_biasadj <- BevertonHolt_data$recruit_deviation * exp(-0.5 * BevertonHolt_data$sigma_r^2)

# Ricker data -------------------------------------------------------------

Ricker_h <- seq(from = 0.21, to = 3, length.out = n)

Ricker_data <- expand.grid(r0, phi0, sb0, ssb, Ricker_h, sigma_r)
colnames(Ricker_data) <- c("r0", "phi0", "sb0", "ssb", "h", "sigma_r")
Ricker_data$deviation <- sapply(seq_along(Ricker_data$sigma_r), function(x) rnorm(1, mean = 0, sd = Ricker_data$sigma_r[x]))
Ricker_data$recruit_expect <- Ricker_data$ssb / Ricker_data$phi0 * exp(Ricker_data$h * (1 - Ricker_data$ssb / (Ricker_data$r0 * Ricker_data$phi0)))
Ricker_data$recruit_deviation <- Ricker_data$recruit_expect * exp(Ricker_data$deviation)
Ricker_data$recruit_biasadj <- Ricker_data$recruit_deviation * exp(-0.5 * Ricker_data$sigma_r^2)

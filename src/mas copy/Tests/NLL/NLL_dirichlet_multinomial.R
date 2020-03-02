# Compute NLL for dirichlet-multinomial model of age composition data

rm(list=ls(all.names=F))    # Remove all variables from  session memory
graphics.off()              # Close all graphics windows

# Input Data
# Amax = Number of age categories [1,2,...,Amax]
# Theta = Input sample size fraction parameter
# N = number of age composition samples
# X_obs = vector of number of fish at age
# P_pred = vector of predicted proportion of fish at age

# Output Data
# NLL = Value of negative multinomial loglkelihood for P
# Neff = Effective sample size estimate using McAllister-Ianelli estimator
# Std_residual = Vector of Pearson residuals for proportion of fish at age

# Case 1
# Input Data
Amax <- 10
Theta <- 0.40
N <- 100
X_obs <- c(2,4,8,16,32,16,8,4,2,8)
P_pred <- c(0.03,0.02,0.09,0.14,0.34,0.13,0.10,0.06,0.04,0.05)

# Calculate Output Data 
P_obs <- X_obs/N

NLL <- 0
NLL <- -lgamma(Theta*N) + lgamma(N+Theta*N)
print(NLL)
for (a in 1:Amax)
{
  print(lgamma(N*P_obs[a]+Theta*N*P_pred[a]) + lgamma(Theta*N*P_pred[a]))
 NLL <- NLL - (lgamma(N*P_obs[a]+Theta*N*P_pred[a]) + lgamma(Theta*N*P_pred[a]))
# print(NLL)
}

print(NLL)

Neff <- (1+Theta*N)/(1+Theta)

Std_residual <- c(rep(0,10))
for (a in 1:Amax)
  Std_residual[a] <- (P_obs[a]-P_pred[a])/sqrt(P_pred[a]*(1-P_pred[a])/Neff)

# Output Data
print("Dirichlet-Multinomial NLL Case 1")
print("____________________________________________________________________")
print("Number of age categories")
print(Amax)
print("Input sample size fraction parameter")
print(Theta)
print("Number of age samples")
print(N)
print("Observed number of fish by age category")
print(P_obs)
print("Predicted proportion of fish by age category")
print(P_pred)
print("Observed proportion of fish by age category")
print(P_obs)
print("Negative loglikelihood")
print(NLL)
print("Effective sample size from Dirichlet-multinomial estimator")
print(Neff)
print("Standardized residuals of proportions at age")
print(Std_residual)
print("____________________________________________________________________")

# Case 2
# Input Data
Amax <- 10
Theta <- 0.50
N <- 100
X_obs <- c(1,4,9,15,34,14,8,4,2,10)
P_pred <- c(0.1,0.02,0.11,0.12,0.33,0.14,0.11,0.06,0.04,0.07)

# Calculate Output Data 
P_obs <- X_obs/N

NLL <- 0
NLL <- -lgamma(Theta*N) + lgamma(N+Theta*N)
for (a in 1:Amax)
{
  print(lgamma(N*P_obs[a]+Theta*N*P_pred[a]) + lgamma(Theta*N*P_pred[a]))
  NLL <- NLL - (lgamma(N*P_obs[a]+Theta*N*P_pred[a]) + lgamma(Theta*N*P_pred[a]))
}

Neff <- (1+Theta*N)/(1+Theta)

Std_residual <- c(rep(0,10))
for (a in 1:Amax)
  Std_residual[a] <- (P_obs[a]-P_pred[a])/sqrt(P_pred[a]*(1-P_pred[a])/Neff)

# Output Data
print("Dirichlet-Multinomial NLL Case 1")
print("____________________________________________________________________")
print("Number of age categories")
print(Amax)
print("Input sample size fraction parameter")
print(Theta)
print("Number of age samples")
print(N)
print("Observed number of fish by age category")
print(P_obs)
print("Predicted proportion of fish by age category")
print(P_pred)
print("Observed proportion of fish by age category")
print(P_obs)
print("Negative loglikelihood")
print(NLL)
print("Effective sample size from Dirichlet-multinomial estimator")
print(Neff)
print("Standardized residuals of proportions at age")
print(Std_residual)
print("____________________________________________________________________")










test_that("garbage-collected submodels leave no registry entries", {
  r4mas <- Rcpp::Module("rmas", dyn.load(dll_path))
  model <- new(r4mas$MASModel)
  model$Reset()
  on.exit(model$Reset(), add = TRUE)

  local({
    discarded <- new(r4mas$LogisticSelectivity)
    invisible(NULL)
  })
  gc()
  config <- jsonlite::fromJSON(model$GetJSONConfig())
  expect_null(config$selectivity)

  live <- new(r4mas$LogisticSelectivity)
  config <- jsonlite::fromJSON(model$GetJSONConfig())
  expect_equal(config$selectivity$id, live$id)
})

test_that("collecting an old object preserves a reused registry ID", {
  r4mas <- Rcpp::Module("rmas", dyn.load(dll_path))
  model <- new(r4mas$MASModel)
  model$Reset()
  on.exit(model$Reset(), add = TRUE)

  old_data <- new(r4mas$IndexData)
  old_id <- old_data$id
  model$Reset()
  data <- new(r4mas$IndexData)
  expect_equal(data$id, old_id)
  data$values <- 42
  data$error <- 0.1
  fleet <- new(r4mas$Fleet)
  fleet$AddIndexData(data$id, "undifferentiated")
  model$nyears <- 1
  model$nseasons <- 1
  model$nages <- 1
  model$ages <- 1

  rm(old_data)
  gc()
  output <- jsonlite::fromJSON(model$GetJSONData())
  expect_equal(as.numeric(unlist(output$data_object$values)), 42)
})

test_that("garbage-collected likelihoods are removed before model setup", {
  r4mas <- Rcpp::Module("rmas", dyn.load(dll_path))
  model <- new(r4mas$MASModel)
  model$Reset()
  on.exit(model$Reset(), add = TRUE)
  local({
    discarded <- new(r4mas$Lognormal)
    invisible(NULL)
  })
  gc()
  model$nyears <- 1
  model$nseasons <- 1
  model$nages <- 1
  model$ages <- 1
  model$extended_plus_group <- 1
  previous_directory <- setwd(tempdir())
  on.exit(setwd(previous_directory), add = TRUE)
  expect_error(model$Run(), NA)
  output <- jsonlite::fromJSON(model$GetOutput())
  expect_true(is.list(output))
  expect_identical(output$uncertainty$backend, "Quadra")
  expect_true(output$uncertainty$fixed_effects$success)
  expect_length(output$uncertainty$fixed_effects$rows, 0)
  expect_true(output$uncertainty$derived$success)
})

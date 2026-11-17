#!/usr/bin/env Rscript
# Run from any directory. Example:
# Rscript benchmarks/fims-vs-r4mas/run.R --reps=5 --r4mas-library=/path/to/library
script <- sub('^--file=', '', grep('^--file=', commandArgs(FALSE), value = TRUE)[1])
root <- dirname(normalizePath(script))
source(file.path(root, 'models.R'))
source(file.path(root, 'analysis.R'))
options <- commandArgs(TRUE)
option <- function(name, default) {
  hit <- grep(paste0('^--', name, '='), options, value = TRUE)
  if (length(hit)) sub(paste0('^--', name, '='), '', tail(hit, 1)) else default
}
repetitions <- as.integer(option('reps', '5'))
if (is.na(repetitions) || repetitions < 3L) stop('--reps must be at least 3.')
output <- option('output', file.path(root, 'results'))
dir.create(output, recursive = TRUE, showWarnings = FALSE)
output <- normalizePath(output)
# Discard only known, generated summaries so a failed new run cannot leave
# an old speed ranking alongside new accuracy failures.
unlink(file.path(output, c('REPORT.md','summary.csv','raw-results.csv',
  'stage-timings.csv','accuracy.csv','benchmark.png','benchmark.svg')))
source_files <- list.files(root,pattern='[.]R$',full.names=TRUE)
write.csv(data.frame(file=basename(source_files),md5=unname(tools::md5sum(source_files))),
  file.path(output,'benchmark-source-md5.csv'),row.names=FALSE)
worker_timeout <- as.integer(option('timeout', '120'))
if (is.na(worker_timeout) || worker_timeout<0L) stop('--timeout must be a nonnegative integer (0 disables it).')
mas_library <- option('r4mas-library', '')
if (nzchar(mas_library)) mas_library <- normalizePath(mas_library)
Sys.setenv(OMP_NUM_THREADS = '1', OPENBLAS_NUM_THREADS = '1', MKL_NUM_THREADS = '1', VECLIB_MAXIMUM_THREADS = '1')
cases <- data.frame(case = c('small', 'medium', 'large'), years = c(10L, 30L, 60L), ages = c(6L, 12L, 12L))
selected <- strsplit(option('cases', 'small,medium,large'), ',', fixed = TRUE)[[1]]
if (any(!selected %in% cases$case)) stop('Unknown case. Choose small,medium,large.')
cases <- cases[match(selected, cases$case), , drop = FALSE]
dir.create(file.path(output, 'cases'), showWarnings = FALSE)
for (i in seq_len(nrow(cases))) saveRDS(make_case(cases$years[i], cases$ages[i]),
  file.path(output, 'cases', paste0(cases$case[i], '.rds')))
run_worker <- function(engine, case, mode, replicate) {
  directory <- file.path(output, 'workers', sprintf('%s-%s-%s-%02d', case, engine, mode, replicate))
  dir.create(directory, recursive = TRUE, showWarnings = FALSE)
  target <- file.path(directory, 'result.rds')
  unlink(target)
  command <- file.path(R.home('bin'), 'Rscript')
  arguments <- c('--vanilla', shQuote(file.path(root, 'worker.R')),
    shQuote(file.path(output, 'cases', paste0(case, '.rds'))), engine, mode,
    shQuote(target), shQuote(if (engine == 'r4MAS') mas_library else ''))
  os <- Sys.info()[['sysname']]
  time_available <- file.exists('/usr/bin/time') && os %in% c('Darwin', 'Linux')
  if (time_available) {
    arguments <- c(if (os == 'Darwin') '-l' else '-v', shQuote(command), arguments)
    command <- '/usr/bin/time'
  }
  started <- as.numeric(Sys.time())
  status <- suppressWarnings(system2(command, arguments,
    stdout = file.path(directory, 'stdout.log'), stderr = file.path(directory, 'stderr.log'), timeout=worker_timeout))
  process_elapsed <- as.numeric(Sys.time()) - started
  if (status==124L) return(list(timed_out=TRUE,engine=engine,case=case,mode=mode,replicate=replicate,process_seconds=process_elapsed))
  if (status != 0L || !file.exists(target)) stop('Worker failed: ', directory, '; inspect stdout.log and stderr.log.')
  result <- readRDS(target)
  stderr <- readLines(file.path(directory, 'stderr.log'), warn = FALSE)
  rss <- NA_real_
  if (time_available && os == 'Darwin') {
    line <- grep('maximum resident set size', stderr, value = TRUE)
    if (length(line)) rss <- as.numeric(sub('^\\s*([0-9]+).*', '\\1', line[1])) / 1024^2
  } else if (time_available && os == 'Linux') {
    line <- grep('Maximum resident set size', stderr, value = TRUE)
    if (length(line)) rss <- as.numeric(sub('.*: *', '', line[1])) / 1024
  }
  result$peak_process_rss_mib <- rss
  result$process_seconds <- process_elapsed
  result$case <- case; result$replicate <- replicate
  saveRDS(result, target)
  result
}
results <- checks <- limits <- list()
completed <- character()
record_limit <- function(x) {
  limits[[length(limits)+1L]] <<- data.frame(case=x$case,engine=x$engine,mode=x$mode,
    replicate=x$replicate,limit_seconds=worker_timeout,observed_process_seconds=x$process_seconds,
    peak_process_rss_mib=NA_real_,
    outcome='worker timeout; case excluded from paired timing summary')
}
for (case in cases$case) {
  cat('Preflight:', case, '\n')
  spec <- readRDS(file.path(output, 'cases', paste0(case, '.rds')))
  limited <- FALSE
  for (engine in c('FIMS', 'r4MAS')) {
    preflight <- run_worker(engine, case, 'fixed', 0L)
    if (isTRUE(preflight$timed_out)) { record_limit(preflight); limited<-TRUE; break }
    checks[[length(checks) + 1L]] <- accuracy(preflight, spec)
  }
  if (limited) next
  if (!all(vapply(tail(checks, 2), function(x) x$passed, logical(1)))) {
    write.csv(do.call(rbind, checks), file.path(output, 'accuracy.csv'), row.names = FALSE)
    stop('Preflight failed; accuracy.csv contains the discrepancies. No speed ranking produced.')
  }
  for (replicate in seq_len(repetitions)) {
    # Alternate order to reduce order/thermal effects; never run competing
    # models simultaneously on the benchmark machine.
    order <- if (replicate %% 2L) c('FIMS', 'r4MAS') else c('r4MAS', 'FIMS')
    for (engine in order) {
      cat('Fit:', case, engine, replicate, '/', repetitions, '\n')
      result <- run_worker(engine, case, 'fit', replicate)
      if (isTRUE(result$timed_out)) { record_limit(result); limited<-TRUE; break }
      results[[length(results) + 1L]] <- result
      checks[[length(checks) + 1L]] <- accuracy(result, spec)
    }
    if (limited) break
  }
  if (!limited) completed<-c(completed,case)
}
finalize_results(results,checks,cases[cases$case %in% completed,,drop=FALSE],output,root,
  if(length(limits)) do.call(rbind,limits) else data.frame())
cat('Results:',file.path(output,'REPORT.md'),'\n')

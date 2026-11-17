write_benchmark_report <- function(output, cases, summary, validation, raw, source_root, limited=data.frame()) {
  source_files <- list.files(source_root, pattern='[.]R$', full.names=TRUE)
  write.csv(data.frame(file=basename(source_files),md5=unname(tools::md5sum(source_files))),
    file.path(output,'benchmark-source-md5.csv'),row.names=FALSE)
  colors <- c(FIMS = '#2978A0', r4MAS = '#E28D38')
  figure <- function() {
    par(mfrow = c(1, 2), mar = c(5, 4.5, 3, 1), oma = c(2, 0, 1, 0))
    labels <- paste0(cases$years, ' yr / ', cases$ages, ' ages')
    mat <- matrix(NA_real_, 2L, nrow(cases))
    # Explicit indexing keeps engine order stable regardless of split ordering.
    for (j in seq_len(nrow(cases)))
      mat[, j] <- vapply(c('FIMS','r4MAS'), function(e) summary$median_ms[summary$case == cases$case[j] & summary$engine == e], numeric(1))
    dimnames(mat) <- list(c('FIMS','r4MAS'), labels)
    upper <- matrix(0, 2, nrow(cases)); lower <- upper; memory <- upper
    for (j in seq_len(nrow(cases))) for (i in 1:2) {
      row <- summary[summary$case == cases$case[j] & summary$engine == rownames(mat)[i], ]
      lower[i,j] <- row$q25_ms; upper[i,j] <- row$q75_ms
      memory[i,j] <- row$median_peak_process_rss_mib
    }
    plot(NA, xlim=c(0.6,nrow(cases)+0.4),
      ylim=c(min(lower)*0.6,max(upper)*2), log='y', xaxt='n',
      xlab='', ylab='Model pipeline wall time (ms; log scale)',
      main='Native model pipeline')
    axis(1,at=seq_len(nrow(cases)),labels=labels)
    for (i in 1:2) {
      xpos <- seq_len(nrow(cases)) + if (i==1) -0.06 else 0.06
      lines(xpos,mat[i,],col=colors[i],lwd=2)
      points(xpos,mat[i,],col=colors[i],pch=c(16,17)[i],cex=1.1)
      arrows(xpos,lower[i,],xpos,upper[i,],angle=90,code=3,length=.035,col=colors[i])
    }
    legend('topleft', c('FIMS / TMB', 'r4MAS / Quadra'), col=colors,
      pch=c(16,17), lty=1, bty='n', cex=.85)
    if (any(is.finite(memory))) {
      barplot(memory, beside = TRUE, col = colors, names.arg = labels,
        ylim = c(0, max(memory, na.rm = TRUE) * 1.2), border = NA,
        ylab = 'Peak process resident memory (MiB)', main = 'Full worker process, including package load')
    } else { plot.new(); title('Process RSS unavailable on this platform') }
    mtext('Median; timing whiskers show interquartile range. One estimated parameter: survey catchability.',
      side = 1, outer = TRUE, line = 0, cex = .9)
  }
  if (nrow(cases)) {
    png(file.path(output, 'benchmark.png'), width = 1950, height = 825, res = 150)
    figure(); dev.off()
  }
  cpu <- if (Sys.info()[['sysname']] == 'Darwin') suppressWarnings(system2('/usr/sbin/sysctl', c('-n','machdep.cpu.brand_string'), stdout=TRUE, stderr=FALSE)) else Sys.info()[['machine']]
  lines <- c('# FIMS/TMB versus r4MAS/Quadra: first matched-model benchmark', '',
    paste('Generated:', format(Sys.time(), tz = 'America/New_York', usetz = TRUE)), '',
    '## Scope', '',
    'A small simulated, one-area, two-sex (r4MAS) / aggregated-sex (FIMS) catch-at-age model with one fishing fleet and one biomass survey. Only survey catchability q is estimated. Natural mortality, annual F, weight at age, selectivity, initial abundance, recruitment scale, and steepness are fixed. The finite-logit h = 1 - 1e-12 Beverton–Holt limit gives effectively constant recruitment. This is a first fixed-dynamics comparison, not a high-dimensional stock-assessment or random-effects benchmark.', '',
    'The shared specification is generated once per case, with a fixed seed. Both models use lognormal catch and survey likelihoods without observation bias correction. r4MAS inputs are converted to thousands of fish and kg; results are converted back to fish and metric tons. The same initial numbers are encoded using fixed r4MAS initial deviations, including its native initial-F bisection and initial-abundance offset.', '',
    '## Timings', '',
    'Each repeat uses a fresh R process and loads only one modeling package. Package and Rcpp interface initialization occur before the model-pipeline timer. Engine order alternates across repeats. Timing includes object construction, native setup/taping, optimization, native uncertainty reporting, native export and reading that export. Native stages differ across packages, so the primary comparison is the total pipeline, not individual stage labels. R4MAS additionally writes its normal JSON output during Run. These are package-workflow timings, not isolated AD backend throughput. r4MAS propagates two sex-specific states and repeats a native initial-F solve; FIMS propagates an aggregated state and accepts initial abundance directly. Those representation and initialization costs are included.', '',
    '| Case | Dimensions | FIMS/TMB median [IQR], ms | r4MAS/Quadra median [IQR], ms | r4MAS / FIMS | Repeats per engine |',
    '|---|---|---:|---:|---:|---:|')
  for (i in seq_len(nrow(cases))) {
    f <- summary[summary$case == cases$case[i] & summary$engine == 'FIMS', ]
    m <- summary[summary$case == cases$case[i] & summary$engine == 'r4MAS', ]
    lines <- c(lines, sprintf('| %s | %d years × %d ages | %.2f [%.2f–%.2f] | %.2f [%.2f–%.2f] | %.2f× | %d |',
      cases$case[i], cases$years[i], cases$ages[i], f$median_ms,f$q25_ms,f$q75_ms,
      m$median_ms,m$q25_ms,m$q75_ms,m$median_ms/f$median_ms,m$repeats))
  }
  if (nrow(cases)) lines<-c(lines,'','![Timing and memory comparison](benchmark.png)','')
  if (nrow(limited)) {
    lines<-c(lines,'## Resource-limited cases','',
      'Cases without the requested number of paired fits are excluded from the speed summary. Completed unpaired measurements remain in raw-results.csv with included_in_summary = FALSE.','',
      '| Case | Engine | Stage | Observed process seconds | Peak RSS, MiB | Budget seconds | Outcome |',
      '|---|---|---|---:|---:|---:|---|')
    for(i in seq_len(nrow(limited))) {
      x<-limited[i,]
      rss<-if('peak_process_rss_mib' %in% names(x)) format(round(x$peak_process_rss_mib,1),trim=TRUE) else 'unavailable'
      lines<-c(lines,sprintf('| %s | %s | %s | %.2f | %s | %.0f | %s |',
        x$case,x$engine,x$mode,x$observed_process_seconds,rss,x$limit_seconds,x$outcome))
    }
    lines<-c(lines,'','The worker budget includes package loading. Manually stopped exploratory runs may have observed times above that budget; the outcome column records what completed and what was interrupted. The runner enforces a configurable 120-second timeout by default. No speed ratio is claimed for an incomplete paired case.','')
  }
  lines <- c(lines, '## Numerical agreement', '',
    'Every fixed-parameter preflight and fitted repeat must pass the checks before a speed summary is generated. The checks compare numbers at age, biomass, catches, survey predictions, normalized likelihood, fitted q, and its analytic SE. With fixed population dynamics, the oracle is log(q_hat) = mean(log(index / vulnerable_biomass)) and SE(q_hat) = q_hat * sqrt(log(1 + CV^2) / n_years).', '',
    'FIMS estimates log(q); its TMB sdreport SE is transformed onto physical q for comparison. r4MAS reports a physical-q SE through Quadra. Likelihood constants (the FIMS lognormal Jacobian and normalizing constants, and constant r4MAS recruitment/selectivity contributions) are removed before comparing objective values. Raw objectives remain in each worker result.', '',
    '| Case | Maximum relative q error | Maximum relative SE error | Maximum N-at-age relative error | Raw SSB discrepancy | SSB discrepancy after convention diagnostic |',
    '|---|---:|---:|---:|---:|---:|')
  for (case in cases$case) {
    x <- validation[validation$case == case, ]; fit <- x[x$mode == 'fit', ]
    lines <- c(lines, sprintf('| %s | %.3g | %.3g | %.3g | %.4f%% | %.3g |',
      case,max(fit$q_relative_error),max(fit$se_relative_error),max(x$numbers_relative_error),
      100 * max(x$ssb_raw_relative_error),max(x$ssb_convention_adjusted_relative_error)))
  }
  lines <- c(lines, '',
    '**SSB finding:** r4MAS omits the first-age recruit contribution to SSB after the initial year in this case. `accuracy.csv` retains that raw discrepancy. Adding the known omitted contribution is a diagnostic only; neither native output is changed. The accuracy gate checks the explained convention difference, rather than claiming raw SSB equality. The discrepancy does not materially affect the selected likelihood in this near-constant-recruitment case. More general stock–recruitment comparisons require resolving that difference first.', '',
    '**Catch convention:** r4MAS adds a 1e-5 metric-ton catch floor. The explicit catch check allows that absolute floor; all other main state comparisons use relative tolerance 1e-8. q uses relative tolerance 1e-7; SE uses 1e-5; normalized NLL uses absolute tolerance 1e-5.', '',
    '**Gradient diagnostics:** each worker retains the package-native gradient and its coordinate (q versus log(q)). A standardized gradient (gradient × parameter SE) is included in raw results for inspection. Numerical fit gates use the analytic MLE/SE and native convergence status; no claim of raw-gradient equality is made.', '',
    '## Memory and reproducibility', '',
    'RSS is the maximum resident memory of the entire worker, measured by the OS, including R and package/module loading. It is not incremental model allocation and should not be interpreted as a pure AD tape-memory comparison. Cold-process elapsed times and package/module load times are separately retained in raw-results.csv. Single-thread environment settings are requested for OpenMP and common BLAS implementations. OS file caches are not flushed; cold process means a fresh R interpreter.', '',
    '| Engine | Package version | Installed library | DLL MD5 |', '|---|---|---|---|')
  for (engine in c('FIMS','r4MAS')) {
    x <- raw[raw$engine == engine, ][1, ]
    lines <- c(lines,sprintf('| %s | %s | `%s` | `%s` |',engine,x$package_version,x$package_path,x$dll_md5))
  }
  lines <- c(lines,'', paste('R:',R.version.string),paste('Platform:',R.version$platform), paste('OS:',Sys.info()[['sysname']],Sys.info()[['release']]), paste('CPU:',paste(cpu,collapse=' ')), '',
    'The exact installed FIMS source commit is not available in package metadata; its DLL hash identifies the binary used. The r4MAS Quadra source revision is recorded in src/mas/third_party/Quadra/UPSTREAM.md.', '',
    'Small samples on one machine are exploratory. Native reporting workloads and Rcpp object layers differ; the timing ratios are not general FIMS/TMB-versus-Quadra performance claims.', '',
    'Files: `summary.csv`, `raw-results.csv`, `stage-timings.csv`, `accuracy.csv`, and generated `cases/*.rds`. Each `workers/` subdirectory retains logs, native JSON, normalized results, and session information.')
  writeLines(lines, file.path(output,'REPORT.md'))
  writeLines(capture.output(sessionInfo()),file.path(output,'driver-session-info.txt'))
}

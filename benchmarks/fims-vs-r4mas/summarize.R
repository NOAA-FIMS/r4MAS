#!/usr/bin/env Rscript
# Re-analyze saved workers without re-running any model.
script <- sub('^--file=', '', grep('^--file=', commandArgs(FALSE), value=TRUE)[1])
root <- dirname(normalizePath(script))
source(file.path(root,'models.R'))
source(file.path(root,'analysis.R'))
args <- commandArgs(TRUE)
output <- normalizePath(if(length(args)) args[1] else file.path(root,'results'))
repetitions <- if(length(args)>=2) as.integer(args[2]) else 5L
files <- list.files(file.path(output,'workers'),pattern='result[.]rds$',recursive=TRUE,full.names=TRUE)
values <- lapply(files,readRDS)
if(!length(values)) stop('No completed worker results.')
case_names <- unique(vapply(values,function(x)x$case,character(1)))
specs <- setNames(lapply(case_names,function(x)readRDS(file.path(output,'cases',paste0(x,'.rds')))),case_names)
cases <- do.call(rbind,lapply(case_names,function(x)data.frame(case=x,years=specs[[x]]$years,ages=length(specs[[x]]$ages))))
cases <- cases[order(cases$years,cases$ages),,drop=FALSE]
checks <- lapply(values,function(x)accuracy(x,specs[[x$case]]))
results <- Filter(function(x)x$mode=='fit',values)
complete <- vapply(cases$case,function(case)all(vapply(c('FIMS','r4MAS'),function(engine)
  sum(vapply(results,function(x)x$case==case && x$engine==engine,logical(1)))>=repetitions,logical(1))),logical(1))
limit_path <- file.path(output,'resource-limits.csv')
limited <- if(file.exists(limit_path))read.csv(limit_path,stringsAsFactors=FALSE) else data.frame()
finalize_results(results,checks,cases[complete,,drop=FALSE],output,root,limited)
cat('Report:',file.path(output,'REPORT.md'),'\n')

# FIMS/TMB versus r4MAS/Quadra

A reproducible first benchmark: one simulated catch-at-age population, one
fleet, one survey, and one estimated survey-catchability parameter. An analytic
MLE and SE validate the fits before timing comparisons are reported. Read the
result report's scope and numerical-convention findings before interpreting
speed ratios.

Requirements: R, FIMS 0.11-style module API (`Catch`, `SetObservedCatchDataID`),
TMB, Rcpp, jsonlite, and the current Quadra-enabled r4MAS. No FIMS checkout is
required; the benchmark uses the installed package and records its DLL hash.

From the r4MAS repository root:

```sh
R CMD INSTALL --preclean .
Rscript --vanilla benchmarks/fims-vs-r4mas/run.R --reps=5
```

To select a separate r4MAS installation:

```sh
Rscript --vanilla benchmarks/fims-vs-r4mas/run.R --reps=5 \
  --r4mas-library=/path/to/R/library
```

Optional arguments: `--cases=small,medium,large`, `--output=/path/to/results`,
`--timeout=120` (per worker, including package load; `0` disables the limit).
Timed-out cases are retained as resource-limited outcomes and excluded from
paired speed summaries. The 60-year case exceeded the budget in the initial
exploratory run; the completed smaller cases are the supported comparison.
At least three repeats are required. Both engines run serially, in independent
R processes; their overlapping Rcpp class names prevent safely loading both
into one R session. Package and module loading are outside the model pipeline
timer and inside the separately measured cold-process time/RSS.

Outputs include a report, PNG figures, raw/stage timings, analytic accuracy
checks, inputs, per-worker native output, package paths, binary hashes, and
session information. Native uncertainty/reporting operations are included;
this harness does not measure isolated function/gradient throughput.

Regenerate tables and figures from saved worker results without fitting again:

```sh
Rscript --vanilla benchmarks/fims-vs-r4mas/summarize.R \
  benchmarks/fims-vs-r4mas/results 5
```

Keep the requested repeat count when summarizing; unpaired or incomplete cases
are excluded from summary medians, with their raw measurements retained.
The output report documents an SSB convention discrepancy found during model
alignment; do not extend this likelihood comparison to general recruitment
feedback before resolving it.

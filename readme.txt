# Performance-Oriented Fluid Simulation
### This project is a fluid simulation focused on optimizing performance by computing particle collisions in parallel.

This project contains fluid (main), sim (simulation modules), ftest and utest (self-explanatory),
and must contain:
- in/ for large.fld and small.fld
- test-in/ for utest and ftest fld test files
- out/ for reference outputs
- trz/ for trace outputs

Note the 4 additional shell statements: build.sh (for building on Release), run.sh (for running 
main on avg 5 times), and run_ftest.sh and run_utest.sh (self-explanatory).

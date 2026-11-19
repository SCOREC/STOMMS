# STOMMS testing for TOMMS Cases.

In STOMMS few of the TOMMS parameters are made obselete because those parameters were not needed given the new developments. Some of such parameters were
still used in the testing for TOMMS. This resulted in failing of the STOMMS tests when I tested TOMMS sample cases (with TOMMS input files). To verify
and update these test cases for STOMMS, I re ran TOMMS without the obselete (for STOMMS) parameters and re adjusted the tests accordingly. I am documenting
what parameters were removed and it affected which test case.

## inter\_curve\_spacing\_min
  - LTX\_1504291255\_47400
  -ltx\_1504291255\_47400\_xgca 

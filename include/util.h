#ifndef UTIL_H
#define UTIL_H

#include <vector>

// ALl the utility functions will be written here.

/*
 * Convert the normalized psi values to actual psi values. Takes normalized psi values
 * in a vector and return a vector of actual psi values.
 * std::vector <double> normPsi (in): vector of normalized psi values.
 * double psiAxis (in): actual psi value at the axis (O-point).
 * double psiLCF (in): actual psi value at the last closed curve.
 * returns a vector of actual psi values.
*/ 
std::vector <double> convertNormToPsi(std::vector <double> normPsi, double psiAxis, double psiLCF);


#endif

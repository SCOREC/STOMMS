#include <util.h>

// Convert the normalized psi values to actual psi values for a single given value.
double convertNormToPsi(double normPsi, double psiAxis, double psiLCF)
{
  double psi = normPsi*(psiLCF - psiAxis) + psiAxis;
  return psi; 
}

// Convert the normalized psi values to actual psi values for the full vector. 
std::vector <double> convertNormToPsiVector(std::vector <double> normPsi, double psiAxis, double psiLCF)
{
  std::vector <double> psiValues;
  for (int i = 0; i < normPsi.size(); i++)
  {
    double psiNorm = normPsi[i];  // Normalized psi value
    double psi = psiNorm*(psiLCF - psiAxis) + psiAxis;
    psiValues.push_back(psi);
  }
  return psiValues;
}


#include "biCubicSpline.h"

std::array <double,16> generateBiCubicCoefficients(const std::array <double,4>& xCoord,
                        const std::array <double,4>& yCoord, double xNewGridPoint, 
                        double yNewGridPoint, const std::array<double,16>& fieldValues)
{
  // Step 1: Construct Matrices tX and tY by initializing them with basic
  // functions (x^j) evaluated at xCoord, or yCoord.
  std::array <double,16> tX, tY;
  for (int i = 0; i < 4; i++)
  {
    double dx = xCoord[i] - xNewGridPoint;
    tX[4*i + 0] = 1.0;
    tX[4*i + 1] = dx;
    tX[4*i + 2] = dx*dx;
    tX[4*i + 3] = dx*dx*dx;

    double dy = yCoord[i] - yNewGridPoint;
    tY[4*i + 0] = 1.0;
    tY[4*i + 1] = dy;
    tY[4*i + 2] = dy*dy;
    tY[4*i + 3] = dy*dy*dy;
  }

  // Step 2: LU Factorization first for tX and then tY 
  std::array <lapack_int, 4> pivotX{}, pivotY{};  // LAPACK integer pivot
  lapack_int info = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, 4, 4, tX.data(), 4, pivotX.data());
  if (info != 0)
    throw std::runtime_error ("LAPACKE LU Factorization (LAPACKE_dgetrf) for tX failed\n");

  info = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, 4, 4, tY.data(), 4, pivotY.data());
  if (info != 0)
    throw std::runtime_error ("LAPACKE LU Factorization (LAPACKE_dgetrf) for tY failed\n");

  // Step 3: Solve the factorized matrices
  // tempMatrix = inv(tX)*fieldValues 
  // argument 'N' in the LAPACKE_dgetrs call means no transpose
  std::array <double,16> tempMatrix{};
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
      tempMatrix[i*4 + j] = fieldValues[j*4 + i];
  }

  info = LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'N', 4, 4, tX.data(), 4, pivotX.data(), tempMatrix.data(), 4); 

  if (info != 0)
    throw std::runtime_error ("LAPACKE Solve (LAPACKE_dgetrs) for tX failed\n");

  // Step 4: Solve for tempMatrix2 = inv(tY)*transpose(tempMatrix)
  // Step 4.1: First get transpose(tempMatrix)
  std::array <double,16> tempMatrix2{};
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
      tempMatrix2[i*4 + j] = tempMatrix[j*4 + i];
  }

  // Step 4.2: Solve for tempMatrix2
  info = LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'N', 4, 4, tY.data(), 4, pivotY.data(), tempMatrix2.data(), 4); 

  if (info != 0)
    throw std::runtime_error ("LAPACKE Solve (LAPACKE_dgetrs) for tY failed\n");

  // Step 5: Final coefficients matrix
  std::array <double,16> coefficients = tempMatrix2;

  return coefficients;
}

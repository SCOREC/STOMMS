#ifndef BICUBICSPLINE_H
#define BICUBICSPLINE_H

#include <lapacke.h>
#include <array>

std::array <double,16> generateBiCubicCoefficients(const std::array <double,4>& xCoord,
                        const std::array <double,4>& yCoord, double xNewGridPoint,
                        double yNewGridPoint, const std::array<double,16>& fieldValues);

#endif

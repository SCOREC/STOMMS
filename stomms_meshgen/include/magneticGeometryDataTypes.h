#ifndef MAGNETICGEOMETRYDATATYPES_H
#define MAGNETICGEOMETRYDATATYPES_H

// TO-DO: Make members in Flux and Plane private and use set-get functions.
/**
 * A class to contain the information fo a flux curve.
 */
class Flux{
  public:
    int planeNumber;  // plane on which flux curve lies.
    double psiNormOnFlux;  // normalized psi value of flux curve
    std::vector <Edge> edgesOnFlux;
    double nodeSpacingOnFlux;  // node spacing on flux curves
};

/**
 * A class to define geometric model on a plane.
 */
class Plane{
  public:
    std::vector <Face> modelFaces;
    std::vector <Flux> fluxCurves;  // vector of flux curves on the poloidal plane.
    Vertex oPoint;
    int planeNumber;  // plane number starting from 0 to numPlanes-1
};

/**
 * Struct VmecData contains all the input VmecData.
 */
struct VmecData{
  double majorR;  // Major radius of the reactor.
  double minorR;  // Minor radius of the reactor.
  int nSurf;  // Number of poloidal flux surfaces.
  int nMode;  // Number of modes for Fourier series.
  std::vector <double> R;  // Vector of cosines coeffiecents of R for Fourier series.
  std::vector <double> Z;  // Vector of sines coeffiecents of Z for Fourier series.
  std::vector <double> L;  // Vector of sines of lambdas coeffiecents for Fourier series.
  std::vector <double> iota;  // Vector of iota values corresponding to flux surfaces.
  std::vector <double> psi;  // Vector of list of psi values of flux surfaces.
  std::vector <double> xm;   // poloidal modes.
  std::vector <double> xn;   // Toroidal modes.
};

/**
 * Struct bmwData contains the data from BMW file.
 */
struct BmwData{
  // Populate it as we move forward.
};

/** 
 * Struct eqdskData contains the magnetic field information from eqdsk file.
 */
struct EqdskData{
  // add data here as we move forward.
  double psiAxis;
  double psiSep;
};

#endif

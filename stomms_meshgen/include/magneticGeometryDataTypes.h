#ifndef MAGNETICGEOMETRYDATATYPES_H
#define MAGNETICGEOMETRYDATATYPES_H

// Struct VmecData contains all the input VmecData.
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

// Struct bmwData contains the data from BMW file.
struct BmwData{
  // Populate it as we move forward.
};

// Struct eqdskData contains the magnetic field information from eqdsk file.
struct EqdskData{
  // add data here as we move forward.
  double psiAxis;
  double psiSep;
};

#endif

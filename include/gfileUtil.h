#ifndef GFILEUTIL_H
#define GFILEUTIL_H
#include <vector>
using std::vector;
class args;
extern "C"{
/*
 * PSPLINE Utility to read data from equilibrium files (.eqd and .eqdsk).
 */

/*
 * Initializes the spline to convert discrete data to continuous.
 */
void init_ez_spline_(int*, int*);

/*
 * Reads eqdsk file.
 */
void readgfile_(const char*, int*);

/*
 * Reads eqd file.
 */ 
void readeqdfile_(const char*, int*);

/*
 * Returns psi value at given point(r,z).
 */
void eval_field_val_ (const double* r, const double* z, double* val, int* ierr);

/*
 * Returns psi field gradient at given point(r,z).
 */
void eval_field_grad_ (const double* r, const double* z, double* val, int* ierr);

/*
 * Returns psi field derivates (dr and dz) at given point(r,z). 
 */
void eval_field_deriv_ (const double* r, const double* z, int* dr, int* dz, double* val, int* ierr);

/*
 * Returns current value at given psi value (phi).
 */
void eval_i_val_ (const double* phi, double* val, int* ierr);

/*
 * Returns current value gradient at given psi value (phi).
 */
void eval_i_grad_ (const double* phi, double* val, int* ierr);

/*
 * Returns magnetic field value b at given point(r,z).
 */
void eval_b_val_(const double* r, const double* z, double* Bval, int *ierr);

/*
 *  Gets the bounds of the bounding box of domain (rectangular grid).
 */
void get_b_box_(double * bbox);

/*
 * I never had to use the remaining functions in TOMMS. I will add the explanation
 * in case, we end up using them.
 */
void get_psi_grid_num_(int* mw, int* mh);
void get_psi_and_its_grid_(double* rgrid, double* zgrid, double* psirz);
void get_bd_pts_(double * x, double*y, int* numPts);
void get_num_bd_pts_(int* n);
void get_sep_pts_(double * x, double* y, int* numPts);
void get_x_pt_(double* xPt);
void get_axis_(double* axisPt);
void remove_duplicate_pt_ (double *inputx, double* inputy, int* numpt);
void set_eqd_psi_factor_ (double * fac);
void initial_setting_prf_ (double * , double *, double *);
void eq_ftn_setup_ti_ (int *, const char *, int *, double*, double*, double*, double*, double*, double*);
void eq_ftn_setup_ts_ (double *, int *, const char *, int *, double*, double*, double*, double*, double*, double*);
void eval_gri_val_ (double *, double *, double *, int *);
void eval_grs_val_ (double *, double *, double *, int *);
}
//return -1 if not found
// else return the idx of startPt
#endif

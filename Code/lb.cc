// In this file, the functions useful to calculate the equilibrium distribution
// function, momentums, the effective von Mises stress and the boundary conditions
// are reported

#include "config.h"


void (*lbmInnerCollision[COLLISION_TYPES]) (double omega, int i, double *density, double *v_x, double *v_y, double *v_z, double f_neq[]);

void (*lbmInterCollision[COLLISION_TYPES]) (double omega, int i, double *density, double *v_x, double *v_y, double *v_z, double f_neq[]);


void lbmConvertBoundaryData (double physical_data[], double lattice_data[], LBM *lbm)
{
  // convert pressure from physical units (mm Hg) to lattice units
  // assuming a reference pressure of 80 mmHg (see config.h for
  // constants setup)
  
  double useful_factor = PULSATILE_PERIOD / (lbm->period * lbm->voxel_size * lbm->voxel_size);
  
  
  useful_factor *= useful_factor;
  
  lattice_data[0] = 1.0 + (physical_data[0] - REFERENCE_PRESSURE) * PASCAL_TO_mmHg *
    useful_factor * lbm->voxel_size * lbm->voxel_size / (BLOOD_DENSITY * Cs2);
  
  lattice_data[1] = physical_data[1] * PASCAL_TO_mmHg *
    useful_factor * lbm->voxel_size * lbm->voxel_size / (BLOOD_DENSITY * Cs2);
  
  lattice_data[2] = physical_data[2] * PI / 180.;
}


double lbmConvertPressureToPhysicalUnits (double lattice_pressure, LBM *lbm)
{
  // convert pressure from lattice units to physical units (mm Hg) to lattice units
  // assuming a reference pressure of 80 mmHg (see config.h for
  // constants setup)
  
  double useful_factor = PULSATILE_PERIOD / (lbm->period * lbm->voxel_size * lbm->voxel_size);
  
  
  useful_factor *= useful_factor;
  
  return REFERENCE_PRESSURE + ((lattice_pressure / Cs2 - 1.0) * Cs2) * BLOOD_DENSITY /
    (PASCAL_TO_mmHg * useful_factor * lbm->voxel_size * lbm->voxel_size);
}


double lbmConvertVelocityToPhysicalUnits (double lattice_velocity, LBM *lbm)
{
  // convert velocity from lattice units to physical units (m/s)
  // (see config.h for constants setup)
  
  float physical_kinematic_viscosity = BLOOD_VISCOSITY / BLOOD_DENSITY;
  
  float useful_factor = (lbm->tau - 0.5) * Cs2 / physical_kinematic_viscosity;
  
  
  return lattice_velocity / useful_factor * lbm->voxel_size;
}


double lbmConvertStressToPhysicalUnits (double lattice_stress, LBM *lbm)
{
  // convert stress from lattice units to physical units (Pa)
  // (see config.h for constants setup)
  
  float physical_kinematic_viscosity = BLOOD_VISCOSITY / BLOOD_DENSITY;
  
  float useful_factor = (lbm->tau - 0.5) * Cs2 / physical_kinematic_viscosity;
  
  
  return lattice_stress * BLOOD_DENSITY /
    (useful_factor * useful_factor * lbm->voxel_size * lbm->voxel_size);
}


  
double lbmCalculateTau (LBM *lbm)
{
  double physical_kinematic_viscosity = BLOOD_VISCOSITY / BLOOD_DENSITY;
  
  
  return 0.5 + (PULSATILE_PERIOD * physical_kinematic_viscosity) /
    (Cs2 * lbm->period * lbm->voxel_size * lbm->voxel_size);
}


void lbmFeq (double f[], double *density, double *v_x, double *v_y, double *v_z, double f_eq[])
{
  double density_1;
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  
  *v_x = f[1] + (f[7] + f[9]) + (f[11] + f[13]);
  *v_y = f[3] + (f[12] + f[14]);
  *v_z = f[5] + f[10];
  
  *density = f[0] + (f[2] + f[4]) + (f[6] + f[8]) + *v_x + *v_y + *v_z;
  
  *v_x -= f[2] + (f[8] + f[10]) + (f[12] + f[14]);
  *v_y += (f[7] + f[9]) - (f[4] + (f[8] + f[10]) + (f[11] + f[13]));
  *v_z += f[7] + f[11] + f[14] - ((f[6] + f[8]) + f[9] + f[12] + f[13]);
  
  density_1 = 1. / *density;
  
  v_xx = *v_x * *v_x;
  v_yy = *v_y * *v_y;
  v_zz = *v_z * *v_z;
  
  temp1 = (1.0 / 8.0) * *density;
  
  f_eq[0] = temp1 - (1.0 / 3.0) * ((v_xx + v_yy + v_zz) * density_1);
  
  temp1 -= (1.0 / 6.0) * ((v_xx + v_yy + v_zz) * density_1);
  
  f_eq[1] = (temp1 + (0.5 * density_1) * v_xx) + ((1.0 / 3.0) * *v_x);   // (+1, 0, 0)
  f_eq[2] = (temp1 + (0.5 * density_1) * v_xx) - ((1.0 / 3.0) * *v_x);   // (+1, 0, 0)
  
  f_eq[3] = (temp1 + (0.5 * density_1) * v_yy) + ((1.0 / 3.0) * *v_y);   // (0, +1, 0)
  f_eq[4] = (temp1 + (0.5 * density_1) * v_yy) - ((1.0 / 3.0) * *v_y);   // (0, -1, 0)
  
  f_eq[5] = (temp1 + (0.5 * density_1) * v_zz) + ((1.0 / 3.0) * *v_z);   // (0, 0, +1)
  f_eq[6] = (temp1 + (0.5 * density_1) * v_zz) - ((1.0 / 3.0) * *v_z);   // (0, 0, -1)
  
  temp1 *= (1.0 / 8.0);
  
  temp2 = (*v_x + *v_y) + *v_z;
  
  f_eq[ 7] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2);   // (+1, +1, +1)
  f_eq[ 8] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2);   // (-1, -1, -1)
  
  temp2 = (*v_x + *v_y) - *v_z;
  
  f_eq[ 9] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2);   // (+1, +1, -1)
  f_eq[10] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2);   // (-1, -1, +1)
  
  temp2 = (*v_x - *v_y) + *v_z;
  
  f_eq[11] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2);   // (+1, -1, +1)
  f_eq[12] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2);   // (-1, +1, -1)
  
  temp2 = (*v_x - *v_y) - *v_z;
  
  f_eq[13] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2);   // (+1, -1, -1)
  f_eq[14] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2);   // (-1, +1, +1)
}


void lbmFeq (double density, double v_x, double v_y, double v_z, double f_eq[])
{
  double density_1;
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  
  density_1 = 1. / density;
  
  v_xx = v_x * v_x;
  v_yy = v_y * v_y;
  v_zz = v_z * v_z;
  
  temp1 = (1.0 / 8.0) * density;
  
  f_eq[0] = temp1 - (1.0 / 3.0) * ((v_xx + v_yy + v_zz) * density_1);
  
  temp1 -= (1.0 / 6.0) * ((v_xx + v_yy + v_zz) * density_1);
  
  f_eq[1] = (temp1 + (0.5 * density_1) * v_xx) + ((1.0 / 3.0) * v_x);   // (+1, 0, 0)
  f_eq[2] = (temp1 + (0.5 * density_1) * v_xx) - ((1.0 / 3.0) * v_x);   // (+1, 0, 0)
  
  f_eq[3] = (temp1 + (0.5 * density_1) * v_yy) + ((1.0 / 3.0) * v_y);   // (0, +1, 0)
  f_eq[4] = (temp1 + (0.5 * density_1) * v_yy) - ((1.0 / 3.0) * v_y);   // (0, -1, 0)
  
  f_eq[5] = (temp1 + (0.5 * density_1) * v_zz) + ((1.0 / 3.0) * v_z);   // (0, 0, +1)
  f_eq[6] = (temp1 + (0.5 * density_1) * v_zz) - ((1.0 / 3.0) * v_z);   // (0, 0, -1)
  
  temp1 *= (1.0 / 8.0);
  
  temp2 = (v_x + v_y) + v_z;
  
  f_eq[ 7] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2);   // (+1, +1, +1)
  f_eq[ 8] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2);   // (-1, -1, -1)
  
  temp2 = (v_x + v_y) - v_z;				     
  
  f_eq[ 9] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2);   // (+1, +1, -1)
  f_eq[10] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2);   // (-1, -1, +1)
  
  temp2 = (v_x - v_y) + v_z;				     
  
  f_eq[11] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2);   // (+1, -1, +1)
  f_eq[12] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2);   // (-1, +1, -1)
  
  temp2 = (v_x - v_y) - v_z;				     
  						     
  f_eq[13] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2);   // (+1, -1, -1)
  f_eq[14] = (temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2);   // (-1, +1, +1)
}


void lbmInnerCollision0 (double omega, int i,
			 double *density, double *v_x, double *v_y, double *v_z,
			 double f_neq[])
{
  double density_1;
  double *f;
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  
  f = &f_old[ i*15 ];
  
  *v_x = f[1] + (f[7] + f[9]) + (f[11] + f[13]);
  *v_y = f[3] + (f[12] + f[14]);
  *v_z = f[5] + f[10];
  
  *density = f[0] + (f[2] + f[4]) + (f[6] + f[ 8 ]) + *v_x + *v_y + *v_z;
  
  *v_x -= f[2] + (f[8] + f[10]) + (f[12] + f[14]);
  *v_y += (f[7] + f[9]) - (f[4] + (f[8] + f[10]) + (f[11] + f[13]));
  *v_z += f[7] + f[11] + f[14] - ((f[6] + f[8]) + f[9] + f[12] + f[13]);
  
  density_1 = 1. / *density;
  
  v_xx = *v_x * *v_x;
  v_yy = *v_y * *v_y;
  v_zz = *v_z * *v_z;
  
  temp1 = (1.0 / 8.0) * *density;
  
  f_new[ f_id[i*15+0]  ] = f[0] + omega * (f_neq[0] = f[0] - (temp1 - (1.0 / 3.0) * ((v_xx + v_yy + v_zz) * density_1)));
  
  temp1 -= (1.0 / 6.0) * ((v_xx + v_yy + v_zz) * density_1);
  
  f_new[ f_id[i*15+1] ] = f[1] + omega * (f_neq[1] = f[1] - ((temp1 + (0.5 * density_1) * v_xx) + ((1.0 / 3.0) * *v_x)));   // (+1, 0, 0)
  f_new[ f_id[i*15+2] ] = f[2] + omega * (f_neq[2] = f[2] - ((temp1 + (0.5 * density_1) * v_xx) - ((1.0 / 3.0) * *v_x)));   // (+1, 0, 0)
  
  f_new[ f_id[i*15+3] ] = f[3] + omega * (f_neq[3] = f[3] - ((temp1 + (0.5 * density_1) * v_yy) + ((1.0 / 3.0) * *v_y)));   // (0, +1, 0)
  f_new[ f_id[i*15+4] ] = f[4] + omega * (f_neq[4] = f[4] - ((temp1 + (0.5 * density_1) * v_yy) - ((1.0 / 3.0) * *v_y)));   // (0, -1, 0)
  
  f_new[ f_id[i*15+5] ] = f[5] + omega * (f_neq[5] = f[5] - ((temp1 + (0.5 * density_1) * v_zz) + ((1.0 / 3.0) * *v_z)));   // (0, 0, +1)
  f_new[ f_id[i*15+6] ] = f[6] + omega * (f_neq[6] = f[6] - ((temp1 + (0.5 * density_1) * v_zz) - ((1.0 / 3.0) * *v_z)));   // (0, 0, -1)
  
  temp1 *= (1.0 / 8.0);
  
  temp2 = (*v_x + *v_y) + *v_z;
  
  f_new[ f_id[i*15+7] ] = f[7] + omega * (f_neq[7] = f[7] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2)));   // (+1, +1, +1)
  f_new[ f_id[i*15+8] ] = f[8] + omega * (f_neq[8] = f[8] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2)));   // (-1, -1, -1)
  
  temp2 = (*v_x + *v_y) - *v_z;
  
  f_new[ f_id[i*15+ 9] ] = f[ 9] + omega * (f_neq[ 9] = f[ 9] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2)));   // (+1, +1, -1)
  f_new[ f_id[i*15+10] ] = f[10] + omega * (f_neq[10] = f[10] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2)));   // (-1, -1, +1)
  
  temp2 = (*v_x - *v_y) + *v_z;
  
  f_new[ f_id[i*15+11] ] = f[11] + omega * (f_neq[11] = f[11] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2)));   // (+1, -1, +1)
  f_new[ f_id[i*15+12] ] = f[12] + omega * (f_neq[12] = f[12] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2)));   // (-1, +1, -1)
  
  temp2 = (*v_x - *v_y) - *v_z;	 
  
  f_new[ f_id[i*15+13] ] = f[13] + omega * (f_neq[13] = f[13] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2)));   // (+1, -1, -1)
  f_new[ f_id[i*15+14] ] = f[14] + omega * (f_neq[14] = f[14] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2)));   // (-1, +1, +1)
}


void lbmInterCollision0 (double omega, int i,
			 double *density, double *v_x, double *v_y, double *v_z,
			 double f_neq[])
{
  double density_1;
  double *f;
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  
  f = &f_old[ i*15 ];
  
  *v_x = f[1] + (f[7] + f[9]) + (f[11] + f[13]);
  *v_y = f[3] + (f[12] + f[14]);
  *v_z = f[5] + f[10];
  
  *density = f[0] + (f[2] + f[4]) + (f[6] + f[ 8 ]) + *v_x + *v_y + *v_z;
  
  *v_x -= f[2] + (f[8] + f[10]) + (f[12] + f[14]);
  *v_y += (f[7] + f[9]) - (f[4] + (f[8] + f[10]) + (f[11] + f[13]));
  *v_z += f[7] + f[11] + f[14] - ((f[6] + f[8]) + f[9] + f[12] + f[13]);
  
  density_1 = 1. / *density;
  
  v_xx = *v_x * *v_x;
  v_yy = *v_y * *v_y;
  v_zz = *v_z * *v_z;
  
  temp1 = (1.0 / 8.0) * *density;
  
  f_new[ f_id[i*15+0] ] = f[0] + omega * (f_neq[0] = f[0] - (temp1 - (1.0 / 3.0) * ((v_xx + v_yy + v_zz) * density_1)));
  
  temp1 -= (1.0 / 6.0) * ((v_xx + v_yy + v_zz) * density_1);
  
  f_new[ f_id[i*15+1] ] = f[1] += omega * (f_neq[1] = f[1] - ((temp1 + (0.5 * density_1) * v_xx) + ((1.0 / 3.0) * *v_x)));   // (+1, 0, 0)
  f_new[ f_id[i*15+2] ] = f[2] += omega * (f_neq[2] = f[2] - ((temp1 + (0.5 * density_1) * v_xx) - ((1.0 / 3.0) * *v_x)));   // (+1, 0, 0)
  
  f_new[ f_id[i*15+3] ] = f[3] += omega * (f_neq[3] = f[3] - ((temp1 + (0.5 * density_1) * v_yy) + ((1.0 / 3.0) * *v_y)));   // (0, +1, 0)
  f_new[ f_id[i*15+4] ] = f[4] += omega * (f_neq[4] = f[4] - ((temp1 + (0.5 * density_1) * v_yy) - ((1.0 / 3.0) * *v_y)));   // (0, -1, 0)
  
  f_new[ f_id[i*15+5] ] = f[5] += omega * (f_neq[5] = f[5] - ((temp1 + (0.5 * density_1) * v_zz) + ((1.0 / 3.0) * *v_z)));   // (0, 0, +1)
  f_new[ f_id[i*15+6] ] = f[6] += omega * (f_neq[6] = f[6] - ((temp1 + (0.5 * density_1) * v_zz) - ((1.0 / 3.0) * *v_z)));   // (0, 0, -1)
  
  temp1 *= (1.0 / 8.0);
  
  temp2 = (*v_x + *v_y) + *v_z;
  
  f_new[ f_id[i*15+7] ] = f[7] += omega * (f_neq[7] = f[7] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2)));   // (+1, +1, +1)
  f_new[ f_id[i*15+8] ] = f[8] += omega * (f_neq[8] = f[8] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2)));   // (-1, -1, -1)
  
  temp2 = (*v_x + *v_y) - *v_z;
  
  f_new[ f_id[i*15+ 9] ] = f[ 9] += omega * (f_neq[ 9] = f[ 9] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2)));   // (+1, +1, -1)
  f_new[ f_id[i*15+10] ] = f[10] += omega * (f_neq[10] = f[10] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2)));   // (-1, -1, +1)
  
  temp2 = (*v_x - *v_y) + *v_z;
  
  f_new[ f_id[i*15+11] ] = f[11] += omega * (f_neq[11] = f[11] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2)));   // (+1, -1, +1)
  f_new[ f_id[i*15+12] ] = f[12] += omega * (f_neq[12] = f[12] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2)));   // (-1, +1, -1)
  
  temp2 = (*v_x - *v_y) - *v_z;	 
  
  f_new[ f_id[i*15+13] ] = f[13] += omega * (f_neq[13] = f[13] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) + ((1.0 / 24.0) * temp2)));   // (+1, -1, -1)
  f_new[ f_id[i*15+14] ] = f[14] += omega * (f_neq[14] = f[14] - ((temp1 + ((1.0 / 16.0) * density_1) * temp2 * temp2) - ((1.0 / 24.0) * temp2)));   // (-1, +1, +1)
}


void lbmCollision1 (double omega, int i,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[])
{
  double *f;
  double temp;
  
  int l;
  
  
  f = &f_old[ i*15 ];
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] = f[l];
    }
  *v_x = *v_y = *v_z = 0.F;
  
  *density = 0.;
  
  for (l = 0; l < 15; l++) *density += f[l];
  
  temp = (1.0 / 8.0) * *density;

  for (l = 0; l < 7; l++) f[l] = temp;
  
  temp *= (1.0 / 8.0);
  
  for (l = 7; l < 15; l++) f[l] = temp;
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] -= (f_new[ f_id[i*15+l] ] = f[l]);
    }
}


void lbmCollision2 (double omega, int i,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[])
{
  double *f;
  double dummy_density;
  
  unsigned int boundary_id, l;
  
  
  f = &f_old[ i*15 ];
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] = f[l];
    }
  boundary_id = (net_site_data[ i ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
  
  *density = inlet_density[ boundary_id ];
  
  lbmDensityAndVelocity (f, &dummy_density, v_x, v_y, v_z);
  lbmFeq (*density, *v_x, *v_y, *v_z, f);
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] -= (f_new[ f_id[i*15+l] ] = f[l]);
    }
}


void lbmCollision3 (double omega, int i,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[])
{
  double *f;
  double dummy_density;
  
  unsigned int boundary_id, l;
  
  
  f = &f_old[ i*15 ];
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] = f[l];
    }
  boundary_id = (net_site_data[ i ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
  
  *density = outlet_density[ boundary_id ];
  
  lbmDensityAndVelocity (f, &dummy_density, v_x, v_y, v_z);
  lbmFeq (*density, *v_x, *v_y, *v_z, f);
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] -= (f_new[ f_id[i*15+l] ] = f[l]);
    }
}


void lbmCollision4 (double omega, int i,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[])
{
  double *f;
  double temp;
  
  int l;
  
  unsigned int boundary_id;
  
  
  f = &f_old[ i*15 ];
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] = f[l];
    }
  boundary_id = (net_site_data[ i ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
  
  *density = inlet_density[ boundary_id ];
  
  *v_x = *v_y = *v_z = 0.F;
  
  temp = (1.0 / 8.0) * *density;
  
  for (l = 0; l < 7; l++) f[l] = temp;
  
  temp *= (1.0 / 8.0);
  
  for (l = 7; l < 15; l++) f[l] = temp;
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] -= (f_new[ f_id[i*15+l] ] = f[l]);
    }
}


void lbmCollision5 (double omega, int i,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[])
{
  double *f;
  double temp;
  
  int l;
  
  unsigned int boundary_id;
  
  
  f = &f_old[ i*15 ];
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] = f[l];
    }
  boundary_id = (net_site_data[ i ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
  
  *density = outlet_density[ boundary_id ];
  
  *v_x = *v_y = *v_z = 0.F;
  
  temp = (1.0 / 8.0) * *density;
  
  for (l = 0; l < 7; l++) f[l] = temp;
  
  temp *= (1.0 / 8.0);
  
  for (l = 7; l < 15; l++) f[l] = temp;
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] -= (f_new[ f_id[i*15+l] ] = f[l]);
    }
}


void lbmDensityAndVelocity (double f[], double *density, double *v_x, double *v_y, double *v_z)
{
  *v_x = f[1] + (f[7] + f[9]) + (f[11] + f[13]);
  *v_y = f[3] + (f[12] + f[14]);
  *v_z = f[5] + f[10];
  
  *density = f[0] + (f[2] + f[4]) + (f[6] + f[8]) + *v_x + *v_y + *v_z;
  
  *v_x -= (f[2] + f[8] + f[10] + (f[12] + f[14]));
  *v_y += (f[7] + f[9]) - ((f[4] + f[8] + f[10] + (f[11] + f[13])));
  *v_z += f[7] + f[11] + f[14] - (((f[6] + f[8]) + f[9] + f[12] + f[13]));
}


void lbmStress (double f[], double *stress)
{
  double sigma_xx_yy, sigma_yy_zz, sigma_xx_zz;
  double sigma_xy, sigma_xz, sigma_yz;
  double a, b;
  
  
  sigma_xx_yy = (f[1] + f[2]) - (f[3] + f[4]);
  sigma_yy_zz = (f[3] + f[4]) - (f[5] + f[6]);
  sigma_xx_zz = (f[1] + f[2]) - (f[5] + f[6]);
  
  sigma_xy = (f[7] + f[8]) + (f[9] + f[10]) - (f[11] + f[12]) - (f[13] + f[14]);
  sigma_xz = (f[7] + f[8]) - (f[9] + f[10]) + (f[11] + f[12]) - (f[13] + f[14]);
  sigma_yz = (f[7] + f[8]) - (f[9] + f[10]) - (f[11] + f[12]) + (f[13] + f[14]);
  
  a = sigma_xx_yy * sigma_xx_yy + sigma_yy_zz * sigma_yy_zz + sigma_xx_zz * sigma_xx_zz;
  b = sigma_xy * sigma_xy + sigma_xz * sigma_xz + sigma_yz * sigma_yz;
  
  *stress = lbm_stress_par * sqrt(a + 6.0 * b);
}


void lbmUpdateFlowField (int perform_rt, int i, double density, double vx, double vy, double vz, double f_neq[])
{
  double velocity, stress;
  
  
  if (perform_rt)
    {
      velocity = sqrt(vx * vx + vy * vy + vz * vz) / density;
      
      lbmStress (f_neq, &stress);
      
      *cluster_voxel[ 3*i   ] = (float)density;
      *cluster_voxel[ 3*i+1 ] = (float)velocity;
      *cluster_voxel[ 3*i+2 ] = (float)stress;
    }

  if (is_bench) return;
  
  if (!perform_rt)
    {
      velocity = sqrt(vx * vx + vy * vy + vz * vz) / density;
      
      lbmStress (f_neq, &stress);
    }
  
  lbm_density_min = (density < lbm_density_min) ? density : lbm_density_min;
  lbm_density_max = (density > lbm_density_max) ? density : lbm_density_max;
  
  lbm_velocity_min = (velocity < lbm_velocity_min) ? velocity : lbm_velocity_min;
  lbm_velocity_max = (velocity > lbm_velocity_max) ? velocity : lbm_velocity_max;
  
  lbm_stress_min = (stress < lbm_stress_min) ? stress : lbm_stress_min;
  lbm_stress_max = (stress > lbm_stress_max) ? stress : lbm_stress_max;
}


int lbmCollisionType (unsigned int site_data)
{
  unsigned int boundary_type, boundary_config;
  
  int unknowns, i;
  
  
  if (site_data == FLUID_TYPE)
    {
      return 0;
    }
  boundary_type = site_data & SITE_TYPE_MASK;
  
  if (boundary_type == FLUID_TYPE)
    {
      return 1;
    }
  boundary_config = (site_data & BOUNDARY_CONFIG_MASK) >> BOUNDARY_CONFIG_SHIFT;
  
  unknowns = 0;
  
  for (i = 0; i < 14; i++)
    {
      if (!(boundary_config & (1U << i))) ++unknowns;
    }
  if (boundary_type == INLET_TYPE)
    {
      if (unknowns <= 5)
	{
	  return 2;
	}
      else
	{
	  return 4;
	}
    }
  else
    {
      if (unknowns <= 5)
	{
	  return 3;
	}
      else
	{
	  return 5;
	}
    }
}


void lbmCalculateBC (double f[], unsigned int site_data, double *density,
		     double *vx, double *vy, double *vz, double f_neq[])
{
  double dummy_density;
  double temp;
  
  int unknowns, i;
  
  unsigned int boundary_type, boundary_config, boundary_id;
  
  
  for (i = 0; i < 15; i++)
    {
      f_neq[ i ] = f[ i ];
    }
  boundary_type = site_data & SITE_TYPE_MASK;
  
  if (boundary_type == FLUID_TYPE)
    {
      *density = 0.;

      for (i = 0; i < 15; i++) *density += f[ i ];
      
      temp = *density * (1.0 / 8.0);
      
      for (i = 0; i < 7; i++) f[ i ] = temp;
      
      temp *= (1.0 / 8.0);
      
      for (i = 7; i < 15; i++) f[ i ] = temp;
      
      *vx = *vy = *vz = 0.F;
    }
  else
    {
      boundary_config = (site_data & BOUNDARY_CONFIG_MASK) >> BOUNDARY_CONFIG_SHIFT;
      boundary_id     = (site_data & BOUNDARY_ID_MASK)     >> BOUNDARY_ID_SHIFT;
      
      unknowns = 0;
      
      for (i = 0; i < 14; i++)
	{
	  if (!(boundary_config & (1U << i))) ++unknowns;
	}
      if (boundary_type == INLET_TYPE)
	{
	  *density = inlet_density[ boundary_id ];
	}
      else
	{
	  *density = outlet_density[ boundary_id ];
	}
      if (unknowns <= 5)
	{
	  lbmDensityAndVelocity (f, &dummy_density, vx, vy, vz);
	  lbmFeq (*density, *vx, *vy, *vz, f);
	}
      else
	{
	  temp = *density * (1.0 / 8.0);
	  
	  for (i = 0; i < 7; i++) f[ i ] = temp;
	  
	  temp *= (1.0 / 8.0);
	  
	  for (i = 7; i < 15; i++) f[ i ] = temp;
	  
	  *vx = *vy = *vz = 0.F;
	}
    }
  for (i = 0; i < 15; i++)
    {
      f_neq[ i ] -= f[ i ];
    }
}


void lbmVaryBoundaryDensities (int cycle_id, int time_step, LBM *lbm)
{
  double w = 2. * PI / lbm->period;
  
  
  for (int i = 0; i < lbm->inlets; i++)
    {
      inlet_density[i] = inlet_density_avg[i] + inlet_density_amp[i] * cos(w * (double)time_step + inlet_density_phs[i]);
    }
  for (int i = 0; i < lbm->outlets; i++)
    {
      outlet_density[i] = outlet_density_avg[i] + outlet_density_amp[i] * cos(w * (double)time_step + outlet_density_phs[i]);
    }
}


void lbmInit (char *system_file_name, LBM *lbm, Net *net)
{
  lbm->system_file_name = system_file_name;
  
  lbmReadConfig (lbm, net);
  
  lbmInnerCollision[0] = lbmInnerCollision0;
  lbmInnerCollision[1] = lbmCollision1;
  lbmInnerCollision[2] = lbmCollision2;
  lbmInnerCollision[3] = lbmCollision3;
  lbmInnerCollision[4] = lbmCollision4;
  lbmInnerCollision[5] = lbmCollision5;
  
  lbmInterCollision[0] = lbmInterCollision0;
  lbmInterCollision[1] = lbmCollision1;
  lbmInterCollision[2] = lbmCollision2;
  lbmInterCollision[3] = lbmCollision3;
  lbmInterCollision[4] = lbmCollision4;
  lbmInterCollision[5] = lbmCollision5;
}


void lbmSetInitialConditions (Net *net)
{
  double *f_old_p, *f_new_p, f_eq[15];
  double density;
  
  int i, l;
  
  
  for (i = 0; i < net->my_sites; i++)
    {
      density = 1.;
      
      density *= (1.0 / 8.0);
      
      for (l = 0; l < 7; l++) f_eq[ l ] = density;
      
      density *= (1.0 / 8.0);
      
      for (l = 7; l < 15; l++) f_eq[ l ] = density;
      
      f_old_p = &f_old[ i*15 ];
      f_new_p = &f_new[ i*15 ];
      
      for (l = 0; l < 15; l++)
	{
	  f_new_p[ l ] = f_old_p[ l ] = f_eq[ l ];
	}
    }
}


int lbmCycle (int cycle_id, int time_step, int perform_rt, LBM *lbm, Net *net)
{
  // the entire simulation time step takes place through this function
  
  double f_neq[15];
  double omega;
  double density;
  double vx, vy, vz;
  double *f_old_p;
  
  double local_data[6];
  double global_data[6];
  
  int collision_type;
  int offset;
  int i, m, n;
  
  NeighProc *neigh_proc_p;
  
  
  for (m = 0; m < net->neigh_procs; m++)
    {
      neigh_proc_p = &net->neigh_proc[ m ];
#ifndef NOMPI
      net->err = MPI_Irecv (&f_old[ neigh_proc_p->f_head ],
			    neigh_proc_p->fs, MPI_DOUBLE,
			    neigh_proc_p->id, 10, MPI_COMM_WORLD,
			    &net->req[ 0 ][ m ]);
#endif
    }
  
  if (time_step == 0)
    {
      lbm_density_min = +1.e+30;
      lbm_density_max = +1.e-30;
      
      lbm_velocity_min = +1.e+30;
      lbm_velocity_max = +1.e-30;
      
      lbm_stress_min = +1.e+30;
      lbm_stress_max = +1.e-30;
    }
  
  omega = lbm->omega;
  
  offset = net->my_inner_sites;
  
  for (collision_type = 0; collision_type < COLLISION_TYPES; collision_type++)
    {
      for (i = offset; i < offset + net->my_inter_collisions[ collision_type ]; i++)
	{
	  (*lbmInterCollision[ collision_type ]) (omega, i, &density, &vx, &vy, &vz, f_neq);
	  
	  lbmUpdateFlowField (perform_rt, i, density, vx, vy, vz, f_neq);
	}
      offset += net->my_inter_collisions[ collision_type ];
    }
  
  for (m = 0; m < net->neigh_procs; m++)
    {
      neigh_proc_p = &net->neigh_proc[ m ];
#ifndef NOMPI
      net->err = MPI_Isend (&f_new[ neigh_proc_p->f_head ],
			    neigh_proc_p->fs, MPI_DOUBLE,
			    neigh_proc_p->id, 10, MPI_COMM_WORLD,
			    &net->req[ 0 ][ net->neigh_procs + m ]);
#endif
    }
  
  offset = 0;
  
  for (collision_type = 0; collision_type < COLLISION_TYPES; collision_type++)
    {
      for (i = offset; i < offset + net->my_inner_collisions[ collision_type ]; i++)
	{
	  (*lbmInnerCollision[ collision_type ]) (omega, i, &density, &vx, &vy, &vz, f_neq);
	  
	  lbmUpdateFlowField (perform_rt, i, density, vx, vy, vz, f_neq);
	}
      offset += net->my_inner_collisions[ collision_type ];
    }

  if (!is_bench && time_step == lbm->period - 1)
    {
#ifndef NOMPI
      local_data[ 0 ] = lbm_density_min;
      local_data[ 1 ] = 1. / lbm_density_max;
      local_data[ 2 ] = lbm_velocity_min;
      local_data[ 3 ] = 1. / lbm_velocity_max;
      local_data[ 4 ] = lbm_stress_min;
      local_data[ 5 ] = 1. / lbm_stress_max;
	  
      MPI_Reduce (local_data, global_data, 6,
		  MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
      
      lbm_density_min  = global_data[ 0 ];
      lbm_density_max  = 1. / global_data[ 1 ];
      lbm_velocity_min = global_data[ 2 ];
      lbm_velocity_max = 1. / global_data[ 3 ];
      lbm_stress_min   = global_data[ 4 ];
      lbm_stress_max   = 1. / global_data[ 5 ];
#endif
    }
  
  for (m = 0; m < net->neigh_procs; m++)
    {
#ifndef NOMPI
      net->err = MPI_Wait (&net->req[ 0 ][ m ], net->status);
      net->err = MPI_Wait (&net->req[ 0 ][ net->neigh_procs + m ], net->status);
#endif
    }
  
  for (n = 0; n < net->shared_fs; n++)
    {
      f_new[ f_recv_iv[n] ] = f_old[ net->neigh_proc[0].f_head + n ];
    }
  f_old_p = f_old;
  f_old = f_new;
  f_new = f_old_p;
  
  int is_unstable = 0;
  
  if (!is_bench && time_step == lbm->period - 1)
    {
      for (i = 0; i < net->my_sites * 15; i++)
	{
	  if (f_old[ i ] < 0.)
	    {
	      is_unstable = 1;
	    }
	}
    }
  
  if (is_unstable)
    {
      return UNSTABLE;
    }
  else
    {
      return STABLE;
    }
}


void lbmEnd (LBM *lbm)
{
  free(outlet_density_avg);
  free(outlet_density_amp);
  free(outlet_density_phs);
  free(outlet_density);
  
  free(inlet_density_avg);
  free(inlet_density_amp);
  free(inlet_density_phs);
  free(inlet_density);
  
  free(lbm->fluid_sites_per_block);
}

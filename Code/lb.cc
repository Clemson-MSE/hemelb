// In this file, the functions useful to calculate the equilibrium distribution
// function, momentums, the effective von Mises stress and the boundary conditions
// are reported

#include "config.h"

void (*lbmCollision[COLLISION_TYPES]) (double omega, double f[], int site_id, double *density, double *v_x, double *v_y, double *v_z, double f_neq[], LBM *lbm, Net *net);
void (*lbmCollisionSIMD[COLLISION_TYPES]) (double omega, double f[], int site_id, double density[], double v_x[], double v_y[], double v_z[], double f_neq[], LBM *lbm, Net *net);



void lbmFeq (double f[], double *density, double *v_x, double *v_y, double *v_z, double f_eq[])
{
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  
  *v_x = f[1] + (f[7] + f[9]) + (f[11] + f[13]);
  *v_y = f[3] + (f[12] + f[14]);
  *v_z = f[5] + f[10];
  
  *density = f[0] + (f[2] + f[4]) + (f[6] + f[8]) + *v_x + *v_y + *v_z;
  
  *v_x -= f[2] + f[8] + f[10] + (f[12] + f[14]);
  *v_y += (f[7] + f[9]) - (f[4] + f[8] + f[10 ] + (f[11] + f[13]));
  *v_z += f[7] + f[11] + f[14] - ((f[6] + f[8]) + f[9] + f[12] + f[13]);

  v_xx = *v_x * *v_x;
  v_yy = *v_y * *v_y;
  v_zz = *v_z * *v_z;
  
  temp1 = (1.0 / 8.0) * *density;
  
  temp2 = v_xx + v_yy + v_zz;
  
  f_eq[0] = temp1 - (1.0 / 3.0) * temp2;
  
  temp1 -= (1.0 / 6.0) * temp2;
  
  f_eq[1] = temp1 + ((1.0 / 3.0) * *v_x) + 0.5 * v_xx;   // (+1, 0, 0)
  f_eq[2] = temp1 - ((1.0 / 3.0) * *v_x) + 0.5 * v_xx;   // (+1, 0, 0)
  
  f_eq[3] = temp1 + ((1.0 / 3.0) * *v_y) + 0.5 * v_yy;   // (0, +1, 0)
  f_eq[4] = temp1 - ((1.0 / 3.0) * *v_y) + 0.5 * v_yy;   // (0, -1, 0)
  
  f_eq[5] = temp1 + ((1.0 / 3.0) * *v_z) + 0.5 * v_zz;   // (0, 0, +1)
  f_eq[6] = temp1 - ((1.0 / 3.0) * *v_z) + 0.5 * v_zz;   // (0, 0, -1)
  
  temp1 *= (1.0 / 8.0);
  
  temp2 = *v_x + *v_y + *v_z;
  
  f_eq[ 7] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, +1, +1)
  f_eq[ 8] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, -1, -1)
							     
  temp2 = *v_x + *v_y - *v_z;				     
  							     
  f_eq[ 9] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, +1, -1)
  f_eq[10] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, -1, +1)
							     
  temp2 = *v_x - *v_y + *v_z;				     
  							     
  f_eq[11] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, -1, +1)
  f_eq[12] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, +1, -1)
							     
  temp2 = *v_x - *v_y - *v_z;				     
  							     
  f_eq[13] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, -1, -1)
  f_eq[14] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, +1, +1)
}


void lbmFeq (double density, double v_x, double v_y, double v_z, double f_eq[])
{
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  
  v_xx = v_x * v_x;
  v_yy = v_y * v_y;
  v_zz = v_z * v_z;
  
  temp1 = (1.0 / 8.0) * density;
  
  temp2 = v_xx + v_yy + v_zz;
  
  f_eq[0] = temp1 - (1.0 / 3.0) * temp2;
  
  temp1 -= (1.0 / 6.0) * temp2;
  
  f_eq[1] = temp1 + ((1.0 / 3.0) * v_x) + 0.5 * v_xx;   // (+1, 0, 0)
  f_eq[2] = temp1 - ((1.0 / 3.0) * v_x) + 0.5 * v_xx;   // (+1, 0, 0)
  
  f_eq[3] = temp1 + ((1.0 / 3.0) * v_y) + 0.5 * v_yy;   // (0, +1, 0)
  f_eq[4] = temp1 - ((1.0 / 3.0) * v_y) + 0.5 * v_yy;   // (0, -1, 0)
  
  f_eq[5] = temp1 + ((1.0 / 3.0) * v_z) + 0.5 * v_zz;   // (0, 0, +1)
  f_eq[6] = temp1 - ((1.0 / 3.0) * v_z) + 0.5 * v_zz;   // (0, 0, -1)
  
  temp1 *= (1.0 / 8.0);
  
  temp2 = v_x + v_y + v_z;
  
  f_eq[ 7] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, +1, +1)
  f_eq[ 8] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, -1, -1)
  
  temp2 = v_x + v_y - v_z;				     
  
  f_eq[ 9] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, +1, -1)
  f_eq[10] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, -1, +1)
  
  temp2 = v_x - v_y + v_z;				     
  
  f_eq[11] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, -1, +1)
  f_eq[12] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, +1, -1)
  
  temp2 = v_x - v_y - v_z;				     
  						     
  f_eq[13] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, -1, -1)
  f_eq[14] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, +1, +1)
}


void lbmCollision0 (double omega, double f[], int site_id,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[], LBM *lbm, Net *net)
{
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  
  *v_x = f[1] + (f[7] + f[9]) + (f[11] + f[13]);
  *v_y = f[3] + (f[12] + f[14]);
  *v_z = f[5] + f[10];
  
  *density = f[0] + (f[2] + f[4]) + (f[6] + f[ 8 ]) + *v_x + *v_y + *v_z;
  
  *v_x -= f[2] + f[8] + f[10] + (f[12] + f[14]);
  *v_y += (f[7] + f[9]) - (f[4] + f[8] + f[10] + (f[11] + f[13]));
  *v_z += f[7] + f[11] + f[14] - ((f[6] + f[8]) + f[9] + f[12] + f[13]);

  v_xx = *v_x * *v_x;
  v_yy = *v_y * *v_y;
  v_zz = *v_z * *v_z;
  
  temp1 = (1.0 / 8.0) * *density;
  
  temp2 = v_xx + v_yy + v_zz;
  
  f[0] += omega * (f_neq[0] = f[0] - (temp1 - (1.0 / 3.0) * temp2));
  
  temp1 -= (1.0 / 6.0) * temp2;
  
  f[1] += omega * (f_neq[1] = f[1] - (temp1 + ((1.0 / 3.0) * *v_x) + 0.5 * v_xx));   // (+1, 0, 0)
  f[2] += omega * (f_neq[2] = f[2] - (temp1 - ((1.0 / 3.0) * *v_x) + 0.5 * v_xx));   // (+1, 0, 0)
  
  f[3] += omega * (f_neq[3] = f[3] - (temp1 + ((1.0 / 3.0) * *v_y) + 0.5 * v_yy));   // (0, +1, 0)
  f[4] += omega * (f_neq[4] = f[4] - (temp1 - ((1.0 / 3.0) * *v_y) + 0.5 * v_yy));   // (0, -1, 0)
  
  f[5] += omega * (f_neq[5] = f[5] - (temp1 + ((1.0 / 3.0) * *v_z) + 0.5 * v_zz));   // (0, 0, +1)
  f[6] += omega * (f_neq[6] = f[6] - (temp1 - ((1.0 / 3.0) * *v_z) + 0.5 * v_zz));   // (0, 0, -1)
  
  temp1 *= (1.0 / 8.0);
  
  temp2 = *v_x + *v_y + *v_z;
  
  f[7] += omega * (f_neq[7] = f[7] - (temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (+1, +1, +1)
  f[8] += omega * (f_neq[8] = f[8] - (temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (-1, -1, -1)
  
  temp2 = *v_x + *v_y - *v_z;				     
  
  f[ 9] += omega * (f_neq[ 9] = f[ 9] - (temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (+1, +1, -1)
  f[10] += omega * (f_neq[10] = f[10] - (temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (-1, -1, +1)
  
  temp2 = *v_x - *v_y + *v_z;				     
  
  f[11] += omega * (f_neq[11] = f[11] - (temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (+1, -1, +1)
  f[12] += omega * (f_neq[12] = f[12] - (temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (-1, +1, -1)
  
  temp2 = *v_x - *v_y - *v_z;				     
  
  f[13] += omega * (f_neq[13] = f[13] - (temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (+1, -1, -1)
  f[14] += omega * (f_neq[14] = f[14] - (temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (-1, +1, +1)
}


void lbmCollisionSIMD0 (double omega, double f[], int site_id,
			double density[], double v_x[], double v_y[], double v_z[],
			double f_neq[], LBM *lbm, Net *net)
{
  double f_vec[15][SIMD_SIZE];
  double f_neq_vec[15][SIMD_SIZE];
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  int j, l;
  
  
  for (j = 0; j < SIMD_SIZE; j++)
    {
      for (l = 0; l < 15; l++)
	{
	  f_vec[l][j] = f[ j*15+l ];
	  
	  f_neq_vec[l][j] = f_neq[ j*15+l ];
	}
    }
  for (j = 0; j < SIMD_SIZE; j++)
    {
      v_x[j] = f_vec[1][j] + (f_vec[7][j] + f_vec[9][j]) + (f_vec[11][j] + f_vec[13][j]);
      v_y[j] = f_vec[3][j] + (f_vec[12][j] + f_vec[14][j]);
      v_z[j] = f_vec[5][j] + f_vec[10][j];
      
      density[j] = f_vec[0][j] + (f_vec[2][j] + f_vec[4][j]) + (f_vec[6][j] + f_vec[8][j]) + v_x[j] + v_y[j] + v_z[j];
      
      v_x[j] -= f_vec[2][j] + f_vec[8][j] + f_vec[10][j] + (f_vec[12][j] + f_vec[14][j]);
      v_y[j] += (f_vec[7][j] + f_vec[9][j]) - (f_vec[4][j] + f_vec[8][j] + f_vec[10][j] + (f_vec[11][j] + f_vec[13][j]));
      v_z[j] += f_vec[7][j] + f_vec[11][j] + f_vec[14][j] - ((f_vec[6][j] + f_vec[8][j]) + f_vec[9][j] + f_vec[12][j] + f_vec[13][j]);
      
      v_xx = v_x[j] * v_x[j];
      v_yy = v_y[j] * v_y[j];
      v_zz = v_z[j] * v_z[j];
      
      temp1 = (1.0 / 8.0) * density[j];
      
      temp2 = v_xx + v_yy + v_zz;
      
      f_vec[0][j] += omega * (f_neq_vec[0][j] = f_vec[0][j] - (temp1 - (1.0 / 3.0) * temp2));
      
      temp1 -= (1.0 / 6.0) * temp2;
      
      f_vec[1][j] += omega * (f_neq_vec[1][j] = f_vec[1][j] - (temp1 + ((1.0 / 3.0) * v_x[j]) + 0.5 * v_xx));   // (+1, 0, 0)
      f_vec[2][j] += omega * (f_neq_vec[2][j] = f_vec[2][j] - (temp1 - ((1.0 / 3.0) * v_x[j]) + 0.5 * v_xx));   // (+1, 0, 0)
      
      f_vec[3][j] += omega * (f_neq_vec[3][j] = f_vec[3][j] - (temp1 + ((1.0 / 3.0) * v_y[j]) + 0.5 * v_yy));   // (0, +1, 0)
      f_vec[4][j] += omega * (f_neq_vec[4][j] = f_vec[4][j] - (temp1 - ((1.0 / 3.0) * v_y[j]) + 0.5 * v_yy));   // (0, -1, 0)
      
      f_vec[5][j] += omega * (f_neq_vec[5][j] = f_vec[5][j] - (temp1 + ((1.0 / 3.0) * v_z[j]) + 0.5 * v_zz));   // (0, 0, +1)
      f_vec[6][j] += omega * (f_neq_vec[6][j] = f_vec[6][j] - (temp1 - ((1.0 / 3.0) * v_z[j]) + 0.5 * v_zz));   // (0, 0, -1)
      
      temp1 *= (1.0 / 8.0);
      
      temp2 = v_x[j] + v_y[j] + v_z[j];
      
      f_vec[7][ j] += omega * (f_neq_vec[ 7][j] = f_vec[7][ j] - (temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (+1, +1, +1)
      f_vec[8][ j] += omega * (f_neq_vec[ 8][j] = f_vec[8][ j] - (temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (-1, -1, -1)
      
      temp2 = v_x[j] + v_y[j] - v_z[j];		     
      
      f_vec[9][ j] += omega * (f_neq_vec[ 9][j] = f_vec[9][ j] - (temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (+1, +1, -1)
      f_vec[10][j] += omega * (f_neq_vec[10][j] = f_vec[10][j] - (temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (-1, -1, +1)
      
      temp2 = v_x[j] - v_y[j] + v_z[j];	     
      
      f_vec[11][j] += omega * (f_neq_vec[11][j] = f_vec[11][j] - (temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (+1, -1, +1)
      f_vec[12][j] += omega * (f_neq_vec[12][j] = f_vec[12][j] - (temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (-1, +1, -1)
      
      temp2 = v_x[j] - v_y[j] - v_z[j];     
      
      f_vec[13][j] += omega * (f_neq_vec[13][j] = f_vec[13][j] - (temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (+1, -1, -1)
      f_vec[14][j] += omega * (f_neq_vec[14][j] = f_vec[14][j] - (temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2));   // (-1, +1, +1)
    }
  for (j = 0; j < SIMD_SIZE; j++)
    {
      for (l = 0; l < 15; l++)
        {
          f[ j*15+l ] = f_vec[l][j];

          f_neq[ j*15+l ] = f_neq_vec[l][j];
        }
    }
}


void lbmCollision1 (double omega, double f[], int site_id,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[], LBM *lbm, Net *net)
{
  int l;
  
  
  *v_x = *v_y = *v_z = 0.F;
  
  *density = 0.;
  
  for (l = 0; l < 15; l++) *density += f[l];
  
  *density *= (1.0 / 8.0);
  
  for (l = 0; l < 7; l++) f[l] = *density;
  
  *density *= (1.0 / 8.0);
  
  for (l = 7; l < 15; l++) f[l] = *density;
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] = 0.F;
    }
}


void lbmCollisionSIMD1 (double omega, double f[], int site_id,
			double density[], double v_x[], double v_y[], double v_z[],
			double f_neq[], LBM *lbm, Net *net)
{
  int l;
  
  
  for (int j = 0; j < SIMD_SIZE; j++)
    {
      v_x[j] = v_y[j] = v_z[j] = 0.F;
      
      density[j] = 0.;
      
      for (l = 0; l < 15; l++) density[j] += f[ j*15+l ];
      
      density[j] *= (1.0 / 8.0);
      
      for (l = 0; l < 7; l++) f[ j*15+l ] = density[j];
      
      density[j] *= (1.0 / 8.0);
      
      for (l = 7; l < 15; l++) f[ j*15+l ] = density[j];
      
      for (l = 0; l < 15; l++)
	{
	  f_neq[ j*15+l ] = 0.F;
	}
    }
}


void lbmCollision2 (double omega, double f[], int site_id,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[], LBM *lbm, Net *net)
{
  double dummy_density;
  
  unsigned int boundary_id;
  
  
  boundary_id = (net->site_data[ site_id ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
  
  *density = lbm->inlet_density[ boundary_id ];
  
  lbmDensityAndVelocity (f, &dummy_density, v_x, v_y, v_z);
  lbmFeq (*density, *v_x, *v_y, *v_z, f);
  
  for (int l = 0; l < 15; l++)
    {
      f_neq[l] = 0.F;
    }
}


void lbmCollisionSIMD2 (double omega, double f[], int site_id,
			double density[], double v_x[], double v_y[], double v_z[],
			double f_neq[], LBM *lbm, Net *net)
{
  double f_vec[15][SIMD_SIZE];
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  int j, l;
  
  unsigned int boundary_id;
  
  
  for (j = 0; j < SIMD_SIZE; j++)
    {
      for (l = 0; l < 15; l++)
	{
	  f_vec[l][j] = f[ j*15+l ];
	}
    }
  for (j = 0; j < SIMD_SIZE; j++)
    {
      boundary_id = (net->site_data[ site_id+j ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
      
      density[j] = lbm->inlet_density[ boundary_id ];
    }
  for (j = 0; j < SIMD_SIZE; j++)
    {
      v_x[j] = f_vec[1][j] + (f_vec[7][j] + f_vec[9][j]) + (f_vec[11][j] + f_vec[13][j]);
      v_y[j] = f_vec[3][j] + (f_vec[12][j] + f_vec[14][j]);
      v_z[j] = f_vec[5][j] + f_vec[10][j];
      
      v_x[j] -= f_vec[2][j] + f_vec[8][j] + f_vec[10][j] + (f_vec[12][j] + f_vec[14][j]);
      v_y[j] += (f_vec[7][j] + f_vec[9][j]) - (f_vec[4][j] + f_vec[8][j] + f_vec[10][j] + (f_vec[11][j] + f_vec[13][j]));
      v_z[j] += f_vec[7][j] + f_vec[11][j] + f_vec[14][j] - ((f_vec[6][j] + f_vec[8][j]) + f_vec[9][j] + f_vec[12][j] + f_vec[13][j]);
      
      v_xx = v_x[j] * v_x[j];
      v_yy = v_y[j] * v_y[j];
      v_zz = v_z[j] * v_z[j];
      
      temp1 = (1.0 / 8.0) * density[j];
      
      temp2 = v_xx + v_yy + v_zz;
      
      f_vec[0][j] = temp1 - (1.0 / 3.0) * temp2;
      
      temp1 -= (1.0 / 6.0) * temp2;
      
      f_vec[1][j] = temp1 + ((1.0 / 3.0) * v_x[j]) + 0.5 * v_xx;   // (+1, 0, 0)
      f_vec[2][j] = temp1 - ((1.0 / 3.0) * v_x[j]) + 0.5 * v_xx;   // (+1, 0, 0)
      
      f_vec[3][j] = temp1 + ((1.0 / 3.0) * v_y[j]) + 0.5 * v_yy;   // (0, +1, 0)
      f_vec[4][j] = temp1 - ((1.0 / 3.0) * v_y[j]) + 0.5 * v_yy;   // (0, -1, 0)
      
      f_vec[5][j] = temp1 + ((1.0 / 3.0) * v_z[j]) + 0.5 * v_zz;   // (0, 0, +1)
      f_vec[6][j] = temp1 - ((1.0 / 3.0) * v_z[j]) + 0.5 * v_zz;   // (0, 0, -1)
      
      temp1 *= (1.0 / 8.0);
      
      temp2 = v_x[j] + v_y[j] + v_z[j];
      
      f_vec[7][ j] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, +1, +1)
      f_vec[8][ j] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, -1, -1)
      
      temp2 = v_x[j] + v_y[j] - v_z[j];		     
      
      f_vec[9][ j] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, +1, -1)
      f_vec[10][j] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, -1, +1)
      
      temp2 = v_x[j] - v_y[j] + v_z[j];	     
      
      f_vec[11][j] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, -1, +1)
      f_vec[12][j] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, +1, -1)
      
      temp2 = v_x[j] - v_y[j] - v_z[j];     
      
      f_vec[13][j] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, -1, -1)
      f_vec[14][j] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, +1, +1)
    }
  for (j = 0; j < SIMD_SIZE; j++)
    {
      for (l = 0; l < 15; l++)
        {
          f[ j*15+l ] = f_vec[l][j];

          f_neq[ j*15+l ] = 0.;
        }
    }
}


void lbmCollision3 (double omega, double f[], int site_id,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[], LBM *lbm, Net *net)
{
  double dummy_density;
  
  unsigned int boundary_id;
  
  
  boundary_id = (net->site_data[ site_id ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
  
  *density = lbm->outlet_density[ boundary_id ];
  
  lbmDensityAndVelocity (f, &dummy_density, v_x, v_y, v_z);
  lbmFeq (*density, *v_x, *v_y, *v_z, f);
  
  for (int l = 0; l < 15; l++)
    {
      f_neq[l] = 0.F;
    }
}



void lbmCollisionSIMD3 (double omega, double f[], int site_id,
			double density[], double v_x[], double v_y[], double v_z[],
			double f_neq[], LBM *lbm, Net *net)
{
  double f_vec[15][SIMD_SIZE];
  double v_xx, v_yy, v_zz;
  double temp1, temp2;
  
  int j, l;
  
  unsigned int boundary_id;
  
  
  for (j = 0; j < SIMD_SIZE; j++)
    {
      for (l = 0; l < 15; l++)
	{
	  f_vec[l][j] = f[ j*15+l ];
	}
    }
  for (j = 0; j < SIMD_SIZE; j++)
    {
      boundary_id = (net->site_data[ site_id+j ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
      
      density[j] = lbm->outlet_density[ boundary_id ];
    }
  for (j = 0; j < SIMD_SIZE; j++)
    {
      v_x[j] = f_vec[1][j] + (f_vec[7][j] + f_vec[9][j]) + (f_vec[11][j] + f_vec[13][j]);
      v_y[j] = f_vec[3][j] + (f_vec[12][j] + f_vec[14][j]);
      v_z[j] = f_vec[5][j] + f_vec[10][j];
      
      v_x[j] -= f_vec[2][j] + f_vec[8][j] + f_vec[10][j] + (f_vec[12][j] + f_vec[14][j]);
      v_y[j] += (f_vec[7][j] + f_vec[9][j]) - (f_vec[4][j] + f_vec[8][j] + f_vec[10][j] + (f_vec[11][j] + f_vec[13][j]));
      v_z[j] += f_vec[7][j] + f_vec[11][j] + f_vec[14][j] - ((f_vec[6][j] + f_vec[8][j]) + f_vec[9][j] + f_vec[12][j] + f_vec[13][j]);
      
      v_xx = v_x[j] * v_x[j];
      v_yy = v_y[j] * v_y[j];
      v_zz = v_z[j] * v_z[j];
      
      temp1 = (1.0 / 8.0) * density[j];
      
      temp2 = v_xx + v_yy + v_zz;
      
      f_vec[0][j] = temp1 - (1.0 / 3.0) * temp2;
      
      temp1 -= (1.0 / 6.0) * temp2;
      
      f_vec[1][j] = temp1 + ((1.0 / 3.0) * v_x[j]) + 0.5 * v_xx;   // (+1, 0, 0)
      f_vec[2][j] = temp1 - ((1.0 / 3.0) * v_x[j]) + 0.5 * v_xx;   // (+1, 0, 0)
      
      f_vec[3][j] = temp1 + ((1.0 / 3.0) * v_y[j]) + 0.5 * v_yy;   // (0, +1, 0)
      f_vec[4][j] = temp1 - ((1.0 / 3.0) * v_y[j]) + 0.5 * v_yy;   // (0, -1, 0)
      
      f_vec[5][j] = temp1 + ((1.0 / 3.0) * v_z[j]) + 0.5 * v_zz;   // (0, 0, +1)
      f_vec[6][j] = temp1 - ((1.0 / 3.0) * v_z[j]) + 0.5 * v_zz;   // (0, 0, -1)
      
      temp1 *= (1.0 / 8.0);
      
      temp2 = v_x[j] + v_y[j] + v_z[j];
      
      f_vec[7][ j] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, +1, +1)
      f_vec[8][ j] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, -1, -1)
      
      temp2 = v_x[j] + v_y[j] - v_z[j];		     
      
      f_vec[9][ j] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, +1, -1)
      f_vec[10][j] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, -1, +1)
      
      temp2 = v_x[j] - v_y[j] + v_z[j];	     
      
      f_vec[11][j] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, -1, +1)
      f_vec[12][j] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, +1, -1)
      
      temp2 = v_x[j] - v_y[j] - v_z[j];     
      
      f_vec[13][j] = temp1 + ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (+1, -1, -1)
      f_vec[14][j] = temp1 - ((1.0 / 24.0) * temp2) + (1.0 / 16.0) * temp2 * temp2;   // (-1, +1, +1)
    }
  for (j = 0; j < SIMD_SIZE; j++)
    {
      for (l = 0; l < 15; l++)
        {
          f[ j*15+l ] = f_vec[l][j];

          f_neq[ j*15+l ] = 0.;
        }
    }
}


void lbmCollision4 (double omega, double f[], int site_id,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[], LBM *lbm, Net *net)
{
  int l;
  
  unsigned int boundary_id;
  
  
  boundary_id = (net->site_data[ site_id ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
  
  *density = lbm->inlet_density[ boundary_id ];
  
  *v_x = *v_y = *v_z = 0.F;
  
  *density *= (1.0 / 8.0);
  
  for (l = 0; l < 7; l++) f[l] = *density;
  
  *density *= (1.0 / 8.0);
  
  for (l = 7; l < 15; l++) f[l] = *density;
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] = 0.F;
    }
}


void lbmCollisionSIMD4 (double omega, double f[], int site_id,
			double density[], double v_x[], double v_y[], double v_z[],
			double f_neq[], LBM *lbm, Net *net)
{
  int l;
  
  unsigned int boundary_id;
  
  
  for (int j = 0; j < SIMD_SIZE; j++)
    {
      boundary_id = (net->site_data[ site_id+j ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
      
      v_x[j] = v_y[j] = v_z[j] = 0.F;
      
      density[j] = lbm->inlet_density[ boundary_id ];
      
      density[j] *= (1.0 / 8.0);
      
      for (l = 0; l < 7; l++) f[ j*15+l ] = density[j];
      
      density[j] *= (1.0 / 8.0);
      
      for (l = 7; l < 15; l++) f[ j*15+l ] = density[j];
      
      for (l = 0; l < 15; l++)
	{
	  f_neq[ j*15+l ] = 0.F;
	}
    }
}


void lbmCollision5 (double omega, double f[], int site_id,
		    double *density, double *v_x, double *v_y, double *v_z,
		    double f_neq[], LBM *lbm, Net *net)
{
  int l;
  
  unsigned int boundary_id;
  
  
  boundary_id = (net->site_data[ site_id ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
  
  *density = lbm->outlet_density[ boundary_id ];
  
  *v_x = *v_y = *v_z = 0.F;
  
  *density *= (1.0 / 8.0);
  
  for (l = 0; l < 7; l++) f[l] = *density;
  
  *density *= (1.0 / 8.0);
  
  for (l = 7; l < 15; l++) f[l] = *density;
  
  for (l = 0; l < 15; l++)
    {
      f_neq[l] = 0.F;
    }
}


void lbmCollisionSIMD5 (double omega, double f[], int site_id,
			double density[], double v_x[], double v_y[], double v_z[],
			double f_neq[], LBM *lbm, Net *net)
{
  int l;
  
  unsigned int boundary_id;
  
  
  for (int j = 0; j < SIMD_SIZE; j++)
    {
      boundary_id = (net->site_data[ site_id+j ] & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
      
      v_x[j] = v_y[j] = v_z[j] = 0.F;
      
      density[j] = lbm->outlet_density[ boundary_id ];
      
      density[j] *= (1.0 / 8.0);
      
      for (l = 0; l < 7; l++) f[ j*15+l ] = density[j];
      
      density[j] *= (1.0 / 8.0);
      
      for (l = 7; l < 15; l++) f[ j*15+l ] = density[j];
      
      for (l = 0; l < 15; l++)
	{
	  f_neq[ j*15+l ] = 0.F;
	}
    }
}


void lbmVelocity (double f[], double *v_x, double *v_y, double *v_z)
{
  *v_x = f[1] + (f[ 7] + f[  9 ]) + (f[ 11 ] + f[ 13 ]);
  *v_y = f[3] + (f[ 12 ] + f[ 14 ]);
  *v_z = f[5] + f[ 10 ];
  
  *v_x -= (f[2] + f[ 8 ] + f[ 10 ] + (f[ 12 ] + f[ 14 ]));
  *v_y += (f[7] + f[ 9 ]) - ((f[4] + f[ 8 ] + f[ 10 ] + (f[ 11 ] + f[ 13 ])));
  *v_z += f[7] + f[ 11 ] + f[ 14 ] - (((f[6] + f[ 8 ]) + f[ 9 ] + f[ 12 ] + f[ 13 ]));
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


void lbmStressSIMD (double f[], double stress[], LBM *lbm)
{
  double f_vec[15][SIMD_SIZE];
  double sigma_xx_yy, sigma_yy_zz, sigma_xx_zz;
  double sigma_xy, sigma_xz, sigma_yz;
  double a, b;
  
  int j, l;
  
  
  for (j = 0; j < SIMD_SIZE; j++)
    {
      for (l = 0; l < 15; l++)
	{
	  f_vec[l][j] = f[ j*15+l ];
	}
    }
  for (j = 0; j < SIMD_SIZE; j++)
    {
      sigma_xx_yy = (f_vec[1][j]+f_vec[2][j]) - (f_vec[3][j]+f_vec[4][j]);
      sigma_yy_zz = (f_vec[3][j]+f_vec[4][j]) - (f_vec[5][j]+f_vec[6][j]);
      sigma_xx_zz = (f_vec[1][j]+f_vec[2][j]) - (f_vec[5][j]+f_vec[6][j]);
      
      sigma_xy = (f_vec[7][j]+f_vec[8][j]) + (f_vec[9][j]+f_vec[10][j]) - (f_vec[11][j]+f_vec[12][j]) - (f_vec[13][j]+f_vec[14][j]);
      sigma_xz = (f_vec[7][j]+f_vec[8][j]) - (f_vec[9][j]+f_vec[10][j]) + (f_vec[11][j]+f_vec[12][j]) - (f_vec[13][j]+f_vec[14][j]);
      sigma_yz = (f_vec[7][j]+f_vec[8][j]) - (f_vec[9][j]+f_vec[10][j]) - (f_vec[11][j]+f_vec[12][j]) + (f_vec[13][j]+f_vec[14][j]);
      
      a = sigma_xx_yy * sigma_xx_yy + sigma_yy_zz * sigma_yy_zz + sigma_xx_zz * sigma_xx_zz;
      b = sigma_xy * sigma_xy + sigma_xz * sigma_xz + sigma_yz * sigma_yz;
      
      stress[j] = lbm->stress_par * sqrt(a + 6.0 * b);
    }
}


void lbmStress (double f[], double *stress, LBM *lbm)
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
  
  *stress = lbm->stress_par * sqrt(a + 6.0 * b);
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
		     double *vx, double *vy, double *vz, LBM *lbm)
{
  double dummy_density;
  
  int unknowns, i;
  
  unsigned int boundary_type, boundary_config, boundary_id;
  
  
  *vx = *vy = *vz = 0.F;
  
  boundary_type = site_data & SITE_TYPE_MASK;
  
  if (boundary_type == FLUID_TYPE)
    {
      *density = 0.;

      for (i = 0; i < 15; i++) *density += f[ i ];
      
      *density *= (1.0 / 8.0);
      
      for (i = 0; i < 7; i++) f[ i ] = *density;
      
      *density *= (1.0 / 8.0);
      
      for (i = 7; i < 15; i++) f[ i ] = *density;
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
	  *density = lbm->inlet_density[ boundary_id ];
	}
      else
	{
	  *density = lbm->outlet_density[ boundary_id ];
	}
      if (unknowns <= 5)
	{
	  lbmDensityAndVelocity (f, &dummy_density, vx, vy, vz);
	  lbmFeq (*density, *vx, *vy, *vz, f);
	}
      else
	{
	  *density *= (1.0 / 8.0);
	  
	  for (i = 0; i < 7; i++) f[ i ] = *density;
	  
	  *density *= (1.0 / 8.0);
	  
	  for (i = 7; i < 15; i++) f[ i ] = *density;
	}
    }
}


void lbmInit (char *system_file_name, char *checkpoint_file_name,
	      LBM *lbm, Net *net)
{
  // basically, this function calls other ones only
  
  lbm->system_file_name     = system_file_name;
  lbm->checkpoint_file_name = checkpoint_file_name;
  
  lbmReadConfig (lbm, net);
  
  lbmCollision[0] = lbmCollision0;
  lbmCollision[1] = lbmCollision1;
  lbmCollision[2] = lbmCollision2;
  lbmCollision[3] = lbmCollision3;
  lbmCollision[4] = lbmCollision4;
  lbmCollision[5] = lbmCollision5;

  lbmCollisionSIMD[0] = lbmCollisionSIMD0;
  lbmCollisionSIMD[1] = lbmCollisionSIMD1;
  lbmCollisionSIMD[2] = lbmCollisionSIMD2;
  lbmCollisionSIMD[3] = lbmCollisionSIMD3;
  lbmCollisionSIMD[4] = lbmCollisionSIMD4;
  lbmCollisionSIMD[5] = lbmCollisionSIMD5;
}


void lbmSetOptimizedInitialConditions (LBM *lbm, Net *net)
{
  double *d_p;
  double **nd_p_p;
  double density;
  double *f_old_p, *f_new_p, f_eq[15];
  double error, error_tot;
  double temp;
  
  int iters;
  int neighs;
  int i;
  int l, m, n;
  int my_sites;
  
  unsigned int site_data, boundary_id;
  
  NeighProc *neigh_proc_p;
  
  
  my_sites = net->my_inner_sites + net->my_inter_sites;
  
  for (i = 0; i < my_sites; i++)
    {
      site_data = net->site_data[ i ];
      
      if ((site_data & SITE_TYPE_MASK) == INLET_TYPE)
	{
	  boundary_id = (site_data & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
	  
	  d[ i ] = lbm->inlet_density[ boundary_id ];
	}
      else if ((site_data & SITE_TYPE_MASK) == OUTLET_TYPE)
	{
	  boundary_id = (site_data & BOUNDARY_ID_MASK) >> BOUNDARY_ID_SHIFT;
	  
	  d[ i ] = lbm->outlet_density[ boundary_id ];
	}
    }
  d[ my_sites ] = -1.;
  
  error_tot = 1.e+30;
  iters = 0;
  
  while (error_tot > 1.e-3)
    {
      ++iters;
      
      error = 0.;
      
      for (m = 0; m < net->neigh_procs; m++)
	{
	  neigh_proc_p = &net->neigh_proc[ m ];
	  
	  for (n = 0; n < neigh_proc_p->fs; n++)
	    {
	      neigh_proc_p->f_to_send[ n ] = *neigh_proc_p->d_to_send_p[ n ];
	    }
#ifndef NOMPI
	  net->err = MPI_Isend (&neigh_proc_p->f_to_send[ 0 ],
				neigh_proc_p->fs, MPI_DOUBLE,
				neigh_proc_p->id, 10, MPI_COMM_WORLD,
				&net->req[ 0 ][ net->id * net->procs + m ]);
	  
	  net->err = MPI_Irecv (&neigh_proc_p->f_to_recv[ 0 ],
				neigh_proc_p->fs, MPI_DOUBLE,
				neigh_proc_p->id, 10, MPI_COMM_WORLD,
				&net->req[ 0 ][ (net->id + net->procs) * net->procs + m ]);
#endif
	}
      for (i = 0; i < net->my_inner_sites; i++)
	{
	  site_data = net->site_data[ i ];
	  
	  if ((site_data & SITE_TYPE_MASK) == INLET_TYPE ||
	      (site_data & SITE_TYPE_MASK) == OUTLET_TYPE)
	    {
	      continue;
	    }
	  d_p = &d[ i ];
	  
	  temp = *d_p;
	  *d_p = 0.;
	  neighs = 0;
	  
	  nd_p_p = &nd_p[ i*14 ];
	  
	  for (l = 0; l < 14; l++)
	    {
	      if (*nd_p_p[ l ] < 0.) continue;
	      
	      ++neighs;
	      *d_p += *nd_p_p[ l ];
	    }
	  *d_p /= (double)neighs;
	  
	  error = fmax(error, fabs(*d_p - temp) / fmax(1.e-30, *d_p));
	}
      for (m = 0; m < net->neigh_procs; m++)
      	{
#ifndef NOMPI
      	  net->err = MPI_Wait (&net->req[ 0 ][ net->id * net->procs + m ], net->status);
      	  net->err = MPI_Wait (&net->req[ 0 ][ (net->id + net->procs) * net->procs + m ], net->status);
#endif
      	}
      for (i = net->my_inner_sites; i < my_sites; i++)
	{
	  site_data = net->site_data[ i ];
	  
	  if ((site_data & SITE_TYPE_MASK) == INLET_TYPE ||
	      (site_data & SITE_TYPE_MASK) == OUTLET_TYPE)
	    {
	      continue;
	    }
	  d_p = &d[ i ];
	  
	  temp = *d_p;
	  *d_p = 0.;
	  neighs = 0;
	  
	  nd_p_p = &nd_p[ i*14 ];
	  
	  for (l = 0; l < 14; l++)
	    {
	      if (*nd_p_p[ l ] < 0.) continue;
	      
	      ++neighs;
	      *d_p += *nd_p_p[ l ];
	    }
	  *d_p /= (double)neighs;
	  
	  error = fmax(error, fabs(*d_p - temp) / fmax(1.e-30, *d_p));
	}
#ifndef NOMPI
      net->err = MPI_Allreduce (&error, &error_tot, 1,
				MPI_DOUBLE_PRECISION, MPI_MAX, MPI_COMM_WORLD);
#else
      error_tot = error;
#endif
    }
  for (i = 0; i < my_sites; i++)
    {
      density = d[ i ];
      
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
      flow_field[ 3*i+0 ] = (float)density;
      flow_field[ 3*i+1 ] = 0.F;
      flow_field[ 3*i+2 ] = 0.F;
    }
  
  for (n = 0; n < net->neigh_procs; n++)
    {
      free(net->neigh_proc[ n ].d_to_send_p);
    }
  free (nd_p);
  nd_p = NULL;
  
  free (d);
  d = NULL;
}

/*
// old version

int lbmCycle (int write_checkpoint, int check_conv, int perform_rt, int *is_converged, LBM *lbm, Net *net)
{
  // the entire simulation time step takes place through this function
  
  double f_eq[15], f_neq[15];
  double omega;
  double density, stress;
  double vx, vy, vz;
  double *f_old_p;

#ifndef BENCH
  double sum1, sum2;
  double stability_and_conv_partial[3];
  double stability_and_conv_total[3];
#endif // BENCH
  
  int unit_level;
  int i, l, m, n;
  int is_unstable;
  int *f_id_p;
  
  unsigned int site_data;
  
#ifndef BENCH
  double *vel_p;
#endif // BENCH
  
  NeighProc *neigh_proc_p;
  
  
  is_unstable = 0;
  
  *is_converged = 0;
  
#ifndef BENCH
  sum1 = 0.0;
  sum2 = 0.0;
#endif // BENCH
  
  omega = lbm->omega;
  
  for (i = net->my_inner_sites;
       i < net->my_inner_sites + net->my_inter_sites; i++)
    {
      site_data = net->site_data[ i ];
      f_old_p = &f_old[ i*15 ];
      
      stress = 0.;
      
      if (site_data == FLUID_TYPE)
	{
	  lbmFeq (f_old_p, &density, &vx, &vy, &vz, f_eq);
	  
	  for (l = 0; l < 15; l++)
	    {
	      f_old_p[l] += omega * (f_neq[l] = f_old_p[l] - f_eq[l]);
	    }
	  if (perform_rt)
	    {
	      lbmStress (f_neq, &stress, lbm);
	    }
	}
      else
	{
	  lbmCalculateBC (f_old_p, site_data, &density, &vx, &vy, &vz, lbm);
	}
#ifndef BENCH
      vel_p = &vel[ 3*i ];
      
      sum1 += fabs(vel_p[0] - vx) + fabs(vel_p[1] - vy) + fabs(vel_p[2] - vz);
      sum2 += fabs(vx) + fabs(vy) + fabs(vz);
      
      vel_p[0] = vx;
      vel_p[1] = vy;
      vel_p[2] = vz;
#endif // BENCH
      
      if (perform_rt)
	{
	  flow_field[ 3 * i + 0 ] = (float)density;
	  flow_field[ 3 * i + 1 ] = (float)sqrt(vx * vx + vy * vy + vz * vz);
	  flow_field[ 3 * i + 2 ] = (float)stress;
	}
    }
  
  for (unit_level = 1; unit_level >= 0; unit_level--)
    {
      for (m = 0; m < net->neigh_procs; m++)
	{
	  neigh_proc_p = &net->neigh_proc[ m ];
	  
	  if ((unit_level == 1 && net->machine_id[ neigh_proc_p->id ] == net->machine_id[ net->id ]) ||
	      (unit_level == 0 && net->machine_id[ neigh_proc_p->id ] != net->machine_id[ net->id ]))
	    {
	      continue;
	    }
	  for (n = 0; n < neigh_proc_p->fs; n++)
	    {
	      neigh_proc_p->f_to_send[ n ] = f_old[ neigh_proc_p->f_send_id[n] ];
	    }
#ifndef NOMPI
	  net->err = MPI_Isend (&neigh_proc_p->f_to_send[ 0 ],
				neigh_proc_p->fs, MPI_DOUBLE,
				neigh_proc_p->id, 10, MPI_COMM_WORLD,
				&net->req[ 0 ][ net->id * net->procs + m ]);
	  
	  net->err = MPI_Irecv (&neigh_proc_p->f_to_recv[ 0 ],
				neigh_proc_p->fs, MPI_DOUBLE,
				neigh_proc_p->id, 10, MPI_COMM_WORLD,
				&net->req[ 0 ][ (net->id + net->procs) * net->procs + m ]);
#endif
	}
    }
  
  for (i = 0; i < net->my_inner_sites; i++)
    {
      site_data = net->site_data[ i ];
      f_old_p = &f_old[ i*15 ];
      
      stress = 0.;
      
      if (site_data == FLUID_TYPE)
	{
	  lbmFeq (f_old_p, &density, &vx, &vy, &vz, f_eq);
	  
	  for (l = 0; l < 15; l++)
	    {
	      f_old_p[l] += omega * (f_neq[ l ] = f_old_p[l] - f_eq[l]);
	    }
	  if (perform_rt)
	    {
	      lbmStress (f_neq, &stress, lbm);
	    }
	}
      else
	{
	  lbmCalculateBC (f_old_p, site_data, &density, &vx, &vy, &vz, lbm);
	}
      f_id_p = &f_id[ i*15 ];
      
      for (l = 0; l < 15; l++)
	{
#ifndef BENCH
	  if (f_old_p[l] < 0.) is_unstable = 1;
#endif // BENCH
	  
	  f_new[ f_id_p[l] ] = f_old_p[l];
	}
#ifndef BENCH
      vel_p = &vel[ 3*i ];
      
      sum1 += fabs(vel_p[0] - vx) + fabs(vel_p[1] - vy) + fabs(vel_p[2] - vz);
      sum2 += fabs(vx) + fabs(vy) + fabs(vz);
      
      vel_p[0] = vx;
      vel_p[1] = vy;
      vel_p[2] = vz;
#endif // BENCH
      
      if (perform_rt)
	{
	  flow_field[ 3 * i + 0 ] = (float)density;
	  flow_field[ 3 * i + 1 ] = (float)sqrt(vx * vx + vy * vy + vz * vz);
	  flow_field[ 3 * i + 2 ] = (float)stress;
	}
    }
  
  for (unit_level = 1; unit_level >= 0; unit_level--)
    {
      for (m = 0; m < net->neigh_procs; m++)
	{
	  neigh_proc_p = &net->neigh_proc[ m ];
	  
	  if ((unit_level == 1 && net->machine_id[ neigh_proc_p->id ] == net->machine_id[ net->id ]) ||
	      (unit_level == 0 && net->machine_id[ neigh_proc_p->id ] != net->machine_id[ net->id ]))
	    {
	      continue;
	    }
#ifndef NOMPI
	  net->err = MPI_Wait (&net->req[ 0 ][ net->id * net->procs + m ], net->status);
	  net->err = MPI_Wait (&net->req[ 0 ][ (net->id + net->procs) * net->procs + m ], net->status);
#endif
	  for (n = 0; n < neigh_proc_p->fs; n++)
	    {
	      f_new[ neigh_proc_p->f_recv_iv[n] ] = neigh_proc_p->f_to_recv[ n ];
	    }
	}
    }
  
  for (i = net->my_inner_sites;
       i < net->my_inner_sites + net->my_inter_sites; i++)
    {
      f_old_p = &f_old[ i*15 ];
      f_id_p = &f_id[ i*15 ];
      
      for (l = 0; l < 15; l++)
	{
#ifndef BENCH
	  if (f_old_p[l] < 0.) is_unstable = 1;
#endif // BENCH
	  
	  f_new[ f_id_p[l] ] = f_old_p[l];
	}
    }
  f_old_p = f_old;
  f_old = f_new;
  f_new = f_old_p;
  
#ifndef BENCH
  if (check_conv)
    {
      if (net->procs > 1)
	{
	  stability_and_conv_partial[ 0 ] = (double)is_unstable;
	  stability_and_conv_partial[ 1 ] = sum1;
	  stability_and_conv_partial[ 2 ] = sum2;
#ifndef NOMPI
	  net->err = MPI_Allreduce (stability_and_conv_partial,
				    stability_and_conv_total, 3,
				    MPI_DOUBLE_PRECISION, MPI_SUM, MPI_COMM_WORLD);
#else
	  stability_and_conv_total[ 0 ] = stability_and_conv_partial[ 0 ];
	  stability_and_conv_total[ 1 ] = stability_and_conv_partial[ 1 ];
	  stability_and_conv_total[ 2 ] = stability_and_conv_partial[ 2 ];
#endif
	  sum1 = stability_and_conv_total[ 1 ];
	  sum2 = stability_and_conv_total[ 2 ];
	  
	  is_unstable = (stability_and_conv_total[ 0 ] >= 1.);
	}
      
      if (sum1 <= sum2 * lbm->tolerance && sum2 > lbm->tolerance)
	{
	  *is_converged = 1;
	}
      lbm->conv_error = sum1 / sum2;
    }
  if (write_checkpoint)
    {
      lbmWriteConfig (!is_unstable, lbm->checkpoint_file_name, 1, lbm, net);
    }
#endif // BENCH
  
  if (is_unstable)
    {
      return UNSTABLE;
    }
  else
    {
      return STABLE;
    }
}
*/

int lbmCycle (int write_checkpoint, int check_conv, int perform_rt, int *is_converged, LBM *lbm, Net *net)
{
  // the entire simulation time step takes place through this function
  
  double f_neq[SIMD_SIZE*15];
  double omega;
  double density[SIMD_SIZE];
  double vx[SIMD_SIZE], vy[SIMD_SIZE], vz[SIMD_SIZE];
  double *f_old_p;
  
#ifndef BENCH
  double vel_x[SIMD_SIZE], vel_y[SIMD_SIZE], vel_z[SIMD_SIZE];
  double stress[SIMD_SIZE];
  double sum1, sum2;
  double stability_and_conv_partial[3];
  double stability_and_conv_total[3];
#endif // BENCH
  
  int collision_type;
  int offset;
  int unit_level;
  int i, j, l, m, n;
  int is_unstable;
  
  NeighProc *neigh_proc_p;
  
  
  is_unstable = 0;
  
  *is_converged = 0;
  
#ifndef BENCH
  sum1 = 0.;
  sum2 = 0.;
#endif // BENCH
  
  omega = lbm->omega;
  
  offset = net->my_inner_sites;

  if (perform_rt)
    {
      for (collision_type = 0; collision_type < COLLISION_TYPES; collision_type++)
	{
	  for (i = offset;
	       i < offset + net->my_inter_collisions_sse[ collision_type ]; i+=SIMD_SIZE)
	    {
	      (*lbmCollisionSIMD[ collision_type ]) (omega, &f_old[ i*15 ], i, density, vx, vy, vz, f_neq, lbm, net);
#ifndef BENCH
	      lbmStressSIMD (f_neq, stress, lbm);
	      
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  vel_x[j] = vel[ 3*(i+j)+0 ];
		  vel_y[j] = vel[ 3*(i+j)+1 ];
		  vel_z[j] = vel[ 3*(i+j)+2 ];
		}
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  sum1 += fabs(vel_x[j]-vx[j]) + fabs(vel_y[j]-vy[j]) + fabs(vel_z[j]-vz[j]);
		  sum2 += fabs(vx[j]) + fabs(vy[j]) + fabs(vz[j]);
		}
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  vel[ 3*(i+j)+0 ] = vx[j];
		  vel[ 3*(i+j)+1 ] = vy[j];
		  vel[ 3*(i+j)+2 ] = vz[j];
		  
		  flow_field[ 3*(i+j)+0 ] = density[j];
		  flow_field[ 3*(i+j)+1 ] = sqrt(vx[j] * vx[j] + vy[j] * vy[j] + vz[j] * vz[j]);
		  flow_field[ 3*(i+j)+2 ] = stress[j];
		}
#endif // BENCH
	    }
	  for (i = offset + net->my_inter_collisions_sse[ collision_type ];
	       i < offset + net->my_inter_collisions[ collision_type ]; i++)
	    {
	      (*lbmCollision[ collision_type ]) (omega, &f_old[ i*15 ], i, density, vx, vy, vz, f_neq, lbm, net);
#ifndef BENCH
	      lbmStress (f_neq, &stress[0], lbm);
	      
	      sum1 += fabs(vel[ 3*i+0 ]-vx[0]) + fabs(vel[ 3*i+1 ]-vy[0]) + fabs(vel[ 3*i+2 ]-vz[0]);
	      sum2 += fabs(vx[0]) + fabs(vy[0]) + fabs(vz[0]);
	      
	      vel[ 3*i+0 ] = vx[0];
	      vel[ 3*i+1 ] = vy[0];
	      vel[ 3*i+2 ] = vz[0];
	      
	      flow_field[ 3*i+0 ] = density[0];
	      flow_field[ 3*i+1 ] = sqrt(vx[0] * vx[0] + vy[0] * vy[0] + vz[0] * vz[0]);
	      flow_field[ 3*i+2 ] = stress[0];
#endif // BENCH
	    }
	  offset += net->my_inter_collisions[ collision_type ];
	}
    }
  else
    {
      for (collision_type = 0; collision_type < COLLISION_TYPES; collision_type++)
	{
	  for (i = offset;
	       i < offset + net->my_inter_collisions_sse[ collision_type ]; i+=SIMD_SIZE)
	    {
	      (*lbmCollisionSIMD[ collision_type ]) (omega, &f_old[ i*15 ], i, density, vx, vy, vz, f_neq, lbm, net);
#ifndef BENCH
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  vel_x[j] = vel[ 3*(i+j)+0 ];
		  vel_y[j] = vel[ 3*(i+j)+1 ];
		  vel_z[j] = vel[ 3*(i+j)+2 ];
		}
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  sum1 += fabs(vel_x[j]-vx[j]) + fabs(vel_y[j]-vy[j]) + fabs(vel_z[j]-vz[j]);
		  sum2 += fabs(vx[j]) + fabs(vy[j]) + fabs(vz[j]);
		}
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  vel[ 3*(i+j)+0 ] = vx[j];
		  vel[ 3*(i+j)+1 ] = vy[j];
		  vel[ 3*(i+j)+2 ] = vz[j];
		}
#endif // BENCH
	    }
	  for (i = offset + net->my_inter_collisions_sse[ collision_type ];
	       i < offset + net->my_inter_collisions[ collision_type ]; i++)
	    {
	      (*lbmCollision[ collision_type ]) (omega, &f_old[ i*15 ], i, density, vx, vy, vz, f_neq, lbm, net);
#ifndef BENCH
	      sum1 += fabs(vel[ 3*i+0 ]-vx[0]) + fabs(vel[ 3*i+1 ]-vy[0]) + fabs(vel[ 3*i+2 ]-vz[0]);
	      sum2 += fabs(vx[0]) + fabs(vy[0]) + fabs(vz[0]);
	      
	      vel[ 3*i+0 ] = vx[0];
	      vel[ 3*i+1 ] = vy[0];
	      vel[ 3*i+2 ] = vz[0];
#endif // BENCH
	    }
	  offset += net->my_inter_collisions[ collision_type ];
	}
    }
  
  for (unit_level = 1; unit_level >= 0; unit_level--)
    {
      for (m = 0; m < net->neigh_procs; m++)
	{
	  neigh_proc_p = &net->neigh_proc[ m ];
	  
	  if ((unit_level == 1 && net->machine_id[ neigh_proc_p->id ] == net->machine_id[ net->id ]) ||
	      (unit_level == 0 && net->machine_id[ neigh_proc_p->id ] != net->machine_id[ net->id ]))
	    {
	      continue;
	    }
	  for (n = 0; n < neigh_proc_p->fs; n++)
	    {
	      neigh_proc_p->f_to_send[ n ] = f_old[ neigh_proc_p->f_send_id[n] ];
	    }
#ifndef NOMPI
	  net->err = MPI_Isend (&neigh_proc_p->f_to_send[ 0 ],
				neigh_proc_p->fs, MPI_DOUBLE,
				neigh_proc_p->id, 10, MPI_COMM_WORLD,
				&net->req[ 0 ][ net->id * net->procs + m ]);
	  
	  net->err = MPI_Irecv (&neigh_proc_p->f_to_recv[ 0 ],
				neigh_proc_p->fs, MPI_DOUBLE,
				neigh_proc_p->id, 10, MPI_COMM_WORLD,
				&net->req[ 0 ][ (net->id + net->procs) * net->procs + m ]);
#endif
	}
    }
  
  offset = 0;
  
  if (perform_rt)
    {
      for (collision_type = 0; collision_type < COLLISION_TYPES; collision_type++)
	{
	  for (i = offset;
	       i < offset + net->my_inner_collisions_sse[ collision_type ]; i+=SIMD_SIZE)
	    {
	      (*lbmCollisionSIMD[ collision_type ]) (omega, &f_old[ i*15 ], i, density, vx, vy, vz, f_neq, lbm, net);
#ifndef BENCH
	      lbmStressSIMD (f_neq, stress, lbm);
#endif // BENCH
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  for (l = 0; l < 15; l++)
		    {
#ifndef BENCH
		      if (f_old[ (i+j)*15+l ] < 0.) is_unstable = 1;
#endif // BENCH
		      f_new[ f_id[(i+j)*15+l] ] = f_old[ (i+j)*15+l ];
		    }
		}
#ifndef BENCH
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  vel_x[j] = vel[ 3*(i+j)+0 ];
		  vel_y[j] = vel[ 3*(i+j)+1 ];
		  vel_z[j] = vel[ 3*(i+j)+2 ];
		}
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  sum1 += fabs(vel_x[j]-vx[j]) + fabs(vel_y[j]-vy[j]) + fabs(vel_z[j]-vz[j]);
		  sum2 += fabs(vx[j]) + fabs(vy[j]) + fabs(vz[j]);
		}
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  vel[ 3*(i+j)+0 ] = vx[j];
		  vel[ 3*(i+j)+1 ] = vy[j];
		  vel[ 3*(i+j)+2 ] = vz[j];
		  
		  flow_field[ 3*(i+j)+0 ] = density[j];
		  flow_field[ 3*(i+j)+1 ] = sqrt(vx[j] * vx[j] + vy[j] * vy[j] + vz[j] * vz[j]);
		  flow_field[ 3*(i+j)+2 ] = stress[j];
		}
#endif // BENCH
	    }
	  for (i = offset + net->my_inner_collisions_sse[ collision_type ];
	       i < offset + net->my_inner_collisions[ collision_type ]; i++)
	    {
	      (*lbmCollision[ collision_type ]) (omega, &f_old[ i*15 ], i, density, vx, vy, vz, f_neq, lbm, net);
#ifndef BENCH
	      lbmStress (f_neq, stress, lbm);
#endif // BENCH
	      for (l = 0; l < 15; l++)
		{
#ifndef BENCH
		  if (f_old[ i*15+l ] < 0.) is_unstable = 1;
#endif // BENCH
		  f_new[ f_id[i*15+l] ] = f_old[ i*15+l ];
		}
#ifndef BENCH
	      sum1 += fabs(vel[ 3*i+0 ]-vx[0]) + fabs(vel[ 3*i+1 ]-vy[0]) + fabs(vel[ 3*i+2 ]-vz[0]);
	      sum2 += fabs(vx[0]) + fabs(vy[0]) + fabs(vz[0]);
	      
	      vel[ 3*i+0 ] = vx[0];
	      vel[ 3*i+1 ] = vy[0];
	      vel[ 3*i+2 ] = vz[0];
	      
	      flow_field[ 3*i+0 ] = density[0];
	      flow_field[ 3*i+1 ] = sqrt(vx[0] * vx[0] + vy[0] * vy[0] + vz[0] * vz[0]);
	      flow_field[ 3*i+2 ] = stress[0];
#endif // BENCH
	    }
	  offset += net->my_inner_collisions[ collision_type ];
	}
    }
  else
    {
      for (collision_type = 0; collision_type < COLLISION_TYPES; collision_type++)
	{
	  for (i = offset;
	       i < offset + net->my_inner_collisions_sse[ collision_type ]; i+=SIMD_SIZE)
	    {
	      (*lbmCollisionSIMD[ collision_type ]) (omega, &f_old[ i*15 ], i, density, vx, vy, vz, f_neq, lbm, net);
	      
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  for (l = 0; l < 15; l++)
		    {
#ifndef BENCH
		      if (f_old[ (i+j)*15+l ] < 0.) is_unstable = 1;
#endif // BENCH
		      f_new[ f_id[(i+j)*15+l] ] = f_old[ (i+j)*15+l ];
		    }
		}
#ifndef BENCH
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  vel_x[j] = vel[ 3*(i+j)+0 ];
		  vel_y[j] = vel[ 3*(i+j)+1 ];
		  vel_z[j] = vel[ 3*(i+j)+2 ];
		}
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  sum1 += fabs(vel_x[j]-vx[j]) + fabs(vel_y[j]-vy[j]) + fabs(vel_z[j]-vz[j]);
		  sum2 += fabs(vx[j]) + fabs(vy[j]) + fabs(vz[j]);
		}
	      for (j = 0; j < SIMD_SIZE; j++)
		{
		  vel[ 3*(i+j)+0 ] = vx[j];
		  vel[ 3*(i+j)+1 ] = vy[j];
		  vel[ 3*(i+j)+2 ] = vz[j];
		}
#endif // BENCH
	    }
	  for (i = offset + net->my_inner_collisions_sse[ collision_type ];
	       i < offset + net->my_inner_collisions[ collision_type ]; i++)
	    {
	      (*lbmCollision[ collision_type ]) (omega, &f_old[ i*15 ], i, density, vx, vy, vz, f_neq, lbm, net);
	      
	      for (l = 0; l < 15; l++)
		{
#ifndef BENCH
		  if (f_old[ i*15+l ] < 0.) is_unstable = 1;
#endif // BENCH
		  f_new[ f_id[i*15+l] ] = f_old[ i*15+l ];
		}
#ifndef BENCH
	      sum1 += fabs(vel[ 3*i+0 ]-vx[0]) + fabs(vel[ 3*i+1 ]-vy[0]) + fabs(vel[ 3*i+2 ]-vz[0]);
	      sum2 += fabs(vx[0]) + fabs(vy[0]) + fabs(vz[0]);
	      
	      vel[ 3*i+0 ] = vx[0];
	      vel[ 3*i+1 ] = vy[0];
	      vel[ 3*i+2 ] = vz[0];
#endif // BENCH
	    }
	  offset += net->my_inner_collisions[ collision_type ];
	}
    }
  
  for (unit_level = 1; unit_level >= 0; unit_level--)
    {
      for (m = 0; m < net->neigh_procs; m++)
	{
	  neigh_proc_p = &net->neigh_proc[ m ];
	  
	  if ((unit_level == 1 && net->machine_id[ neigh_proc_p->id ] == net->machine_id[ net->id ]) ||
	      (unit_level == 0 && net->machine_id[ neigh_proc_p->id ] != net->machine_id[ net->id ]))
	    {
	      continue;
	    }
#ifndef NOMPI
	  net->err = MPI_Wait (&net->req[ 0 ][ net->id * net->procs + m ], net->status);
	  net->err = MPI_Wait (&net->req[ 0 ][ (net->id + net->procs) * net->procs + m ], net->status);
#endif
	  for (n = 0; n < neigh_proc_p->fs; n++)
	    {
	      f_new[ neigh_proc_p->f_recv_iv[n] ] = neigh_proc_p->f_to_recv[ n ];
	    }
	}
    }
  
  for (collision_type = 0; collision_type < COLLISION_TYPES; collision_type++)
    {
      for (i = offset;
	   i < offset + net->my_inter_collisions[ collision_type ]; i++)
	{
	  for (l = 0; l < 15; l++)
	    {
#ifndef BENCH
	      if (f_old[ i*15+l ] < 0.) is_unstable = 1;
#endif // BENCH
	      f_new[ f_id[i*15+l] ] = f_old[ i*15+l ];
	    }
	}
      offset += net->my_inter_collisions[ collision_type ];
    }
  f_old_p = f_old;
  f_old = f_new;
  f_new = f_old_p;
  
#ifndef BENCH
  if (check_conv)
    {
      if (net->procs > 1)
	{
	  stability_and_conv_partial[ 0 ] = (double)is_unstable;
	  stability_and_conv_partial[ 1 ] = sum1;
	  stability_and_conv_partial[ 2 ] = sum2;
#ifndef NOMPI
	  net->err = MPI_Allreduce (stability_and_conv_partial,
				    stability_and_conv_total, 3,
				    MPI_DOUBLE_PRECISION, MPI_SUM, MPI_COMM_WORLD);
#else
	  stability_and_conv_total[ 0 ] = stability_and_conv_partial[ 0 ];
	  stability_and_conv_total[ 1 ] = stability_and_conv_partial[ 1 ];
	  stability_and_conv_total[ 2 ] = stability_and_conv_partial[ 2 ];
#endif
	  sum1 = stability_and_conv_total[ 1 ];
	  sum2 = stability_and_conv_total[ 2 ];
	  
	  is_unstable = (stability_and_conv_total[ 0 ] >= 1.);
	}
      
      if (sum1 <= sum2 * lbm->tolerance && sum2 > lbm->tolerance)
	{
	  *is_converged = 1;
	}
      lbm->conv_error = sum1 / sum2;
    }
  if (write_checkpoint)
    {
      lbmWriteConfig (!is_unstable, lbm->checkpoint_file_name, 1, lbm, net);
    }
#endif // BENCH
  
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
  free(lbm->outlet_density);
  lbm->outlet_density = NULL;
  
  free(lbm->inlet_density);
  lbm->inlet_density = NULL;
  
  free(lbm->fluid_sites_per_block);
  lbm->fluid_sites_per_block = NULL;
}


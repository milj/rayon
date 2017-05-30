
#include "headers.h"


double *makeVector (double x, double y, double z)
{
   double *tmp;
   tmp = (double *) fmalloc (3*sizeof (double));
   tmp[0] = x;
   tmp[1] = y;
   tmp[2] = z;
   return tmp;
}

void setVector (double x, double y, double z, double *v)
{
   v[0] = x;
   v[1] = y;
   v[2] = z;
}

void copyVector (double *u, double *v)
{
   v[0] = u[0];
   v[1] = u[1];
   v[2] = u[2];
}

void getVectorLength (double *v, double *result)
{
   (*result) = sqrt (SQR(v[0]) + SQR(v[1]) + SQR(v[2]));
}

void normalizeVector (double *v, double *result)
{
   double l;

   getVectorLength (v, &l);
   if (l == 0.0)
   {
      result[0] = 0.0;
      result[1] = 0.0;
      result[2] = 0.0;
   }
   else
   {
      result[0] = v[0] / l;
      result[1] = v[1] / l;
      result[2] = v[2] / l;
   }
}

void addVectors (double *u, double *v, double *result)
{
   result[0] = u[0] + v[0];
   result[1] = u[1] + v[1];
   result[2] = u[2] + v[2];
}

void subVectors (double *u, double *v, double *result)
{
   result[0] = u[0] - v[0];
   result[1] = u[1] - v[1];
   result[2] = u[2] - v[2];
}

void multVector (double *v, double r, double *result)
{
   result[0] = r*v[0];
   result[1] = r*v[1];
   result[2] = r*v[2];
}

void crossProduct (double *u, double *v, double *result)
{
   result[0] = u[1]*v[2] - u[2]*v[1];
   result[1] = u[2]*v[0] - u[0]*v[2];
   result[2] = u[0]*v[1] - u[1]*v[0];
}

void dotProduct (double *v, double *u, double *result)
{
   (*result) = v[0]*u[0]+v[1]*u[1]+v[2]*u[2];
}

void cosAngle (double *v, double *u, double *result)
{
   double lv, lu;
   double dp;

   getVectorLength (v, &lv);
   getVectorLength (u, &lu);
   dotProduct (v, u, &dp);
   (*result) = dp / (lv * lu);
}

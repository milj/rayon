
#include "headers.h"


extern scene scena;


void normalizeColour (float *a, float *b)
{
   float max = 0.0;
   int i;

   for (i = 0; i < 3; i++)
   {
      if (a[i] > max)
      {
         max = a[i];
      }
   }

   multColour (a, 1.0 / max, b);
}


void setColour (float r, float g, float b, float *c)
{
   c[0] = r;
   c[1] = g;
   c[2] = b;
}


void copyColour (float *u, float *v)
{
   v[0] = u[0];
   v[1] = u[1];
   v[2] = u[2];
}


void addColours (float *c1, float *c2, float *result)
{
   result[0] = c1[0]+c2[0];
   result[1] = c1[1]+c2[1];
   result[2] = c1[2]+c2[2];
}


void multColours (float *c1, float *c2, float *result)
{
   result[0] = c1[0]*c2[0];
   result[1] = c1[1]*c2[1];
   result[2] = c1[2]*c2[2];
}


void multColour (float *c, float r, float *result)
{
   result[0] = r*c[0];
   result[1] = r*c[1];
   result[2] = r*c[2];
}


void recalculateLights (float *ambient)
{
   lightList *k;
   float ed_max = 0.0;
   float sum_weight = 0.0;
   float tm1[3];

   //normalizacja
   for (k = scena.lights.next; k != NULL; k = k->next)
   {
      if (k->lgh->m->cm.ed > ed_max)
      {
         ed_max = k->lgh->m->cm.ed;
      }
   }

   for (k = scena.lights.next; k != NULL; k = k->next)
   {
      k->lgh->m->ed_norm = k->lgh->m->cm.ed / ed_max;
   }

   //ambient
   setColour (0.0, 0.0, 0.0, ambient);
   for (k = scena.lights.next; k != NULL; k = k->next)
   {
      multColour (k->lgh->m->rgb_ed_c, k->lgh->m->ed_norm, tm1);
      addColours (ambient, tm1, ambient);
      sum_weight += k->lgh->m->ed_norm;
   }
   multColour (ambient, AMBIENT / sum_weight, ambient);
}

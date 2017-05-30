
#include "headers.h"


extern camera_type camera;
extern scene scena;
extern mode_type mode;
extern int screenX;
extern int screenY;
extern int raytracingDepth;
extern int pathsPerPixel;
extern int lightSamples;
extern float ambient[3];
extern GLubyte *pictureBufferByte;
extern float *pictureBufferFloat;

float sumIllum;


void calculateTrianglesData ()
{
   triangleList *l;
   double tm1[3], tm2[3], tm3[3];
   double a, b, c, p;
   sumIllum = 0.0;

   for (l = scena.triangles.next; l != NULL; l = l->next)
   {
      subVectors (l->trg->vs[1], l->trg->vs[0], tm1);
      subVectors (l->trg->vs[2], l->trg->vs[0], tm2);
      crossProduct (tm1, tm2, l->trg->n);
      normalizeVector (l->trg->n, l->trg->n);

      if (l->trg->m->cm.ed > LIGHT_THR)
      {
         subVectors (l->trg->vs[2], l->trg->vs[1], tm3);
         getVectorLength (tm1, &a);
         getVectorLength (tm2, &b);
         getVectorLength (tm3, &c);
         p = (a + b + c) / 2.0;
         l->trg->area = sqrt (p * (p-a) * (p-b) * (p-c));
         sumIllum += l->trg->area * l->trg->m->cm.ed;
      }
   }
}


void diffuseRay (ray_type type, double *normi, triangle *trg, double *r)
{
   double tm1[3];
   double r1, r2;
   double x, y, z;
   double vx[3], vy[3];
   double t;

   r1 = UNIFORM(0.0,1.0);
   r2 = UNIFORM(0.0,1.0);

   x = cos (2*PI*r1) * sqrt (1.0-r2);
   y = sin (2*PI*r1) * sqrt (1.0-r2);
   z = sqrt (r2);

   subVectors (trg->vs[1], trg->vs[0], tm1);
   normalizeVector (tm1, vx);
   crossProduct (vx, trg->n, vy);

   multVector (vx, x, r);
   multVector (vy, y, tm1);
   addVectors (r, tm1, r);
   cosAngle (normi, trg->n, &t);
   if (t > 0.0)
   {
      z *= -1.0;
   }
   if (type == TRANSMIT)
   {
      z *= -1.0;
   }
   multVector (trg->n, z, tm1);
   addVectors (r, tm1, r);
}


void reflectedRay (double *normi, double *n, double *r)
{
   double t;
   double tm1[3];

   cosAngle (normi, n, &t);
   multVector (n, -2*t, tm1);
   addVectors (normi, tm1, r);
}


void specularRay (ray_type type, double *normi, triangle *trg, double *r)
{
   double tm1[3];
   double r1, r2;
   double teh, fih;
   double xh, yh, zh;
   double vx[3], vy[3], vh[3];

   if (trg->m->cm.rs_a < EPS)   // gladko
   {
      if (type == REFLECT)
      {
         reflectedRay (normi, trg->n, r);
      }
      else
      {
         copyVector (normi, r);
      }
   }
   else
   {
      r1 = UNIFORM(0.0,1.0);
      r2 = UNIFORM(0.0,1.0);

      fih = 2*PI*r1;
      teh = atan (trg->m->cm.rs_a * sqrt (-log (r2)));

      xh = cos (fih) * sin (teh);
      yh = sin (fih) * sin (teh);
      zh = cos (teh);

      subVectors (trg->vs[1], trg->vs[0], tm1);
      normalizeVector (tm1, vx);
      crossProduct (vx, trg->n, vy);

      multVector (vx, xh, vh);
      multVector (vy, yh, tm1);
      addVectors (vh, tm1, vh);
      multVector (trg->n, zh, tm1);
      addVectors (vh, tm1, vh);

      if (type == REFLECT)
      {
         reflectedRay (normi, vh, r);
      }
      else
      {
         reflectedRay (normi, trg->n, tm1);
         reflectedRay (tm1, vh, r);
      }
   }
}


void putPixelFloat (int x, int y, float r, float g, float b)
{
   if ((x >= 0) && (x < screenX) && (y >= 0) && (y < screenY))
   {
      pictureBufferFloat[3*x+3*y*screenX] = r;
      pictureBufferFloat[3*x+3*y*screenX+1] = g;
      pictureBufferFloat[3*x+3*y*screenX+2] = b;
   }
}


bool intersectionRayTriangle (double *position, double *ray, triangle *trg, double *point, double *distance)
{
   double u, v;

   if (intersect_triangle (position, ray,
                           trg->vs[0], trg->vs[1], trg->vs[2],
                           distance, &u, &v))
   {
      if (*distance > 0.0)
      {
         setVector ((1.0-u-v)*trg->vs[0][0] + u*trg->vs[1][0] + v*trg->vs[2][0],
                    (1.0-u-v)*trg->vs[0][1] + u*trg->vs[1][1] + v*trg->vs[2][1],
                    (1.0-u-v)*trg->vs[0][2] + u*trg->vs[1][2] + v*trg->vs[2][2],
                    point);
         return TRUE;
      }
   }
   return FALSE;
}


void intersectRay (triangleList list, double *position, double *normalizedRay, double min, double max, triangle **minDistTriangle, double *minDistPoint, float *lightColour)
{
   double intersectPoint[3];
   double distance;
   double minDistance;
   triangleList *l;

   minDistance = FHUGE;
   *minDistTriangle = NULL;
   for (l = list.next; l != NULL; l = l->next)
   {
      if (intersectionRayTriangle (position, normalizedRay, l->trg, intersectPoint, &distance))
      {
         if ((min <= distance) && (distance < max))
         {
            if (lightColour == NULL)
            {
               if (distance < minDistance)
               {
                  minDistance = distance;
                  *minDistTriangle = l->trg;
                  copyVector (intersectPoint, minDistPoint);
               }
            }
            else
            {
//               if (l->trg->m->cm.ed < LIGHT_THR)
               {
                  multColours (lightColour, l->trg->m->rgb_ts_c, lightColour);
                  multColour (lightColour, l->trg->m->cm.ts, lightColour);
               }
            }
         }
      }
   }
}


void getRndTrgPoint (triangle *trg, double *point)
{
   double a, b;
   double tm1[3], tm2[3];

   a = UNIFORM(0.0,1.0);
   b = UNIFORM(0.0,1.0);
   if (a + b > 1.0)
   {
      a = 1.0 - a;
      b = 1.0 - b;
   }

   subVectors (trg->vs[1], trg->vs[0], tm1);
   subVectors (trg->vs[2], trg->vs[0], tm2);
   multVector (tm1, a, tm1);
   multVector (tm2, b, tm2);
   addVectors (trg->vs[0], tm1, point);
   addVectors (point, tm2, point);
}


void traceRay (int depth, double *position, double *ray, float weight, float *colour)
{
   triangle *minDistTriangle;
   double minDistPoint[3];
   double normalizedRay[3];

   double normalizedReflRay[3];
   float reflRayColour[3];
   double normalizedTransmRay[3];
   float transmRayColour[3];

   double normalizedHalf[3];

   double rndLightPoint[3];
   double lightRay[3];
   double lightDist;
   double normalizedLightRay[3];

   float m;
   float lightColour[3];
   float Ldiff;
   double cosLN;
   double cosRL;
   double cosVN;
   double cosHN;
   double tanHN;

   float illum;

   double rnd;
   triangle *tmt;
   double tm1[3];
   float tm2[3];
   lightList *k;
   int i;
   triangleList *l;

   normalizeVector (ray, normalizedRay);
   intersectRayTree (scena.triangles, position, normalizedRay, EPS, FHUGE, &minDistTriangle, minDistPoint, NULL);

   if (minDistTriangle != NULL)
   {
      if (depth == 0)
      {
         copyColour (minDistTriangle->m->rgb_disp, colour);
      }
      else
      {
         setColour (0.0, 0.0, 0.0, colour);

         dotProduct (normalizedRay, minDistTriangle->n, &cosVN);

         if ((minDistTriangle->m->cm.rd > EPS)
          || (minDistTriangle->m->cm.td > EPS))
         {
            for (i = 0; i < lightSamples; i++)
            {
               rnd = UNIFORM(0.0,sumIllum);
               illum = 0.0;
               for (k = scena.lights.next; k != NULL; k = k->next)
               {
                  for (l = k->lgh->lightTrgs.next; l != NULL; l = l->next)
                  {
                     illum += l->trg->m->cm.ed * l->trg->area;
                     if (illum > rnd)
                     {
                        goto tu;
                     }
                  }
               }
tu:
               getRndTrgPoint (l->trg, rndLightPoint);

               subVectors (rndLightPoint, minDistPoint, lightRay);
               getVectorLength (lightRay, &lightDist);
               normalizeVector (lightRay, normalizedLightRay);
               dotProduct (normalizedLightRay, minDistTriangle->n, &cosLN);
               dotProduct (normalizedLightRay, l->trg->n, &cosRL);



	       /*
               copyColour (k->lgh->m->rgb_ed_c, lightColour);
               intersectRayTree (scena.triangles, minDistPoint, normalizedLightRay, EPS, lightDist-EPS, &tmt, tm1, lightColour);

               Ldiff = (sumIllum * fabs (cosLN) * fabs (cosRL)) / SQR(lightDist);
               multColour (lightColour, Ldiff, tm2);
	       */



               intersectRayTree (scena.triangles, minDistPoint, normalizedLightRay, EPS, FHUGE, &tmt, tm1, NULL);

               if (tmt==l->trg)
               {
                  Ldiff = (sumIllum * fabs (cosLN) * fabs (cosRL)) / SQR(lightDist);
                  multColour (k->lgh->m->rgb_ed_c, Ldiff, tm2);

                  if (cosVN * cosLN < 0.0)
                  {
                     multColours (tm2, minDistTriangle->m->rgb_rd_c, tm2);
                     multColour (tm2, minDistTriangle->m->cm.rd/PI, tm2);
                     addColours (colour, tm2, colour);
                  }
                  else
                  {
                     multColours (tm2, minDistTriangle->m->rgb_td_c, tm2);
                     multColour (tm2, minDistTriangle->m->cm.td/PI, tm2);
                     addColours (colour, tm2, colour);
                  }
               }
            }
            multColour (colour, 1.0 / (float) lightSamples, colour);
         }

         if (minDistTriangle->m->cm.rs > EPS)
         {
            specularRay (REFLECT, normalizedRay, minDistTriangle, normalizedLightRay);
            intersectRayTree (scena.triangles, minDistPoint, normalizedLightRay, EPS, FHUGE, &tmt, tm1, NULL);
            if (tmt != NULL)
            {
               if (tmt->m->cm.ed > LIGHT_THR)
               {
                  multColour (tmt->m->rgb_ed_c, tmt->m->cm.ed, tm2);
                  multColours (tm2, minDistTriangle->m->rgb_rs_c, tm2);
                  multColour (tm2, minDistTriangle->m->cm.rs, tm2);
                  addColours (colour, tm2, colour);
               }
            }
         }

         if (minDistTriangle->m->cm.ts > EPS)
         {
            specularRay (TRANSMIT, normalizedRay, minDistTriangle, normalizedLightRay);
            intersectRayTree (scena.triangles, minDistPoint, normalizedLightRay, EPS, FHUGE, &tmt, tm1, NULL);
            if (tmt != NULL)
            {
               if (tmt->m->cm.ed > LIGHT_THR)
               {
                  multColour (tmt->m->rgb_ed_c, tmt->m->cm.ed, tm2);
                  multColours (tm2, minDistTriangle->m->rgb_ts_c, tm2);
                  multColour (tm2, minDistTriangle->m->cm.ts, tm2);
                  addColours (colour, tm2, colour);
               }
            }
         }

         addColours (colour, ambient, colour);

         if (depth >= 2)
         {
            rnd = UNIFORM(0.0,1.0);

            if (rnd < minDistTriangle->m->cm.rd)
            {
               diffuseRay (REFLECT, normalizedRay, minDistTriangle, normalizedReflRay);
               traceRay (depth-1, minDistPoint, normalizedReflRay, weight * minDistTriangle->m->cm.rd, reflRayColour);
               multColours (reflRayColour, minDistTriangle->m->rgb_rd_c, tm2);
               addColours (colour, tm2, colour);
            }
            else if (rnd < minDistTriangle->m->cm.rd
                         + minDistTriangle->m->cm.rs)
            {
               specularRay (REFLECT, normalizedRay, minDistTriangle, normalizedReflRay);
               traceRay (depth-1, minDistPoint, normalizedReflRay, weight * minDistTriangle->m->cm.rs, reflRayColour);
               multColours (reflRayColour, minDistTriangle->m->rgb_rs_c, tm2);
               addColours (colour, tm2, colour);
            }
            else if (rnd < minDistTriangle->m->cm.rd
                         + minDistTriangle->m->cm.rs
                         + minDistTriangle->m->cm.td)
            {
               diffuseRay (TRANSMIT, normalizedRay, minDistTriangle, normalizedTransmRay);
               traceRay (depth-1, minDistPoint, normalizedTransmRay, weight * minDistTriangle->m->cm.td, transmRayColour);
               multColours (transmRayColour, minDistTriangle->m->rgb_td_c, tm2);
               addColours (colour, tm2, colour);
            }
            else if (rnd < minDistTriangle->m->cm.rd
                         + minDistTriangle->m->cm.rs
                         + minDistTriangle->m->cm.td
                         + minDistTriangle->m->cm.ts)
            {
               specularRay (TRANSMIT, normalizedRay, minDistTriangle, normalizedTransmRay);
               traceRay (depth-1, minDistPoint, normalizedTransmRay, weight * minDistTriangle->m->cm.ts, transmRayColour);
               multColours (transmRayColour, minDistTriangle->m->rgb_ts_c, tm2);
               addColours (colour, tm2, colour);
            }
	 }

         if ((minDistTriangle->m->cm.ed > LIGHT_THR) && (depth == raytracingDepth))
         {
            multColour (minDistTriangle->m->rgb_ed_c, minDistTriangle->m->cm.ed, tm2);
            addColours (colour, tm2, colour);
         }
      }
   }
   else
   {
      setColour (0.0, 0.0, 0.0, colour);
   }
}


void prepareRaytracing ()
{
   if (raytracingDepth > 0)
   {
      calculateTrianglesData ();
      recalculateLights (ambient);
   }
   buildKDTree ();
}


void raytracing ()
{
   double startPoint[3];
   double pixelPoint[3];
   double width, height;
   double pixelX, pixelY;
   double ray[3];
   float pixelColour[3];
   int i, j, k;
   double tm1[3];
   float tm2[3];

   height = 2.0 * tan (camera.angleOfView * (PI/360.0));
   width = height * ((double) screenX / (double) screenY);

   addVectors (camera.position, camera.viewVector, startPoint);

   //loadRadiance ("temp.pic");
   //saveRadiance ("temp.pic");
   printf ("%4d/%4d\n", 0, screenY);
   for (j = screenY-1-0; j >= 0; j--)
   {
      for (i = 0; i < screenX; i++)
      {
         setColour (0.0, 0.0, 0.0, pixelColour);
         for (k = 0; k < pathsPerPixel; k++)
         {
            pixelX = ( (UNIFORM(0.0,1.0) + (double) i) / ((double) screenX) ) * width;
            pixelY = ( (UNIFORM(0.0,1.0) + (double) j) / ((double) screenY) ) * height;

            multVector (camera.upVector, pixelY - (height/2.0), tm1);
            addVectors (startPoint, tm1, pixelPoint);

            multVector (camera.rightVector, pixelX - (width/2.0), tm1);
            addVectors (pixelPoint, tm1, pixelPoint);

            subVectors (pixelPoint, camera.position, ray);

	    traceRay (raytracingDepth, camera.position, ray, 1.0, tm2);

            addColours (pixelColour, tm2, pixelColour);
         }
         multColour (pixelColour, 1.0 / (float)pathsPerPixel, pixelColour);
         putPixelFloat (i, j, pixelColour[0], pixelColour[1], pixelColour[2]);
      }
//printf (".\n");
      if (j % 10 == 0)
      {
//saveRadiance ("temp.pic");
         printf ("%4d/%4d\n", screenY-j, screenY);
      }
   }
}


bool pointInRect (double *point, double *minCoords, double *maxCoords, int axis)
{
   if (axis == 0)
   {
      if ((point[1] > minCoords[1])
       && (point[1] < maxCoords[1])
       && (point[2] > minCoords[2])
       && (point[2] < maxCoords[2]))
      {
         return TRUE;
      }
   }
   else if (axis == 1)
   {
      if ((point[0] > minCoords[0])
       && (point[0] < maxCoords[0])
       && (point[2] > minCoords[2])
       && (point[2] < maxCoords[2]))
      {
         return TRUE;
      }
   }
   else if (axis == 2)
   {
      if ((point[0] > minCoords[0])
       && (point[0] < maxCoords[0])
       && (point[1] > minCoords[1])
       && (point[1] < maxCoords[1]))
      {
         return TRUE;
      }
   }
   return FALSE;
}


bool intersectionRayRect (double *position, double *normalizedRay, double *minCoords, double *maxCoords, int axis, double *point, double *distance)
{
   intersectionRayPlane (position, normalizedRay, minCoords[axis], axis, distance);
   multVector (normalizedRay, *distance, point);
   addVectors (position, point, point);
   if (pointInRect (point, minCoords, maxCoords, axis))
   {
      return TRUE;
   }
   return FALSE;
}



void updateDP (int *cuts, double *d, double *p, double *d1, double *p1, double *d2, double *p2)
{
   (*cuts)++;
   if (*cuts == 1)
   {
      *d1 = *d;
      copyVector (p, p1);
   }
   else
   {
      *d2 = *d;
      copyVector (p, p2);
   }
}


bool intersectionRayBoxOLD (double *position, double *normalizedRay, double *minCoords, double *maxCoords, double *distance1, double *distance2)
{
   double minC[3];
   double maxC[3];
   double d;
   double d1;
   double d2;
   double p[3];
   double p1[3];
   double p2[3];
   int cuts = 0; 

   copyVector (minCoords, minC);
   copyVector (maxCoords, maxC);

   maxC[0] = minC[0];
   if (intersectionRayRect (position, normalizedRay, minC, maxC, 0, p, &d))
   {
      updateDP (&cuts, &d, p, &d1, p1, &d2, p2);
   }
   maxC[0]= maxCoords[0];

   minC[0] = maxC[0];
   if (intersectionRayRect (position, normalizedRay, minC, maxC, 0, p, &d))
   {
      updateDP (&cuts, &d, p, &d1, p1, &d2, p2);
   }
   minC[0] = minCoords[0];

   maxC[1] = minC[1];
   if (intersectionRayRect (position, normalizedRay, minC, maxC, 1, p, &d))
   {
      updateDP (&cuts, &d, p, &d1, p1, &d2, p2);
   }
   maxC[1] = maxCoords[1];

   minC[1] = maxC[1];
   if (intersectionRayRect (position, normalizedRay, minC, maxC, 1, p, &d))
   {
      updateDP (&cuts, &d, p, &d1, p1, &d2, p2);
   }
   minC[1] = minCoords[1];

   maxC[2] = minC[2];
   if (intersectionRayRect (position, normalizedRay, minC, maxC, 2, p, &d))
   {
      updateDP (&cuts, &d, p, &d1, p1, &d2, p2);
   }
   maxC[2] = maxCoords[2];

   minC[2] = maxC[2];
   if (intersectionRayRect (position, normalizedRay, minC, maxC, 2, p, &d))
   {
      updateDP (&cuts, &d, p, &d1, p1, &d2, p2);
   }
   minC[2] = minCoords[2];

   if (cuts > 0)
   {
      if (d1 < d2)
      {
         *distance1 = d1;
         copyVector (p1, point1);
         *distance2 = d2;
         copyVector (p2, point2);
      }
      else
      {
         *distance1 = d2;
         copyVector (p2, point1);
         *distance2 = d1;
         copyVector (p1, point2);
      }
      return TRUE;
   }
   return FALSE;
}


double GLa (double La)
{
   if (La > 100.0)
   {
      return 2.655;
   }
   else
   {
      return 1.855 + 0.4 * log10 (La + 2.3e-5);
   }
}


void toneMappingTM ()
{
   double lumi;
   double sumlog = 0.0;
   double Cmax = 50.0;
   double Lda = 15.0;
   double Lwa;
   double Gd;
   double Gw;
   double Gwd;
   double mLwa;
   int i, j, k;

   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         lumi = LUMI_R * pictureBufferFloat[3*i+3*j*screenX+0];
              + LUMI_G * pictureBufferFloat[3*i+3*j*screenX+1];
              + LUMI_B * pictureBufferFloat[3*i+3*j*screenX+2];
         sumlog += log10 (lumi + 2.3e-5);
      }
   }

   Lwa = pow (10.0, sumlog / ((double) (screenX*screenY)));
   Gd = GLa (Lda);
   Gw = GLa (Lwa);
   Gwd = Gw / (1.855 + 0.4 * log10 (Lda));
   mLwa = pow (Cmax, (Gwd-1.0) / 2.0);

   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         for (k = 0; k < 3; k++)
         {
            pictureBufferFloat[3*i+3*j*screenX+k] =
               mLwa * Lda * pow (pictureBufferFloat[3*i+3*j*screenX+k]/Lwa, Gw/Gd);
         }
      }
   }

   toneMapping256 ();
}


void loadRadiance (char *s)
{
   FILE *plik;
   int i, j;
   int tmp;

   if ( (plik = fopen (s, "r")) == NULL )
   {
      printf ("Nie moge otworzyc pliku %s\n", s);
      exit (5);
   }
   else
   {
      fscanf (plik, "P3\n%d %d\n10000\n", &screenX, &screenY); 

      pictureBufferFloat = (float *) fmalloc (screenX*screenY*3 * sizeof (float));
      pictureBufferByte = (GLubyte *) fmalloc (screenX*screenY*3 * sizeof (GLubyte));

      for (j = screenY-1; j >= 0; j--) 
      {
         for (i = 0; i < screenX; i++)
         {
            fscanf (plik,"%d ", &tmp);
            pictureBufferFloat[3*i + 3*j*screenX] = (float) tmp / 10000.0; 
            fscanf (plik,"%d ", &tmp);
            pictureBufferFloat[3*i + 3*j*screenX+1] = (float) tmp / 10000.0; 
            fscanf (plik,"%d\n", &tmp);
            pictureBufferFloat[3*i + 3*j*screenX+2] = (float) tmp / 10000.0; 
         }
      }
      fclose (plik);
   }
}


void saveRadiance (char *s)
{
   FILE *plik;
   int i, j;

   if ( (plik = fopen (s, "w")) == NULL )
   {
      printf ("Nie moge otworzyc pliku %s\n", s);
      exit (5);
   }
   else
   {
      fprintf (plik, "P3\n%d %d\n10000\n", screenX, screenY); 
      for (j = screenY-1; j >= 0; j--) 
      {
         for (i = 0; i < screenX; i++)
         {
            fprintf (plik,"%d ", (int) (10000.0 * pictureBufferFloat[3*i + 3*j*screenX])); 
            fprintf (plik,"%d ", (int) (10000.0 * pictureBufferFloat[3*i + 3*j*screenX+1])); 
            fprintf (plik,"%d\n", (int) (10000.0 * pictureBufferFloat[3*i + 3*j*screenX+2])); 
         }
      }
      fclose (plik);
   }
}


/*
void toneMappingTumblinRushmeier ()
{
   double sumlog;
   double Lw;
   double Ld;
   double Law;
   double Lad;
   double Cmax = 50.0;
   double Lamb = 1.0;
   double Ldmax = 100.0;
   float n;
   int i, j, k;

   for (k = 0; k < 3; k++)
   {
      sumlog = 0.0;
      for (j = 0; j < screenY; j++)
      {
         for (i = 0; i < screenX; i++)
         {
            Lw = pictureBufferFloat[3*i+3*j*screenX+k] + EPS;
            sumlog += (log10 (Lw) / ((double) (screenX*screenY)));
         }
      }
      Law = pow (10.0, sumlog);
      Lad = Ldmax / sqrt (Cmax);

      for (j = 0; j < screenY; j++)
      {
         for (i = 0; i < screenX; i++)
         {
            Lw = pictureBufferFloat[3*i+3*j*screenX+k] + EPS;

            Ld = (10000.0/PI)
               * pow (10.0, (betaTR(Law)-betaTR(Lad))/alphaTR(Lad))
               * pow ((PI*Lw)/10000.0, alphaTR(Law)/alphaTR(Lad));

            n = (float) pow (((Ld - Lamb) / Ldmax), 1.0/GAMMA);

            pictureBufferFloat[3*i+3*j*screenX+k] = n;
         }
      }
   }

   toneMapping256 ();
}
*/



               m = minDistTriangle->m->cm.rs_a;
               multVector (normalizedRay, -1.0, tm1);
               addVectors (tm1, normalizedLightRay, tm1);
               normalizeVector (tm1, normalizedHalf);
               dotProduct (normalizedHalf, minDistTriangle->n, &cosHN);
               tanHN = tan(acos(cosHN));
               Lspec = l->trg->m->cm.ed * minDistTriangle->m->cm.rs
                     * exp (- SQR(tanHN) / SQR(m))
                     / (4.0 * PI  * SQR(m) * sqrt (fabs (cosLN * cosVN)));
               multColour (lightColour, Lspec, tm2);
               addColours (spec, tm2, spec);
               multColours (spec, minDistTriangle->m->rgb_rs_c, tm2);

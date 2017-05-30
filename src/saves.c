
#include "headers.h"


extern camera_type camera;
extern tone_mapping_type tone_mapping;
extern int screenX;
extern int screenY;
extern int raytracingDepth;
extern int pathsPerPixel;
extern int lightSamples;
extern float *pictureBufferFloat;
extern GLubyte *pictureBufferByte;


void saveConfig (void)
{
   FILE *plik;
   char *s = "rt.cfg";
   double angleOfViewX;

   if ((plik = fopen (s, "w")) == NULL)
   {
      printf ("Nie moge zapisac pliku %s\n", s);
      exit (5);
   }
   else
   {
      fprintf (plik, "%d %d\n%d\n", screenX, screenY, DEFAULT_RAYTRACING_DEPTH); 
      fprintf (plik, "%f %f %f\n", camera.position[0],
                                   camera.position[1],
                                   camera.position[2]); 
      fprintf (plik, "%f %f %f\n", camera.viewVector[0],
                                   camera.viewVector[1],
                                   camera.viewVector[2]); 
      fprintf (plik, "%f %f %f\n", camera.upVector[0],
                                   camera.upVector[1],
                                   camera.upVector[2]); 
      angleOfViewX = (360.0/PI) * atan (tan (camera.angleOfView * (PI/360.0))
                                         * ((double) screenX / (double) screenY));
      fprintf (plik, "%f\n", angleOfViewX); 
      fprintf (plik, "HD\n"); 
      fprintf (plik, "%d %d\n", DEFAULT_PATHS_PER_PIXEL, DEFAULT_LIGHT_SAMPLES); 
      fclose (plik);
   }
}


void loadConfig (char *s)
{
   FILE *plik;
   double angleOfViewX;
   char tmp[4];

   if ((plik = fopen (s, "r")) == NULL)
   {
      printf ("Nie moge otworzyc pliku %s\n", s);
      exit (5);
   }
   else
   {
      fscanf (plik, "%d %d\n%d\n", &screenX, &screenY, &raytracingDepth); 

      fscanf (plik, "%lf %lf %lf\n", &(camera.position[0]),
                                   &(camera.position[1]),
                                   &(camera.position[2])); 
      fscanf (plik, "%lf %lf %lf\n", &(camera.viewVector[0]),
                                   &(camera.viewVector[1]),
                                   &(camera.viewVector[2])); 
      fscanf (plik, "%lf %lf %lf\n", &(camera.upVector[0]),
                                   &(camera.upVector[1]),
                                   &(camera.upVector[2])); 
      crossProduct (camera.viewVector, camera.upVector, camera.rightVector);

      fscanf (plik, "%lf\n", &angleOfViewX); 
      camera.angleOfView = (360.0/PI) * atan (tan (angleOfViewX * (PI/360.0))
                                               / ((double) screenX / (double) screenY));

      fscanf (plik, "%s\n", tmp); 
      if (strcmp (tmp, "HD") == 0)
      {
         tone_mapping = MAP_HD;
      }
      else if (strcmp (tmp, "EV") == 0)
      {
         tone_mapping = MAP_EV;
      }
      else if (strcmp (tmp, "TM") == 0)
      {
         tone_mapping = MAP_TM;
      }
      else if (strcmp (tmp, "TG") == 0)
      {
         tone_mapping = MAP_TG;
      }
      else if (strcmp (tmp, "TL") == 0)
      {
         tone_mapping = MAP_TL;
      }

      if (fscanf (plik, "%d %d\n", &pathsPerPixel, &lightSamples) == EOF)
      {
         pathsPerPixel = DEFAULT_PATHS_PER_PIXEL;
         lightSamples = DEFAULT_LIGHT_SAMPLES;
      }

      fclose (plik);
   }
}


void savePPM (char *s)
{
   FILE *plik;
   int i, j;

   if ((plik = fopen (s, "w")) == NULL)
   {
      printf ("Nie moge otworzyc pliku %s\n", s);
      exit (5);
   }
   else
   {
      fprintf (plik, "P6\n%d %d\n255\n", screenX, screenY); 
      for (j = screenY-1; j >= 0; j--) 
      {
         for (i = 0; i < screenX; i++)
         {
            fprintf (plik,"%c", pictureBufferByte[3*i+3*j*screenX+0]); 
            fprintf (plik,"%c", pictureBufferByte[3*i+3*j*screenX+1]); 
            fprintf (plik,"%c", pictureBufferByte[3*i+3*j*screenX+2]); 
         }
      }
      fclose (plik);
   }
}


void loadRadiance (char *s)
{
   FILE *plik;
   int i, j;
   int tmp;
   COLR *scanin;
   COLOR c;

   if ((plik = fopen (s, "r")) == NULL)
   {
      printf ("Nie moge otworzyc pliku %s\n", s);
      exit (5);
   }
   else
   {
      if ((checkheader (plik, COLRFMT, NULL) < 0)
       || (fgetresolu (&screenX, &screenY, plik) < 0))
      {
         printf ("Bad picture format\n");
         exit (5);
      }

      pictureBufferFloat = (float *) fmalloc (screenX*screenY*3 * sizeof (float));
      pictureBufferByte = (GLubyte *) fmalloc (screenX*screenY*3 * sizeof (GLubyte));
      scanin = (COLR *) fmalloc (screenX * sizeof (COLR));

      for (j = screenY-1; j >= 0; j--) 
      {
         if (freadcolrs (scanin, screenX, plik) < 0)
         {
            printf ("Error reading Radiance picture\n");
            exit (5);
         }

         for (i = 0; i < screenX; i++)
         {
            colr_color (c, scanin[i]);
            pictureBufferFloat[3*i+3*j*screenX+0] = c[RED]; 
            pictureBufferFloat[3*i+3*j*screenX+1] = c[GRN]; 
            pictureBufferFloat[3*i+3*j*screenX+2] = c[BLU]; 
         }
      }
      free (scanin);
      fclose (plik);
   }
}


void saveRadiance (char *s)
{
   FILE *plik;
   int i, j;
   COLR *scanout;

   if ( (plik = fopen (s, "w")) == NULL )
   {
      printf ("Nie moge otworzyc pliku %s\n", s);
      exit (5);
   }
   else
   {
      newheader ("RADIANCE", plik);
      fputformat (COLRFMT, plik);
      putc ('\n', plik);
      fprtresolu (screenX, screenY, plik);

      scanout = (COLR *) fmalloc (screenX * sizeof (COLR));

      for (j = screenY-1; j >= 0; j--) 
      {
         for (i = 0; i < screenX; i++)
         {
            setcolr (scanout[i], (double) pictureBufferFloat[3*i+3*j*screenX+0],
                                 (double) pictureBufferFloat[3*i+3*j*screenX+1],
                                 (double) pictureBufferFloat[3*i+3*j*screenX+2]);
         }
         if (fwritecolrs (scanout, screenX, plik) < 0)
         {
            printf ("Error writing Radiance picture\n");
            exit (5);
         }
      }
      free (scanout);
      fclose (plik);
   }
}

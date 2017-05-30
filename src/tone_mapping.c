
#include "headers.h"


extern tone_mapping_type tone_mapping;
extern int screenX;
extern int screenY;
extern float *pictureBufferFloat;
extern GLubyte *pictureBufferByte;


void toneMapping ()
{
   switch (tone_mapping)
   {
      case MAP_EV:
         toneMappingEV ();
         break;
      case MAP_TM:
         toneMappingTumblinRushmeier ();
         break;
      case MAP_TW:
         toneMappingWard ();
         break;
      case MAP_TG:
         toneMappingGamma ();
         break;
      case MAP_TL:
         toneMapping256 ();
         break;
   }
}


double calculateLuma (float r, float g, float b)
{
   return LUMI_R * r + LUMI_G * g + LUMI_B * b;
}


void toneMapping256 ()
{
   float maxRGB = 0.0;
   int i, j, k;

   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         for (k = 0; k < 3; k++)
         {
            if (pictureBufferFloat[3*i+3*j*screenX+k] > maxRGB)
            {
               maxRGB = pictureBufferFloat[3*i+3*j*screenX+k];
            }
         }
      }
   }

   if (maxRGB == 0.0) 
   {
      maxRGB = 1.0;
   }

   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         for (k = 0; k < 3; k++)
         {
            pictureBufferByte[3*i+3*j*screenX+k] =
               (GLubyte) (255.0 * (pictureBufferFloat[3*i+3*j*screenX+k] / maxRGB));
         }
      }
   }
}


void toneMappingGamma ()
{
   int i, j, k;

   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         for (k = 0; k < 3; k++)
         {
            pictureBufferFloat[3*i+3*j*screenX+k] =
               pow ((pictureBufferFloat[3*i+3*j*screenX+k]), 1.0/GAMMA);
         }
      }
   }

   toneMapping256 ();
}


void toneMappingEV ()      //    -8  ...  20 1/3  co 1/3 EV   =>   0, 3, 6, ..., 252, 255
{
   double lumi;
   double ev;
   GLubyte ev_disp;
   int i, j, k;

   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         lumi = calculateLuma (pictureBufferFloat[3*i+3*j*screenX+0],
                               pictureBufferFloat[3*i+3*j*screenX+1],
                               pictureBufferFloat[3*i+3*j*screenX+2]);
         ev = log (lumi / 2.5) / log (2.0);
         ev_disp = MIN(255,3*((GLubyte)(MAX(0.0,3.0*(ev+8.0)))));

         for (k = 0; k < 3; k++)
         {
            pictureBufferByte[3*i+3*j*screenX+k] = ev_disp;
         }
      }
   }
}


double alphaTR (double l)
{
   return 0.4 * log10 (l) + 1.519;
}


double betaTR (double l)
{
   return -0.4 * SQR(log10 (l)) + 0.218 * log10 (l) + 6.1642;
}


void toneMappingTumblinRushmeier ()
{
   double sumlog;
   double Lw;
   double Ld;
   double Law;
   double Lad;
   double Cmax = 50.0;
   double Lamb = 0.0;
   double Ldmax = 100.0;
   float rgb[3];
   float xyy[3];
   float n;
   int i, j, k;

   sumlog = 0.0;
   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         rgb[0] = pictureBufferFloat[3*i+3*j*screenX+0];
         rgb[1] = pictureBufferFloat[3*i+3*j*screenX+1];
         rgb[2] = pictureBufferFloat[3*i+3*j*screenX+2];
         rgb2xyy (rgb, xyy);
         pictureBufferFloat[3*i+3*j*screenX+0] = xyy[0];
         pictureBufferFloat[3*i+3*j*screenX+1] = xyy[1];
         pictureBufferFloat[3*i+3*j*screenX+2] = xyy[2];

         Lw = xyy[2] + 3.0;
         sumlog += (log10 (Lw) / ((double) (screenX*screenY)));
      }
   }
   Law = pow (10.0, sumlog);
   Lad = Ldmax / sqrt (Cmax);

   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         xyy[0] = pictureBufferFloat[3*i+3*j*screenX+0];
         xyy[1] = pictureBufferFloat[3*i+3*j*screenX+1];
         xyy[2] = pictureBufferFloat[3*i+3*j*screenX+2];

         Lw = xyy[2];
         Ld = (10000.0/PI)
            * pow (10.0, (betaTR(Law)-betaTR(Lad))/alphaTR(Lad))
            * pow ((PI*Lw)/10000.0, alphaTR(Law)/alphaTR(Lad));

         n = (float) pow (((Ld - Lamb) / Ldmax), 1.0/GAMMA);
         xyy[2] = n;

         xyy2rgb (xyy, rgb);
         pictureBufferFloat[3*i+3*j*screenX+0] = rgb[0];
         pictureBufferFloat[3*i+3*j*screenX+1] = rgb[1];
         pictureBufferFloat[3*i+3*j*screenX+2] = rgb[2];

         for (k = 0; k < 3; k++)
         {
            if (pictureBufferFloat[3*i+3*j*screenX+k] > 1.0)
            {
               pictureBufferByte[3*i+3*j*screenX+k] = 255;
            }
            else
            {
               pictureBufferByte[3*i+3*j*screenX+k] =
                  (GLubyte) (255.0 * pictureBufferFloat[3*i+3*j*screenX+k]);
            }
         }
      }
   }
}


void toneMappingWard ()
{
   double Lw;
   double Ld;
   double Law;
   double Lad;
   double Lamb = 0.0;
   double Ldmax = 100.0;
   double m;
   float rgb[3];
   float xyy[3];
   float n;
   int i, j, k;

   Law = 0.0;
   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         rgb[0] = pictureBufferFloat[3*i+3*j*screenX+0];
         rgb[1] = pictureBufferFloat[3*i+3*j*screenX+1];
         rgb[2] = pictureBufferFloat[3*i+3*j*screenX+2];
         rgb2xyy (rgb, xyy);
         pictureBufferFloat[3*i+3*j*screenX+0] = xyy[0];
         pictureBufferFloat[3*i+3*j*screenX+1] = xyy[1];
         pictureBufferFloat[3*i+3*j*screenX+2] = xyy[2];

         Lw = xyy[2];
         Law += (Lw / ((double) (screenX*screenY)));
      }
   }
   Lad = Ldmax / 2.0;

   m = pow ((1.219 + pow (Lad, 0.4)) / (1.219 + pow (Law, 0.4)), 2.5);

   for (j = 0; j < screenY; j++)
   {
      for (i = 0; i < screenX; i++)
      {
         xyy[0] = pictureBufferFloat[3*i+3*j*screenX+0];
         xyy[1] = pictureBufferFloat[3*i+3*j*screenX+1];
         xyy[2] = pictureBufferFloat[3*i+3*j*screenX+2];

         Lw = xyy[2];
         Ld = m * Lw;

         n = (float) pow (((Ld - Lamb) / Ldmax), 1.0/GAMMA);
         xyy[2] = n;

         xyy2rgb (xyy, rgb);
         pictureBufferFloat[3*i+3*j*screenX+0] = rgb[0];
         pictureBufferFloat[3*i+3*j*screenX+1] = rgb[1];
         pictureBufferFloat[3*i+3*j*screenX+2] = rgb[2];

         for (k = 0; k < 3; k++)
         {
            if (pictureBufferFloat[3*i+3*j*screenX+k] > 1.0)
            {
               pictureBufferByte[3*i+3*j*screenX+k] = 255;
            }
            else
            {
               pictureBufferByte[3*i+3*j*screenX+k] =
                  (GLubyte) (255.0 * pictureBufferFloat[3*i+3*j*screenX+k]);
            }
         }
      }
   }
}

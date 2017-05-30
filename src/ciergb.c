// na podstawie:
// cvrgb.c (libmgf)
// spec_rgb.c (Radiance)

#include "headers.h"


			/* Change the following to suit your standard */
#define  CIE_x_r		0.640		/* nominal CRT primaries */
#define  CIE_y_r		0.330
#define  CIE_x_g		0.290
#define  CIE_y_g		0.600
#define  CIE_x_b		0.150
#define  CIE_y_b		0.060
#define  CIE_x_w		0.3333		/* use true white */
#define  CIE_y_w		0.3333

#define CIE_C_rD	( (1./CIE_y_w) * \
				( CIE_x_w*(CIE_y_g - CIE_y_b) - \
				  CIE_y_w*(CIE_x_g - CIE_x_b) + \
				  CIE_x_g*CIE_y_b - CIE_x_b*CIE_y_g	) )
#define CIE_C_gD	( (1./CIE_y_w) * \
				( CIE_x_w*(CIE_y_b - CIE_y_r) - \
				  CIE_y_w*(CIE_x_b - CIE_x_r) - \
				  CIE_x_r*CIE_y_b + CIE_x_b*CIE_y_r	) )
#define CIE_C_bD	( (1./CIE_y_w) * \
				( CIE_x_w*(CIE_y_r - CIE_y_g) - \
				  CIE_y_w*(CIE_x_r - CIE_x_g) + \
				  CIE_x_r*CIE_y_g - CIE_x_g*CIE_y_r	) )

float	xyz2rgbmat[3][3] = {		/* XYZ to RGB conversion matrix */
	{(CIE_y_g - CIE_y_b - CIE_x_b*CIE_y_g + CIE_y_b*CIE_x_g)/CIE_C_rD,
	 (CIE_x_b - CIE_x_g - CIE_x_b*CIE_y_g + CIE_x_g*CIE_y_b)/CIE_C_rD,
	 (CIE_x_g*CIE_y_b - CIE_x_b*CIE_y_g)/CIE_C_rD},
	{(CIE_y_b - CIE_y_r - CIE_y_b*CIE_x_r + CIE_y_r*CIE_x_b)/CIE_C_gD,
	 (CIE_x_r - CIE_x_b - CIE_x_r*CIE_y_b + CIE_x_b*CIE_y_r)/CIE_C_gD,
	 (CIE_x_b*CIE_y_r - CIE_x_r*CIE_y_b)/CIE_C_gD},
	{(CIE_y_r - CIE_y_g - CIE_y_r*CIE_x_g + CIE_y_g*CIE_x_r)/CIE_C_bD,
	 (CIE_x_g - CIE_x_r - CIE_x_g*CIE_y_r + CIE_x_r*CIE_y_g)/CIE_C_bD,
	 (CIE_x_r*CIE_y_g - CIE_x_g*CIE_y_r)/CIE_C_bD}
};

float rgb2xyzmat[3][3] = {		/* RGB to XYZ (no white balance) */
	{CIE_x_r*CIE_C_rD/CIE_D,CIE_x_g*CIE_C_gD/CIE_D,CIE_x_b*CIE_C_bD/CIE_D},
	{CIE_y_r*CIE_C_rD/CIE_D,CIE_y_g*CIE_C_gD/CIE_D,CIE_y_b*CIE_C_bD/CIE_D},
	{(1.-CIE_x_r-CIE_y_r)*CIE_C_rD/CIE_D,
	 (1.-CIE_x_g-CIE_y_g)*CIE_C_gD/CIE_D,
	 (1.-CIE_x_b-CIE_y_b)*CIE_C_bD/CIE_D}
};


void colortrans (float *c2, float mat[3][3], float *c1)
{
   c2[0] = mat[0][0]*c1[0] + mat[0][1]*c1[1] + mat[0][2]*c1[2];
   if(c2[0] < 0.)
      c2[0] = 0.;
   c2[1] = mat[1][0]*c1[0] + mat[1][1]*c1[1] + mat[1][2]*c1[2];
   if(c2[1] < 0.)
      c2[1] = 0.;
   c2[2] = mat[2][0]*c1[0] + mat[2][1]*c1[1] + mat[2][2]*c1[2];
   if(c2[2] < 0.)
      c2[2] = 0.;
}


void rgb2xyy (float *rgb, float *xyy)
{
   float xyz[3];

   colortrans (xyz, rgb2xyzmat, rgb);

   xyy[0] = xyz[0]/(xyz[0]+xyz[1]+xyz[2]);
   xyy[1] = xyz[1]/(xyz[0]+xyz[1]+xyz[2]);
   xyy[2] = xyz[1];
}


void xyy2rgb (float *xyy, float *rgb)
{
   float xyz[3];

   xyz[0] = xyy[2]*xyy[0]/xyy[1];
   xyz[1] = xyy[2];
   xyz[2] = xyy[2]*(1./xyy[1] - 1.) - xyz[0];

   colortrans (rgb, xyz2rgbmat, xyz);
}


void mgf2rgb (C_COLOR *cin, float *rgb)
{
   float xyy[3];

   c_ccvt (cin, C_CSXY);
   xyy[0] = cin->cx;
   xyy[1] = cin->cy;
   xyy[2] = 1.0;

   xyy2rgb (xyy, rgb);
}

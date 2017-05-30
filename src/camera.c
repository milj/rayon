
#include "headers.h"


extern int screenX;
extern int screenY;


void cameraInit (camera_type *c)
{
   c->position = makeVector (0.0, 0.0, 0.0);
   c->viewVector = makeVector (0.0, 0.0, -1.0);
   c->rightVector = makeVector (1.0, 0.0, 0.0);
   c->upVector = makeVector (0.0, 1.0, 0.0);
   c->rotatedX = 0.0;
   c->rotatedY = 0.0;
   c->rotatedZ = 0.0;
   c->angleOfView = DEFAULT_ANGLE_OF_VIEW;
   c->clipNear = CLIP_NEAR;
   c->clipFar = CLIP_FAR;
}

void cameraMove (camera_type *c, double *direction)
{
   addVectors (c->position, direction, c->position);
}

void cameraRotateX (camera_type *c, double angle)
{
   double tm1[3], tm2[3], tm3[3];

   c->rotatedX += angle;

   multVector (c->viewVector, cos (angle * (PI/180.0)), tm1);
   multVector (c->upVector, sin (angle * (PI/180.0)), tm2);
   addVectors (tm1, tm2, tm3);
   normalizeVector (tm3, c->viewVector);

   crossProduct (c->viewVector, c->rightVector, tm3);
   multVector (tm3, -1.0, c->upVector);
}

void cameraRotateY (camera_type *c, double angle)
{
   double tm1[3], tm2[3], tm3[3];

   c->rotatedY += angle;

   multVector (c->viewVector, cos (angle * (PI/180.0)), tm1);
   multVector (c->rightVector, sin (angle * (PI/180.0)), tm2);
   subVectors (tm1, tm2, tm3);
   normalizeVector (tm3, c->viewVector);

   crossProduct (c->viewVector, c->upVector, c->rightVector);
}

void cameraRotateZ (camera_type *c, double angle)
{
   double tm1[3], tm2[3], tm3[3];

   c->rotatedZ += angle;

   multVector (c->rightVector, cos (angle * (PI/180.0)), tm1);
   multVector (c->upVector, sin (angle * (PI/180.0)), tm2);
   addVectors (tm1, tm2, tm3);
   normalizeVector (tm3, c->rightVector);

   crossProduct (c->viewVector, c->rightVector, tm3);
   multVector (tm3, -1.0, c->upVector);
}

void cameraMoveForward (camera_type *c, double distance)
{
   double tm1[3];

   multVector (c->viewVector, distance, tm1);
   addVectors (c->position, tm1, c->position);
}

void cameraMoveRight (camera_type *c, double distance)
{
   double tm1[3];

   multVector (c->rightVector, distance, tm1);
   addVectors (c->position, tm1, c->position);
}

void cameraMoveUpward (camera_type *c, double distance)
{
   double tm1[3];

   multVector (c->upVector, distance, tm1);
   addVectors (c->position, tm1, c->position);
}

void cameraZoom (camera_type *c, int angle)
{
   c->angleOfView += angle;
   if (c->angleOfView < 1)
   {
      c->angleOfView = 1;
   }
   if (c->angleOfView > 179)
   {
      c->angleOfView = 179;
   }
}


void cameraRender (camera_type *c)
{
   double lookAtPoint[3];

   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective (c->angleOfView, ((double) screenX / (double) screenY), c->clipNear, c->clipFar);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();

   addVectors (c->position, c->viewVector, lookAtPoint);
   gluLookAt (c->position[0], c->position[1], c->position[2],
              lookAtPoint[0], lookAtPoint[1], lookAtPoint[2],
              c->upVector[0], c->upVector[1], c->upVector[2]);
}



typedef struct
{
   double *position;
   double *viewVector;
   double *rightVector;
   double *upVector;
   double rotatedX;
   double rotatedY;
   double rotatedZ;
   double angleOfView;
   double clipNear;
   double clipFar;
}
camera_type;


void cameraInit (camera_type *);
void cameraMove (camera_type *, double *);
void cameraRotateX (camera_type *, double);
void cameraRotateY (camera_type *, double);
void cameraRotateZ (camera_type *, double);
void cameraMoveForward (camera_type *, double);
void cameraMoveRight (camera_type *, double);
void cameraMoveUpward (camera_type *, double);
void cameraZoom (camera_type *, int);
void cameraRender (camera_type *);

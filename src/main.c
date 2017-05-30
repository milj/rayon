
#include "headers.h"


camera_type camera;
scene scena;
view_type view;
mode_type mode;
tone_mapping_type tone_mapping;
GLUtesselator  *tobj;
int screenX;
int screenY;
int raytracingDepth;
int pathsPerPixel;
int lightSamples;
float ambient[3];
float *pictureBufferFloat;
GLubyte *pictureBufferByte;
KDNode *kdRoot;
float ambient[3];


void setProjection ()
{
   glViewport (0, 0, screenX, screenY);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   switch (mode)
   {
      case VIEWER:
         gluPerspective (camera.angleOfView, ((double) screenX / (double) screenY), camera.clipNear, camera.clipFar);
         break;
      case RAYTRACER:
         gluOrtho2D (0.0, (double) screenX, 0.0, (double) screenY);
         break;
   }
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();
}


void changeSize (int w, int h)
{
   if (w == 0 || h == 0)
   {
      return;
   }

   screenX = w;
   screenY = h;

   pictureBufferByte = (GLubyte *) frealloc (pictureBufferByte, screenX*screenY*3 * sizeof (GLubyte));
   pictureBufferFloat = (float *) frealloc (pictureBufferFloat, screenX*screenY*3 * sizeof (float));
   mode = VIEWER;
   setProjection ();
}


void displayScene (void)
{
   triangleList *l;
   lightList *k;
   int i;

   switch(view)
   {
      case SOLID:
         glBegin (GL_TRIANGLES);
         for (l = scena.triangles.next; l != NULL; l = l->next)
         {
            glColor3f (l->trg->m->rgb_disp[0], l->trg->m->rgb_disp[1], l->trg->m->rgb_disp[2]);
            for (i = 0; i < 3; i++)
            {
               glVertex3f (l->trg->vs[i][0], l->trg->vs[i][1], l->trg->vs[i][2]);
            }
         }
         glEnd();
         break;
      case WIREFRAME:
         for (l = scena.triangles.next; l != NULL; l = l->next)
         {
            glBegin (GL_LINE_LOOP);
            glColor3f (l->trg->m->rgb_disp[0], l->trg->m->rgb_disp[1], l->trg->m->rgb_disp[2]);
            for (i = 0; i < 3; i++)
            {
               glVertex3f (l->trg->vs[i][0], l->trg->vs[i][1], l->trg->vs[i][2]);
            }
            glEnd();
         }
         break;
      case LIGHTS:
         glBegin (GL_TRIANGLES);
         for (k = scena.lights.next; k != NULL; k = k->next)
         {
            for (l = k->lgh->lightTrgs.next; l != NULL; l = l->next)
            {
               glColor3f (l->trg->m->rgb_disp[0], l->trg->m->rgb_disp[1], l->trg->m->rgb_disp[2]);
               for (i = 0; i < 3; i++)
               {
                  glVertex3f (l->trg->vs[i][0], l->trg->vs[i][1], l->trg->vs[i][2]);
               }
            }
         }
         glEnd();
         break;
   }
   cameraRender(&camera);
}


void displayBuffer ()
{
   glRasterPos2i (0, 0);
   glDrawPixels (screenX, screenY, GL_RGB, GL_UNSIGNED_BYTE, pictureBufferByte);
}


void displayFunction ()
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   switch (mode)
   {
      case VIEWER:
         displayScene ();
         break;
      case RAYTRACER:
         displayBuffer ();
         break;
   }
   glutSwapBuffers();
}


void pressSpecialKey (int key, int x, int y)
{
   if (mode == VIEWER)
   {
      switch (key)
      {
         case GLUT_KEY_LEFT:
            cameraMoveRight (&camera, -0.1);
            break;
         case GLUT_KEY_RIGHT:
            cameraMoveRight (&camera, 0.1);
            break;
         case GLUT_KEY_HOME:
            cameraMoveUpward (&camera, 0.1);
            break;
         case GLUT_KEY_END:
            cameraMoveUpward (&camera, -0.1);
            break;
         case GLUT_KEY_UP:
            cameraMoveForward (&camera, 0.1);
            break;
         case GLUT_KEY_DOWN:
            cameraMoveForward (&camera, -0.1);
            break;
      }
   }
}


void pressNormalKey (unsigned char key, int x, int y)
{
   if (mode == VIEWER)
   {
      switch (key)
      {
         case 'w':
            cameraRotateX (&camera, 1.0);
            break;
         case 's':
            cameraRotateX (&camera, -1.0);
            break;
         case 'a':
            cameraRotateY (&camera, 1.0);
            break;
         case 'd':
            cameraRotateY (&camera, -1.0);
            break;
         case 'z':
            cameraRotateZ (&camera, 1.0);
            break;
         case 'x':
            cameraRotateZ (&camera, -1.0);
            break;
         case 'l':
            cameraZoom (&camera, 1.0);
            mode = VIEWER;
            break;
         case 'o':
            cameraZoom (&camera, -1.0);
            break;
         case 'b':
            view = SOLID;
            break;
         case 'n':
            view = WIREFRAME;
            break;
         case 'm':
            view = LIGHTS;
            break;
      }
   }

   switch (key)
   {
      case 27: 
         exit(0);
         break;
      case 'r': 
         mode = RAYTRACER;
         setProjection ();
         raytracing ();
         toneMapping ();
         break;
      case 't': 
         mode = VIEWER;
         setProjection ();
         break;
      case 'v': 
         saveConfig ();
         break;
   }
}


void mainInteractive (int argc, char **argv)
{
   int i;

   if (argc < 3)
   {
      error (BAD_USAGE);
   }

   mode = VIEWER;
   view = SOLID;

   screenX = DEFAULT_SCREENX;
   screenY = DEFAULT_SCREENY;
   raytracingDepth = DEFAULT_RAYTRACING_DEPTH;
   pathsPerPixel = DEFAULT_PATHS_PER_PIXEL;
   lightSamples = DEFAULT_LIGHT_SAMPLES;
   tone_mapping = INTERACTIVE_MAP;

   printf ("loading MGF...\n");
   for (i = 2; i < argc; i++)
   {
      if (mg_load (argv[i]) != MG_OK)
      {
         exit (1);
      }
   }
   mg_clear ();
   printf ("%d materials, %d triangles, %d lights\n", scena.materialsNum, scena.trianglesNum, scena.lightsNum);

   glutInit (&argc, argv);
   glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
   glutInitWindowSize (screenX, screenY);
   glutInitWindowPosition (INITX, INITY);
   glutCreateWindow ("rayon");

   glClearColor (0.0, 0.0, 0.25, 0.0);
   glEnable (GL_DEPTH_TEST);
   glShadeModel (GL_FLAT);
   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

   glutKeyboardFunc (pressNormalKey);
   glutSpecialFunc (pressSpecialKey);
   glutDisplayFunc (displayFunction);
   glutIdleFunc (displayFunction);
   glutReshapeFunc (changeSize);

   prepareRaytracing ();
   pictureBufferFloat = (float *) fmalloc (screenX*screenY*3 * sizeof (float));
   pictureBufferByte = (GLubyte *) fmalloc (screenX*screenY*3 * sizeof (GLubyte));
   cameraMove (&camera, makeVector ((kdRoot->minCoords[0] + kdRoot->maxCoords[0]) / 2.0,
                                    (kdRoot->minCoords[1] + kdRoot->maxCoords[1]) / 2.0,
                                    (kdRoot->minCoords[2] + kdRoot->maxCoords[2]) / 2.0));

   glutMainLoop ();
}


void mainBatchRaytracer (int argc, char **argv)
{
   int i;

   if (argc < 4)
   {
      error (BAD_USAGE);
   }

   mode = BATCH_RAYTRACER;

   loadConfig (argv[1]);

   for (i = 2; i < argc-1; i++)
   {
      if (mg_load (argv[i]) != MG_OK)
      {
         exit (1);
      }
   }
   mg_clear ();

   prepareRaytracing ();
   pictureBufferFloat = (float *) fmalloc (screenX*screenY*3 * sizeof (float));

   raytracing ();

   if (tone_mapping == MAP_HD)
   {
      saveRadiance (argv[argc-1]);
   }
   else
   {
      pictureBufferByte = (GLubyte *) fmalloc (screenX*screenY*3 * sizeof (GLubyte));
      toneMapping ();
      savePPM (argv[argc-1]);
   }
}


void mainToneMapper (int argc, char **argv)
{
   int i;

   if (argc < 4)
   {
      error (BAD_USAGE);
   }

   mode = TONE_MAPPER;

   if (strcmp (argv[1], "-ev") == 0)
   {
      tone_mapping = MAP_EV;
   }
   else if (strcmp (argv[1], "-tm") == 0)
   {
      tone_mapping = MAP_TM;
   }
   else if (strcmp (argv[1], "-tw") == 0)
   {
      tone_mapping = MAP_TW;
   }
   else if (strcmp (argv[1], "-tg") == 0)
   {
      tone_mapping = MAP_TG;
   }
   else if (strcmp (argv[1], "-tl") == 0)
   {
      tone_mapping = MAP_TL;
   }

   loadRadiance (argv[2]);
   toneMapping ();
   savePPM (argv[3]);
}


int main (int argc, char **argv)
{
   cameraInit (&camera);
   sceneInit (&scena);
   tobj = gluNewTess ();
   setCallbacks ();
   mg_nqcdivs = NQCDIVS;
   mg_init ();

   if (argc < 2)
   {
      error (BAD_USAGE);
   }

   if (strcmp (argv[1], "-i") == 0)
   {
      mainInteractive (argc, argv);
   }
   else if ((strcmp (argv[1], "-ev") == 0)
         || (strcmp (argv[1], "-tm") == 0)
         || (strcmp (argv[1], "-tw") == 0)
         || (strcmp (argv[1], "-tg") == 0)
         || (strcmp (argv[1], "-tl") == 0))
   {
      mainToneMapper (argc, argv);
   }
   else
   {
      mainBatchRaytracer (argc, argv);
   }

   exit (0);
}

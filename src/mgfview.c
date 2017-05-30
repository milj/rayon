
#include "headers.h"


extern scene scena;
extern GLUtesselator  *tobj;

double v[3][3];
int ile;
material *currentMaterial;
char currentObjectName[128];
bool currentObjectLightAdded;
light *currentLight;




material *makeMaterial (C_MATERIAL *cm)
{
   material *tmp;
   float max;
   float *col;

   tmp = (material *) fmalloc (sizeof (material));
   (*tmp).cm = *cm;

   mgf2rgb (&(cm->rd_c), (*tmp).rgb_rd_c);
   normalizeColour ((*tmp).rgb_rd_c, (*tmp).rgb_rd_c);

   mgf2rgb (&(cm->td_c), (*tmp).rgb_td_c);
   normalizeColour ((*tmp).rgb_td_c, (*tmp).rgb_td_c);

   mgf2rgb (&(cm->ed_c), (*tmp).rgb_ed_c);
   normalizeColour ((*tmp).rgb_ed_c, (*tmp).rgb_ed_c);

   mgf2rgb (&(cm->rs_c), (*tmp).rgb_rs_c);
   normalizeColour ((*tmp).rgb_rs_c, (*tmp).rgb_rs_c);

   mgf2rgb (&(cm->ts_c), (*tmp).rgb_ts_c);
   normalizeColour ((*tmp).rgb_ts_c, (*tmp).rgb_ts_c);

   if (cm->ed > LIGHT_THR)
   {
      max = 1.0;
      col = (*tmp).rgb_ed_c;
   }
   else
   {
      max = 0.0;
      if (cm->rd > max)
      {
         max = cm->rd;
         col = (*tmp).rgb_rd_c;
      }
      if (cm->rs > max)
      {
         max = cm->rs;
         col = (*tmp).rgb_rs_c;
      }
      if (cm->td > max)
      {
         max = cm->td;
         col = (*tmp).rgb_td_c;
      }
      if (cm->ts > max)
      {
         max = cm->ts;
         col = (*tmp).rgb_ts_c;
      }
   }
   multColour (col, max, (*tmp).rgb_disp);

   (*tmp).cm_org = cm;
   (*tmp).clock_org = cm->clock;
   return tmp;
}


material *addNewMaterial (C_MATERIAL *cm)
{
   material *mtr;

   mtr = makeMaterial (cm);
   sceneAddMaterial (&scena, mtr);
   return mtr;
}


material *materialPointer (C_MATERIAL *cm)
{
   materialList *l;

   for (l = scena.materials.next; l != NULL; l = l->next)
   {
      if (l->mtr->cm_org == cm)
      {
         if (l->mtr->clock_org == cm->clock)
         {

            return l->mtr;
         }
         else
         {
            return addNewMaterial (cm);
         }

      }
   }
   return addNewMaterial (cm);
}


triangle *makeTriangle (double vs[3][3], material *m)
{
   triangle *tmp;
   int i;

   tmp = (triangle *) fmalloc (sizeof (triangle));
   for (i = 0; i < 3; i++)
   {
      (*tmp).vs[i][0] = vs[i][0];
      (*tmp).vs[i][1] = vs[i][1];
      (*tmp).vs[i][2] = vs[i][2];
   }
   (*tmp).m = currentMaterial;
   return tmp;
}


light *makeLight (double v[3], material *m)
{
   light *tmp;
   int i;

   tmp = (light *) fmalloc (sizeof (light));
   (*tmp).v[0] = v[0];
   (*tmp).v[1] = v[1];
   (*tmp).v[2] = v[2];
   triangleListInit (&((*tmp).lightTrgs));
   (*tmp).lightTrgsNum = 0;
   (*tmp).m = currentMaterial;
   return tmp;
}


void checkCurrentObject ()
{
   if (obj_nnames == 0)
   {
      currentObjectName[0] = 0;
      currentObjectLightAdded = FALSE;
   }
   else if (strcmp (currentObjectName, obj_name[obj_nnames-1]) != 0)
   {
      strcpy (currentObjectName, obj_name[obj_nnames-1]);
      currentObjectLightAdded = FALSE;
   }
}


int faceHandler (int ac, char **av)
{
   C_VERTEX *vp;
   FVECT vert, vert1;
   double *vert2;
   int i;

   if (ac < 4)
   {
      return(MG_EARGC);
   }

   setVector (0.0, 0.0, 0.0, vert1);
   currentMaterial = materialPointer (c_cmaterial);
   checkCurrentObject ();
   if ((currentMaterial->cm.ed > LIGHT_THR) && (!currentObjectLightAdded))
   {
      for (i = 1; i < ac; i++)
      {
         if ((vp = c_getvert(av[i])) == NULL)
         {
            return(MG_EUNDEF);
         }
         xf_xfmpoint(vert, vp->p);

         addVectors (vert1, vert, vert1);
      }
      multVector (vert1, 1.0 / (double) (ac - 1), vert1);
      currentLight = makeLight (vert1, currentMaterial);
      sceneAddLight (&scena, currentLight);
      currentObjectLightAdded = TRUE;
   }

   gluTessBeginPolygon(tobj, NULL);
   gluTessBeginContour(tobj);
   for (i = 1; i < ac; i++)
   {
      if ((vp = c_getvert(av[i])) == NULL)
      {
         return(MG_EUNDEF);
      }
      xf_xfmpoint(vert, vp->p);

      vert2 = (double *) fmalloc(3 * sizeof(double));
      setVector (vert[0], vert[1], vert[2], vert2);

      gluTessVertex(tobj, vert2, vert2);
   }
   gluTessEndContour(tobj);
   gluTessEndPolygon(tobj);
   return(MG_OK);
}


int sphHandler (int ac, char **av)
{
   C_VERTEX *vp;
   FVECT vert;

   if (ac != 3)
   {
      return(MG_EARGC);
   }
   currentMaterial = materialPointer (c_cmaterial);
   checkCurrentObject ();
   if ((currentMaterial->cm.ed > LIGHT_THR) && (!currentObjectLightAdded))
   {
      if ((vp = c_getvert(av[1])) == NULL)
      {
         return(MG_EUNDEF);
      }
      xf_xfmpoint(vert, vp->p);

      currentLight = makeLight (vert, currentMaterial);
      sceneAddLight (&scena, currentLight);
      currentObjectLightAdded = TRUE;
   }
   return e_sph (ac, av);
}


int ringHandler (int ac, char **av)
{
   C_VERTEX *vp;
   FVECT vert;

   if (ac != 4)
   {
      return(MG_EARGC);
   }
   currentMaterial = materialPointer (c_cmaterial);
   checkCurrentObject ();
   if ((currentMaterial->cm.ed > LIGHT_THR) && (!currentObjectLightAdded))
   {
      if ((vp = c_getvert(av[1])) == NULL)
      {
         return(MG_EUNDEF);
      }
      xf_xfmpoint(vert, vp->p);

      currentLight = makeLight (vert, currentMaterial);
      sceneAddLight (&scena, currentLight);
      currentObjectLightAdded = TRUE;
   }
   return e_ring (ac, av);
}


int cylHandler (int ac, char **av)
{
   C_VERTEX *vp;
   FVECT vert1, vert2, vert;

   if (ac != 4)
   {
      return(MG_EARGC);
   }
   currentMaterial = materialPointer (c_cmaterial);
   checkCurrentObject ();
   if ((currentMaterial->cm.ed > LIGHT_THR) && (!currentObjectLightAdded))
   {
      if ((vp = c_getvert(av[1])) == NULL)
      {
         return(MG_EUNDEF);
      }
      xf_xfmpoint(vert1, vp->p);
      if ((vp = c_getvert(av[3])) == NULL)
      {
         return(MG_EUNDEF);
      }
      xf_xfmpoint(vert2, vp->p);

      setVector ((vert1[0]+vert2[0])/2.0, (vert1[1]+vert2[1])/2.0, (vert1[2]+vert2[2])/2.0, vert);

      currentLight = makeLight (vert, currentMaterial);
      sceneAddLight (&scena, currentLight);
      currentObjectLightAdded = TRUE;
   }
   return e_cyl (ac, av);
}


int torusHandler (int ac, char **av)
{
   C_VERTEX *vp;
   FVECT vert;

   if (ac != 4)
   {
      return(MG_EARGC);
   }
   currentMaterial = materialPointer (c_cmaterial);
   checkCurrentObject ();
   if ((currentMaterial->cm.ed > LIGHT_THR) && (!currentObjectLightAdded))
   {
      if ((vp = c_getvert(av[1])) == NULL)
      {
         return(MG_EUNDEF);
      }
      xf_xfmpoint(vert, vp->p);

      currentLight = makeLight (vert, currentMaterial);
      sceneAddLight (&scena, currentLight);
      currentObjectLightAdded = TRUE;
   }
   return e_torus (ac, av);
}


int coneHandler (int ac, char **av)
{
   C_VERTEX *vp;
   FVECT vert1, vert2, vert;

   if (ac != 5)
   {
      return(MG_EARGC);
   }
   currentMaterial = materialPointer (c_cmaterial);
   checkCurrentObject ();
   if ((currentMaterial->cm.ed > LIGHT_THR) && (!currentObjectLightAdded))
   {
      if ((vp = c_getvert(av[1])) == NULL)
      {
         return(MG_EUNDEF);
      }
      xf_xfmpoint(vert1, vp->p);
      if ((vp = c_getvert(av[3])) == NULL)
      {
         return(MG_EUNDEF);
      }
      xf_xfmpoint(vert2, vp->p);

      setVector ((vert1[0]+vert2[0])/2.0, (vert1[1]+vert2[1])/2.0, (vert1[2]+vert2[2])/2.0, vert);

      currentLight = makeLight (vert, currentMaterial);
      sceneAddLight (&scena, currentLight);
      currentObjectLightAdded = TRUE;
   }
   return e_cone (ac, av);
}


int prismHandler (int ac, char **av)
{
   C_VERTEX *vp;
   FVECT vert, vert1;
   int i;

   if (ac < 5)
   {
      return(MG_EARGC);
   }

   setVector (0.0, 0.0, 0.0, vert1);
   currentMaterial = materialPointer (c_cmaterial);
   checkCurrentObject ();
   if ((currentMaterial->cm.ed > LIGHT_THR) && (!currentObjectLightAdded))
   {
      for (i = 1; i < ac-1; i++)
      {
         if ((vp = c_getvert(av[i])) == NULL)
         {
            return(MG_EUNDEF);
         }
         xf_xfmpoint(vert, vp->p);

         addVectors (vert1, vert, vert1);
      }
      multVector (vert1, 1.0 / (double) (ac - 2), vert1);
      currentLight = makeLight (vert1, currentMaterial);
      sceneAddLight (&scena, currentLight);
      currentObjectLightAdded = TRUE;
   }
   return e_prism (ac, av);
}


void vertexCallback (void *vertex)
{
   triangle *trg;
   const double *pv;
   pv = (double *) vertex;

   v[ile][0] = pv[0];
   v[ile][1] = pv[1];
   v[ile][2] = pv[2];
   ile++;
   if (ile == 3)
   {
      trg = makeTriangle (v, currentMaterial);
      sceneAddTriangle (&scena, trg);
      if (currentMaterial->cm.ed > LIGHT_THR)
      {
         triangleListAdd (&(currentLight->lightTrgs), trg);
         currentLight->lightTrgsNum++;
      }
      ile = 0;
   }
}


void edgeFlagCallback (GLboolean flag)
{
   ;
}


void beginCallback (GLenum which)
{
   ile = 0;
}


void endCallback (void)
{
   ;
}


void errorCallback (GLenum errorCode)
{
   const GLubyte *estring;
   estring = gluErrorString(errorCode);
   fprintf (stderr, "Tessellation Error: %s\n", estring);
   exit (0);
}


void combineCallback (double coords[3], double *vertex_data[4], float weight[4], double **dataOut)
{
   double *vertex;
   int i;

   vertex = (double *) fmalloc (3 * sizeof (double));
   setVector (coords[0], coords[1], coords[2], vertex);
   *dataOut = vertex;
}


void setCallbacks ()
{
   gluTessCallback (tobj, GLU_TESS_VERTEX, (GLvoid (*) ()) &vertexCallback);
   gluTessCallback (tobj, GLU_TESS_EDGE_FLAG, (GLvoid (*) ()) &edgeFlagCallback);
   gluTessCallback (tobj, GLU_TESS_BEGIN, (GLvoid (*) ()) &beginCallback);
   gluTessCallback (tobj, GLU_TESS_END, (GLvoid (*) ()) &endCallback);
   gluTessCallback (tobj, GLU_TESS_ERROR, (GLvoid (*) ()) &errorCallback);
   gluTessCallback (tobj, GLU_TESS_COMBINE, (GLvoid (*) ()) &combineCallback);
   gluTessProperty (tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);

   mg_ehand[MG_E_COLOR] = c_hcolor;
   mg_ehand[MG_E_CMIX] = c_hcolor;
   mg_ehand[MG_E_CXY] = c_hcolor;
   mg_ehand[MG_E_NORMAL] = c_hvertex;
   mg_ehand[MG_E_POINT] = c_hvertex;
   mg_ehand[MG_E_VERTEX] = c_hvertex;
   mg_ehand[MG_E_ED] = c_hmaterial;
   mg_ehand[MG_E_IR] = c_hmaterial;
   mg_ehand[MG_E_MATERIAL] = c_hmaterial;
   mg_ehand[MG_E_RD] = c_hmaterial;
   mg_ehand[MG_E_RS] = c_hmaterial;
   mg_ehand[MG_E_SIDES] = c_hmaterial;
   mg_ehand[MG_E_TD] = c_hmaterial;
   mg_ehand[MG_E_TS] = c_hmaterial;
   mg_ehand[MG_E_OBJECT] = obj_handler;
   mg_ehand[MG_E_XF] = xf_handler;
   mg_ehand[MG_E_FACE] = faceHandler;
   mg_ehand[MG_E_SPH] = sphHandler;
   mg_ehand[MG_E_RING] = ringHandler;
   mg_ehand[MG_E_CYL] = cylHandler;
   mg_ehand[MG_E_TORUS] = torusHandler;
   mg_ehand[MG_E_CONE] = coneHandler;
   mg_ehand[MG_E_PRISM] = prismHandler;
   mg_uhand = NULL;
}


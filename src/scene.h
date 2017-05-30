

typedef struct
{
   C_MATERIAL cm;
   float rgb_rd_c[3];
   float rgb_td_c[3];
   float rgb_ed_c[3];
   float rgb_rs_c[3];
   float rgb_ts_c[3];
   float rgb_disp[3];
   float ed_norm;
   C_MATERIAL *cm_org;
   int clock_org;
}
material;


typedef struct mt_ls
{ 
   material *mtr;
   struct mt_ls *next;
}
materialList;


typedef struct
{
   double vs[3][3];
   double n[3];
   material *m;
   unsigned int lastRayID;
   double area;
}
triangle;


typedef struct tr_ls
{ 
   triangle *trg;
   struct tr_ls *next;
}
triangleList;


typedef struct
{
   double v[3]; // srodek swiatla
   triangleList lightTrgs; // trojkaty swiatla
   unsigned int lightTrgsNum;
   material *m;
}
light;


typedef struct lt_ls
{ 
   light *lgh;
   struct lt_ls *next;
}
lightList;


typedef struct
{
   materialList materials;
   unsigned int materialsNum;
   triangleList triangles;
   unsigned int trianglesNum;
   lightList lights;
   unsigned int lightsNum;
}                                                                                             
scene;


void materialListInit (materialList *);
void materialListAdd (materialList *, material *);
unsigned int triangleListLength (triangleList);
void triangleListInit (triangleList *);
void triangleListAdd (triangleList *, triangle *);
void lightListInit (lightList *);
void lightListAdd (lightList *, light *);
void sceneInit (scene *);
void sceneAddMaterial (scene *, material *);
void sceneAddTriangle (scene *, triangle *);
void sceneAddLight (scene *, light *);

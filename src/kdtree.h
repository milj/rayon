
typedef struct kd_tn
{
   double minCoords[3];
   double maxCoords[3];
   double minCEPS[3];
   double maxCEPS[3];
   triangleList triangles;
   unsigned int trianglesNum;
   int dividingAxis;
   struct kd_tn *childL;
   struct kd_tn *childR;
}
KDNode;


typedef struct
{
   KDNode *node;
   double a;
   double b;
}
KDStackElem;


void calculateBoundingBox (triangleList, double *, double *);
void enlargeBoundingBox (double *, double *, double *, double *);
bool pointInBox (double *, double *, double *);
bool intersectionRayPlane (double *, double *, double, int, double *);
bool intersectionRayBox (double *, double *, double *, double *, double *, double *);
bool testEdge (double, double, double, double *, triangle *);
bool intersectionTriangleBox (triangle *, double *, double *);
int cmpDouble (const void *, const void *);
double calculateDivPoint (KDNode *, int);
void subdivide (KDNode *, int);
void buildKDTree ();
void intersectRayTree (triangleList, double *, double *, double, double, triangle **, double *, float *);

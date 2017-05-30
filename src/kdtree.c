
#include "headers.h"


extern scene scena;
extern int screenX;
extern int screenY;
extern KDNode *kdRoot;
double *table;


void calculateBoundingBox (triangleList k, double *minCoords, double *maxCoords)
{ 
   triangleList *l;
   int i;
   int j;

   setVector (FHUGE, FHUGE, FHUGE, minCoords);
   setVector (-FHUGE, -FHUGE, -FHUGE, maxCoords);

   for (l = k.next; l != NULL; l = l->next)
   {
      for (i = 0; i < 3; i++)                          // wierzcholki
      {
         for (j = 0; j < 3; j++)                       // wspolrzedne
         {
            if (l->trg->vs[i][j] < minCoords[j])
            { 
               minCoords[j] = l->trg->vs[i][j];
            }
            if (l->trg->vs[i][j] > maxCoords[j])
            { 
               maxCoords[j] = l->trg->vs[i][j];
            }
         }
      }
   }
} 


void enlargeBoundingBox (double *minCoords, double *maxCoords, double *minCEPS, double *maxCEPS)
{
   minCEPS[0] = minCoords[0] - EPS;
   maxCEPS[0] = maxCoords[0] + EPS;
   minCEPS[1] = minCoords[1] - EPS;
   maxCEPS[1] = maxCoords[1] + EPS;
   minCEPS[2] = minCoords[2] - EPS;
   maxCEPS[2] = maxCoords[2] + EPS;
}


bool pointInBox (double *point, double *minCoords, double *maxCoords)
{
   if ((point[0] > minCoords[0])
    && (point[0] < maxCoords[0])
    && (point[1] > minCoords[1])
    && (point[1] < maxCoords[1])
    && (point[2] > minCoords[2])
    && (point[2] < maxCoords[2]))
   { 
      return TRUE;
   }
   return FALSE;
}


bool intersectionRayPlane (double *position, double *normalizedRay, double coord, int axis, double *distance)
{
   double r;
 
   r = normalizedRay[axis];
   if (r == 0.0)
   {
      r = EPS;
   }

   *distance = (coord - position[axis]) / r;

   if (*distance > 0.0)
   {
      return TRUE;
   }
   return FALSE;
}


bool intersectionRayBox (double *position, double *normalizedRay, double *minCoords, double *maxCoords, double *near, double *far)
{
   double d1, d2;

   *near = -FHUGE;
   *far = FHUGE;

   intersectionRayPlane (position, normalizedRay, minCoords[0], 0, &d1);
   intersectionRayPlane (position, normalizedRay, maxCoords[0], 0, &d2);
   *near = MAX (*near, MIN(d1,d2));
   *far = MIN (*far, MAX(d1,d2));

   intersectionRayPlane (position, normalizedRay, minCoords[1], 1, &d1);
   intersectionRayPlane (position, normalizedRay, maxCoords[1], 1, &d2);
   *near = MAX (*near, MIN(d1,d2));
   *far = MIN (*far, MAX(d1,d2));

   intersectionRayPlane (position, normalizedRay, minCoords[2], 2, &d1);
   intersectionRayPlane (position, normalizedRay, maxCoords[2], 2, &d2);
   *near = MAX (*near, MIN(d1,d2));
   *far = MIN (*far, MAX(d1,d2));

   if (*near <= *far)
   {
      return TRUE;
   }
   return FALSE;
}


bool testEdge (double a, double b, double c, double *ray, triangle *trg)
{
   double pos[3];
   double point[3];
   double dist;

   setVector (a, b, c, pos);
   if (intersectionRayTriangle (pos, ray, trg, point, &dist))
   {
      if (dist < 1.0)
      {
         return TRUE;
      }
   }
   return FALSE;
}


bool intersectionTriangleBox (triangle *trg, double *minCoords, double *maxCoords)
{
   double ray[3];
   double d1;
   double d2;
   int i;

   for (i = 0; i < 3; i++)                          // wierzcholki
   {
      if (pointInBox (trg->vs[i], minCoords, maxCoords))
      {
         return TRUE;
      }
   }

   setVector (maxCoords[0] - minCoords[0], 0.0, 0.0, ray);
   if (testEdge (minCoords[0], minCoords[1], minCoords[2], ray, trg)
    || testEdge (minCoords[0], maxCoords[1], minCoords[2], ray, trg)
    || testEdge (minCoords[0], minCoords[1], maxCoords[2], ray, trg)
    || testEdge (minCoords[0], maxCoords[1], maxCoords[2], ray, trg))
   {
      return TRUE;
   }
   setVector (0.0, maxCoords[1] - minCoords[1], 0.0, ray);
   if (testEdge (minCoords[0], minCoords[1], minCoords[2], ray, trg)
    || testEdge (maxCoords[0], minCoords[1], minCoords[2], ray, trg)
    || testEdge (minCoords[0], minCoords[1], maxCoords[2], ray, trg)
    || testEdge (maxCoords[0], minCoords[1], maxCoords[2], ray, trg))
   {
      return TRUE;
   }
   setVector (0.0, 0.0, maxCoords[2] - minCoords[2], ray);
   if (testEdge (minCoords[0], minCoords[1], minCoords[2], ray, trg)
    || testEdge (maxCoords[0], minCoords[1], minCoords[2], ray, trg)
    || testEdge (minCoords[0], maxCoords[1], minCoords[2], ray, trg)
    || testEdge (maxCoords[0], maxCoords[1], minCoords[2], ray, trg))
   {
      return TRUE;
   }

   subVectors (trg->vs[1], trg->vs[0], ray);
   if (intersectionRayBox (trg->vs[0], ray, minCoords, maxCoords, &d1, &d2))
   {
      if (((0.0 < d1) && (d1 < 1.0)) || ((0.0 < d2) && (d2 < 1.0)))
      {
         return TRUE;
      }
   }
   subVectors (trg->vs[2], trg->vs[0], ray);
   if (intersectionRayBox (trg->vs[0], ray, minCoords, maxCoords, &d1, &d2))
   {
      if (((0.0 < d1) && (d1 < 1.0)) || ((0.0 < d2) && (d2 < 1.0)))
      {
         return TRUE;
      }
   }
   subVectors (trg->vs[2], trg->vs[1], ray);
   if (intersectionRayBox (trg->vs[1], ray, minCoords, maxCoords, &d1, &d2))
   {
      if (((0.0 < d1) && (d1 < 1.0)) || ((0.0 < d2) && (d2 < 1.0)))
      {
         return TRUE;
      }
   }

   return FALSE;
}


int cmpDouble (const void *aa, const void *bb)
{
   double *a;
   double *b;

   a = (double *) aa;
   b = (double *) bb;

   if (*a < *b)
   {
      return -1;
   }
   else if (*a > *b)
   {
      return 1;
   }
   else
   {
      return 0;
   }
}


double calculateDivPoint (KDNode *node, int currentAxis)
{
   double spatial;
   double triangle;
   double val;
   triangleList *l;
   int i, k;

   k = 0;
   for (l = node->triangles.next; l != NULL; l = l->next)
   {
      for (i = 0; i < 3; i++)
      {
         table[k++] = l->trg->vs[i][node->dividingAxis];
      }
   }
   qsort (table, k, sizeof (double), cmpDouble);

   spatial = 0.5*node->minCoords[node->dividingAxis] + 0.5*node->maxCoords[node->dividingAxis];

   triangle = table[k/2];

   if ((triangle > node->maxCoords[node->dividingAxis])
    || (triangle < node->minCoords[node->dividingAxis]))
   {
      val = spatial;
   }
   else
   {
      val = 0.4*spatial + 0.6*triangle;
   }

   return val;
}


void subdivide (KDNode *currentNode, int currentDepth)
{
   triangleList *l;
   double divPoint;

   if ((currentNode->trianglesNum > KD_MIN_NUM) && (currentDepth <= KD_DEPTH))
   {
      currentNode->childL = (KDNode *) fmalloc (sizeof (KDNode));
      currentNode->childR = (KDNode *) fmalloc (sizeof (KDNode));

      copyVector (currentNode->minCoords, currentNode->childL->minCoords);
      copyVector (currentNode->maxCoords, currentNode->childL->maxCoords);
      triangleListInit (&(currentNode->childL->triangles));
      currentNode->childL->trianglesNum = 0;

      copyVector (currentNode->minCoords, currentNode->childR->minCoords);
      copyVector (currentNode->maxCoords, currentNode->childR->maxCoords);
      triangleListInit (&(currentNode->childR->triangles));
      currentNode->childR->trianglesNum = 0;

      if (currentNode->dividingAxis == 0)
      {
         divPoint = calculateDivPoint (currentNode, 0);
         currentNode->childL->maxCoords[0] = divPoint;
         currentNode->childR->minCoords[0] = divPoint;
         currentNode->childL->dividingAxis = 1;
         currentNode->childR->dividingAxis = 1;
      }
      else if (currentNode->dividingAxis == 1)
      {
         divPoint = calculateDivPoint (currentNode, 1);
         currentNode->childL->maxCoords[1] = divPoint;
         currentNode->childR->minCoords[1] = divPoint;
         currentNode->childL->dividingAxis = 2;
         currentNode->childR->dividingAxis = 2;
      }
      else if (currentNode->dividingAxis == 2)
      {
         divPoint = calculateDivPoint (currentNode, 2);
         currentNode->childL->maxCoords[2] = divPoint;
         currentNode->childR->minCoords[2] = divPoint;
         currentNode->childL->dividingAxis = 0;
         currentNode->childR->dividingAxis = 0;
      }

      enlargeBoundingBox (currentNode->childL->minCoords,
                          currentNode->childL->maxCoords,
                          currentNode->childL->minCEPS,
                          currentNode->childL->maxCEPS);
      enlargeBoundingBox (currentNode->childR->minCoords,
                          currentNode->childR->maxCoords,
                          currentNode->childR->minCEPS,
                          currentNode->childR->maxCEPS);

      currentNode->childL->childL = NULL;
      currentNode->childL->childR = NULL;
      currentNode->childR->childL = NULL;
      currentNode->childR->childR = NULL;

      for (l = currentNode->triangles.next; l != NULL; l = l->next)
      {
         if (intersectionTriangleBox (l->trg, currentNode->childL->minCEPS, currentNode->childL->maxCEPS))
         {
            triangleListAdd (&(currentNode->childL->triangles), l->trg);
            currentNode->childL->trianglesNum++;
         }
         if (intersectionTriangleBox (l->trg, currentNode->childR->minCEPS, currentNode->childR->maxCEPS))
         {
            triangleListAdd (&(currentNode->childR->triangles), l->trg);
            currentNode->childR->trianglesNum++;
         }
      }

      subdivide (currentNode->childL, currentDepth + 1);
      subdivide (currentNode->childR, currentDepth + 1);
   }
}


void buildKDTree ()
{
   kdRoot = (KDNode *) fmalloc (sizeof (KDNode));
   kdRoot->childL = NULL;
   kdRoot->childR = NULL;
   calculateBoundingBox ((&scena)->triangles, kdRoot->minCoords, kdRoot->maxCoords);
   enlargeBoundingBox (kdRoot->minCoords, kdRoot->maxCoords, kdRoot->minCEPS, kdRoot->maxCEPS);
   kdRoot->triangles = (&scena)->triangles;
   kdRoot->trianglesNum = scena.trianglesNum;
   kdRoot->dividingAxis = 0;
   table = (double *) fmalloc (3 * kdRoot->trianglesNum * sizeof (double));
   subdivide (kdRoot, 1);
}


void intersectRayTree (triangleList list, double *position, double *normalizedRay, double min, double max, triangle **minDistTriangle, double *minDistPoint, float *lightColour)
{
   KDStackElem stack[KD_MAX_DEPTH];
   unsigned int stackPtr = 0;
   double t;
   double a;
   double b;
   KDNode *node;
   KDNode *near;
   KDNode *far;
   double shift;
   double aa;
   double bb;

   if (!intersectionRayBox (position, normalizedRay, kdRoot->minCEPS, kdRoot->maxCEPS, &a, &b))
   {
      *minDistTriangle = NULL;
   }
   else
   {
      stack[stackPtr].node = kdRoot;
      stack[stackPtr].a = a;
      stack[stackPtr].b = b;
      stackPtr++;

      while (stackPtr > 0)
      {
         stackPtr--;
         node = stack[stackPtr].node;
         a = stack[stackPtr].a;
         b = stack[stackPtr].b;

         while (node->childL != NULL)
         {
            shift = 0.0;
            if (fabs(position[node->dividingAxis] - node->childL->maxCoords[node->dividingAxis]) < EPS)
            {
               if (position[node->dividingAxis] < node->childL->maxCoords[node->dividingAxis])
               {
                  shift = EPS;
               }
               else
               {
                  shift = -EPS;
               }
            }
            intersectionRayPlane (position, normalizedRay, node->childL->maxCoords[node->dividingAxis] + shift, node->dividingAxis, &t);
            if (position[node->dividingAxis] < node->childL->maxCoords[node->dividingAxis] + shift)
            {
               near = node->childL;
               far = node->childR;
            }
            else
            {
               near = node->childR;
               far = node->childL;
            }

            if ((t > b) || (t < 0.0))
            {
               node = near;
            }
            else if (t < a)
            {
               node = far;
            }
            else
            {
               stack[stackPtr].node = far;
               stack[stackPtr].a = t;
               stack[stackPtr].b = b;
               stackPtr++;

               node = near;
               b = t;
            }
         }

         if (node->trianglesNum == 0)
         {
            *minDistTriangle = NULL;
         }
         else
         {
            if ((lightColour == NULL))
            {
               aa = MAX(0,min);
               bb = MIN(b,max);
               intersectRay (node->triangles, position, normalizedRay, aa, bb, minDistTriangle, minDistPoint, lightColour);
               if (*minDistTriangle != NULL)
               {
                  return;
               }
            }
            else
            {
               aa = MAX(a,min);
               bb = MIN(b,max);
               intersectRay (node->triangles, position, normalizedRay, aa, bb, minDistTriangle, minDistPoint, lightColour);
               if ((lightColour[0] < EPS) && (lightColour[1] < EPS) && (lightColour[2] < EPS))
               {
                  return;
               }
            }
	 }
      }
   }
}


#include "headers.h"


void materialListInit (materialList *lista)
{
   lista->mtr = NULL;
   lista->next = NULL;   
}


void materialListAdd (materialList *lista, material *mtr)
{
   materialList *l;

   l = (materialList *) fmalloc (sizeof (materialList));
   l->mtr = mtr;
   l->next = lista->next;
   lista->next = l;
}


void triangleListInit (triangleList *lista)
{
   lista->trg = NULL;
   lista->next = NULL;   
}


void triangleListAdd (triangleList *lista, triangle *trg)
{
   triangleList *l;

   l = (triangleList *) fmalloc (sizeof (triangleList));
   l->trg = trg;
   l->next = lista->next;
   lista->next = l;
}


unsigned int triangleListLength (triangleList list)
{
   triangleList *l;
   unsigned int length = 0;

   for (l = list.next; l != NULL; l = l->next)
   {
      length++;
   }
   return length;
}


void lightListInit (lightList *lista)
{
   lista->lgh = NULL;
   lista->next = NULL;   
}


void lightListAdd (lightList *lista, light *lgh)
{
   lightList *l;

   l = (lightList *) fmalloc (sizeof (lightList));
   l->lgh = lgh;
   l->next = lista->next;
   lista->next = l;
}


void sceneInit (scene *s)
{
   materialListInit (&(s->materials));
   s->materialsNum = 0;
   triangleListInit (&(s->triangles));
   s->trianglesNum = 0;
   lightListInit (&(s->lights));
   s->lightsNum = 0;
}


void sceneAddMaterial (scene *s, material *mtr)
{
   materialListAdd (&(s->materials), mtr);
   s->materialsNum++;
}


void sceneAddTriangle (scene *s, triangle *trg)
{
   triangleListAdd (&(s->triangles), trg);
   s->trianglesNum++;
}


void sceneAddLight (scene *s, light *lgh)
{
   lightListAdd (&(s->lights), lgh);
   s->lightsNum++;
}

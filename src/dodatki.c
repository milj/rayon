// drobne procedury

#include "headers.h"



/*-------------------------------------------------------------- fmalloc -----*/


void *fmalloc (size_t s)                         // malloc sprawdzajacy, co dostal
{
   void *ptr = malloc (s);
   if (ptr == NULL)
   {
      error (MG_EMEM);
   }
   return ptr;
}





/*------------------------------------------------------------- frealloc -----*/


void *frealloc (void *p, size_t s)              // realloc sprawdzajacy, co dostal
{
   void *ptr = realloc (p, s);
   if (ptr == NULL)
   {
      error (MG_EMEM);
   }
   return ptr;
}





/*---------------------------------------------------------------- error -----*/


void error (error_type e)
{                                                // obsluga bledow
   switch (e)
   {
      case OUT_OF_MEM:
         fprintf (stderr, "out of memory\n");
         exit (5);
         break;
      case BAD_USAGE:
         fprintf (stderr, "usage: rayon rt.cfg scene1.mgf ... sceneN.mgf pic.(hdr|ppm)\n");
         fprintf (stderr, "       rayon -i scene1.mgf ... sceneN.mgf\n");
         fprintf (stderr, "       rayon -ev pic.hdr pic.ppm\n");
         fprintf (stderr, "       rayon -tm pic.hdr pic.ppm\n");
         fprintf (stderr, "       rayon -tw pic.hdr pic.ppm\n");
         fprintf (stderr, "       rayon -tg pic.hdr pic.ppm\n");
         fprintf (stderr, "       rayon -tl pic.hdr pic.ppm\n");
         exit (5);
         break;
   }
}

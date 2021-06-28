#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statfs.h>
 
const char* path = "/mnt";
int main()
{
   struct statfs s;
 
   int total = 0;
   int free = 0;

   memset(&s, 0, sizeof(struct statfs));
   
 
   if( 0 != statfs(path, &s) )
   {
	return -1;
   }
 
   if(s.f_bsize >= 1024)
   {
       printf("\n if(s.f_bsize >= 1024)\n");
       total = (int)(  (s.f_bsize / 1024 ) * s.f_blocks );
       free = (int) ( ( s.f_bsize / 1024 ) * s.f_bavail );
   }
   else
   {
	printf("\n if(s.f_bsize >= 1024)  else\n");
       total = (int)(  (s.f_blocks / 1024 ) * s.f_bsize );
       free = (int) ( ( s.f_bavail / 1024 ) * s.f_bsize );
   }
   
   printf("\n total [%d] KB    free[%d] KB\n", total, free);
   return 0;
}
 
 
#include <stdio.h>
#include <stdlib.h>

#define assert(test) \
    if (!(test)) {\
        fprintf(stderr, \
                "Assertion error in file %s, line %d, condition \"%s\"\n",\
                __FILE__,\
                __LINE__,\
                #test);\
        exit(1); \
    }

int main(){
printf("This program has been compiled at %s %s\n", __DATE__, __TIME__);
assert(5 < 0 );
printf("This string shouldn't be printed if the assertion fails.\n");
return 0;
}
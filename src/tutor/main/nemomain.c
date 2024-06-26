/*
 *	Example of a NEMO C program
 *
 *       On *Unix* to be compiled and linked as:
 *       
 *       cc -g -o main main.c $NEMOLIB/libnemo.a -lm
 * or:
 *	 cc -g -o main main.c -lnemo -lm
 *
 * within the NEMO environment.
 *     
 *
 *  22-oct-90	V1.0	Created		    PJT
 *  21-may-92	V1.1	added usage	    PJT
 *  12-feb-95   V1.2    added format=       PJT
 *  30-aug-23   V1.3    added a,b,c         PJT
 */

#include <stdinc.h>                             /* standard NEMO include */
#include <getparam.h>

string defv[] = {                               /* keywords definitions */
    "nmax=10\n          Number of iterations",
    "format=%20.10f\n	Format to print result",
    "a=1\n              the a number",
    "b=2\n              the b number",
    "c=3\n              the c number",        
    "VERSION=1.3\n      30-aug-2023 PJT",
    NULL,
};

string usage = "Example C program with nemo_main convention";	/* Usage */

void nemo_main(void)                            /* NEMO's program main entry */
{
    real a,b,c;         /* becomes 'float' or 'double' depending on compile flag */
    int   i, nmax;
    char fmt[64];

    nmax = getiparam("nmax");      /* obtain 'nmax' from commmand line */
    if (nmax<1) warning("%d: Unexpected value for nmax",nmax);
    dprintf(1,"Iteration counter = %d\n",nmax);
    
    a = 1.0;
    for (i=0; i<nmax; i++) {              /* loop the loop */
         a = a + a;
    }

    sprintf(fmt,"The sum is %s\n",getparam("format")); /* set format string */
    printf(fmt,a);                                         /* output */

    a = getrparam("a");
    b = getrparam("b");
    c = getrparam("c");
    printf("a=%g b=%g c=%g\n",a,b,c);
}

/* 
 * CCDCLIP: clip an image
 *	quick and dirty: 22-mar-99
 *
 *	22-mar-99   Created
 *                      
 */


#include <stdinc.h>		/* also gets <stdio.h>	*/
#include <getparam.h>
#include <vectmath.h>		/* otherwise NDIM undefined */
#include <filestruct.h>
#include <image.h>

string defv[] = {
        "in=???\n       Input image file",
	"out=???\n      Output file",
	"min=\n         Minimum value below which replace w/ clipvalue",
	"max=\n         Maximum value below which replace w/ clipvalue",
	"VERSION=1.1\n  24-aug-2022 PJT",
	NULL,
};

string usage = "clip an image";



void nemo_main()
{
    stream  instr, outstr;
    int     nx, ny, nz;        /* size of scratch map */
    int     ix, iy, iz;
    imageptr iptr=NULL;        /* pointer to image */
    real    newmin, newmax, tmp;
    int     countmin, countmax;
    bool    Qmin, Qmax;

    Qmin = hasvalue("min");
    Qmax = hasvalue("max");
    if (Qmin) newmin = getrparam("min");
    if (Qmax) newmax = getrparam("max");

    instr = stropen(getparam("in"), "r");
    outstr = stropen(getparam("out"), "w");

    read_image( instr, &iptr);

    nx = Nx(iptr);	
    ny = Ny(iptr);
    nz = Nz(iptr);

    countmin = countmax = 0;

    for (iz=0; iz<nz; iz++) {
      for (iy=0; iy<ny; iy++) {
        for (ix=0; ix<nx; ix++) {
            tmp = CubeValue(iptr,ix,iy,iz);
            if (Qmin && tmp < newmin) {
                CubeValue(iptr,ix,iy,iz) = newmin;
                countmin++;
		dprintf(1,"min %d  %d %d %d  %g\n", countmin,ix,iy,iz,tmp);
            }
            if (Qmax && tmp > newmax) {
                CubeValue(iptr,ix,iy,iz) = newmax;
                countmax++;
		dprintf(1,"max %d  %d %d %d  %g\n", countmax,ix,iy,iz,tmp);		
            }
        }
      }
    }
    write_image(outstr, iptr);
    if (Qmin) dprintf(0,"Clipped %d values below %g\n",countmin,newmin);
    if (Qmax) dprintf(0,"Clipped %d values above %g\n",countmax,newmax);
}

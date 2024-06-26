/*
 * LOAD.C: routines to create body-tree.
 * Public routines: maketree().
 *
 *	19-jun-92  PJT  replaced an 'assert' by 'error' call
 *      18-jul-92  PJT  replaced many if(debug)printf(...) by dprintf(1,...)
 *      24-may-02  pjt  while waiting for the coffee to brew.... laptop on lap....
 *                      changed the 32bit depth treebuild to 64bit... (see int_hack)
 *      29-mar-02  pjt  add 'nudge' to maketree
 */

#include "defs.h"

local cellptr ctab = NULL;	/* cells are allocated from here */
local int ncell, maxcell;	/* count cells in use, max available */


static void expandbox(bodyptr p);
static void loadtree(bodyptr p, bodyptr btab, double nudge);
static bool intcoord(int_hack xp[3], vector rp);
static int_hack subindex(int_hack x[3], int_hack l);
static void hackcofm(register nodeptr q);
static cellptr makecell(void);

/*
 * MAKETREE: initialize tree structure for hack force calculation.
 */

void maketree(
	 bodyptr btab,			/* array of bodies to build into tree */
	 int nbody,			/* number of bodies in above array */
	 double nudge)
{
    register bodyptr p;

    if (ctab == NULL) {				/* first time through?      */
	maxcell = fcells * nbody;		/*   typ. need: 0.5 nbody   */
	ctab = (cellptr) allocate(maxcell * sizeof(cell));  /* alloc  cells */
    }
    ncell = 0;					/* reset cells in use       */
    troot = NULL;				/* deallocate current tree  */
    for (p = btab; p < btab+nbody; p++)		/* loop over all bodies     */
	if (Mass(p) != 0.0) {			/*   only load massive ones */
	    expandbox(p);			/*     expand root to fit   */
	    loadtree(p,btab,nudge);		/*     insert into tree     */
	}
    hackcofm(troot);				/* find c-of-m coordinates  */
}

/*
 * EXPANDBOX: enlarge cubical "box", salvaging existing tree structure.
 */

local void expandbox(
		bodyptr p)                      /* body to be loaded */
{
    bool intcoord();
    int_hack k, xtmp[NDIM], xmid[NDIM], subindex();
    vector rmid;
    cellptr makecell(), newt;

    while (! intcoord(xtmp, Pos(p))) {		/* expand box (rarely)      */
            dprintf(1,"expandbox: expanding box\n");
        ADDVS(rmid, rmin, 0.5 * rsize);         /*   find box midpoint      */
        for (k = 0; k < NDIM; k++)              /*   loop over dimensions   */
            if (Pos(p)[k] < rmid[k])            /*     is p left of mid?    */
                rmin[k] -= rsize;               /*       extend to left     */
        rsize = 2.0 * rsize;                    /*   double length of box   */
        if (debug)
            dprintf(0,"\t   rmin = [%8.4f,%8.4f,%8.4f]\trsize = %8.4f\n",
                   rmin[0], rmin[1], rmin[3], rsize);
        if (troot != NULL) {                     /*   repot existing tree?   */
            newt = makecell();                  /*     create new root cell */
            assert(intcoord(xmid, rmid));	/*     locate old root cell */
            k = subindex(xmid, IMAX >> 1);      /*     find old tree index  */
            Subp(newt)[k] = troot;               /*     graft old on new     */
            dprintf(1,"expandbox: old root goes in subcell %d\n", k);
            troot = (nodeptr) newt;              /*     plant new tree       */
        }
    }
}

/*
 * LOADTREE: descend tree and insert a particle.
 */

local void loadtree(
	       bodyptr p,			/* body to load into tree */
	       bodyptr btab,	                /* pointer to base array  */
	       double nudge)
{
  int_hack l, xp[NDIM], xq[NDIM], subindex();
  nodeptr *qptr;
  int i;
  cellptr c, makecell();

  do {
    assert(intcoord(xp, Pos(p)));		/* form integer coords      */
    l = IMAX >> 1;				/* start with top bit       */
    qptr = &troot;				/* start with tree root     */
    while (*qptr != NULL) {			/* loop descending tree     */
        dprintf(1,"loadtree: descending tree  l = %o\n", l);
	/* assert(l != 0); */			/*   dont run out of bits   */
	if(l==0) {
	  int offset = p-btab;
	  warning("loadtree: ran out of bits for particle %d",offset+1);
	  break;
	}
	if (Type(*qptr) == BODY) {		/*   reached a "leaf"?      */
	    dprintf(1,"loadtree: replacing body with cell\n");
	    c = makecell();			/*     alloc a new cell     */
	    assert(intcoord(xq, Pos(*qptr)));	/*     get integer coords   */
	    Subp(c)[subindex(xq, l)] = *qptr;	/*     put body in cell     */
	    *qptr = (nodeptr) c;		/*     link cell in tree    */
	}
	qptr = &Subp(*qptr)[subindex(xp, l)];	/*   move down one level    */
	l = l >> 1;				/*   and test next bit      */
    }
    if (l==0) {
      warning("Nudging particle by %g",nudge);
      for (i=0; i<NDIM; i++)
	Pos(p)[i] += xrandom(-nudge,nudge);
    }
  } while (l==0);
  dprintf(1,"loadtree: installing body  l = %o\n", l);
  *qptr = (nodeptr) p;			/* found place, store p     */
}

/*
 * INTCOORD: compute integerized coordinates.
 * Returns: TRUE unless rp was out of bounds.
 */

local bool intcoord(
		    int_hack xp[NDIM],		/* integerized coordinate vector [0,IMAX) */
		    vector rp)			/* real coordinate vector (system coords) */
{
    register int k;
    bool inb;
    double xsc;

    dprintf(1,"intcoord: rp = [%8.4f,%8.4f,%8.4f]\n", rp[0], rp[1], rp[2]);
    inb = TRUE;					/* use to check bounds      */
    for (k = 0; k < NDIM; k++) {		/* loop over dimensions     */
        xsc = (rp[k] - rmin[k]) / rsize;        /*   scale to range [0,1)   */
        if (0.0 <= xsc && xsc < 1.0)            /*   within unit interval?  */
            xp[k] = floor(IMAX * xsc);          /*     then integerize      */
        else                                    /*   out of range           */
            inb = FALSE;                        /*     then remember that   */
    }
    dprintf(1,"\t  xp = [%8x,%8x,%8x]\tinb = %d\n", xp[0], xp[1], xp[2], inb);
    return inb;
}

/*
 * SUBINDEX: determine which subcell to select.
 */

local int_hack subindex(
			int_hack x[NDIM],	/* integerized coordinates of particle */
			int_hack l)		/* current level of tree */
{
    register int_hack i, k;

    dprintf(1,"subindex: x = [%8x,%8x,%8x]\tl = %8x\n", x[0],x[1],x[2],l);
    i = 0;                                      /* sum index in i           */
    for (k = 0; k < NDIM; k++)                  /* check each dimension     */
        if (x[k] & l)                           /*   if beyond midpoint     */
            i += NSUB >> (k + 1);               /*     skip over subcells   */
    dprintf(1,"          returning %d\n", i);
    return i;
}

/*
 * HACKCOFM: descend tree finding center-of-mass coordinates.
 */

local void hackcofm(
	       register nodeptr q)             /* pointer into body-tree */
{
    register int i;
    register nodeptr r;
    static vector tmpv, dr;
    static real drsq;
    static matrix drdr, Idrsq, tmpm;

    if (Type(q) == CELL) {                      /* is this a cell?          */
        Mass(q) = 0.0;                          /*   init total mass        */
        CLRV(Pos(q));				/*   and c. of m.           */
        for (i = 0; i < NSUB; i++) {            /*   loop over subcells     */
            r = Subp(q)[i];
            if (r != NULL) {                    /*     does subcell exist?  */
                hackcofm(r);                    /*       find subcell cm    */
                Mass(q) += Mass(r);             /*       sum total mass     */
                MULVS(tmpv, Pos(r), Mass(r));   /*       find moment        */
                ADDV(Pos(q), Pos(q), tmpv);     /*       sum tot. moment    */
            }
        }
        DIVVS(Pos(q), Pos(q), Mass(q));         /*   rescale cms position   */
#ifdef QUADPOLE
	CLRM(Quad(q));				/*   init. quad. moment     */
	for (i = 0; i < NSUB; i++) {		/*   loop over subnodes     */
	    r = Subp(q)[i];
	    if (r != NULL) {			/*     does subnode exist?  */
		SUBV(dr, Pos(r), Pos(q));	/*       displacement vect. */
		OUTVP(drdr, dr, dr);		/*       outer prod. of dr  */
		DOTVP(drsq, dr, dr);		/*       dot prod. dr * dr  */
		SETMI(Idrsq);			/*       init unit matrix   */
		MULMS(Idrsq, Idrsq, drsq);	/*       scale by dr * dr   */
		MULMS(tmpm, drdr, 3.0);		/*       scale drdr by 3    */
		SUBM(tmpm, tmpm, Idrsq);	/*       form quad. moment  */
		MULMS(tmpm, tmpm, Mass(r));	/*       of cm of subnode,  */
		if (Type(r) == CELL)		/*       if subnode is cell */
		    ADDM(tmpm, tmpm, Quad(r));	/*         use its moment   */
		ADDM(Quad(q), Quad(q), tmpm);	/*       add to qm of cell  */
	    }
	}
#endif
    }
}

/*
 * MAKECELL: allocation routine for cells.
 */

local cellptr makecell()
{
    register cellptr c;
    register int i;

    if (ncell >= maxcell)
	error("makecell: need more than %d cells; increase fcells\n",
	      maxcell);
    c = ctab + ncell;
    ncell++;
    dprintf(1,"cell %d allocated at address 0%o\n", ncell, (int) c);
    Type(c) = CELL;
    for (i = 0; i < NSUB; i++)
	Subp(c)[i] = NULL;
    return c;
}

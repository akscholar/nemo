/*
 * TABCOLS: select columns from a table
 *
 *      27-jan-00   created
 */

#include <stdinc.h>
#include <getparam.h>
#include <table.h>
#include <extstring.h>
#include <ctype.h>

string defv[] = {                /* DEFAULT INPUT PARAMETERS */
    "in=???\n           input file name(s)",
    "select=all\n       columns to select",
    "colsep=SP\n        Column separator (SP,TAB,NL)",    
    "out=-\n            output file name",
    "VERSION=2.3\n      30-mar-2024 PJT",
    NULL
};

string usage = "Select column(s) from a table";


string input, output;			/* file names */
stream instr, outstr;			/* file streams */
table  *tptr;                           /* table */
int    ninput;				/* number of input files */

#ifndef MAX_COL
#define MAX_COL 256
#endif


int    keep[MAX_COL+1];                 /* column numbers to keep */
int    nkeep;                           /* actual number of skip columns */
int    maxcol = 0;                      /* largest column number */
char   colsep;                          /* character to separate columns */
bool   Qall;

local void setparams(void);
local void convert(stream , stream);
local void tab2space(char *);

extern  string *burststring(string, string);


void nemo_main()
{
    setparams();
    convert (instr,outstr);
}

local void setparams(void)
{
    string select;                          /* which columns not to write */
    string separator;
    int i;

    input = getparam("in");
    output = getparam("out");

    instr  = stropen(input,"r");
    tptr   = table_open(instr,1);
    outstr = stropen (output,"w");

    // dprintf(1,"table: %d x %d\n", table_nrows(tptr), table_ncols(tptr));
    

    select = getparam("select");
    if (select==NULL || *select=='\0' || streq(select,"all")) {
        Qall = TRUE;
        for (i=0; i<MAX_COL; i++) keep[i] = i+1;
    } else {
        Qall = FALSE;
        nkeep = nemoinpi(select,keep,MAX_COL);
        if (nkeep<=0 || nkeep>MAX_COL)
            error("Too many columns given (%d) to keep (MAX_COL %d)",nkeep,MAX_COL);
	// @todo allow -1 as the last columns
	if (nkeep==1 && keep[0]==-1) {
	  warning("new feature -1");
	} else {
	  for (i=0; i<nkeep; i++){
            if (keep[i]<0) error("Negative column number %d",keep[i]);
            maxcol = MAX(maxcol,keep[i]);
	  }
        }
    }

    separator = getparam("colsep");
    switch (*separator) {
        case 's':
        case 'S':       colsep =  ' ';   break;
        case 't':
        case 'T':       colsep = '\t';  break;
        case 'n':
        case 'N':       colsep = '\n';  break;
        case 'r':
        case 'R':       colsep = '\n';  break;
        case ':':       
        case ',':       
        case '|':
        case '\\':
        case '/':       colsep = separator[0]; break;
        default:        error("Illegal separator: s t n r : ,");
    }
}


local void convert(stream instr, stream outstr)
{
    char   *line;
    int    i, nlines, noutv;
    string *outv;                   /* pointer to vector of strings to write */
    char   *seps=", |\t";           /* column separators  */
        
    nlines=0;               /* count lines read so far */

    for (;;) {

        line = table_line(tptr);
	if (line == NULL) return;

        dprintf(3,"LINE: (%s)\n",line);
        if (iscomment(line)) continue;
        
        nlines++;
        tab2space(line);                  /* work around a Gipsy (?) problem */

        outv = burststring(line,seps);
        noutv = xstrlen(outv,sizeof(string)) - 1;
        if (noutv < maxcol) {
	  warning("skipping line %d; Too few columns in input file (%d < %d)",nlines,noutv,maxcol);
	  continue;
	}
        if (Qall) nkeep = noutv;
                        
        for (i=0; i<nkeep; i++) {
            if (keep[i] == 0)
                fprintf(outstr,"%d",nlines);
            else
                fprintf(outstr,"%s",outv[keep[i]-1]);
            if (i < nkeep-1) fprintf(outstr,"%c",colsep);
        }
        if (colsep != 'n') fprintf(outstr,"\n");    /* end of line */

	// @todo    free
    }
}
/*
 * small helper function, replaces tabs by spaces before processing.
 * this prevents me from diving into gipsy parsing routines and  fix
 * the problem there 
 * PJT - June 1998.
 */

local void tab2space(char *cp)
{
    while (*cp) {
        if (*cp == '\t') *cp = ' ';
        cp++;
    }
}

/*
 *  NEMOVAR:   display a NEMO variable from the $NEMOVAR file (or future shared memory)
 */

#include <nemo.h>
#include <table.h>
#include <extstring.h>


string defv[] = {
  "var=\n             Name of  variable(s)",
  "val=\n             Optional new value",
  "VERSION=0.3\n      18-mar-2024 PJT",
  NULL,
};

string usage="display NEMO variable(s)";


void nemo_main()
{
  string var = getparam("var");
  bool hasVar = hasvalue("var");
  string value = getparam("val");  
  bool hasVal = hasvalue("val");
  string nemovar = getenv("NEMOVAR");
  if (nemovar == NULL) error("$NEMOVAR was not set");
  dprintf(1,"nemovar: %s\n", nemovar);
  
  stream tstr = stropen(nemovar,"r");
  table *t = table_open(tstr, -1);      // read table as a dumb set of lines
  int i, nrows = t->nr; 
  string s, vareq;
  
  // check if just to show all variables
  if (!hasVar && !hasVal) {
    dprintf(1,"Show all variables\n");
    for (i=0; i<nrows; i++)
	    printf("%s\n", table_row(t, i));
    return;
  }

  //@TODO - Should use hash in the future?
  // ensure we find the "var=" string
  vareq = allocate(strlen(var) + 2);
  sprintf(vareq,"%s=", var);
  dprintf(1,"vareq:%s\n",vareq);

  // @todo - this is where a delete= option would come out
  if (hasVar && !hasVal) { 
    // loop over lines in reverse and find the last keyword, then display it
    for (i=nrows-1; i>=0; i--) {
      s = table_row(t, i);
      if (strncmp(s,vareq,strlen(vareq))==0) {
          printf("%s\n",s+strlen(var)+1);      
	  return;
      }    
    }
    dprintf(0,"Variable %s not found\n",var);
    return;
  }

  // writing code might need flock() to lock file
  // @todo currently this allows a variable to appear multiple times
  //       but all var's will get the new value
  //       'nemovar | sort | uniq' would solve that

  if (hasVar && hasVal) {
    stream write_stream = stropen(nemovar,"w!");
    bool found_var = FALSE;
    
    for (i=0; i<nrows; i++) {
      s = table_row(t, i);
      if (strncmp(s,vareq,strlen(vareq))==0) {
        fprintf(write_stream,"%s=\"%s\"\n",var,value);    
        found_var = TRUE;  
      } else {
        fprintf(write_stream,"%s\n",s);
      }
    }

    if (!found_var) {
      fprintf(write_stream,"%s=\"%s\"\n",var,value);    
    }
    return;
  }

  // nothing more to do yet
}

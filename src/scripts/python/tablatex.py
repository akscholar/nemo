#! /usr/bin/env python
#
#   convert a table to latex.
#       table should only have values, though # comment lines are converted to latex % lines
#
#   Test example:      tabgen  - 10 4  | ./tablatex.py

import sys

h1 = """
%    this table has been generated by the tablatex.py converter, you will likely need to edit it
\\begin{table}[!h]
\\caption{YOUR CAPTION}
\\smallskip
\\begin{center}
{\\small
"""

h2t = "\\begin{tabular}{%s}  %%l=left c=center r=right"

h3 = """
\\tableline
\\noalign{\\smallskip}
"""

h4t = "%s\\\\"

h5 = """
\\noalign{\\smallskip}
\\tableline
\\noalign{\\smallskip}
"""

#   0.005 & 0.42-0.48 & 50\\

h6 = """
\\noalign{\\smallskip}
\\tableline % Sometimes you just need a line between table rows
\\end{tabular}
}
\\end{center}
\\end{table}
% end of generated table
"""


def readtable(file=sys.stdin):
    """ gobble the file
    """
    if file is not sys.stdin:
        fp = open(file,'r')
    else:
        fp = sys.stdin
    lines = fp.readlines()
    if file is not sys.stdin:
        fp.close()
    return lines


if __name__ == '__main__':
    if len(sys.argv) == 1:
        lines = readtable()
    else:
        lines = []
        for f in sys.argv[1:]:
            lines = lines + readtable(f)

    ncols = len(lines[0].split())
    fmt = ""
    col = ""
    for i in range(ncols):
        fmt = fmt + "l"
        if i==0:
            col = "col1 "
        else:
            col = col + "& col%d " % (i+1)
    
    print(h1)
    h2 = h2t % fmt
    print(h2)
    print(h3)
    h4 = h4t % col
    print(h4)
    print(h5)
    for line in lines:
        if line[0] == '#':
            print('% ',line)
            continue
        words = line.strip().split()
        out = ""
        for w in words:
            if len(out)==0:
                out = w
            else:
                out = out + " & " + w
        out = out + "\\\\"
        print(out)
    print(h6)

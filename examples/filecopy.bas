10 open "prime10M.c" as #1
20 open "nprime10M.c" as #2
30 input #1 a$
35 if eof(#1) = 1 then 90
40 print #2 a$
50 goto 30
90 close #1
100 close #2
110 print "done"
120 end

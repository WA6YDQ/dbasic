1 rem this program copies the text file in line 10
2 rem to the text file in line 20
3 rem use quotes around the filenames
4 rem to protect mixed case letters
5 rem
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

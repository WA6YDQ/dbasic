5 rem test of gosub
10 print "at 10"
20 gosub 100
30 print "at 30"
99 end
100 print "at 100"
110 gosub 200
120 return
200 print "at 200"
210 gosub 300
220 return
300 print "at 300"
310 gosub 400
320 return
400 print "at 400"
420 return

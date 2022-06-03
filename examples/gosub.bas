10 rem test gosub/return
20 print "starting"
30 for n = 1 to 10
40 gosub 100
50 next n
60 end
100 print n,n*n
110 return


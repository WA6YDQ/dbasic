5 rem    test of user defined functions
6 rem    note the function defination is 
7 rem    surrounded by double quotes
10 def fnf = "1/(6.28*sqr(l*c))"
20 let l = 1.2e-6
21 print "l=";l
30 let c = 120e-12
31 print "c=";c
40 let f = fnf
50 print "l=";l,"c=";c,"f=";f
99 end


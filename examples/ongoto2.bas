5 rem test of sgn() and on/goto
10 input "enter a number ";n
20 let s=sgn(n)
30 on s+2 goto 100 200 300
40 print "should never be here"
50 end
100 print "Number ";n;" is negative"
110 end
200 print "Number ";n;" is 0"
210 end
300 print "Number ";n;" is positive"
310 end

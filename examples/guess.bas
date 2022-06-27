10 rem guess a number
20 let r=int(rnd()*10)+1
30 let c=0
40 input "Enter your guess (1-10): ",g
50 if g=r then 300
55 c=c+1
60 if c>3 then 400
70 if g > r then 100
80 if g < r then 200
90 goto 40
100 print "too high"
110 goto 40
200 print "too low"
210 goto 40
300 print "Correct!"
310 end
400 print "The number was ";r
410 end

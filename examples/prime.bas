10 rem prime.bas
11 rem run the sieve of erastosthenes program
12 rem this returns 1899 primes in about 1 second
13 rem on a raspberry pi 4 running dbasic
20 let s=8190
30 dim f(8191)
40 print "1 iteration"
50 let c=0
60 for i=0 to s
70 let f(i) = 1
80 next i
90 for i=0 to s
100 if f(i) = 0 then 180
110 let p=i+i+3
120 let k=i+p
130 if k > s then 170
140 let f(k)=0
150 let k=k+p
160 goto 130
170 let c=c+1
175 rem p is a prime. uncomment 176 to print it
176 rem print p
180 next i
190 print c,"primes"
200 end

5 rem print on screen a simple sine wave
6 rem use ctrl-c to stop
10 let s=3/8
20 for n=-6.28 to 6.28 step s
30 let x=deg(sin(n))
32 let x=x/5
33 let x=x+30
40 print tab(x);"*"
50 next n
60 goto 20
99 end

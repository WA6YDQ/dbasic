5 rem more compact version of sinewave.bas
10 let s=3/8
20 for n=-2*pi(0) to 2*pi(0) step s
30 rem +30 is an x offset
40 print spc(deg(sin(n))/5+30) "*"
50 next n

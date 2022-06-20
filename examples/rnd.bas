10 rem This shows an example of the rnd() function
20 rem When BASIC is invoked, the random number generator is seeded with a
30 rem random number. This lasts until BASIC is exited, and should be
40 rem OK for most applications.
50 rem If the random number generator needs to be reseeded,     
60 rem then put rnd(-1) in a line. This will reseed the random number
70 rem generator with a new random number.
80 rem Using rnd(0) or rnd() just returns a random number.
90 rem Using rnd(1) (or any positive number) will restart the
100 rem random number generator with a predictable value. Use this
110 rem when you need a fixed sequence of random numbers.
200 print "5 random numbers, seeded at BASIC start"
210 for n=1 to 5
220 print rnd()
230 next n
240 print "5 more random numbers, seeded at BASIC start"
250 for n=1 to 5
260 print rnd()
270 next n
280 print "reseeding the generator, and 5 random numbers"
290 let r=rnd(-1)
300 for n=1 to 5
310 print rnd()
320 next n
330 print "Seeding the random number generator with a defined value"
340 let r=rnd(1)
350 print "5 semi-random numbers"
360 for n=1 to 5
370 print rnd()
380 next n
390 print "repeat the last sequence"
400 let r=rnd(1)
410 for n=1 to 5
420 print rnd()
430 next n
500 print "done"
999 end


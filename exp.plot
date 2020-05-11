#
# Exponentielle pour le joystick arduino
#

set size 1, 1
#set xrange 0.1

set xtics 0,0.05
set ytics 0,0.05,1
set grid

plot [x=0:1] f(x,a)= (exp(a*x)-1)/(exp(a)-1), \
f(x,0.01), \
f(x,1),  f(x,2),  f(x,3),  f(x,4),  f(x,5), f(x,9), \
f(x,-1), f(x,-2), f(x,-3), f(x,-4), f(x,-5)

#
# Exponentielle pour le joystick arduino
#

#set size 1, 1
#set xrange 0.1

set xtics 0,0.1
set ytics 0,5,150
set grid


plot [x=0:1] g(x,a)= (exp(a*x)-1)/(exp(a)-1), \
_min=150.0, _max=6.0, f(x,a)=(_max -_min) * g(x,a) + _min, \
f(x,0.001), \
f(x,1),  f(x,2),  f(x,3),  f(x,4),  f(x,6),  f(x,10), \
f(x,-1), f(x,-2), f(x,-3), f(x,-4), f(x,-6), f(x,-10)


#!/bin/bash -f 
# Algorithme de tracé de cercle dû à Éric Andres
# wikipedia : https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_de_cercle_d%27Andres
#
function circle () {
    # x, y, rayon, couleur

    r=$3
    x=$1
    y=$2

    d=$(($r-1))
    a=$(($r-1))
    b=0

    tput setaf $4
    while test $a -ge $b ; do
        tput cup $(($y+$b)) $(($x+$a)); echo "@";
        #tput cup $(($y+$a)) $(($x+$b)); echo "@";
        #tput cup $(($y-$b)) $(($x+$a)); echo "@";
        tput cup $(($y-$a)) $(($x+$b)); echo "@";
        #tput cup $(($y+$b)) $(($x-$a)); echo "@";
        tput cup $(($y+$a)) $(($x-$b)); echo "#";
        tput cup $(($y-$b)) $(($x-$a)); echo "@";
        #tput cup $(($y-$a)) $(($x-$b)); echo "#";
        if test $d -ge $((2*$b)) ; then
            d=$(($d-2*$b-1))
            b=$(($b+1))
        elif test $d -lt $((2*($r-$a))); then
            d=$(($d+2*$a-1))
            a=$(($a-1))
        else
            d=$(($d+2*($a-$b-1)))
            a=$(($a-1))
            b=$(($b+1))
        fi
    done
}

clear

# Cercle centré sur le centre de l'écran
xc=$(($(tput cols)/2))
yc=$(($(tput lines)/2))
# Nombre de couleurs disponibles dans le terminal
nc=$(tput colors)

# Tracé de cercles concentriques montrant le parfait remplissage
for i in $(seq 1 $(($yc-5))) ; do
    circle $xc $yc $i $(($RANDOM%$nc))
done


circle $xc $yc 20 $(($RANDOM%$nc))
circle $xc $yc 21 $(($RANDOM%$nc))

tput cup $(tput lines) 0
tput oc


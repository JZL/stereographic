mogrify  -trim +repage -transparent white -shave 1x1 -trim -format png -path out/png out/*.pgm

convert out/png/0.png -flip  out/png/rot/0.png
convert out/png/4.png -flip  out/png/rot/4.png
convert out/png/8.png -flip  out/png/rot/8.png
convert out/png/12.png -flip  out/png/rot/12.png
convert out/png/16.png -flip  out/png/rot/16.png
convert out/png/2.png -flip out/png/rot/2.png
convert out/png/6.png -flip out/png/rot/6.png
convert out/png/10.png -flip out/png/rot/10.png
convert out/png/14.png -flip out/png/rot/14.png
convert out/png/18.png -flip out/png/rot/18.png

#convert \
        #out/png/rot/0.png \
        #out/png/1.png -smush 0  \
        #-transparent white \
        #out/png/rot/2.png -gravity South +smush 0  \
        #-transparent white \
        #out/png/3.png -gravity SouthEast -smush 0  \
        #net.png                   
convert \
    out/png/rot/0.png \
    out/png/rot/4.png \
    out/png/rot/8.png \
    out/png/rot/12.png \
    out/png/rot/16.png \
    +smush 1 top.png

convert \
    out/png/1.png \
    out/png/rot/2.png \
    out/png/5.png \
    out/png/rot/6.png \
    out/png/9.png \
    out/png/rot/10.png \
    out/png/13.png \
    out/png/rot/14.png \
    out/png/17.png \
    out/png/rot/18.png \
    +smush -2 -transparent white middle.png

convert \
    out/png/3.png \
    out/png/7.png \
    out/png/11.png \
    out/png/15.png \
    out/png/19.png \
    +smush 1 bottom.png


convert bottom.png -gravity West -splice 100x0 -transparent white bottomExp.png

convert top.png middle.png -smush 0 -transparent white topMiddle.png

convert topMiddle.png bottomExp.png -gravity West -smush 0 net.png

#convert \
    #topMiddle.png bottom.png \
    #-set page "+0+200" \
    #-layers Merge +repage \
    #net.png

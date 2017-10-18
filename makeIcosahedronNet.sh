echo "Starting to make Icosahedron net..."
cd out/
mogrify  -trim +repage -transparent white -shave 1x1 -trim -format png -path ./png ./*.p*m

convert ./png/0.png  -quality 01 -flip ./png/rot/0.png
convert ./png/4.png  -quality 01 -flip ./png/rot/4.png
convert ./png/8.png  -quality 01 -flip ./png/rot/8.png
convert ./png/12.png -quality 01 -flip ./png/rot/12.png
convert ./png/16.png -quality 01 -flip ./png/rot/16.png
convert ./png/2.png  -quality 01 -flip ./png/rot/2.png
convert ./png/6.png  -quality 01 -flip ./png/rot/6.png
convert ./png/10.png -quality 01 -flip ./png/rot/10.png
convert ./png/14.png -quality 01 -flip ./png/rot/14.png
convert ./png/18.png -quality 01 -flip ./png/rot/18.png

#convert \
        #./png/rot/0.png \
        #./png/1.png -smush 0  \
        #-transparent white \
        #./png/rot/2.png -gravity South +smush 0  \
        #-transparent white \
        #./png/3.png -gravity SouthEast -smush 0  \
        #net.png                   
convert \
    ./png/rot/0.png \
    ./png/rot/4.png \
    ./png/rot/8.png \
    ./png/rot/12.png \
    ./png/rot/16.png \
    -quality 01 \
    +smush 1 top.png

convert \
    ./png/1.png \
    ./png/rot/2.png \
    ./png/5.png \
    ./png/rot/6.png \
    ./png/9.png \
    ./png/rot/10.png \
    ./png/13.png \
    ./png/rot/14.png \
    ./png/17.png \
    ./png/rot/18.png \
    -quality 01 \
    +smush -2 -transparent white middle.png

convert \
    ./png/3.png \
    ./png/7.png \
    ./png/11.png \
    ./png/15.png \
    ./png/19.png \
    -quality 01 \
    +smush 1 bottom.png


convert bottom.png -quality 01 -gravity West -splice 100x0 -transparent white bottomExp.png

convert top.png middle.png -quality 01 -smush 0 -transparent white topMiddle.png

convert topMiddle.png bottomExp.png -quality 01 -gravity West -smush 0 ../net.png

echo "DONE! Check net.png"

#convert \
    #topMiddle.png bottom.png \
    #-set page "+0+200" \
    #-layers Merge +repage \
    #net.png

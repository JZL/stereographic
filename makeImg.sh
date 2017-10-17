#http://www.imagemagick.org/Usage/resize/
size=5000;
if [ -z "$1" ]; then
        echo "PASS IN IMAGE"
        exit 0
fi
echo "**Starting to make input image**"
#convert "$1" -quality 100 -density 300 -resize "$size"x$size^ -gravity center -extent "$size"x$size -shave 1x1 -bordercolor black -border 1 cropped.png
#convert "$1" -adaptive-sharpen 0x2.0 -quality 100 -density 1000 -gravity center -crop "$size"x$size+0+0 -shave 1x1 -bordercolor black -border 1 +repage cropped.jpg
convert "$1" -gravity center -crop "$size"x"$size"+0+0 -extent "$size"x"$size" cropped.png
ls -lh cropped*
echo "done converting to png"
convert -compress none cropped.png withSpaces.pbm
echo "done converting to interim pbm"
sed -i "/#/d" withSpaces.pbm
cat <(head -2 withSpaces.pbm) <(tail -n +3 withSpaces.pbm|tr -d "[ \n]") > in.pbm
echo "done converting to correct!"

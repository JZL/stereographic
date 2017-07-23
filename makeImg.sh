#http://www.imagemagick.org/Usage/resize/
size=2000
convert "$1" -resize "$size"x$size^ -gravity center -extent "$size"x$size -shave 1x1 -bordercolor black -border 1 cropped.png
convert -compress none cropped.png withSpaces.pbm
sed -i "/#/d" withSpaces.pbm
cat <(head -2 withSpaces.pbm) <(tail -n +3 withSpaces.pbm|sed 's/\s//g' |tr -d "\n") > in.pbm


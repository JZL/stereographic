#http://www.imagemagick.org/Usage/resize/
convert "$1" -resize 1000x1000^ -gravity center -extent 1000x1000 cropped.png
convert -compress none cropped.png withSpaces.pbm
sed -i "/#/d" withSpaces.pbm
cat <(head -2 withSpaces.pbm) <(tail -n +3 withSpaces.pbm|sed 's/\s//g' |tr -d "\n") > in.pbm


BEGIN {
    FS = " "
}
NR<4{
    print;    
}
NR>=4 {
    for(i =1; i<=NF; i+=3){
        printf ("%09i", $i*1000000+$(i+1)*1000+$(i+2))
    }
}

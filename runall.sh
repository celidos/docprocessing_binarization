#!/bin/bash
START=$(date +%s.%N)
TOTALPIXELS=0
for i in {01..15}
do
echo "Processing file $i.JPG"
W=`identify ./$i.JPG | cut -f 3 -d " " | sed s/x.*//` #width
H=`identify ./$i.JPG | cut -f 3 -d " " | sed s/.*x//` #height
AREA=$((W * H))
echo "Area: $AREA"
TOTALPIXELS=$((TOTALPIXELS + AERA))
./main "$i.JPG" 
done
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "Done in $DIFF s."
echo "Total pixels $TOTALPIXELS"

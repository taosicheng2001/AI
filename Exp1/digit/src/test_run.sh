#!/bin/bash
rm -rf ../output/*.txt

for var in {00,01,02,03,04,05,06,07,08,09,10,11}
    do
        for func in {A_h1,A_h2,IDA_h1,IDA_h2}
        do
./a.out $func ../data/input${var}.txt ../data/target${var}.txt 
        done
    done
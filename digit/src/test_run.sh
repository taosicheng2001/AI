#!/bin/bash
fuc=A_h1

for var in {00,01,02,03,04,05,06,07,08,09,10,11}
    do
./a.out $fuc ../data/input${var}.txt ../data/target${var}.txt
    done
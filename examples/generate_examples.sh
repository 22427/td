#!/bin/bash

# Generate example .td files. These are uncompressed binary blobs containing 
# the converted image and - if generated - the mip map levels.

../bin/td -i monarch.png -o 5551.td -dt UNSIGNED_SHORT_5_5_5_1 -dd
../bin/td -i monarch.png -o 5551_d.td -dt UNSIGNED_SHORT_5_5_5_1
../bin/td -i monarch.png -o 5551_mm.td -dt UNSIGNED_SHORT_5_5_5_1 -dd -mm
../bin/td -i monarch.png -o 5551_d_mm.td -dt UNSIGNED_SHORT_5_5_5_1 -mm

../bin/td -i monarch.png -o 4444.td -dt UNSIGNED_SHORT_4_4_4_4 -dd
../bin/td -i monarch.png -o 4444_d.td -dt UNSIGNED_SHORT_4_4_4_4
../bin/td -i monarch.png -o 4444_mm.td -dt UNSIGNED_SHORT_4_4_5_4 -dd -mm
../bin/td -i monarch.png -o 4444_d_mm.td -dt UNSIGNED_SHORT_4_5_5_4 -mm

../bin/td -i monarch.png -o 565.td -dt UNSIGNED_SHORT_5_6_5 -dd
../bin/td -i monarch.png -o 565_d.td -dt UNSIGNED_SHORT_5_6_5
../bin/td -i monarch.png -o 565_mm.td -dt UNSIGNED_SHORT_5_6_5 -dd -mm
../bin/td -i monarch.png -o 565_d_mm.td -dt UNSIGNED_SHORT_5_6_5 -mm

# now lets convert some of the .td files back to .pngs

../bin/td -i 5551_d.td -o 5551_d.png
../bin/td -i 4444_d.td -o 4444_d.png
../bin/td -i 4444.td -o 4444.png
../bin/td -i 565_d.td -o 565_d.png

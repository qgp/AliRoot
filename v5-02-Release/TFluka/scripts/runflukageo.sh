#!/bin/sh
# Remove the previous temporary directory
rm -rf tmp
# Make a new temporary directory and move to it
mkdir tmp
cd tmp

# Link here some special Fluka files needed
ln -s $FLUPRO/neuxsc-ind_260.bin neuxsc.bin
ln -s $FLUPRO/random.dat random.dat
# Copy the random seed
cp $FLUPRO/random.dat old.seed

# Give some meaningfull name to the output
ln -s fluka.out fort.11

#Link FlukaConfig.C as Config.C
ln -fs $ALICE_ROOT/TFluka/macro/FlukaConfig.C Config.C
ln -fs $ALICE_ROOT/TFluka/input/coreFlukaVmc.inp coreFlukaVmc.inp
echo 'Execute at the root prompt:'
echo 'AliSimulation sim'
echo 'sim.Run()'

# Launch aliroot
aliroot
# Go back on exit
cd ..

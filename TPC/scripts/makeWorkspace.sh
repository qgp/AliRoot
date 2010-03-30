#
# marian.ivanov@cern.ch
# argument 1  - run.list
#
# Make workspace structure
# Create a list for each run 
# and make directory structure
# This is fast procedure
#
mydir=`pwd`
runlist=$1
touch raw.list
touch esd.list
for adir in `cat $runlist`; do
    echo Creating dir $adir
    mkdirhier $adir;
    rm -f raw${adir}.txt
    rm -f esd${adir}.txt
    cat  $mydir/raw.list | grep $adir >raw${adir}.txt
    cat  $mydir/esd.list | grep $adir >esd${adir}.txt
    cp raw${adir}.txt   ${adir}/raw.txt
    cp esd${adir}.txt   ${adir}/esd.txt
    cp raw${adir}.txt   ${adir}/raw.txt.Good
    cp esd${adir}.txt   ${adir}/esd.txt.Good
done;




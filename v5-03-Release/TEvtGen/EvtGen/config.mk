EVTCXX=gcc
EVTF77=g77
EVTCXXFLAGS=-I. -DEVTSTANDALONE -I/home/fionda/cern/root/v5-23-04/include -I/home/fionda/cern/alice/v4-16-Rev-13_G3v1-10_Rv5-23-04/include -I../ 
EVTDEPINC=-I. -DEVTSTANDALONE -I/home/fionda/cern/root/v5-23-04/include -I/home/fionda/cern/alice/v4-16-Rev-13_G3v1-10_Rv5-23-04/include -I../   -Iignoring -Inonexistent -Idirectory -I"/usr/lib/gcc/i386-redhat-linux/3.4.6/../../../../i386-redhat-linux/include" -I/usr/lib/gcc/i386-redhat-linux/3.4.6/../../../../include/c++/3.4.6 -I/usr/lib/gcc/i386-redhat-linux/3.4.6/../../../../include/c++/3.4.6/i386-redhat-linux -I/usr/lib/gcc/i386-redhat-linux/3.4.6/../../../../include/c++/3.4.6/backward -I/usr/local/include -I/usr/lib/gcc/i386-redhat-linux/3.4.6/include -I/usr/include
EVTF77FLAGS= -I/home/fionda/cern/root/v5-23-04/include -I../ -I/cern/pro/lib
EVTLINK=-L. -L/cern/pro/lib -L/cern/pro/lib -z muldefs
EVTLINKOPT=
EVTSYSLIB=-lg2c -lnsl
EVTPYLIB=pythia6205
EVTPHLIB=photos202
EVTROOTLIBS=-L/home/fionda/cern/root/v5-23-04/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic

very rough sandbox version of a max external that works with YARP libraries.

expects ACE_ROOT and YARP_DIR to be defined;
expects YARP libs to be compiled as static libraries
expects ACE(d).dll dynamic library in runtime path of Max/Msp

Use with MAX 5 SDK (should work in MAX 6 SDK too; but compiles in 32bit only).

most likely won't work "out of the box" - this is just a backup version

currently performs the following:

- register/change port name (using message or object initialization in Max)
- select as "reader" or "writer"
- for "reader, uses thread to check port
- adjustable poll time for read thread
- writes to port.

xcode project tested and should work as well.

compiled with VS2012, 32bit; XCode 5.0, 32bit

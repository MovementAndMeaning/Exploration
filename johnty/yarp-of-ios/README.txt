yarp of empty project

expects YARP_DIR and ACE_ROOT to point to correct locations, and YARP libs to be compiled.

should compile when deployed in an oF apps directory using oF 0.8 iOS and Xcode. May have to manually modify YARP and ACE include directories. Compiled static .a libraries for YARP_OS, YARP_init and libACE are included, and they are "FAT" libraries containing both i386 (for the simulator) and armv7 (for recent iOS device) binaries. may consider including armv6 for older devices in the future? (or probably not...)

if you want to compile ACE and YARP for iOS, read the post here:

http://mnm.hplustech.com/member_center/libace-yarp-and-of-for-ios/

example performs a simple register of a port. currently working on location of namespace server issues.

johntywang@gmail.com - January 2014
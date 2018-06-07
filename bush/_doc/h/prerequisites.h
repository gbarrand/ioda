/**

@page bush_prerequisites <h1>Prerequisites</h1>

@section bush_prerequisites_linux Linux

  You need :
@verbatim
 - a C++ compiler: g++ or clang++. In bush/use_cc, use_cpp, the default uses g++ for Linux.
   If having to tweak things, you have to follow the ${build_gnu} or ${build_clang}
   usage in these scripts and customize what is found within the corresponding "if".
 - zip to build the final binary kit file, but if not here tar is used.
 - ar, ranlib.
 - for graphics apps (most of them!), the X11 and OpenGL "devels" to have the .h headers files.
   (In general the run time .so shared libraries are already here).
@endverbatim
 and... that's all (folks). On most Linux distributions g++, zip, ar, ranlib, X11 and OpenGL are here.
 If not, install them with "sudo apt-get..." or "sudo yum..." according your Linux distribution,
 for example with something as:
@verbatim
    ubuntu> sudo apt-get install libglu1-mesa-dev  # to have OpenGL .h.
 or on centos7:
    centos> sudo yum install gcc gcc-c++
    centos> sudo yum install zip unzip
    centos> sudo yum install libX11-devel libXpm-devel libXft-devel libXext-devel
    centos> sudo yum install mesa-libGL-devel mesa-libGLU-devel
@endverbatim

@section bush_prerequisites_macOS macOS

  In bush/use_cc, use_cpp, the default built for apps is to build with Cocoa, then
 the compiler (clang++) and Cocoa access (under /Applications/Xcode.app/Contents/...)
 uses the one of a Xcode installation, then you have to install Xcode and the
 "Xcode command line tools" to build. If having to tweak things, you have to act
 within the "if" for ${buidl_sdk_mac} or ${build_sdk_mac_gnu}.

  Note that if building a non Cocoa app (for example the X11 version of the pmx application),
 it is the build with clang (${build_clang}) which is used.
@verbatim
    macOS> cd <some_path>/pmx/mgr
    macOS> ./build  # it uses clang++ and put binaries in bin_sdk_mac.
    macOS> open ./bin_sdk_mac/distrib/pmx/<verions>/pmx.app  # to run the Cocoa version.
    macOS> ./build -x11 -bin_x11  # it uses clang++.
    macOS> ./bin_x11/distrib/pmx/<verions>/bin/pmx  # to run the X11 version.
 or:
    macOS> cd <some_path>/inlib/examples/cpp
    macOS> ./build wroot.cpp
    macOS> ./bin_clang/wroot
 or:
    macOS> cd <some_path>/exlib/examples/cpp
    macOS> ./build GL_X11.cpp
    macOS> ./bin_clang/GL_X11
@endverbatim

@section bush_prerequisites_windows Windows-10

  On Windows-10 we build with VisualC++ (cl.exe) from a cygwin prompt.
 VisualC++ offers clearly the best performances at run time. (The cygwin/g++ is known
 to be a snail here and we did not tried to use clang++).

 Then you need:
@verbatim
 - Visual Studio. We use the 2015 one.
 - cygwin with at least zip (bash is here by default). We have also installed tcsh
   (because we prefer it as a prompt), git (to get code) and openssh (to log in
   and out to remote machines), and... it must be all.
@endverbatim
 If having to tweak bush/use_cc, use_cpp follow ${build_visual}. Note that the access
 to cl.exe to compile and link.exe to link is concentrated in the script bush/vc++.
 In particular all other build scripts (for example bush/compile_one_cpp) uses bush/vc++
 with the same options than g++ and clang++ to declare include paths (-I), link path
 (-L), etc... The script bush/vc++ translates these to the /I, /L known by cl.exe, link.exe
 and does also the job to translate cygwin unix paths to Windows paths.

*/

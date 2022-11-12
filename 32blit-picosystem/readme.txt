/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */

PlayStation built with https://archive.org/details/net-yaroze-dev-winxp-vm-2022



Other builds (using 32blit) with https://archive.org/details/vm-01

1) make sure 32blit and picosystem SDK's are setup, compiles and runs the examples first. 
2) copy these files to parent folder
  cp -a * ..; cd ..; 

6) Make and build as normal
mkdir build; cd build
cmake .. -D32BLIT_DIR=YOURPATHHERE/32blit-sdk; make

7) executable is called pico 
profit (in theory anyway)

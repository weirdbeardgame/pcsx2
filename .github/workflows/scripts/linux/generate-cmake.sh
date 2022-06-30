#!/bin/bash

set -e

echo "Additional CMake Args - ${ADDITIONAL_CMAKE_ARGS}"

# Generate CMake into ./build
cmake                                     	\
-DCMAKE_BUILD_TYPE=Release		          	\
-DCMAKE_INSTALL_PREFIX="squashfs-root/usr/"	\
-DCMAKE_PREFIX_PATH="$HOME/Depends"        	\
-DWAYLAND_API=ON                           	\
-DXDG_STD=TRUE                             	\
-DDISABLE_ADVANCE_SIMD=TRUE					\
-DUSE_VULKAN=ON                            	\
-DPACKAGE_MODE=ON                          	\
-DDISABLE_SETCAP=ON                        	\
"$ADDITIONAL_CMAKE_ARGS"                   	\
-GNinja                                    	\
-B build

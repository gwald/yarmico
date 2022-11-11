#rm -rf *; cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/32blit-sdk-new/pico.toolchain   -D32BLIT_DIR=$HOME/32blit-sdk-new -DPICO_SDK_PATH=$HOME/pico-sdk -DPICO_EXTRAS_PATH=$HOME/pico-extras -DPICO_BOARD=pimoroni_picosystem  -DCMAKE_BUILD_TYPE=Release -DALLOW_HIRES=1
#rm -rf *; cmake ..  -D32BLIT_DIR=$HOME/32blit-sdk-new  -DCMAKE_BUILD_TYPE=Debug
# Basic parameters; check that these match your project / environment
cmake_minimum_required(VERSION 3.9)

# Replace "game" with a name for your project (this is used the name of the output)
project(pico)

# Add your sources here (adding headers is optional, but helps some CMake generators)
set(PROJECT_SOURCE game.c game.h yarmico_API.c yarmico_API.h  platform_32blit.cpp platform_32blit.hpp)

# ... and any other files you want in the release here
set(PROJECT_DISTRIBS LICENSE README.md)

# Build configuration; approach this with caution!
if(MSVC)
  add_compile_options("/W4" "/wd4244" "/wd4324" "/wd4458" "/wd4100")
else()
  add_compile_options("-Wall" "-Wextra" "-Wdouble-promotion" "-Wno-unused-parameter")
endif()

find_package (32BLIT CONFIG REQUIRED PATHS ../32blit-sdk $ENV{PATH_32BLIT_SDK})

blit_executable (${PROJECT_NAME} ${PROJECT_SOURCE})


if(PICO_SDK_PATH)
target_compile_definitions(${PROJECT_NAME} PRIVATE ALLOW_HIRES=0)
add_custom_target (flash DEPENDS ${PROJECT_NAME}.flash)
add_definitions( -DHARDWARE_TARGET=HARDWARE_TARGET_PICOSYSTEM )
	blit_assets_yaml (${PROJECT_NAME} assets-pico.yml)
else()
	blit_assets_yaml (${PROJECT_NAME} assets.yml)
endif()



blit_metadata (${PROJECT_NAME} metadata.yml)



# setup release packages
install (FILES ${PROJECT_DISTRIBS} DESTINATION .)
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set (CPACK_GENERATOR "ZIP" "TGZ")
include (CPack)





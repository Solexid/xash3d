# Copyright (C) 2016 a1batross
# Licensed under WTFPL Public License


#if( NOT TIZEN_SDK )
	set( TIZEN_SDK "$ENV{HOME}/tizen-sdk" CACHE STRING "Path to Tizen SDK" )
#endif( )

#if( NOT TIZEN_PLATFORM )
	set( TIZEN_PLATFORM "mobile" CACHE STRING "Target platform: tv, wearable, mobile" )
#endif()

#if( NOT TIZEN_API_VER )
	set( TIZEN_API_VER "2.4" CACHE STRING "Target platform version: 2.3, 2.3.1 or 2.4" )
#endif()

#if( NOT TIZEN_ARCH )
	set( TIZEN_ARCH "arm" CACHE STRING "Target architecture. arm for devices and x86 for emulator" )
#endif()

#if( NOT TIZEN_COMPILER_VER )
	set( TIZEN_COMPILER_VER "4.9" CACHE STRING "Toolchain version" )
#endif()

set( TIZEN_TARGET "device" )
set( TIZEN_TRIPLET "arm-linux-gnueabi" )

if( TIZEN_ARCH STREQUAL "arm" )
	set( TIZEN_TARGET "device" )
	set( TIZEN_TRIPLET "arm-linux-gnueabi" )
elseif( TIZEN_ARCH STREQUAL "x86" )
	set( TIZEN_TARGET "emulator" )
	set( TIZEN_TRIPLET "i386-linux-gnueabi" )
else()
	message( ERROR "This script supports only device for arm and emulator for x86 targets")
endif()

set( TIZEN_TOOLCHAIN "${TIZEN_SDK}/tools/${TIZEN_TRIPLET}-gcc-${TIZEN_COMPILER_VER}")

set( CMAKE_SYSROOT "${TIZEN_SDK}/platforms/tizen-${TIZEN_API_VER}/${TIZEN_PLATFORM}/rootstraps/${TIZEN_PLATFORM}-${TIZEN_API_VER}-${TIZEN_TARGET}.core/" )
set( CMAKE_SYSTEM_NAME "Linux" )
set( CMAKE_CXX_COMPILER "${TIZEN_TOOLCHAIN}/bin/${TIZEN_TRIPLET}-g++")
set( CMAKE_C_COMPILER "${TIZEN_TOOLCHAIN}/bin/${TIZEN_TRIPLET}-gcc")
set( CMAKE_C_COMPILER_TARGET ${TIZEN_TRIPLET} )
set( CMAKE_CXX_COMPILER_TARGET ${TIZEN_TRIPLET} )
set( CMAKE_CROSSCOMPILING TRUE )

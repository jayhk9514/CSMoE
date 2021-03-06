cmake_minimum_required(VERSION 3.10.2)

set(SDL2_PATH ${CMAKE_CURRENT_SOURCE_DIR}/SDL2 CACHE PATH "SDL2 path")

if(XASH_SDL)
	add_library(sdl2 INTERFACE)
	if(XASH_VCPKG)
		find_package(SDL2 CONFIG REQUIRED)
		target_link_libraries(sdl2 INTERFACE SDL2::SDL2)
	elseif(IOS)
		target_include_directories(sdl2 INTERFACE "${SDL2_PATH}/include")
	else()
		if(NOT CMAKE_CROSSCOMPILING)
			find_package(SDL2 QUIET)
		endif()

		if(SDL2_FOUND)
			target_link_libraries(sdl2 INTERFACE ${SDL2_LIBRARY})
			target_include_directories(sdl2 INTERFACE ${SDL2_INCLUDE_DIR})
			if(MSVC)
				string(REGEX REPLACE "lib$" "dll" SDL2_DLL "${SDL2_LIBRARY}")
				install(FILES ${SDL2_LIBRARY} DESTINATION "." OPTIONAL)
			endif()
		else()
			if(MSVC)
				add_definitions(-D_ftol2_sse=SDL_ftol2_sse) # fix x86 compile
			endif()
			set(DIRECTX OFF CACHE BOOL "" FORCE) # win10 arm dont have dx8
			set(LIBC ON CACHE BOOL "" FORCE) # win10 arm dont have dx8
			add_subdirectory(SDL2 EXCLUDE_FROM_ALL)
			target_link_libraries(sdl2 INTERFACE SDL2-static)
		endif()
	endif()

endif()

find_package(LuaJit QUIET)
if(LuaJit_FOUND AND FALSE) # we are using our own luajit and build from source.
	add_library(luajit_3p INTERFACE)
	target_link_libraries(luajit_3p INTERFACE ${LUAJIT_LIBRARIES})
	target_include_directories(luajit_3p INTERFACE ${LUAJIT_INCLUDE_DIR})
else()
	set(LUAJIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/LuaJIT")
	include(luajit-cmake/LuaJIT.cmake)

	add_library(luajit_3p INTERFACE)
	target_link_libraries(luajit_3p INTERFACE libluajit)
	target_include_directories(luajit_3p INTERFACE "${LUAJIT_DIR}/src")
endif()

if(XASH_VCPKG)
	find_package(nameof CONFIG QUIET)
endif()
if(nameof_FOUND)
else()
	add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/nameof EXCLUDE_FROM_ALL)
endif()
add_library(nameof_3p INTERFACE)
target_link_libraries(nameof_3p INTERFACE nameof::nameof)

add_library(qrcode INTERFACE)
target_sources(qrcode INTERFACE ${CMAKE_SOURCE_DIR}/3rdparty/QRCode/src/qrcode.c)
target_include_directories(qrcode INTERFACE ${CMAKE_SOURCE_DIR}/3rdparty/QRCode/src)
# Install script for directory: /home/rpdzkj/jsoncpp-0.y.z/src/lib_json

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/aarch64-linux-gnu/libjsoncpp.so.0.10.7"
      "$ENV{DESTDIR}/usr/local/lib/aarch64-linux-gnu/libjsoncpp.so.0"
      "$ENV{DESTDIR}/usr/local/lib/aarch64-linux-gnu/libjsoncpp.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/aarch64-linux-gnu/libjsoncpp.so.0.10.7;/usr/local/lib/aarch64-linux-gnu/libjsoncpp.so.0;/usr/local/lib/aarch64-linux-gnu/libjsoncpp.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib/aarch64-linux-gnu" TYPE SHARED_LIBRARY FILES
    "/home/rpdzkj/jsoncpp-0.y.z/build/debug/src/lib_json/libjsoncpp.so.0.10.7"
    "/home/rpdzkj/jsoncpp-0.y.z/build/debug/src/lib_json/libjsoncpp.so.0"
    "/home/rpdzkj/jsoncpp-0.y.z/build/debug/src/lib_json/libjsoncpp.so"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/aarch64-linux-gnu/libjsoncpp.so.0.10.7"
      "$ENV{DESTDIR}/usr/local/lib/aarch64-linux-gnu/libjsoncpp.so.0"
      "$ENV{DESTDIR}/usr/local/lib/aarch64-linux-gnu/libjsoncpp.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/rpdzkj/jsoncpp-0.y.z/build/debug/aarch64-linux-gnu/libjsoncpp.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/rpdzkj/jsoncpp-0.y.z/build/debug/aarch64-linux-gnu" TYPE STATIC_LIBRARY FILES "/home/rpdzkj/jsoncpp-0.y.z/build/debug/src/lib_json/libjsoncpp.a")
endif()


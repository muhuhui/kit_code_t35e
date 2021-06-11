cmake_minimum_required(VERSION 2.8.3)
#cmake_minimum_required(VERSION 3.0.0)
project(slave_pkg)

## Compile as C++11, supported in ROS Kinetic and newer
#add_compile_options(-g -std=c++11)
add_compile_options(-g)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a exec_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a exec_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs  # Or other packages containing msgs
# )

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a exec_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
#catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES slave_pkg
#  CATKIN_DEPENDS std_msgs roscpp rospy
#  DEPENDS system_lib
#)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  include/Common
  include/Common/ipTint
  include/Common/HttpPost
  include/Common/HIKClient
  include/Common/TcpClient
  include/Common/IRAYClient
  include/Common/TcpClient/TinyFrame
  include/Common/TcpClient/TinyFrame/demo
  include/Common/TcpClient/TinyFrame/demo/socket_demo
  include/Driver
  include/ThirdParty
  include/ThirdParty/log4cplus
  include/ThirdParty/alsalib
  include/ThirdParty/zmq
  include/Transaction
  include/Transaction/MoveTaskManage
  #  ${catkin_INCLUDE_DIRS}
)

link_directories(lib
                lib/modbus/arm_64
                lib/json/arm_64
                lib/onvif/arm_64
                lib/Poco/arm_64                
                lib/IUlirNet/arm_64
                lib/log4cplus/arm_64
                lib/alsalib/arm_64
                lib/zmq/arm_64
                lib/IRAY/arm_64
#               lib/httppost
                )

file(GLOB DIRSRCS "src/main.cpp" "src/Transaction/*.cpp" "src/Driver/*.cpp" "include/Common/ipTint/*.c" "include/Common/FtpClient/*.cpp" "include/Common/TcpClient/*.cpp" "include/Common/HIKClient/*.cpp" "include/Common/HttpPost/*.cpp" "include/Common/IRAYClient/*.cpp" "src/Transaction/MoveTaskManage/*.cpp" "src/base/*.cpp" "include/Common/TcpClient/TinyFrame/TinyFrame.c" "include/Common/TcpClient/TinyFrame/demo/utils.c")
add_executable(ORSlave ${DIRSRCS})

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against
target_link_libraries(ORSlave
    #  ${catkin_LIBRARIES}
  libpthread.so
  libzmq.so
  libzmqpp.so
  libmodbus.so
  libjsoncpp.so
# libUlirNetDevSDK.so
  libonvifCamera.so
  libPocoUtil.so
  libPocoData.so
  libPocoFoundation.so
  libPocoDataMySQL.so
  libasound.so
  libcurl.so
  libIRNetClient.so
  PocoData
  PocoFoundation
  PocoNet
  PocoUtil
  PocoJSON
  log4cplus
 # libmyLib.so
)

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
 # install(TARGETS ORSlave
   # ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   # LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   # RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
 # )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   launch/Robot_Slave.launch
#   Config/scp.sh
#   Config/ParametersConfig.json
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_slave_pkg.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)

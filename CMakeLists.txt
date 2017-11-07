cmake_minimum_required(VERSION 2.8.3)
project(fluent_logger_ros)

find_package(catkin REQUIRED COMPONENTS roscpp rosgraph_msgs)

catkin_package()

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

include(ExternalProject)

ExternalProject_Add(
    build_libfluent
    URL https://github.com/m-mizutani/libfluent/archive/v1.1.0.zip
    URL_MD5 83a67a80b8066f17b97be19993d4e990
    BUILD_IN_SOURCE YES
    CONFIGURE_COMMAND cmake -DCMAKE_INSTALL_PREFIX=${CATKIN_DEVEL_PREFIX}
)

add_library(libfluent SHARED IMPORTED)
set_property(TARGET libfluent PROPERTY
             IMPORTED_LOCATION ${CATKIN_DEVEL_PREFIX}/lib/libfluent.so)
add_dependencies(libfluent build_libfluent)

include_directories(include ${catkin_INCLUDE_DIRS}
                    ${CATKIN_DEVEL_PREFIX}/include)

add_executable(fluent_logger src/fluent_logger_ros/node.cpp)
target_link_libraries(fluent_logger ${catkin_LIBRARIES} libfluent)

install(FILES ${CATKIN_DEVEL_PREFIX}/lib/libfluent.so
        DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})

install(TARGETS fluent_logger
   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)


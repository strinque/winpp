cmake_minimum_required(VERSION 3.20)

set(project_name winpp)
set(project_version 1.0)
project(${project_name} 
  LANGUAGES CXX
  VERSION ${project_version})

# set required c++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

add_library(${project_name} INTERFACE)
add_library(${project_name}::${project_name} ALIAS ${project_name})

target_include_directories(${project_name} 
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

include(CMakePackageConfigHelpers)
set(version_config ${PROJECT_BINARY_DIR}/${project_name}-config-version.cmake)
set(project_config ${PROJECT_BINARY_DIR}/${project_name}-config.cmake)
set(targets_export_name ${project_name}-targets)

# generate version and config files
write_basic_package_version_file(
  ${version_config}
  VERSION ${project_version}
  COMPATIBILITY AnyNewerVersion)
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/${project_name}-config.cmake.in
  ${project_config}
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/${project_name})

# install library and headers
install(TARGETS ${project_name}
  EXPORT ${targets_export_name}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/${project_name}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# install version, config and target files
export(
  TARGETS ${project_name}
  NAMESPACE ${project_name}::
  FILE ${PROJECT_BINARY_DIR}/${targets_export_name}.cmake)
install(
  FILES ${project_config} ${version_config}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${project_name})
install(
  EXPORT ${targets_export_name}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${project_name}
  NAMESPACE ${project_name}::)
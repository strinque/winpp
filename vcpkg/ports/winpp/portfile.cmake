vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 1e2696643640090f6668aad00d38408823fb5dda
    SHA512 85c5198d29701b745ab583e3e952447a9cef6522a60f2195b3f3c6c9b3b861db48388f2c092b5c379541ab249750f4e183bd2ba641c15cb9f1a7887323306318
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

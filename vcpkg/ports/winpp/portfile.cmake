vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF fbcce9141579824e44cebb838fce3b2d0083d662
    SHA512 4cf478ab4aeea1eb4c32d73008058018bdb8c596ede51e855a062705983a496270c90ead2704f9028a223e5c1398ae875b24636e3b5dda5384e4cea76122cef8
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

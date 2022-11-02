vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 205ba052c9f19ce2be9271f046b63306834d55f6
    SHA512 2eeecd290495242f0c47c2ba07c4a49894f9a8e1f240653a326e43025d1f5678b024f8af0ccc39efc3bbbeeae0f7d396944796569d25629d242a8e72773430f9
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

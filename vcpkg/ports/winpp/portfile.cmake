vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 21cb8960502fe03fc9085811874580aab85b9abc
    SHA512 25a0b488e4376234f79d5a0a78f33c813eef859cd5640e1e37af78a6d8b86660abec0488377a46dbc79e08600d23d4f59dc87c140211d40c49993b95d9aa1dc7
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

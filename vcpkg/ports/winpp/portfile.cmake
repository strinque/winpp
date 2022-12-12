vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF d3abf830c5c03a763dd4146a6f0ac1b3e4a1c6d9
    SHA512 b3be3e0f0b4ee0e2db6917b560e60c55edc3578cab3da654235267bb079376a6a127815848271f8ea2c74d978b78fadf820217340b7c6623d10c132ab89ab4c0
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

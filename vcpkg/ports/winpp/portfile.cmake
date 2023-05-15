vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 53091d71e2326354bf596a6994fa65f3bdedcc93
    SHA512 32c92bf00314a1ac05021e6a8ae9ffc313d14278114f63107c782985cf51598be736e13e8f2678a7f3aa9ed2de541db58122771e0fefe3eda57146502ab460df
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

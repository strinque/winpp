vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF e51f55e228b50e5078b9e8f5fd12f50caf9d2fbc
    SHA512 cc3fcc76535d2a10e6c4304f21509a9f0655f47f7089b9d2614f5317a87185002581118f0bb89f9c46d15a4e766dd8f7097703ac5befa637e03961a71947c764
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

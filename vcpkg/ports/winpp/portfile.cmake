vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 7d4b708c511748e80ad79000dcc5a2c6b4615294
    SHA512 5650ff0e69d345d63f2bd39e2fd0505ffcb64a5401ccf5f9bb3f6be98ab14e414580fe481dfe625215a5bb0be8fcb6a32dca47b78db69565d0a170f6fb8e9714
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

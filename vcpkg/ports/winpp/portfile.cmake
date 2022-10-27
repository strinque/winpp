vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 015629ca7db587c8245bced16e9cf7b81f22b8a5
    SHA512 e7d51b6f5325acf1f27f3831b38a81e2b0f195d459d213ec6b2e1013a4a3ecc0755949c488e1143ce9c9af9590591a08039ee41564bf48eec601af2687340c91
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

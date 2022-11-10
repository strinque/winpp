vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF c5e61545b0f1ca4ed437296c9f516670abdd60d0
    SHA512 957fe7b2ec61369dfbad14026f077a754964f9b52c8d886f026ec8f3458d7fae62969bdc33ec77bb3e9443ec0d62e9f3d34afbe0fad0eff767dfedfe361fc564
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF ad55515fe2f948f95ea91bb159aba7aba3ba97de
    SHA512 19f45633f91ad795b1b827d7a827b64656531f929fdff80879b3b012d481485c29fa77ff53e3d2a2f3c72c26540f77b465101574d19beae886b68f6d74b50b11
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

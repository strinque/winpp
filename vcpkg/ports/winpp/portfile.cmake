vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 334c0e283f7df056d6188221da82bbf6f433e17a
    SHA512 db8f5bd184587b4c479f8acfbc2fc8a512d1166e5dda9273d006bc0c7a92f9b798dc207411afeee93a032da5ec36bf96a1abde0dfc2a4567e0151aa7427e64fa
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

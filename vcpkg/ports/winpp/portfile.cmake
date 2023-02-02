vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF e121fde8eb121b5d3c54eab70ec8fa7799f09b3d
    SHA512 1c5818a5e355ce51c41537d5e6a0a814c6ede0052a946a09e625ca363a8a0ebe3e9a1cc957cbec5fe2902d6d36654859c6dca19284075d27d6fa66d032310cc1
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

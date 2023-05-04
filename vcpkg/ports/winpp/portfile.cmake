vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 66d36ecb0fd9517272e7c3e66726043f6c26e6fb
    SHA512 46e7c4bf454ccfe4808bd7c485eaf1b233e3184c2307be277ce8d012a46c9824a03b3fc9078fbf61581f27786c7639158db720148f6e53711a2be7ce466bc8e3
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

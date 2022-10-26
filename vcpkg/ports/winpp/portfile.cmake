vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF feaafc9a5fe75035594b81430a9955f62164cf4c
    SHA512 0793ef1cbbb9775263e56e5974e903425abcac948ecc280d290a39eefe21049f3c47c6015169dba93a21207db2469fb88756e5b488a72f4715ff86d8157eccad
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 963fb1067a1df6b30b391fd41565fa45f90a426e
    SHA512 1dffec550c146a8f6a2daae29e4b85b39b8af8c2320b1b6043dba29e2fe3ad54d93fe48832630cfa2ad17b910928e28ccd300328eb8eae231509f3eeaf024cda
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

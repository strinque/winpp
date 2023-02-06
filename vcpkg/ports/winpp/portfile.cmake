vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 92341ab1b24beb326cea99c5d4efffa1026579b2
    SHA512 e432c72ff454572d5cad2cf8f31bf7691177c931680498cb62964968cd1ccd690c38398d21726072b534fecd462d56cb4527c28bea867a07eec5b1174a7ccac2
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

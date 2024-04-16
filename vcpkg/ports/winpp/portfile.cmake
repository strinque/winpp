vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF cbdc96dca32dffb35e3df961d538cd39f38a1c3f
    SHA512 6a1112c2cd58df74ebd5cd92312d0b3b6e637b6a343c23d5084d8bd7e3a7b19a9702f1b9e5e1f758b013de874508400d06afa5ddffd1508849039912afb3b1ed
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 9b905141136031d69eb417e8e2d15edf8b1ec590
    SHA512 5502e6140ff6ff73c241c2d354f8e4c941e8d46669c6fb2ffdd3b26ac0b03eba0d9e1c09034e67f95983c12d9ebb911822d991726e1931012aefc16dd7116862
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 24e47e42ccbe47f5e44b1d7ba8d847cecdd9b31a
    SHA512 8e19eef643631688561fe28932c9b6e124605fb783c3ad2292a58b5a24fcc93ba86894646b442aa35899acf09e66b39f28256acd4aa6cb4f1a427c34cb37b79a
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

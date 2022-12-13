vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 5212b00fbe47eb2a161140311853238f50fabec1
    SHA512 20a3f0f8e390e8002107620b42559b5beda1f2b8c30df426a0ef9680be0b9ef9fe14eb3464cb8747c72eca789c076a510112ff1cd6d594ae9f830e004cbaf0c4
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

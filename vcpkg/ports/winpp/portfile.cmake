vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF b54eb00ea8bea23ff780a4a7d787e501c7bf2254
    SHA512 dc0216aa8651d4bd9bfe378aa93dfd0fc1f889a624d867f6c5ec74732633a9ed06980a9b06253844858631d05d6200023356a4d29240f4c2eb1b7317b28674f9
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

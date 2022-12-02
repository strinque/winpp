vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 8afc34abc0c22f28c2b9eb2b4963de4ca3ed34f7
    SHA512 9d42962be8e14070af13358a5b5c815b02ea95d6472424956c8ad431516dee1dc770bff21ccecced2bf775085c49fc067eab94be256cbd72e6d228bf730bb9c7
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

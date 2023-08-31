vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 00e4ac2d16d7efc2efed77d6aa26ad38c5f71a4e
    SHA512 edc8e37d526b6f9c195d14f277f7619ca2fef37b73cf397acde4e8ead56c2d6d78172dd96d47ed6fb8878ada822d2bc88cb65fc94914eb549bc06cd8096cadf8
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

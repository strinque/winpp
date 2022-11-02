vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/winpp
    REF 896525a4058ab978465f85c12ac35a66ef4b6aa4
    SHA512 05480ffb54db01d033b502e351e258857308d6695acd8f75c30b441664276c65f456dfc38963fc155de387849f40c87096156b9dcd292a459879da72dba683ce
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

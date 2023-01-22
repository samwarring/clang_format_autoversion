include(GNUInstallDirs)

# Install the license file
configure_file("${PROJECT_SOURCE_DIR}/LICENSE" LICENSE.txt)
if(WIN32)
    set(INSTALL_LICENSE_DIR ".")
else()
    set(INSTALL_LICENSE_DIR "${CMAKE_INSTALL_DATAROOTDIR}/clang-format-autoversion")
endif()
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt" DESTINATION "${INSTALL_LICENSE_DIR}")

# Variables common to all CPack generators
set(CPACK_PACKAGE_NAME clang-format-autoversion)
set(CPACK_PACKAGE_VENDOR "Sam Warring")
set(CPACK_PACKAGE_DESCRIPTION
    "Distribution of multiple clang-format versions with a launcher to auto-select the correct version"
)
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/samwarring/clang_format_autoversion")
set(CPACK_PACKAGE_CONTACT "Sam Warring <samwarring@gmail.com>")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Clang Format AutoVersion") # (Only used by WIX)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")

# Variables specific to CPack DEB generator
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
if(TARGET_ARCH STREQUAL x64)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
elseif(TARGET_ARCH STREQUAL arm64)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE arm64)
endif()

# Variables specific to CPack WIX generator
set(CPACK_WIX_UPGRADE_GUID EE4C9757-9349-4B61-865E-6A3BED156681)
set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/patch.xml")
set(CPACK_WIX_EXTRA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/extra.wxs")
set(CPACK_WIX_EXTENSIONS WixUIExtension)
set(CPACK_WIX_UI_REF WixUI_FeatureTree)

include(CPack)

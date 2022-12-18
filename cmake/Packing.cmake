set(CPACK_PACKAGE_NAME ${EXE_NAME})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY 
  "Scientific software for molecular volumes and surface areas\nProvides calculation and surface map exports.")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "jmaglic@outlook.de")
set(CPACK_RESOURCE_FILE_README ${README_FILE})

if (APPLE)
  set(CPACK_RESOURCE_FILE_LICENSE ${OSX_LICENSE_RTF})
else()
  set(CPACK_RESOURCE_FILE_LICENSE ${LICENSE_FILE})
endif()

set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jasmin B. Maglic <${CPACK_PACKAGE_CONTACT}>")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${LINUX_ARCHITECTURE})
set(CPACK_DEBIAN_FILE_NAME 
  ${PROJECT_NAME}_debian_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}_v${CPACK_PACKAGE_VERSION}.deb)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_DMG_VOLUME_NAME ${PROJECT_NAME})
set(CPACK_DMG_BACKGROUND_IMAGE ${OSX_DMG_BACKGROUND})
set(CPACK_DMG_DS_STORE ${OSX_DMG_DSSTORE})

include(CPack)

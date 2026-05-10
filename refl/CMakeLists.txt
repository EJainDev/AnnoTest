add_library(cpputils_refl SHARED)
target_sources(
  cpputils_refl
  PUBLIC FILE_SET
         public_modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         ${CMAKE_CURRENT_SOURCE_DIR}
         FILES
         utils.cxx)

add_library(cpputils::refl ALIAS cpputils_refl)

add_dependencies(cpputils cpputils_refl)

install(
  TARGETS cpputils_refl
  EXPORT cpputils_targets
  FILE_SET public_modules DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpputils/refl
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

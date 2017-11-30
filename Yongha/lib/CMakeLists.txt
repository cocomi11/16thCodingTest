set (LIB_DIRECTORY "/usr/local/lib/")

add_library(NTL STATIC IMPORTED GLOBAL)		  
add_library(GMP STATIC IMPORTED GLOBAL)

set_target_properties(NTL PROPERTIES IMPORTED_LOCATION ${LIB_DIRECTORY}libntl.a)
set_target_properties(GMP PROPERTIES IMPORTED_LOCATION ${LIB_DIRECTORY}libgmp.a)

set (HELIB_SOURCE_DIRECTORY "~/Library/HElib/src/")

add_library(HElib STATIC IMPORTED GLOBAL)		  

set_target_properties(HElib PROPERTIES IMPORTED_LOCATION ${HELIB_SOURCE_DIRECTORY}fhe.a)
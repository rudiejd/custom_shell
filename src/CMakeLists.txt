set(HEADER_LIST "${minsh_shell_SOURCE_DIR}/include/minsh.hpp")

add_library(minsh_lib minsh.cpp ${HEADER_LIST})

target_include_directories(minsh_lib PUBLIC ../include )

if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(minsh_lib PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(minsh_lib "${CMAKE_THREAD_LIBS_INIT}")
endif()

target_compile_features(minsh_lib PUBLIC cxx_std_11)

source_group(
	TREE "${PROJECT_SOURCE_DIR}/include"
	PREFIX "Header Files"
	FILES ${HEADER_LIST}
	)

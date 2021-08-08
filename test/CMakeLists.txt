# use catch2 as a testing library
FetchContent_Declare(
	catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v2.13.7
	)
FetchContent_MakeAvailable(catch2)

add_executable(test_minsh test_minsh.cpp)

target_compile_features(test_minsh PRIVATE cxx_std_11)

target_link_libraries(test_minsh PRIVATE minsh_lib Catch2::Catch2)

add_test(NAME test_minsh_test COMMAND test_minsh)


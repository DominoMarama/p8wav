option(CLIP_EXAMPLES "Compile clip examples" off)
option(CLIP_TESTS "Compile clip tests" off)

add_subdirectory(clip)

add_executable(p8wav p8wav.cpp)

target_link_libraries(p8wav PUBLIC clip)

target_include_directories(p8wav PUBLIC
	"${PROJECT_SOURCE_DIR}/include"
	"clip"
)

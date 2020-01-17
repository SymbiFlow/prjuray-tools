add_executable(bitread bitread.cc)
target_link_libraries(bitread absl::optional absl::strings gflags libprjxray)

add_executable(segmatch segmatch.cc)
target_link_libraries(segmatch gflags absl::strings)

add_executable(bittool bittool.cc)
target_link_libraries(bittool libprjxray gflags absl::strings absl::span)

add_executable(xc7_frame_address_decoder xcu_frame_address_decoder.cc)
target_link_libraries(xc7_frame_address_decoder libprjxray)

add_executable(xcu_frame_address_decoder xc7_frame_address_decoder.cc)
target_link_libraries(xcu_frame_address_decoder libprjxray)

add_executable(gen_part_base_yaml gen_part_base_yaml.cc)
target_link_libraries(gen_part_base_yaml
	absl::optional
	absl::span
	libprjxray
	yaml-cpp
	gflags
)
add_executable(xcpatch xcpatch.cc)
target_link_libraries(xcpatch
	absl::strings
	absl::time
	gflags
	libprjxray
)

add_executable(xcframes2bit xcframes2bit.cc)
target_link_libraries(xcframes2bit
	absl::strings
	absl::time
	gflags
	libprjxray
)

add_library(common common.cpp)

add_executable(dump_bitstream dump_bitstream.cpp)
target_compile_options(dump_bitstream PRIVATE -Wno-error)
target_link_libraries(dump_bitstream common)

add_executable(correlate correlate.cpp)

add_executable(correlate_segdata correlate_segdata.cpp)
target_link_libraries(correlate_segdata absl::strings  absl::str_format absl::hash gflags)

add_executable(stripdb stripdb.cpp)

add_executable(explain explain.cpp)
target_link_libraries(explain common)

add_executable(assemble assemble.cpp)
target_link_libraries(assemble common)

add_subdirectory(bits2rbt)

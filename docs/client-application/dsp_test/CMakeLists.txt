
if (${ARM})
	set(dsp_SRCS libdsp.cpp)
else (${ARM})
	# Simulator!
	set(dsp_SRCS libdsp_simulation.cpp)
endif (${ARM})

add_library(dsp SHARED ${dsp_SRCS})
target_link_libraries(dsp utils)

install(TARGETS dsp
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)
project( recordVideo )

find_package( OpenCV 3.2.0 REQUIRED )
find_package( Boost 1.40 COMPONENTS program_options filesystem thread REQUIRED )

set(SOURCE_FILES
		main.cpp
		src/VideoRecorder.cpp
		src/autopilot_interface.cpp
		src/mavlink_control.cpp
		src/serial_port.cpp
		src/IMUrecorder.cpp
		)
set(HEADER_FILES
		include/VideoRecorder.h
		include/autopilot_interface.h
		include/mavlink_control.h
		include/serial_port.h
		include/mavlink/v1.0/common
		include/IMUrecorder.h
		)
include_directories(
		${CMAKE_SOURCE_DIR}/include
		${Boost_INCLUDE_DIR}
		${CMAKE_SOURCE_DIR}/include/mavlink/v1.0/
)

add_executable( recordVideo ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries( recordVideo ${OpenCV_LIBS} ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY}
		${Boost_THREAD_LIBRARY})
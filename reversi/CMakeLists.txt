find_library (SIO_LIB
  NAMES libsioclient sioclient
  PATHS /usr/lib)
find_path (SIO_PATH
  NAMES sio_client.h sio_message.h sio_socket.h
  PATHS /usr/include)
if (NOT SIO_LIB OR NOT SIO_PATH)
	message (FATAL_ERROR "Socket io library or headers not found")
endif()
find_package (Threads)
find_package (Boost COMPONENTS thread REQUIRED)
set (GAME_SRC_FILES reversi.cpp game/game.cpp ai/ai.cpp)

add_executable (reversi ${GAME_SRC_FILES})
target_link_libraries(reversi
  ${SIO_LIB}
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT})
target_include_directories (reversi
  PUBLIC
  ${SIO_PATH}
  ${Boost_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/reversi)



message("\n----- checking libevent -----")
find_package(libevent REQUIRED)

set(RECEIVER_FILES receiver.cpp)
set(SENDER_FILES sender.cpp)
add_executable(libevent_integration_sender ${SENDER_FILES})
add_executable(libevent_integration_receiver ${RECEIVER_FILES})

include_directories(${LIBEVENT_INCLUDE_DIR})
target_link_libraries(libevent_integration_receiver ${LIBEVENT_LIB} zmq)
target_link_libraries(libevent_integration_sender ${LIBEVENT_LIB} zmq)

SET(SRC_FILES dealer_reach_sndhwm.cpp)
add_executable(dealer_reach_sndhwm ${SRC_FILES})
target_link_libraries(dealer_reach_sndhwm zmq)
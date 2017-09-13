//
// Created by Wang Ke on 13/09/2017.
//

#include <zmq.hpp>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

int main() {
    const int sndhwm = 1000;
    zmq::context_t ctx;
    zmq::socket_t dealer(ctx, ZMQ_DEALER);
    dealer.setsockopt(ZMQ_SNDHWM, sndhwm);


    // for this program to exit normally since we'll not really
    // start a router to received all these 'sent messages'
    dealer.setsockopt(ZMQ_LINGER, 0);


    pid_t pid = getpid();
    std::string ipc_name = "ipc://dealer_reach_sndhwm";
    ipc_name.append(std::to_string(static_cast<unsigned int>(pid)));
    dealer.connect(ipc_name);

    std::cout << "ZMQ_SNDHWM: " << sndhwm << std::endl;


    // Test if we'll be blocked when sending the first through <ZMQ_SNDHWM>th message
    bool ever_blocked = false;
    for (int i = 0; i < sndhwm; i++) {
        size_t ret = dealer.send("abc", 3, ZMQ_DONTWAIT);
        if (ret == 0) {
            std::cout << "blocked when sending message NO." << i+1 << std::endl;
            ever_blocked = true;
        }
    }

    if (!ever_blocked) {
        std::cout << sndhwm << " messages sent in queue without blocking" << std::endl;
    } else {
        std::cout << "fatal error, unexpected ZeroMQ behaviour" << std::endl;
        return -1;
    }

    // Test if we'll be blocked when sending the <ZMQ_SNDHWM + 1>th message
    std::cout << "trying to send another message, it's expected to be blocked" << std::endl;
    size_t ret = dealer.send("abc", 3, ZMQ_DONTWAIT);
    if (0 == ret) {
        std::cout << "it's blocked" << std::endl;
    } else {
        std::cout << "fatal error, unexpected ZeroMQ behaviour" << std::endl;
        return -1;
    }

    // Given a ZMQ socket already in `mute status` due to having reached high water mark,
    // test if this socket won't wake up zmq_poll of its ZMQ_POLLOUT event
    // yes, zmq::socket_t has overload operator void*
    zmq_pollitem_t item { (void *)dealer, 0, ZMQ_POLLOUT, 0};
    std::cout << "now, give it 5 seconds to allow the event loop be signaled by ZMQ_POLLOUT event..." << std::endl;
    int count = zmq::poll(&item, 1, 5000);
    if (count <= 0) {
        std::cout << "5 seconds passed and received no ZMQ_POLLOUT event." << std::endl;
    }

    return 0;
}


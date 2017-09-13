//
// Created by Wang Ke on 13/09/2017.
//

#include <zmq.hpp>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <atomic>

int main() {
    const int sndhwm = 1000;
    zmq::context_t ctx;
    zmq::socket_t dealer(ctx, ZMQ_DEALER);
    dealer.setsockopt(ZMQ_SNDHWM, sndhwm);


    // for this program to exit normally since we'll not really
    // start a router to received all these 'sent messages'
    dealer.setsockopt(ZMQ_LINGER, 0);


    pid_t pid = getpid();
    std::string ipc_name = "ipc://dealer_fillup_sndhwm";
    ipc_name.append(std::to_string(static_cast<unsigned int>(pid)));
    dealer.connect(ipc_name);

    std::cout << "connect to " << ipc_name << std::endl;
    std::cout << "ZMQ_SNDHWM: " << sndhwm << std::endl;

    // yes, zmq::socket_t has overload operator void*
    zmq_pollitem_t item { (void *)dealer, 0, ZMQ_POLLOUT, 0};


    std::cout << "now,  using zmq_poll to detect ZMQ_POLLOUT event to fill up send queue" << std::endl;

    std::atomic<int> ever_timeout(0);
    size_t sent = 0;
    while (!ever_timeout) {
        int count = zmq::poll(&item, 1, 3000);
        if (count <= 0) {
            std::cout << "3 seconds passed and received no ZMQ_POLLOUT event, exit event loop, has sent "
                      << sent << " messages" << std::endl;
            ever_timeout.store(1);
        } else {
            if (item.revents & ZMQ_POLLOUT) {
                size_t ret = dealer.send("abc", 3, ZMQ_DONTWAIT);
                if (ret == 0) {
                    std::cout << "fatal error, blocked when not expected" << std::endl;
                    return -1;
                }
                sent++;
            } else {
                std::cout << "unexpected behaviour" << std::endl;
                return -1;
            }
        }
    }


    return 0;
}


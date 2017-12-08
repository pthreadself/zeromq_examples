//
// Created by pthreadself on 07/12/2017.
//

#include <memory>
#include <iostream>
#include <zmq.hpp>
#include <chrono>
#include <thread>


int main() {

    std::shared_ptr<zmq::context_t> ctx;
    std::shared_ptr<zmq::socket_t> socket;

    try {
        ctx = std::make_shared<zmq::context_t>(1);
        socket = std::make_shared<zmq::socket_t>(*ctx, ZMQ_DEALER);
        socket->connect("ipc://libevent_integration_server");

    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }

    std::string data("hello world");

    std::chrono::seconds interval(1);
    while(1) {
        zmq::message_t msg(data.c_str(), data.size());
        socket->send(msg);
        std::cout << "sending msg..." << std::endl;
        std::this_thread::sleep_for(interval);
    }


    return 0;
}


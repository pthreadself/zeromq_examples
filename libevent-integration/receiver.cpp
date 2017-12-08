//
// Created by pthreadself on 07/12/2017.
//

#include <memory>
#include <iostream>
#include <event2/event.h>
#include <zmq.hpp>

void business_logic(evutil_socket_t fd, short flags, void * context) {
    printf("Got an event on socket %d:%s%s%s%s\n",
           fd,
           (flags&EV_TIMEOUT) ? " timeout" : "",
           (flags&EV_READ)    ? " read" : "",
           (flags&EV_WRITE)   ? " write" : "",
           (flags&EV_SIGNAL)  ? " signal" : "");

    std::shared_ptr<zmq::socket_t> socket =  *(std::shared_ptr<zmq::socket_t>*)context;
    int flag = socket->getsockopt<int>(ZMQ_EVENTS);
    if (flag & ZMQ_POLLIN) {
        std::cout << "ZMQ_POLLIN effective" << std::endl;
        bool should_continue;
        do {
            zmq::message_t msg;
            should_continue = socket->recv(&msg, ZMQ_DONTWAIT);
            if (should_continue)
                std::cout << "ZMQ_POLLIN  msg size is:" << msg.size() << ",msg is:" << (char*)msg.data() << std::endl;
        }while(should_continue);
    } else {
        std::cout << "msg is not complete" << std::endl;
    }
}

int main() {

    std::shared_ptr<zmq::context_t> ctx;
    std::shared_ptr<zmq::socket_t> socket;

    try {
        ctx = std::make_shared<zmq::context_t>(1);
        socket = std::make_shared<zmq::socket_t>(*ctx, ZMQ_DEALER);
        socket->bind("ipc://libevent_integration_server");
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }

    struct event_base * base = event_base_new();
    if (!base)
        return -1;


    struct event * read_event = event_new(base, socket->getsockopt<int>(ZMQ_FD), EV_READ | EV_PERSIST, business_logic, (void*)&socket);

    if (!read_event)
        return -1;

    event_add(read_event, nullptr);
    event_base_dispatch(base);

    return 0;
}
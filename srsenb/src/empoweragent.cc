#include "srsenb/hdr/empoweragent.h"

#include "srslte/common/threads.h"

#include <empoweragentproto/empoweragentproto.hh>


#include <iostream>

namespace Empower {
namespace Agent {

/// @brief Private attributes of the Empower agent
struct Agent::PrivateBits {
    // Identifier of the agent thread
    pthread_t  agentThread;
    NetworkLib::IPv4Address controllerAddress;
    std::uint16_t           controllerPort;
};

Agent::Agent(void) {
    mPrivateBits = std::unique_ptr<PrivateBits>(new PrivateBits);
}

Agent::~Agent() {
}

bool Agent::init(void) {
    // mPrivateBits->controllerAddress = ...
    // mPrivateBits->contorllerPort = ...

    // No error occurred while initializing
    return false;
}


void Agent::start() {

    pthread_attr_t     attr;
    struct sched_param param;
    param.sched_priority = 0;
    pthread_attr_init(&attr);
    // pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
    pthread_attr_setschedparam(&attr, &param);
    
    if (pthread_create(&(mPrivateBits->agentThread),
                       &attr,
                       run,
                       reinterpret_cast<void *>(this))) {
        perror("pthread_create");
        exit(-1);
    }    
}


void * Agent::run(void *arg) {
    Agent *thisInstance = reinterpret_cast<Agent *>(arg);
    thisInstance->mainLoop();
    return nullptr;
}


void Agent::mainLoop() {

    IO io;

    try {
        auto readBuffer = io.makeMessageBuffer();
        auto writeBuffer = io.makeMessageBuffer();

        for (;;) {

            bool performPeriodicTasks = false;
            bool dataIsAvailable = false;

            if (io.isConnectionClosed()) {
                io.openSocket();
            }

            // Retest
            if (io.isConnectionClosed()) {
                // Connection still closed. Sleep for a while
                io.sleep();
                performPeriodicTasks = true;
            } else {
                // The connection is opened. Let's see if there's data
                dataIsAvailable = io.isDataAvailable();

                if (!dataIsAvailable) {
                    // Timeout expired
                    performPeriodicTasks = true;
                }
            }

            if (dataIsAvailable) {
                // Read a message
                auto messageBuffer = io.readMessage(readBuffer);

                if (!messageBuffer.empty()) {

                    std::cout << "Received message\n" << messageBuffer;

                    // Decode the message
                    MessageDecoder messageDecoder(messageBuffer);

                    if (!messageDecoder.isFailure()) {

                        switch (messageDecoder.header().entityClass()) {
                        case EntityClass::ECHO_SERVICE:
                            std::cout << "Got message class for ECHO SERVICE\n";

                            {
                                TLVBinaryData tlv;
                                messageDecoder.get(tlv);
                                tlv.stringData(tlv.stringData() +
                                               " Here I am!");

                                MessageEncoder messageEncoder(writeBuffer);

                                messageEncoder.header()
                                    .messageClass(
                                        MessageClass::RESPONSE_SUCCESS)
                                    .entityClass(
                                        EntityClass::ECHO_SERVICE);

                                messageEncoder.add(tlv).end();

                                std::cout << "Sending back reply\n"
                                          << messageEncoder.data();

                                // Write a message to the socket
                                size_t len =
                                    io.writeMessage(messageEncoder.data());

                                std::cout << "Wrote " << len << " bytes\n";
                            }

                            break;

                        default:
                            std::cout << "Got unmanaged message class\n";
                            break;
                        }
                    }
                }
            } else if (performPeriodicTasks) {
                // Timeout expired
                std::cout << "AGENT: still waiting for messages... "
                             "(isConnectionClosed() is "
                          << io.isConnectionClosed() << ")\n";


                if (! io.isConnectionClosed()) {
                    // TODO: Here I'd send a HELLO request to the controller.
                    //                     
                }

            } else {
                // We should never end here.
                // throw ...
            }
        }

    } catch (std::exception &e) {
        std::cerr << "Caught exception in main agent loop: " << e.what()
                  << '\n';
    }
}

} // namespace Agent
} // namespace Empower




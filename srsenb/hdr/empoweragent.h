#ifndef EMPOWER_AGENT_H
#define EMPOWER_AGENT_H

// For srslte::logger
#include "srslte/common/logger.h"

// For std::unique_ptr<T>
#include <memory>

namespace Empower {
namespace Agent {

class Agent {
public:
    Agent();
    ~Agent();

    /// @brief Initialize the agent
    bool init(void);

    /// @brief Start the agent main loop in its own thread.
    void start();
  private:
    // Helper method
    static void *run(void *arg);

    // The agent main loop.
    void mainLoop();

    // Private bits of the Agent (using PIMPL idiom).
    struct PrivateBits;
    std::unique_ptr<PrivateBits> mPrivateBits;
};

}
}

#endif

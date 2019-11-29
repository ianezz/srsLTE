#ifndef EMPOWER_AGENT_H
#define EMPOWER_AGENT_H

// For std::unique_ptr<T>
#include <memory>

namespace Empower {
namespace Agent {

struct AgentPrivateBits;

class Agent {
public:
    Agent();
    ~Agent();
    
    bool init(void);

    void start();
    void stop();
  private:
    static void *run(void *arg);

    
    void mainLoop();

    // Note: not a std::unique_ptr<AgentPrivateBits> because it's an
    // incomplete type.
    AgentPrivateBits *mPrivateBits;
};

}
}

#endif

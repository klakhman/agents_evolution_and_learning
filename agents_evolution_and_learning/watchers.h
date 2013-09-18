#ifndef WATCHERS_H
#define WATCHERS_H

#include "TEnvironment.h"

#include <string>
#include <vector>

class TAgent;

namespace watchers{
  class LifeWatcher{
  public:
    virtual void watch(TAgent& agent, TEnvironment& env) = 0;
    virtual void print(const std::string& filename) = 0;
    virtual void clear() = 0;
    virtual ~LifeWatcher() {}
  };

  class ActNeuronsWatcher : public LifeWatcher{
  public:
    virtual void watch(const TAgent& agent, const TEnvironment& env);
    virtual void print(const std::string& filename);
    virtual void clear();
    virtual ~ActNeuronsWatcher() {};
  private:
    std::vector<unsigned int> actNeuronsQ;
  };
}

#endif // WATCHERS_H
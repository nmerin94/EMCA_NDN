#ifndef NDN_RELAY_H
#define NDN_RELAY_H



#include "ndn-app.hpp"
#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "ns3/nstime.h"
#include "ns3/ptr.h"

#include "ns3/random-variable-stream.h"
#include "ns3/data-rate.h"

#include "ns3/ndnSIM/utils/ndn-rtt-estimator.hpp"

#include <set>
#include <map>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

namespace ns3 {
namespace ndn {


class Relay : public App {
public:
  static TypeId
  GetTypeId(void);

  Relay();

  // inherited from NdnApp
  virtual void
  OnInterest(shared_ptr<const Interest> interest);

  virtual void 
  OnData(shared_ptr<const Data> data);


protected:
  // inherited from Application base class.
  virtual void
  StartApplication(); // Called at time specified by Start

  virtual void
  StopApplication(); // Called at time specified by Stop

private:
  Name m_prefix;
  /*Name m_postfix;

  uint32_t m_signature;
  Name m_keyLocator;*/
};

} // namespace ndn
} // namespace ns3

#endif // NDN_RELAY

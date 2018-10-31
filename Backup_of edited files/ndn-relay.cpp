#include "ndn-relay.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"

#include "utils/ndn-ns3-packet-tag.hpp"
#include "utils/ndn-rtt-mean-deviation.hpp"

#include <ndn-cxx/lp/tags.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>
#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"
#include <memory>

NS_LOG_COMPONENT_DEFINE("ndn.Relay");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(Relay);

TypeId
Relay::GetTypeId(void)
{
  static TypeId tid = TypeId("ns3::ndn::Relay").SetParent<ndn::App>().AddConstructor<Relay>()
  .AddAttribute("Prefix", "Prefix, for which relay forwards the interest and data", StringValue("/"),
                    MakeNameAccessor(&Relay::m_prefix), MakeNameChecker());
  /*.AddAttribute(
         "Postfix",
         "Postfix that is added to the output data (e.g., for addingRelay-uniqueness)",
         StringValue("/"), MakeNameAccessor(Relay::m_postfix), MakeNameChecker())
  .AddAttribute(
         "Signature",
         "Fake signature, 0 valid signature (default), other values application-specific",
         UintegerValue(0), MakeUintegerAccessor(Relay::m_signature),
         MakeUintegerChecker<uint32_t>())
      .AddAttribute("KeyLocator",
                    "Name to be used for key locator.  If root, then key locator is not used",
                    NameValue(), MakeNameAccessor(Relay::m_keyLocator), MakeNameChecker());*/
  return tid;
}
Relay::Relay()
{
  NS_LOG_FUNCTION_NOARGS();
}

// inherited from Application base class.
void
Relay::StartApplication()
{
  NS_LOG_FUNCTION_NOARGS();
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
}
void
Relay::StopApplication()
{
  NS_LOG_FUNCTION_NOARGS();

  App::StopApplication();
}

void
Relay::OnInterest(shared_ptr<const Interest> interest)
{
	App::OnInterest(interest); // tracing inside

 	NS_LOG_FUNCTION(this << interest);
 	NS_LOG_INFO("> Interest recieved " << interest->getName());
 	if (!m_active)
    return;
	m_transmittedInterests(interest, this, m_face);

  	m_appLink->onReceiveInterest(*interest);

}


void
Relay::OnData(shared_ptr<const Data> data)
{
	if (!m_active)
    	return;

    App::OnData(data); // tracing inside
    NS_LOG_FUNCTION(this << data);
	//FibHelper::AddRoute(GetNode(), data->getName().
	NS_LOG_INFO("node(" << GetNode()->GetId() << ") forwarding Data: " << data->getName());
	/*auto hopCountTag = data->getTag<lp::HopCountTag>();
	int HopCount = *hopCountTag + 1;
	shared_ptr<lp::HopCountTag> hc = &HopCount;
	data->setTag(hc);*/
	m_transmittedDatas(data, this, m_face);
 	m_appLink->onReceiveData(*data);
}

}
}//ndn//ns3

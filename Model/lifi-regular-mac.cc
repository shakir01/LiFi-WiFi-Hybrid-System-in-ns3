#include "ns3/lifi-regular-mac.h"
#include "ns3/assert.h"
#include "ns3/enum.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "tdma-mac-low.h"
#include "ns3/abort.h"

NS_LOG_COMPONENT_DEFINE ("LiFiRegularMac");

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (LiFiRegularMac);

/*************************************************************
 * Tdma Controller Class Functions
 ************************************************************/
TypeId
LiFiRegularMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3:LiFiRegularMac")
    .SetParent<Object> ()
    .AddConstructor<LiFiRegularMac> ();
    return tid;
}

 LiFiRegularMac::LiFiRegularMac ()
 {

 }
 
LiFiRegularMac::~LiFiRegularMac ()
{

}

void LiFiRegularMac::Start (void)
{
  
}

 void LiFiRegularMac::SetChannel (Ptr<LiFiChannel> c)
 {

 }

Ptr<LiFiChannel> LiFiRegularMac::GetChannel (void)
{
  return 0;
}

Time LiFiRegularMac::CalculateTxTime (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION (packet);
//std::cout<<"packet->GetSize () "<<packet->GetSize ()<<std::endl;
  NS_ASSERT_MSG (packet->GetSize () < 1500,"PacketSize must be less than 1500B, it is: " << packet->GetSize ());

  return Time(Seconds(0));
}

}

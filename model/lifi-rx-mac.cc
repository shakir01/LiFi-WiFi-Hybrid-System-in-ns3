#include "lifi-rx-mac.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/wifi-mac-trailer.h"

NS_LOG_COMPONENT_DEFINE ("LiFiRxMac");

namespace ns3
{

void LiFiRxMac::NotifyRx (Ptr<const Packet> packet)
{
  m_macRxTrace (packet);
}

void
LiFiRxMac::NotifyPromiscRx (Ptr<const Packet> packet)
{
  m_macPromiscRxTrace (packet);
}

void
LiFiRxMac::SetForwardUpCallback (Callback<void,Ptr<Packet>,WifiMacHeader *> upCallback)
{
  //NS_LOG_FUNCTION (this);
 // std::cout<<"TdmaCentralMac::SetForwardUpCallback (Callback<void,Ptr<Packet>, Mac48Address, Mac48Address> upCallback)"<<std::endl;
  m_upCallback = upCallback;
}

void
LiFiRxMac::NotifyRxDrop (Ptr<const Packet> packet)
{
  m_macRxDropTrace (packet);
}

void LiFiRxMac::DoDispose (void)
{

}

void LiFiRxMac::ForwardUp (Ptr<Packet> packet, WifiMacHeader *hdr)
{
 /* ns3::LlcSnapHeader llc;
  packet->RemoveHeader (llc);
  //std::cout<<"TdmaCentralMac::ForwardUp() llc.GetType() "<<llc.GetType()<<std::endl;*/
  m_upCallback (packet,hdr);
  NotifyRx(packet);
}

void
LiFiRxMac::Receive (Ptr<Packet> packet)
{
  WifiMacHeader hdr;
  packet->PeekHeader (hdr);
  packet->RemoveHeader (hdr);
  WifiMacTrailer fcs;
  packet->RemoveTrailer (fcs);
  ForwardUp (packet, &hdr);

}

}
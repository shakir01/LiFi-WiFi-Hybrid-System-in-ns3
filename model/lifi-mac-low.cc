#include "ns3/packet.h"
#include "ns3/log.h"
#include "lifi-mac-low.h"
#include "ns3/wifi-mac-trailer.h"
#include "ns3/lifi-tx-net-device.h"
#include "lifi-rx-mac.h"

NS_LOG_COMPONENT_DEFINE ("LiFiMacLow");
namespace ns3 {

LiFiMacLow::LiFiMacLow () :m_currentPacket (0)
{
}

LiFiMacLow::~LiFiMacLow ()
{
}

void
LiFiMacLow::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_channel = 0;
  m_device = 0;
}

void
LiFiMacLow::SetAddress (Mac48Address ad)
{
  m_self = ad;
}

void
LiFiMacLow::SetBssid (Mac48Address bssid)
{
  m_bssid = bssid;
}

Mac48Address
LiFiMacLow::GetAddress (void) const
{
  return m_self;
}

Mac48Address
LiFiMacLow::GetBssid (void) const
{
  return m_bssid;
}

void
LiFiMacLow::SetRxCallback (Callback<void,Ptr<Packet> > callback)
{
  m_rxCallback = callback;
}

void LiFiMacLow::SetForwardUpManagmentCallback (Callback<void,Ptr<Packet> > upCallback)
{
  m_ManagmentCallback = upCallback;
}

 void LiFiMacLow::SetMac(Ptr<LiFiRxMac> regularMac)
 {
   m_regularMac = regularMac;
 }

Ptr<LiFiRxMac> LiFiMacLow::GetMac()
 {
  return m_regularMac;
 }
 
Ptr<LiFiChannel>
LiFiMacLow::GetChannel (void) const
{
  return m_channel;
}

void
LiFiMacLow::SetChannel (Ptr<LiFiChannel> channel)
{
  NS_LOG_FUNCTION (channel);
  m_channel = channel;
  //m_channel->Add (this);
}

void LiFiMacLow::SetPhy (Ptr<LiFiPhy> phy)
{
    m_phy = phy;
}

Ptr<LiFiPhy> LiFiMacLow::GetPhy ()
{
  return m_phy;
}

Ptr<LiFiNetDevice>
LiFiMacLow::GetDevice (void) const
{
  return m_device;
}

void
LiFiMacLow::SetDevice (Ptr<ns3::LiFiNetDevice> device)
{
  NS_LOG_FUNCTION (device);
  m_device = device;
}

Ssid
LiFiMacLow::GetSsid (void) const
{
  return m_ssid;
}

void
LiFiMacLow::SetSsid (Ssid ssid)
{
  NS_LOG_FUNCTION (ssid);
  m_ssid = ssid;
}

void
LiFiMacLow::StartTransmission (Ptr<const Packet> packet,
                               const WifiMacHeader* hdr)
{
  NS_LOG_FUNCTION (this << packet << hdr);
  if (packet == 0)
  {
   // std::cout<<"LiFiMacLow::StartTransmission() packet "<<packet<<std::endl;
    return;
  }
  
  m_currentPacket = packet->Copy ();
  m_currentHdr = *hdr;

  NS_LOG_DEBUG ("startTx size=" << GetSize (m_currentPacket, &m_currentHdr) <<
                ", to=" << m_currentHdr.GetAddr1 ());
  m_currentPacket->AddHeader (m_currentHdr);
  WifiMacTrailer fcs;
  m_currentPacket->AddTrailer (fcs);
  ForwardDown (m_currentPacket, &m_currentHdr);
  m_currentPacket = 0;
}

void
LiFiMacLow::Receive (Ptr<Packet> packet)
{
  NS_LOG_DEBUG (*packet);
  /*WifiMacHeader hdr;
  packet->PeekHeader (hdr);
  packet->RemoveHeader (hdr);
  NS_LOG_DEBUG ("rx group from= " << hdr.GetAddr2 ());*/
  WifiMacTrailer fcs;
  packet->RemoveTrailer (fcs);
  m_rxCallback (packet);
}

uint32_t
LiFiMacLow::GetSize (Ptr<const Packet> packet, const WifiMacHeader *hdr) const
{
  WifiMacTrailer fcs;
  return packet->GetSize () + hdr->GetSize () + fcs.GetSerializedSize ();
}

void
LiFiMacLow::ForwardDown (Ptr<const Packet> packet, const WifiMacHeader* hdr)
{
  NS_LOG_DEBUG ("send " << hdr->GetTypeString () <<
                ", to=" << hdr->GetAddr1 () <<
                ", size=" << packet->GetSize ());
  m_phy->Send (packet, this->GetDevice());
}

} // namespace ns3

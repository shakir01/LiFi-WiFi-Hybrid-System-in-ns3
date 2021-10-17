#ifndef LIFI_MAC_LOW_H
#define LIFI_MAC_LOW_H

#include <vector>
#include <stdint.h>
#include <ostream>
#include <map>
#include "ns3/wifi-mac-header.h"
#include "lifi-channel.h"
#include "ns3/data-rate.h"
#include "ns3/mac48-address.h"
#include "ns3/callback.h"
#include "ns3/event-id.h"
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/qos-utils.h"
#include "ns3/node.h"
#include "ns3/lifi-phy.h"
#include "ns3/data-rate.h"
#include "ns3/nstime.h"
#include "ns3/ssid.h"

namespace ns3 {


class LiFiChannel;
class LiFiNetDevice;
class LiFiPhy;
class LiFiRxMac;
class LiFiMacLow : public Object
{
public:
  typedef Callback<void, Ptr<Packet>> LiFiMacLowRxCallback;

  LiFiMacLow ();
  virtual ~LiFiMacLow ();
  void SetDevice (Ptr<ns3::LiFiNetDevice> device);
  void SetAddress (Mac48Address ad);
  void SetBssid (Mac48Address ad);
  Mac48Address GetAddress (void) const;
  Mac48Address GetBssid (void) const;
  Ptr<LiFiNetDevice> GetDevice (void) const;

  Ssid GetSsid (void) const;
  void SetChannel (Ptr<LiFiChannel> channel);
  void SetForwardUpManagmentCallback (Callback<void,Ptr<Packet> > upCallback);
  void StartTransmission (Ptr<const Packet> packet,const WifiMacHeader* hdr);
  void SetRxCallback (Callback<void,Ptr<Packet>> callback);
  void Receive (Ptr<Packet> packet);
  void SetPhy (Ptr<LiFiPhy> phy);
  Ptr<LiFiPhy> GetPhy ();
  void SetSsid (Ssid ssid);
  void SetMac(Ptr<LiFiRxMac>);
  Ptr<LiFiRxMac> GetMac();

private:
  uint32_t GetSize (Ptr<const Packet> packet, const WifiMacHeader *hdr) const;
  void ForwardDown (Ptr<const Packet> packet, const WifiMacHeader *hdr);
  virtual Ptr<LiFiChannel> GetChannel (void) const;
  virtual void DoDispose (void);
  LiFiMacLowRxCallback m_rxCallback;
  Ptr<Packet> m_currentPacket;
  Callback<void, ns3::Ptr<Packet> > m_ManagmentCallback;

  Ptr<LiFiChannel> m_channel;
  Ptr<ns3::LiFiNetDevice> m_device;
  WifiMacHeader m_currentHdr;
  Mac48Address m_self;
  Mac48Address m_bssid;
  Ptr<LiFiPhy> m_phy;
  Ssid m_ssid;
  ns3::Ptr<LiFiRxMac> m_regularMac;

};

} // namespace ns3

#endif /* TDMA_MAC_LOW_H */

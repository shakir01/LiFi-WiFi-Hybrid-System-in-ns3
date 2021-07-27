#ifndef WIFI_AP_NET_DEVICE_H
#define WIFI_AP_NET_DEVICE_H

#include "ns3/wifi-net-device.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/mac48-address.h"
#include <string>
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
namespace ns3  {
class WifiNetDevice;
/**
 * \defgroup wifi Wifi Models
 *
 * This section documents the API of the ns-3 Wifi module. For a generic functional description, please refer to the ns-3 manual.
 */

/**
 * \brief Hold together all Wifi-related objects.
 * \ingroup wifi
 *
 * This class holds together ns3::WifiChannel, ns3::WifiPhy,
 * ns3::WifiMac, and, ns3::WifiRemoteStationManager.
 */
class WifiApNetDevice : public WifiNetDevice
{
public:
  static TypeId GetTypeId (void);

  WifiApNetDevice ();
  //virtual ~WifiApNetDevice ();
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  Ipv4Header* PrintPacket (Ptr<Packet> packet);
  void CreateIpv4Header (Ptr<Packet> packet, Ipv4Header& header, Ipv4Address source, Ipv4Address destination);
  virtual void AddLinkChangeCallback (Callback<void> callback);
  //  Ptr<Ipv4Route> LookupStatic (Ipv4Address dest, Ptr<NetDevice> oif);
  virtual bool NeedsArp (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
  void SetGatewayMacAddress(Address address);
  Address GetGatewayMacAddress( );

  void SetPeerNetDevice(Ptr<ns3::NetDevice> netdevice);
  Ptr<ns3::NetDevice> GetPeerNetDevice ();
  void ForwardDown (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);

protected:
  virtual void DoDispose (void);
  virtual void DoInitialize (void);
private:
  void CompleteConfiguration (); 
  Address m_GatewayAddress;
  Ptr<ns3::NetDevice> m_peerNetDevice;
};

} //namespace ns3

#endif /* WIFI_NET_DEVICE_H */

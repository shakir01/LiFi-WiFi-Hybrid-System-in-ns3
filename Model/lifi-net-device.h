
#ifndef LIFI_NET_DEVICE_H
#define LIFI_NET_DEVICE_H

#include "ns3/lifi-mobility-model.h"
#include "ns3/pointer.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/object.h"
#include "ns3/type-id.h"
#include <cstring>
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/simple-net-device.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/ptr.h"
#include "ns3/mac48-address.h"
#include "ns3/error-model.h"
#include "ns3/queue.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/tdma-mac.h"
#include "ns3/lifi-channel.h"
#include "tdma-controller.h"
#include "ns3/llc-snap-header.h"
#include "ns3/wifi-mac-trailer.h"
#include "ns3/lifi-regular-mac.h"
namespace ns3 {

class LiFiChannel;
class TdmaController;
class LiFiRegularMac;
class LiFiNetDevice : public SimpleNetDevice
{
public:
  static TypeId GetTypeId (void);

  LiFiNetDevice ();
  virtual ~LiFiNetDevice ();
  double GetAzmuth();

  void SetAzmuth(double az);

  ns3::Vector GetPosition();

  void SetPosition(ns3::Vector position);

  double GetElevation();

  void SetElevation(double elevation);

   ns3::Ptr<LiFiMobilityModel> GetMobilityModel();

   void SetMobilityModel(ns3::Ptr<LiFiMobilityModel> model);

   void SetDeviceName (std::string name);

  void SetMac (Ptr<TdmaMac> mac);
  	
  Ptr<TdmaMac> GetMac (void) const;
 
  void SetChannel (Ptr<LiFiChannel> channel);
  void SetTdmaController (Ptr<TdmaController> controller);
  Ptr<TdmaController>  GetTdmaController (void) const;
  virtual Ptr<LiFiChannel> DoGetChannel (void) const;
  virtual Ptr<Channel> GetChannel (void) const;
  virtual void SetIfIndex (const uint32_t index);
  virtual uint32_t GetIfIndex (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual bool IsLinkUp (void) const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool IsBridge (void) const;
  virtual bool SetMtu (const uint16_t mtu);
  virtual uint16_t GetMtu (void) const;
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);
//virtual void Receive(ns3::Ptr<ns3::Packet>);
  virtual bool NeedsArp (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual Address GetMulticast (Ipv6Address addr) const;
  virtual bool IsMulticast (void) const;
  void SetMacHigh (Ptr<LiFiRegularMac> regularMac);
  Ptr<TdmaController>  GetMacHigh (void) const;
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual bool SupportsSendFrom (void) const;
  virtual void Receive (Ptr<Packet> packet, uint16_t protocol, Mac48Address to, Mac48Address from);
  virtual void SetReceiveCallback (SimpleNetDevice::ReceiveCallback cb);
  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
  virtual void SetQueueStateChangeCallback (Callback<void,uint32_t> callback);
  virtual uint32_t GetQueueState (uint32_t index);
  virtual uint32_t GetNQueues (void);
  virtual void ForwardUp (Ptr<Packet> packet);
  virtual void DoDispose (void);
  virtual void DoInitialize (void);
  virtual void CompleteConfig (void);
  virtual void LinkUp (void);
  virtual void LinkDown (void);
  void SetQoSCategory (uint8_t);
	uint8_t GetQoSCategory (void);
  Ptr<LiFiUserDeviceInfos> SearchForReceiverModel (Address address);
  void RemoveRxDevice (uint32_t index);
  void AddReceiverModel (Ptr<LiFiUserDeviceInfos> recieverModel);
  void SetTrafficType(uint8_t);
  uint8_t GetTrafficType();

private:
   bool TxQueueStart (uint32_t index);
   bool TxQueueStop (uint32_t index);
         std::vector<Ptr<LiFiUserDeviceInfos> > m_ReceiverModelList;

 // void Setup (void);
  //Ptr<LiFiChannel> DoGetChannel (void) const;
  uint32_t m_ifIndex;
  Ptr<Node> m_node;
  Ptr<TdmaController> m_tdmaController;
  SimpleNetDevice::ReceiveCallback m_forwardUp;
  SimpleNetDevice::PromiscReceiveCallback m_promiscRx;

  TracedCallback<Ptr<const Packet>, Mac48Address> m_rxLogger;
  TracedCallback<Ptr<const Packet>, Mac48Address> m_txLogger;
  TracedCallback<uint32_t> m_queueStateChanges;

  Ptr<TdmaMac> m_mac;

  Ptr<LiFiChannel> m_channel;
  bool m_linkUp;
  bool m_configComplete;
  ns3::Vector m_position;


  TracedCallback<> m_linkChanges;
  mutable uint16_t m_mtu;

  ns3::Ptr<LiFiMobilityModel> m_mobilityModel;
  std::string deviceName;
  uint8_t m_QoS_category;
  uint8_t m_trafficType;


};

} // namespace ns3

#endif /* TDMA_NET_DEVICE_H */

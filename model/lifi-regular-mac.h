#ifndef LIFI_REGULAR_MAC_H
#define LIFI_REGULAR_MAC_H

#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/data-rate.h"
#include "ns3/timer.h"
#include <vector>
#include <map>
#include "ns3/lifi-channel.h"
#include "ns3/packet.h"
#include "lifi-mac-low.h"
#include "lifi-tx-mac.h"
#include "lifi-rx-mac.h"

namespace ns3 {
class LiFiMacLow;
class LiFiTxMac;
class LiFiRxMac;
class LiFiRegularMac : public Object
{
public:
  static TypeId GetTypeId (void);
   LiFiRegularMac ();
  ~LiFiRegularMac ();
  virtual void Start (void);
  virtual void SetChannel (Ptr<LiFiChannel> c);
  virtual Ptr<LiFiChannel> GetChannel (void);
  virtual void Receive (Ptr<Packet> packet,WifiMacHeader *hdr);
  
  void SetNode(Ptr<Node>);
  Ptr<Node> GetNode(void);

 /**
   * \returns the MAC address associated to this MAC layer.
   */
  virtual Mac48Address GetAddress (void) const;
  /**
   * \returns the ssid which this MAC layer is going to try to stay in.
   */
  virtual Ssid GetSsid (void) const;
  /**
   * \param address the current address of this MAC layer.
   */
  virtual void SetAddress (Mac48Address address);
  virtual void SetMacLow(ns3::Ptr<LiFiMacLow>);
  virtual ns3::Ptr<LiFiMacLow> GetMacLow();
  /**
   * \param ssid the current ssid of this MAC layer.
   */
  virtual void SetSsid (Ssid ssid);
  /**
   * \returns the bssid of the network this device belongs to.
   */
  //virtual void SetDevice (Ptr<LiFiNetDevice> device);
  virtual void Enqueue (Ptr<Packet> packet, Mac48Address to, Mac48Address from);
 
  virtual void Enqueue (Ptr<Packet> packet, Mac48Address to);
  virtual bool SupportsSendFrom (void) const;
 
  virtual void SetForwardUpCallback (Callback<void,Ptr<Packet>,uint16_t,Mac48Address, Mac48Address> upCallback);

 
  /**
   * \param queuespace the callback to invoke network protocol to senddown packet
   * when there is space in the mac queue.
   */
  virtual void SetTxQueueStartCallback (Callback<bool,uint32_t> queueStart) ;
  virtual void SetTxQueueStopCallback (Callback<bool,uint32_t> queueStop) ;
  virtual uint32_t GetQueueState (uint32_t index);
  virtual uint32_t GetNQueues (void);
  virtual void StartTransmission (uint64_t transmissionTime,bool isControlFrame);
  virtual void StopTransmission (bool);
  /**
   * Public method used to fire a MacTx trace.  Implemented for encapsulation
   * purposes.
   */
  virtual void NotifyTx (Ptr<const Packet> packet);

  /**
   * Public method used to fire a MacTxDrop trace.  Implemented for encapsulation
   * purposes.
   */
  virtual void NotifyTxDrop (Ptr<const Packet> packet) ;

  /**
   * Public method used to fire a MacRx trace.  Implemented for encapsulation
   * purposes.
   */
  virtual void NotifyRx (Ptr<const Packet> packet);

  /**
   * Public method used to fire a MacPromiscRx trace.  Implemented for encapsulation
   * purposes.
   */
  virtual void NotifyPromiscRx (Ptr<const Packet> packet);
  void TxFailed (const WifiMacHeader &hdr);

  /**
   * Public method used to fire a MacRxDrop trace.  Implemented for encapsulation
   * purposes.
   */
  virtual void NotifyRxDrop (Ptr<const Packet> packet) ;
  void TxOk (const WifiMacHeader &hdr);
  virtual void Initialize ();
  virtual void SetQosSupported (bool enable);
  virtual bool GetQosSupported () const;
 // typedef Callback<void, Ptr<const Packet>, Mac48Address, Mac48Address> ForwardUpCallback;
  //void SetForwardUpCallback (ForwardUpCallback upCallback);
  void SetLinkUpCallback (Callback<void> linkUp);
  void SetLinkDownCallback (Callback<void> linkDown);
  Time CalculateTxTime (Ptr<const Packet> packet);

  ns3::Ptr<LiFiTxMac> GetLiFiTxMac();  
protected:
Ptr<LiFiTxMac> m_txMac;  //!< RX middle (defragmentation etc.)
Ptr<LiFiRxMac> m_rxMac; //!< TX middle (aggregation etc.)
  Ptr<LiFiMacLow> m_low;            //!< MacLow (RTS, CTS, Data, Ack etc.)
  virtual void DoInitialize ();
  virtual void DoDispose ();
  


  Callback<void,Ptr<Packet>,uint16_t,Mac48Address, Mac48Address>  m_forwardUp; //!< Callback to forward packet up the stack
  Callback<void> m_linkUp;       //!< Callback when a link is up
  Callback<void> m_linkDown;     //!< Callback when a link is down

  Ssid m_ssid; //!< Service Set ID (SSID)

  /** This holds a pointer to the TXOP instance for this WifiMac - used
  for transmission of frames to non-QoS peers. */

  private:
  Ptr<Node> m_node;
   // Ptr<ns3::LiFiNetDevice> m_device;
    bool m_qosSupported;
    Mac48Address m_address;
};

}
#endif

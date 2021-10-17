#ifndef LIFI_TX_MAC_H
#define LIFI_TX_MAC_H

#include "ns3/data-rate.h"
#include "ns3/nstime.h"
#include "lifi-mac-queue.h"
//#include "ns3/lifi-ap-mac.h"


//#include "receiver-model.h"
namespace ns3 
{
class WifiMacHeader;
//class TdmaController;
class LiFiMacLow;
//class LiFiApMac;
class LiFiRegularMac;
class LiFiTxMac : public Object
{
public:
  static TypeId GetTypeId (void);
  LiFiTxMac ();
  ~LiFiTxMac ();

  virtual void SetLinkDownCallback (Callback<void> linkDown);
  virtual void SetControllingMac (Ptr<LiFiRegularMac> controller);
  virtual Ptr<LiFiRegularMac> GetControllingMac (void) const;

  virtual void Initialize (void);
  virtual void StartTransmission (uint64_t transmissionTime);
  virtual void StopTransmission (bool);

  virtual void NotifyTx (Ptr<const Packet> packet);
  virtual void NotifyTxDrop (Ptr<const Packet> packet);
  
  virtual void SetTxQueueStartCallback (Callback<bool,uint32_t> queueStart);
  virtual void SetTxQueueStopCallback (Callback<bool,uint32_t> queueStop);
  virtual uint32_t GetQueueState (uint32_t index);
  virtual uint32_t GetNQueues (void);

  void Queue (Ptr<const Packet> packet, const WifiMacHeader &hdr);
  void SetQueueSize (uint32_t size);
  void SetMaximumDelay (Time delay);

  virtual void SetMacLow(Ptr<LiFiMacLow>);
  virtual Ptr<LiFiMacLow> GetMacLow();
  Time CalculateTxTime (Ptr<const Packet> packet);
  void SetDataRate (DataRate bps);
  DataRate GetDataRate (void) const;

  Ptr<LiFiMacQueue> GetMacQueue();
  virtual void DoDispose (void);

  void TxQueueStart (uint32_t index);
  void TxQueueStop (uint32_t index);
  void SendPacketDown (Time remainingTime);

  void TxOk (const WifiMacHeader &hdr);
  void TxFailed (const WifiMacHeader &hdr);
  //void SetTxOkCallback(Callback<bool,uint32_t> queueStop);
private:
  //LiFiTxMac (const LiFiTxMac & ctor_arg);
  //LiFiTxMac &operator = (const LiFiTxMac &o);
 
  TracedCallback<Ptr<const Packet> > m_macTxTrace;
  TracedCallback<Ptr<const Packet> > m_macTxDropTrace;

  Callback<bool,uint32_t> m_queueStart;
  Callback<bool,uint32_t> m_queueStop;
  
  Ptr<LiFiMacQueue> m_queue;
  Ptr<LiFiMacQueue>  m_control_queue;

  Ptr<LiFiMacLow> m_low;

  Ptr<LiFiRegularMac> m_controlMac;

  DataRate m_bps;

  bool m_isTdmaRunning;
};
} // namespace ns3
#endif /* TDMA_CENTRAL_MAC_H */
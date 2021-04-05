#ifndef LIFI_REGULAR_MAC_H
#define LIFI_REGULAR_MAC_H

#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/data-rate.h"
#include "ns3/timer.h"
#include <vector>
#include <map>
#include "ns3/lifi-channel.h"

namespace ns3 {
class LiFiRegularMac : public Object
{
public:
  static TypeId GetTypeId (void);
   LiFiRegularMac ();
  ~LiFiRegularMac ();
  virtual void Start (void);
  virtual void SetChannel (Ptr<LiFiChannel> c);
  virtual Ptr<LiFiChannel> GetChannel (void);
  virtual Time CalculateTxTime (Ptr<const Packet> packet);
  virtual void Receive (Ptr<Packet> packet, double snrLevel) = 0;



};

}
#endif

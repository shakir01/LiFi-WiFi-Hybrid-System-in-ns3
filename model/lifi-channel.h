#ifndef LIFI_CHANNEL_H
#define LIFI_CHANNEL_H

#include "ns3/simple-channel.h"
#include "ns3/mac48-address.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "lifi-mac-low.h"
#include "ns3/lifi-error-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include <unordered_map>
#include "ns3/lifi-snr.h"
#include <vector>
#include "ns3/mobility-model.h"
#include <algorithm>
#include "ns3/lifi-propagation-loss-model.h"
#include "ns3/commons.h"
#include "ns3/lifi-phy.h"

namespace ns3 {

class LiFiMacLow;
class Packet;
class ReceivedDataModel;
class LiFiRxNetDevice;
class LiFiTxNetDevice;
class LiFiNetDevice;

class LiFiErrorModel;
struct LiFiUserData
{
  double 		  ap_NodeId;
  double 		  userDeviceNodeId;
  double      ap_x;
  double      ap_y;
  double      ap_z;
  double      ud_x;
  double      ud_y;
  double      ud_z;
  double 		  polarAngle;
  double 		  rxGain;
  double 		  losChannelGain;
  double 		  receivedSNR;
  double 		  ber;
};

class LiFiPhy;
 
//class LifiPropagationLossModel;
/**
 * \ingroup channel
 * \brief A simple channel, for simple things and testing
 */
class LiFiChannel : public SimpleChannel
{
public:
  typedef std::vector<Ptr<LiFiNetDevice> > LiFiRxNetDevicesList;
  static TypeId GetTypeId (void);
  LiFiChannel ();

  /**
   * Copy the packet to be received at a time equal to the transmission
   * time plus the propagation delay between sender and all receivers
   * on the channel that are within the range of the sender
   *
   * \param p Pointer to packet
   * \param sender sending NetDevice
   * \param txTime transmission time (seconds)
   */
   //void Send (Ptr<const Packet> p, Ptr<NetDevice> sender);
   void Send (Ptr<const Packet> p, Ptr<LiFiPhy> sender);

  /**
   * Add a device to the channel
   *
   * \param device Device to add
   */
  void Add (Ptr<LiFiNetDevice> rxNetDevice);
  double GetMaxRange (void) const;

    void AddPhy (Ptr<LiFiPhy> phy);


  // inherited from ns3::Channel
  virtual std::size_t GetNDevices (void) const;
  virtual Ptr<NetDevice> GetDevice (uint32_t i) const;
  Ptr<NetDevice> FindDeviceById (uint32_t i) ;
  Ptr<NetDevice> GetTxDevice () ;

  Ptr<LiFiNetDevice> GetLiFiRxNetDevice (uint32_t i) const;
  void SetTxDevice (Ptr<LiFiTxNetDevice> txNetDevice);

   void AddPropagationLossModel (ns3::Ptr<LiFiPropagationLossModel>, uint32_t);
   void AddLiFiSNRModel (ns3::Ptr<LiFiSnr>, uint32_t);
   void AddLiFiErrorModel (ns3::Ptr<LiFiErrorModel>, uint32_t);
  std::map<std::string, std::vector<LiFiUserData >  > GetUserData ( );
  double GetDistance(ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX, Ptr<LiFiNetDevice> rxd);
  typedef void (* MonitorRxDataCallback)(Ptr<LiFiChannel> channel, uint32_t apId,std::unordered_map <uint32_t, std::vector <ChannelStats > >  maps);

  void RemoveUserDevice (uint32_t index);
   
private:
  ns3::Ptr < ns3::LiFiErrorModel >  ErrorForRxNetDevice (ns3::Ptr<LiFiRxNetDevice> rxNetDevice);
  double GetDistance(ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX, Ptr<LiFiRxNetDevice> rx);
  void SaveSNRBERLoss();

  LiFiRxNetDevicesList m_LiFiRxNetDevicesList;
  double m_range;
  Ptr<LiFiTxNetDevice> m_lifiTxNetDevice;
  std::vector<Ptr<LiFiPhy> > m_PhyVector;

  ns3::Ptr<PropagationLossModel> m_loss;
  ns3::Ptr<PropagationLossModel> m_loss_current;
  double m_TxRxDistance;

  std::unordered_map <uint32_t, ns3::Ptr<ns3::LiFiPropagationLossModel> > m_propagationLossMap;
  std::unordered_map <uint32_t, ns3::Ptr<ns3::LiFiSnr> > m_LiFiSnrMap;
  std::unordered_map <uint32_t, ns3::Ptr<ns3::LiFiErrorModel> > m_LiFiErrorModelMap;
  std::vector<std::vector<double> > m_statsVector;
  TracedCallback<Ptr<LiFiChannel>, uint32_t,std::unordered_map <uint32_t, std::vector <ChannelStats > >  > m_monitorRxDataCallback;
  std::unordered_map <uint32_t,std::vector <ChannelStats > > channelStatsMap;

  bool startStatsCollection;
};

} // namespace ns3

#endif /* LIFI_CHANNEL_H */

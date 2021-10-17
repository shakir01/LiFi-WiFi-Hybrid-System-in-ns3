#ifndef LIFI_AP_MAC_H
#define LIFI_AP_MAC_H

#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/data-rate.h"
#include "ns3/packet.h"
#include "ns3/lifi-channel.h"
#include "ns3/timer.h"
#include <vector>
#include <map>
#include "lifi-user-device-infos.h"
#include <unordered_map> 
#include <map>
#include "ns3/lifi-regular-mac.h"
#include "ns3/remote-station-manager.h"
#include "ns3/commons.h"
#include "ns3/my-tag.h"
#include "ns3/beacon-response-tag.h"
#include "ns3/mac-resources-tag.h"
#include "ns3/mgt-headers.h"

namespace ns3 {
class LiFiChannel;
class LiFiUserDeviceInfos;
class LiFiCapabilityInformation;
class LiFiMgtBeaconHeader;
class LiFiMgtProbeResponseHeader;
class LiFiMgtProbeRequestHeader;
class RemoteStationManager;
class MacResourcesTag;
class MgtBeaconHeader;
//class BeaconResponseTag;
class MyTag;
struct MacTransmissionUnit 
{
  uint32_t slot_id;
  uint32_t node_id;
  Mac48Address destinationAddress;
  std::string transmissionType;//uplink or downlink
  uint8_t access_category;
};

struct BeaconRecords 
{
  uint32_t node_id;
  uint32_t loss_counter;
  ns3::Ptr<LiFiUserDeviceInfos> lifiUserDeviceInfos;
};

class LiFiApMac : public LiFiRegularMac
{
public:
  static TypeId GetTypeId (void);
  LiFiApMac ();
  ~LiFiApMac ();
  typedef std::map<uint32_t,Ptr<LiFiUserDeviceInfos> > TdmaMacPtrMap;

  
    void SetAccessCategory(uint8_t);
  /**
   * \param slotTime the duration of a slot.
   *
   * It is a bad idea to call this method after RequestAccess or
   * one of the Notify methods has been invoked.
   */
  void SetSlotTime (Time slotTime);
  /**
   */
  void SetGuardTime (Time guardTime);
  /**
   */
  void SetDataRate (DataRate bps);
  /**
   */
  void AddTdmaSlot (uint32_t slot, Ptr<LiFiUserDeviceInfos> recModel);

  void AddTdmaSlots (uint32_t slotPos, Ptr<LiFiUserDeviceInfos> macPtr);

  /**
   */
  void SetInterFrameTimeInterval (Time interFrameTime);
  /**
   */
  void SetTotalSlotsAllowed (uint32_t slotsAllowed);
  /**
   */
  Time GetSlotTime (void) const;
  /**
   */
  Time GetGuardTime (void) const;
  /**
   */
  DataRate GetDataRate (void) const;
  /**
   */
  Time GetInterFrameTimeInterval (void) const;
  /**
   */
  uint32_t GetTotalSlotsAllowed (void) const;
  /**
   * \param duration expected duration of reception
   *
   * Notify the DCF that a packet reception started
   * for the expected duration.
   */
  void NotifyRxStartNow (Time duration);
  /**
   * Notify the DCF that a packet reception was just
   * completed successfully.
   */
  void NotifyRxEndOkNow (void);
  /**
   * Notify the DCF that a packet reception was just
   * completed unsuccessfully.
   */
  void NotifyRxEndErrorNow (void);
  /**
   * \param duration expected duration of transmission
   *
   * Notify the DCF that a packet transmission was
   * just started and is expected to last for the specified
   * duration.
   */
  void SetChannel (Ptr<LiFiChannel> c);

  void SetCCMacAddress (Address);

  void NotifyTxStartNow (Time duration);
  Time CalculateTxTime (Ptr<const Packet> packet);
  void StartFrameTransmission (void);
 // virtual void Start (void);
  void Start (void);
  void MarkNodeForHandoverNodes(Ptr<Node> node);
  void UpdateUserRegistration();
  void ReAssignSlots();
  void TransmitDataFrame (MacTransmissionUnit userDevice);
  uint8_t GetNumberOfSlotsForAccessCategory (uint8_t ac);
  Ptr<LiFiChannel> GetChannel (void);
  typedef void (* CompleteHandoverllback)(Ptr<LiFiApMac>, std::vector <Ptr<Node> > );
  void AddUserDeviceInfos(Ptr<LiFiUserDeviceInfos > userDeviceInfos);
  //void SetMac (Ptr<TdmaMac> mac);
  void StartSuperSessions (void);
  void ReigsterUserDevice(Ptr<Packet> packet, WifiMacHeader machHeader);
/**
   * \return the interval between two beacon transmissions.
   */
  Time GetBeaconInterval (void) const;
  void SetBeaconInterval (Time interval); 
  void SetBeaconGeneration (bool enable);
  void SendOneBeacon (void);
  void TransmitControlFrames(uint32_t index);
  void SendControlPackets ();
  virtual void Receive (Ptr<Packet> packet,WifiMacHeader *hdr);
  //virtual void Receive (Ptr<Packet> packet, double snrLevel);

  void SetRemoteStationManager (ns3::Ptr<RemoteStationManager> remoteStationManager);
  void  DistributeSlots();
  virtual void SetMacLow(ns3::Ptr<LiFiMacLow>);
  virtual ns3::Ptr<LiFiMacLow> GetMacLow();
  Address GetCCMacAddress (void);

  void SetControlGuardSlotTime (uint32_t time);
  Time GetControlGuardSlotTime ();
  void ResetDataStructures();
  void SendRecordToCentralizedControllers (uint32_t index);
  Ptr<PointToPointNetDevice> GetPointerToP2PNetDevice ();
  
  ///from central mac as modification classes
  
  virtual void Enqueue (Ptr<Packet> packet, Mac48Address to, Mac48Address from);
  virtual void Enqueue (Ptr<Packet> packet, Mac48Address to);
  virtual bool SupportsSendFrom (void) const;
  void UpdateUserRegistration ( ns3::Ptr<LiFiUserDeviceInfos> lfUserDeviceInfos);
  
  void ForwardDown (Ptr<Packet> packet, Mac48Address from, Mac48Address to);
private:
  static Time GetDefaultSlotTime (void);
  static Time GetDefaultGuardTime (void);
  static DataRate GetDefaultDataRate (void);
  void DoRestartAccessTimeoutIfNeeded (void);
  void AccessTimeout (void);
  void DoGrantAccess (void);
  bool IsBusy (void) const;
  void UpdateFrameLength (void);
  bool IsUserAssociated(Mac48Address to);
  uint32_t m_slotTime;
  uint32_t m_guardTime;
  uint32_t m_tdmaFrameLength; //total frameLength in microseconds
  uint32_t m_tdmaInterFrameTime;
  uint32_t m_totalSlotsAllowed;
  uint32_t m_controlGuardTime;
  bool m_activeEpoch;

  TdmaMacPtrMap m_slotPtrs;
  Ptr<LiFiChannel> m_channel;
  ns3::Ptr<LiFiMacLow> m_MacLow;
  //std::vector<Ptr<Node> > markedForHandoverNodesVector;
  std::map<uint32_t, ns3::Ptr<Node> >  markedUeNodesMap;
  std::vector<ns3::Ptr<Node> >  markedUeNodesVector;
  TracedCallback<Ptr<LiFiApMac>, std::vector < Ptr<Node> > > m_CompleteHandoverllback;
  uint16_t m_current_slot;
  uint16_t m_current_device_index;
  std::vector <Ptr<LiFiUserDeviceInfos> > m_slotPtrs_vector;
  std::vector <Ptr<LiFiUserDeviceInfos> > m_lastepoch_slotPtrs_vector;
  std::map<uint32_t, BeaconRecords> m_beaconsRecordsMap;

  std::vector <MacTransmissionUnit > transmissionsVector;


  Time m_beaconInterval;
  Ptr<UniformRandomVariable> m_beaconJitter; //!< UniformRandomVariable used to randomize the time of the first beacon
  bool m_enableBeaconJitter;                 //!< Flag whether the first beacon should be generated at random time
  EventId m_beaconEvent;                     //!< Event to generate one beacon
  bool m_enableBeaconGeneration;             //!< Flag whether beacons are being generated
  uint16_t frame_number;
  ns3::Ptr<RemoteStationManager> m_remoteStationManager;
  std::map<uint32_t, MacResourcesTag>  m_slotInfosMaps;
  Ptr<LiFiTxMac> m_beaconTxop;
  //Ptr<LiFiTxMac> m_txMac;
  DataRate m_bps;
  Address m_cc_address;

};

} // namespace ns3
#endif 

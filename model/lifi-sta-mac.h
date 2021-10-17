#ifndef LIFI_STA_MAC_H
#define LIFI_STA_MAC_H

#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/data-rate.h"
#include "ns3/packet.h"
#include "ns3/lifi-channel.h"
#include "ns3/timer.h"
#include <vector>
#include <map>
#include "lifi-user-device-infos.h"
#include "ns3/lifi-regular-mac.h"
#include "ns3/mac-resources-tag.h"
#include "ns3/lifi-net-device.h"
#include "ns3/mgt-headers.h"

namespace ns3 {
class MacResourcesTag;
class LiFiMacLow;
class LiFiChannel;
class LiFiUserDeviceInfos;
class LiFiNetDevice;
class BeaconResponseTag;

struct LiFiApInfo
{
  Mac48Address m_bssid;               ///< BSSID
  Mac48Address m_apAddr;              ///< AP MAC address
  double m_snr;                       ///< SNR in linear scale
  bool m_activeProbing;               ///< Flag whether active probing is used or not
//  MgtProbeResponseHeader m_probeResp; ///< Probe Response header
  MacResourcesTag macResourceTag;
};

class LiFiRemoteStationManager: public Object
{
  public:
      Mac48Address m_bssid;               ///< BSSID
      Mac48Address m_apAddr;              ///< AP MAC address
      double m_snr;                       ///< SNR in linear scale
      bool m_activeProbing;               ///< Flag whether active probing is used or not
     // MgtBeaconHeader m_beacon;           ///< Beacon header
      //MgtProbeResponseHeader m_probeResp; ///< Probe Response header
  private:
      std::vector <uint16_t>  dLinkSlotsVector;
      std::vector <uint16_t>  uLinkSlotsVector;
      uint16_t current_slot_number;
};

class LiFiStaMac : public LiFiRegularMac
{
public:
  static TypeId GetTypeId (void);
  LiFiStaMac ();
  ~LiFiStaMac ();
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
  void NotifyTxStartNow (Time duration);
  Time CalculateTxTime (Ptr<const Packet> packet);
  void StartTdmaSessions (void);
  void SetChannel (Ptr<LiFiChannel> c);
  virtual void Start (void);
  void SendAssociationRequest ();
  void UpdateCandidateApList (LiFiApInfo newApInfo);

  void SetDevice (ns3::Ptr<LiFiNetDevice> netDevice);
  ns3::Ptr<LiFiNetDevice> GetDevice ();
  void AddUserDeviceInfos(Ptr<LiFiUserDeviceInfos > );
  std::vector <Ptr<LiFiUserDeviceInfos > > GetUserDeviceInfos(void);

  uint32_t GetNumberOfSlotsForAccessCategory (uint16_t ac);
  //void TransmitFrame (Ptr<LiFiUserDeviceInfos> userDevice);
  void TransmitFrame ();

  void CalculateTotalSlots ();

  //virtual void Receive (Ptr<Packet> packet, double snrLevel);
  virtual void Receive (Ptr<Packet> packet,WifiMacHeader *hdr);

  void SendBeaconResponse();
  void UpdateApInfos();
  void UpdateStationSlotInfoFromAssocResp (MacResourcesTag assocResp, Mac48Address apAddr);

  ///from central mac as modification classes
  
  virtual void Enqueue (Ptr<Packet> packet, Mac48Address to, Mac48Address from);
  virtual void Enqueue (Ptr<Packet> packet, Mac48Address to);
  virtual bool SupportsSendFrom (void) const;
  
  void ForwardDown (Ptr<Packet> packet, Mac48Address from, Mac48Address to);
  virtual void SetMacLow(ns3::Ptr<LiFiMacLow>);
  virtual ns3::Ptr<LiFiMacLow> GetMacLow();

private:
  static Time GetDefaultSlotTime (void);
  static Time GetDefaultGuardTime (void);
  static DataRate GetDefaultDataRate (void);
  void DoRestartAccessTimeoutIfNeeded (void);
  void AccessTimeout (void);
  void DoGrantAccess (void);
  bool IsBusy (void) const;
  void UpdateFrameLength (void);
  void ScheduleTdmaSession (const uint32_t slotNum);
  Ptr<LiFiChannel> GetChannel (void) const;
  ns3::Ptr<LiFiMacLow> m_MacLow;
  DataRate m_bps;
  uint32_t m_slotTime;
  uint32_t m_guardTime;
  uint32_t m_tdmaFrameLength; //total frameLength in microseconds
  uint32_t m_tdmaInterFrameTime;
  uint32_t m_totalSlotsAllowed;
  bool m_activeEpoch;
  TdmaMacPtrMap m_slotPtrs;
  std::vector <Ptr<LiFiUserDeviceInfos > > m_slotPtrs_vector;
  uint16_t m_current_slot;
  uint16_t m_current_device_index;
  ns3::Ptr<LiFiNetDevice> m_lifiNetDevice;
  uint32_t m_maxMissedBeacons; ///< maximum missed beacons
  Ptr<LiFiChannel> m_channel;
  Time m_waitBeaconTimeout;
  std::vector<LiFiApInfo> m_candidateAps; ///< list of candidate APs to associate to
  Ptr<LiFiTxMac> m_controlTxop;
};

} // namespace ns3

#endif /* AERO_TDMA_CONTROLLER_H */

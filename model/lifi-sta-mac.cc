#include "ns3/assert.h"
#include "ns3/enum.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "lifi-sta-mac.h"
#include "lifi-mac-low.h"
#include "ns3/abort.h"
#include "ns3/beacon-response-tag.h"

NS_LOG_COMPONENT_DEFINE ("LiFiStaMac");

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (LiFiStaMac);
TypeId
LiFiStaMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3:LiFiStaMac")
    .SetParent<Object> ()
    .AddConstructor<LiFiStaMac> ()
    .AddAttribute ("DataRate",
                   "The default data rate for point to point links",
                   DataRateValue (GetDefaultDataRate ()),
                   MakeDataRateAccessor (&LiFiStaMac::SetDataRate,
                                         &LiFiStaMac::GetDataRate),
                   MakeDataRateChecker ())
    .AddAttribute ("SlotTime", "The duration of a Slot in microseconds.",
                   TimeValue (GetDefaultSlotTime ()),
                   MakeTimeAccessor (&LiFiStaMac::SetSlotTime,
                                     &LiFiStaMac::GetSlotTime),
                   MakeTimeChecker ())
    .AddAttribute ("GuardTime", "GuardTime between TDMA slots in microseconds.",
                   TimeValue (GetDefaultGuardTime ()),
                   MakeTimeAccessor (&LiFiStaMac::SetGuardTime,
                                     &LiFiStaMac::GetGuardTime),
                   MakeTimeChecker ())
    .AddAttribute ("InterFrameTime", "The wait time between consecutive tdma frames.",
                   TimeValue (MicroSeconds (3)),
                   MakeTimeAccessor (&LiFiStaMac::SetInterFrameTimeInterval,
                                     &LiFiStaMac::GetInterFrameTimeInterval),
                   MakeTimeChecker ())
    .AddAttribute ("MaxMissedBeacons",
                   "Number of beacons which much be consecutively missed before "
                   "we attempt to restart association.",
                   UintegerValue (10),
                   MakeUintegerAccessor (&LiFiStaMac::m_maxMissedBeacons),
                   MakeUintegerChecker<uint32_t> ());
  return tid;
}

LiFiStaMac::LiFiStaMac ()
  : m_slotTime (15),
		m_guardTime (6),
		m_tdmaFrameLength (1500),
		m_tdmaInterFrameTime (15),
		m_totalSlotsAllowed (10000),
		m_activeEpoch (false),
    m_channel (0)
{
  NS_LOG_FUNCTION (this);
  LiFiRegularMac();
  m_current_slot = 0;
  m_current_device_index = 0;
  m_controlTxop = CreateObject<LiFiTxMac> ();;
  m_controlTxop->SetMacLow (m_low);
}

LiFiStaMac::~LiFiStaMac ()
{
  m_channel = 0;
  m_bps = 0;
  m_slotPtrs.clear ();
}

void
LiFiStaMac::Start (void)
{
  NS_LOG_FUNCTION (this);

  if (!m_activeEpoch)
    {
      m_activeEpoch = true;
     // Simulator::Schedule (NanoSeconds (20),&LiFiStaMac::StartTdmaSessions, this);
    }
}

Time
LiFiStaMac::GetDefaultSlotTime (void)
{
  return MicroSeconds (20);
}

Time
LiFiStaMac::GetDefaultGuardTime (void)
{
  return MicroSeconds (7);
}

DataRate
LiFiStaMac::GetDefaultDataRate (void)
{
  NS_LOG_DEBUG ("Setting default");
  return DataRate ("800000000b/s");
}

void
LiFiStaMac::StartTdmaSessions (void)
{
  NS_LOG_FUNCTION_NOARGS ();
 // ScheduleTdmaSession (0);
 CalculateTotalSlots();
 TransmitFrame ();
}

void
LiFiStaMac::AddTdmaSlot (uint32_t slotPos, Ptr<LiFiUserDeviceInfos> macPtr)
{
  NS_LOG_FUNCTION (slotPos << macPtr);
  std::pair<std::map<uint32_t, Ptr<LiFiUserDeviceInfos> >::iterator, bool> result =
  m_slotPtrs.insert (std::make_pair (slotPos,macPtr));
  if (result.second == true)
    {
      NS_LOG_DEBUG ("Added mac : " << macPtr << " in slot " << slotPos);
    }
  else
    {
      NS_LOG_WARN ("Could not add mac: " << macPtr << " to slot " << slotPos);
    }
}

void LiFiStaMac::SetDevice (ns3::Ptr<LiFiNetDevice> netDevice)
{
  m_lifiNetDevice = netDevice;
}

ns3::Ptr<LiFiNetDevice> LiFiStaMac::GetDevice ()
{
  return m_lifiNetDevice;
}

bool LiFiStaMac::SupportsSendFrom (void) const
{
  return false;
}

void
LiFiStaMac::SetSlotTime (Time slotTime)
{
  NS_LOG_FUNCTION (this << slotTime);
  m_slotTime = slotTime.GetMicroSeconds ();
}

Time
LiFiStaMac::GetSlotTime (void) const
{
  return MicroSeconds (m_slotTime);
}

void
LiFiStaMac::SetDataRate (DataRate bps)
{
  NS_LOG_FUNCTION (this << bps);
  m_bps = bps;
}

DataRate
LiFiStaMac::GetDataRate (void) const
{
  return m_bps;
}

void
LiFiStaMac::SetChannel (Ptr<LiFiChannel> c)
{
  NS_LOG_FUNCTION (this << c);
  m_channel = c;
}


Ptr<LiFiChannel>
LiFiStaMac::GetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  return m_channel;
}

void
LiFiStaMac::SetGuardTime (Time guardTime)
{
  NS_LOG_FUNCTION (this << guardTime);
  //guardTime is based on the SimpleWirelessChannel's max range
  if (m_channel != 0)
    {
      //m_guardTime = Seconds (m_channel->GetMaxRange () / 300000000.0).GetMicroSeconds ();
      m_guardTime = guardTime.GetMicroSeconds();
    }
  else
    {
      m_guardTime = guardTime.GetMicroSeconds ();
    }

}

Time
LiFiStaMac::GetGuardTime (void) const
{
  return MicroSeconds (m_guardTime);
}

void
LiFiStaMac::SetInterFrameTimeInterval (Time interFrameTime)
{
  NS_LOG_FUNCTION (interFrameTime);
  m_tdmaInterFrameTime = interFrameTime.GetMicroSeconds ();
}

Time
LiFiStaMac::GetInterFrameTimeInterval (void) const
{
  return MicroSeconds (m_tdmaInterFrameTime);
}

void
LiFiStaMac::SetTotalSlotsAllowed (uint32_t slotsAllowed)
{
  m_totalSlotsAllowed = slotsAllowed;
  m_slotPtrs.clear ();
}

uint32_t
LiFiStaMac::GetTotalSlotsAllowed (void) const
{
  return m_totalSlotsAllowed;
}

void LiFiStaMac::CalculateTotalSlots ()
{
  m_totalSlotsAllowed = 0;
    for (size_t i = 0; i < m_slotPtrs_vector.size(); i++)
    {
        Ptr<LiFiUserDeviceInfos > userdeviceInfos = m_slotPtrs_vector[i];
        //std::cout<<"GetNumberOfSlotsForAccessCategory (userdeviceInfos->GetQoSRequirements() "<<GetNumberOfSlotsForAccessCategory (userdeviceInfos->GetQoSRequirements())<<std::endl;

        m_totalSlotsAllowed = m_totalSlotsAllowed + GetNumberOfSlotsForAccessCategory (userdeviceInfos->GetQoSRequirements());
    }
}

void LiFiStaMac::AddUserDeviceInfos(Ptr<LiFiUserDeviceInfos > userDeviceInfos)
{
    m_slotPtrs_vector.push_back(userDeviceInfos);
}

std::vector <Ptr<LiFiUserDeviceInfos > > LiFiStaMac::GetUserDeviceInfos(void)
{
  return m_slotPtrs_vector;
}
//, double snrLevel
void LiFiStaMac::Receive (Ptr<Packet> packet,WifiMacHeader *hdr)
{
 // std::cout<<"void LiFiStaMac::Receive (Ptr<Packet> packet)  Beacons "<<std::endl;
    NS_LOG_DEBUG (*packet);
   // WifiMacHeader hdr;
    //packet->RemoveHeader (hdr);
    if (hdr->IsBeacon ())
    {
      //NS_LOG_DEBUG ("rx group from=" << hdr.GetAddr2 ());
      //LlcSnapHeader llc;
      //packet->RemoveHeader (llc);
     // std::cout<<"void LiFiStaMac::Receive (Ptr<Packet> packet)  Beacons "<<std::endl;
      LiFiApInfo liFiApInfo;
      liFiApInfo.m_apAddr = hdr->GetAddr2 ();
      liFiApInfo.m_bssid = hdr->GetAddr3 ();
      liFiApInfo.m_activeProbing = false;
      liFiApInfo.m_snr = 0;
      bool found = false;
      for (std::vector<LiFiApInfo>::iterator i = m_candidateAps.begin(); i != m_candidateAps.end(); ++i)
      {
        if (liFiApInfo.m_apAddr == (*i).m_apAddr)
        {
            //m_candidateAps.erase(i);
            found = true;
            break;
        }
      }

      if(found == false)
      {
        m_candidateAps.push_back(liFiApInfo);

      }
       // std::cout<<"void LiFiStaMac::Receive (Ptr<Packet> packet)  Beacons m_candidateAps "<<m_candidateAps.size()<<std::endl;

        SendAssociationRequest();

      //liFiApInfo.m_beacon = beacon;
      //UpdateCandidateApList (liFiApInfo);
    }
    else if (hdr->IsProbeResp ())
    {  
      //UpdateStationSlotInfoFromAssocResp(tagCopy,hdr.GetAddr1 ());
    }
    else if (hdr->IsAssocResp ())
    {
      if(hdr->GetAddr1 () != GetAddress())
      {
          return;
      }
      ns3::MacResourcesTag macResourceTag;
      packet->RemovePacketTag(macResourceTag);
      UpdateStationSlotInfoFromAssocResp (macResourceTag, hdr->GetAddr2 ());     
    }
    else if (hdr->IsData ())
    {
      ns3::LlcSnapHeader llc;
      packet->RemoveHeader (llc);
      m_forwardUp(packet,llc.GetType(),hdr->GetAddr1 (),hdr->GetAddr2 ());
    }
    else {
          //  std::cout<<"void LiFiStaMac::Receive (Ptr<Packet> packet) Not Beacons "<<std::endl;

    }
}

void LiFiStaMac::SendBeaconResponse()
{
    //NS_LOG_DEBUG ("Attempting to associate with BSSID " << bestAp.m_bssid);
    //LiFiApInfo bestAp = m_candidateAps.front();
    // SendAssociationRequest ();
}

void LiFiStaMac::UpdateStationSlotInfoFromAssocResp (MacResourcesTag macResourceTag, Mac48Address apAddr)
{
    LiFiApInfo liFiApInfo = m_candidateAps[m_candidateAps.size()-1];
    liFiApInfo.macResourceTag = macResourceTag;
}

void LiFiStaMac::UpdateCandidateApList (LiFiApInfo newApInfo)
{
 // NS_LOG_FUNCTION (this << newApInfo.m_bssid << newApInfo.m_apAddr << newApInfo.m_snr << newApInfo.m_activeProbing << newApInfo.m_beacon << newApInfo.m_probeResp);
  // Remove duplicate LiFiApInfo entry
  for (std::vector<LiFiApInfo>::iterator i = m_candidateAps.begin(); i != m_candidateAps.end(); ++i)
  {
    if (newApInfo.m_bssid == (*i).m_bssid)
    {
        //m_candidateAps.erase(i);
        break;
    }
  }

  // Insert before the entry with lower SNR
  for (std::vector<LiFiApInfo>::iterator i = m_candidateAps.begin(); i != m_candidateAps.end(); ++i)
  {
    if (newApInfo.m_snr > (*i).m_snr)
      {
        m_candidateAps.insert (i, newApInfo);
        return;
      }
  }
  // If new LiFiApInfo is the lowest, insert at back
  m_candidateAps.push_back(newApInfo);
  //std::cout<<"m_candidateAps m_candidateApsm_candidateAps  m_candidateAps m_candidateAps  m_candidateAps "<<m_candidateAps.size()<<std::endl;
}

/*void StaWifiMac::ScanningTimeout (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_candidateAps.empty ())
    {
      LiFiApInfo bestAp = m_candidateAps.front();
      m_candidateAps.erase(m_candidateAps.begin ());
      NS_LOG_DEBUG ("Attempting to associate with BSSID " << bestAp.m_bssid);*/
     /* Time beaconInterval;
      if (bestAp.m_activeProbing)
        {
          UpdateApInfoFromProbeResp (bestAp.m_probeResp, bestAp.m_apAddr, bestAp.m_bssid);
          beaconInterval = MicroSeconds (bestAp.m_probeResp.GetBeaconIntervalUs ());
        }
      else
        {
          UpdateApInfoFromBeacon (bestAp.m_beacon, bestAp.m_apAddr, bestAp.m_bssid);
          beaconInterval = MicroSeconds (bestAp.m_beacon.GetBeaconIntervalUs ());
        }

      Time delay = beaconInterval * m_maxMissedBeacons;
      RestartBeaconWatchdog (delay);
      SetState (WAIT_ASSOC_RESP);*/
     // SendAssociationRequest ();
   // }
    /*
  else
    {
      NS_LOG_DEBUG ("Exhausted list of candidate AP; restart scanning");
      StartScanning ();
    }*/
//}

void LiFiStaMac::SendAssociationRequest ()
{
  //std::cout<<"void LiFiStaMac::SendAssociationRequest () "<<std::endl;
  //NS_LOG_FUNCTION (this << GetBssid () << isReassoc);
  LiFiApInfo bestAp = m_candidateAps.front();
  //m_candidateAps.erase(m_candidateAps.begin ());
  WifiMacHeader hdr;
  hdr.SetType (WIFI_MAC_MGT_ASSOCIATION_REQUEST);
  hdr.SetAddr1 (bestAp.m_apAddr);
  hdr.SetAddr2 (GetAddress());
  hdr.SetAddr3 (bestAp.m_bssid);
  //std::cout<<"void LiFiStaMac::SendAssociationRequest () sta address "<<m_centralMac->GetAddress()<<" destination address "<<bestAp.m_apAddr<<" liFiApInfo.m_bssid "<<bestAp.m_bssid<<std::endl;
  //hdr.SetDsNotFrom ();
  //hdr.SetDsNotTo ();
  Ptr<Packet> packet = Create<Packet> ();
  BeaconResponseTag responseTag;
  //std::cout<<"ns3::Ptr<LiFiNetDevice> LiFiStaMac::GetDevice () "<<GetDevice()->GetNode()->GetId()<<std::endl;
  responseTag.SetDeviceID(GetDevice()->GetNode()->GetId());
  responseTag.SetAccessCateogy(GetNumberOfSlotsForAccessCategory(GetDevice()->GetQoSCategory()));
  responseTag.SetTrafficType(GetDevice()->GetTrafficType());
  responseTag.SetSnrLevel(0);
  packet->AddPacketTag(responseTag);
  //The standard is not clear on the correct queue for management
  //frames if we are a QoS AP. The approach taken here is to always
  //use the non-QoS for these regardless of whether we have a QoS
  //association or not.
  //std::cout<<"void LiFiStaMac::SendAssociationRequest () "<<m_controlTxop<<std::endl;
  m_controlTxop->Queue (packet, hdr);
  m_controlTxop->StartTransmission(2); 
  //m_txop->Queue (packet, hdr);
}

void
LiFiStaMac::ScheduleTdmaSession (const uint32_t slotNum)
{
  NS_LOG_FUNCTION (slotNum);
}

void LiFiStaMac::TransmitFrame ()
{
  Time transmissionSlot = MicroSeconds (GetSlotTime ().GetMicroSeconds ());
  Time totalTransmissionTimeUs =  GetGuardTime () + transmissionSlot;
  //std::cout<<"LiFiStaMac::TransmitFrame () m_centralMac "<<m_centralMac<<" totalTransmissionTimeUs "<<totalTransmissionTimeUs<<std::endl;
  m_txMac->StartTransmission (2);
  Simulator::Schedule (totalTransmissionTimeUs, &LiFiStaMac::TransmitFrame, this);
 
}

uint32_t LiFiStaMac::GetNumberOfSlotsForAccessCategory (uint16_t ac)
{
  uint16_t  numOfSlotsAllotted = 1;
  switch (ac)
  {
    case 0:
      numOfSlotsAllotted = 1;
      break;
    case 1:
      numOfSlotsAllotted = 1;
      break;
    case 2:
      numOfSlotsAllotted = 2;
      break;
    default:
      numOfSlotsAllotted = 3;
      break;
   }

  return numOfSlotsAllotted;
}

void LiFiStaMac::SetMacLow(ns3::Ptr<LiFiMacLow> macLow)
{
  m_MacLow = macLow;
  //m_MacLow->SetForwardUpManagmentCallback (MakeCallback (&LiFiStaMac::Receive, this));
}

ns3::Ptr<LiFiMacLow> LiFiStaMac::GetMacLow()
{
  return m_low;
}

void
LiFiStaMac::Enqueue (Ptr<Packet> packet, Mac48Address to, Mac48Address from)
{

  NS_LOG_FUNCTION (this << packet << to << from);

  for (std::vector<LiFiApInfo>::iterator i = m_candidateAps.begin(); i != m_candidateAps.end(); ++i)
  {
//std::cout<<"LiFiStaMac::Enqueue (Ptr<Packet> packet, Mac48Address to, Mac48Address from) from "<<from<<" to "<<to<<std::endl;
    if (to == (*i).m_apAddr)
    {
        ForwardDown (packet, from, to);
        return;
    }
  }

  NotifyTxDrop (packet);
}

void
LiFiStaMac::Enqueue (Ptr<Packet> packet, Mac48Address to)
{
  NS_LOG_FUNCTION (this << packet << to);
  //We're sending this packet with a from address that is our own. We
  //get that address from the lower MAC and make use of the
  //from-spoofing Enqueue() method to avoid duplicated code.
  Enqueue (packet, to, m_low->GetAddress ());
}

void
LiFiStaMac::ForwardDown (Ptr<Packet> packet, Mac48Address from, Mac48Address to)
{
  NS_LOG_FUNCTION (this << packet << from << to);
  WifiMacHeader hdr;
  //For now, an AP that supports QoS does not support non-QoS
  //associations, and vice versa. In future the AP model should
  //support simultaneously associated QoS and non-QoS STAs, at which
  //point there will need to be per-association QoS state maintained
  //by the association state machine, and consulted here.
  hdr.SetType (WIFI_MAC_DATA);
  hdr.SetAddr1 (to);
  hdr.SetAddr2 (m_low->GetAddress ());
  hdr.SetAddr3 (from);
  hdr.SetDsFrom ();
  hdr.SetDsNotTo ();
  m_txMac->Queue (packet, hdr);
  //std::cout<<"LiFiStaMac::ForwardDown (Ptr<Packet> packet, Mac48Address from, Mac48Address to) from "<<from<<" to "<<to<<std::endl;
}

Time
LiFiStaMac::CalculateTxTime (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION (packet);
//std::cout<<"packet->GetSize () "<<packet->GetSize ()<<std::endl;
  NS_ASSERT_MSG (packet->GetSize () < 1500,"PacketSize must be less than 1500B, it is: " << packet->GetSize ());
  return m_bps.CalculateBytesTxTime (packet->GetSize ());
}

} // namespace ns3

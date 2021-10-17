/*std::map<uint32_t, ns3::Ptr<MacResourcesTag> >:: iterator it1;
     it1 = m_slotInfosMaps.find(udInfos->GetDeviceId());
     std::vector<uint16_t> downlinkSlotVecotrs;
     if (it1 != m_slotInfosMaps.end())
     {
          slotVectors = it1->second;
          downlinkSlotVecotrs = slotVectors[0];
     }*/
#include "ns3/assert.h"
#include "ns3/enum.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/lifi-ap-mac.h"
#include "ns3/abort.h"
#include "ns3/lifi-capability-information.h"
#include "ns3/lifi-net-device.h"
#include "measurement-report.h"
NS_LOG_COMPONENT_DEFINE ("LiFiApMac");

#define MY_DEBUG(x) \
  NS_LOG_DEBUG (Simulator::Now () << " " << this << " " << x)

namespace ns3 {

template <typename K, typename V>
V getSecond(std::pair<K, V> const & pair)
{
    return pair.second;
}
NS_OBJECT_ENSURE_REGISTERED (LiFiApMac);

TypeId LiFiApMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3:LiFiApMac")
    .SetParent<Object> ()
    .AddConstructor<LiFiApMac> ()
    .AddAttribute ("DataRate",
                   "The default data rate for point to point links",
                   DataRateValue (GetDefaultDataRate ()),
                   MakeDataRateAccessor (&LiFiApMac::SetDataRate,
                                         &LiFiApMac::GetDataRate),
                   MakeDataRateChecker ())
    .AddAttribute ("SlotTime", "The duration of a Slot in microseconds.",
                   TimeValue (GetDefaultSlotTime ()),
                   MakeTimeAccessor (&LiFiApMac::SetSlotTime,
                                     &LiFiApMac::GetSlotTime),
                   MakeTimeChecker ())
    .AddAttribute ("GuardTime", "GuardTime between TDMA slots in microseconds.",
                   TimeValue (GetDefaultGuardTime ()),
                   MakeTimeAccessor (&LiFiApMac::SetGuardTime,
                                     &LiFiApMac::GetGuardTime),
                   MakeTimeChecker ())
    .AddAttribute ("InterFrameTime", "The wait time between consecutive tdma frames.",
                   TimeValue (MicroSeconds (3)),
                   MakeTimeAccessor (&LiFiApMac::SetInterFrameTimeInterval,
                                     &LiFiApMac::GetInterFrameTimeInterval),
                   MakeTimeChecker ())
    .AddAttribute ("BeaconInterval",
                   "Delay between two beacons",
                   TimeValue (MicroSeconds (102400)),
                   MakeTimeAccessor (&LiFiApMac::GetBeaconInterval,
                                     &LiFiApMac::SetBeaconInterval),
                   MakeTimeChecker ())
    .AddAttribute ("BeaconJitter",
                   "A uniform random variable to cause the initial beacon starting time (after simulation time 0) "
                   "to be distributed between 0 and the BeaconInterval.",
                   StringValue ("ns3::UniformRandomVariable"),
                   MakePointerAccessor (&LiFiApMac::m_beaconJitter),
                   MakePointerChecker<UniformRandomVariable> ())
    .AddAttribute ("EnableBeaconJitter",
                   "If beacons are enabled, whether to jitter the initial send event.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&LiFiApMac::m_enableBeaconJitter),
                   MakeBooleanChecker ())
    .AddAttribute ("BeaconGeneration",
                   "Whether or not beacons are generated.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&LiFiApMac::SetBeaconGeneration),
                   MakeBooleanChecker ())
    .AddTraceSource ("HandoverUpdate", "Information about users who slots been removed from the frame and therefore, need handover", 
                   MakeTraceSourceAccessor (&LiFiApMac::m_CompleteHandoverllback),
                   "ns3::LiFiApMac::CompleteHandoverllback");
  return tid;
}

LiFiApMac::LiFiApMac ()
  : m_slotTime (25),
		m_guardTime (2),
		m_tdmaFrameLength (1500),
		m_tdmaInterFrameTime (6),
		m_totalSlotsAllowed (0),
		m_activeEpoch (false),
    m_channel (0)
{
  LiFiRegularMac();
  NS_LOG_FUNCTION (this);
  m_current_slot = 0;
  m_current_device_index = 0;
  frame_number = 0;
  m_controlGuardTime = 2;
  m_totalSlotsAllowed = 0;
 // Simulator::Schedule (MicroSeconds (100),&LiFiApMac::StartSuperFrame, this);
  m_beaconTxop = CreateObject<LiFiTxMac> ();;
  m_beaconTxop->SetMacLow (m_low);
  //m_beaconTxop->SetTxOkCallback (MakeCallback (&ApWifiMac::TxOk, this));

Simulator::Schedule (NanoSeconds (10),&LiFiApMac::Start, this);
  NS_LOG_DEBUG("LiFiApMac constructor");
}

LiFiApMac::~LiFiApMac ()
{
  m_channel = 0;
  m_bps = 0;
 // std::cout<<"LiFiApMac::~LiFiApMac () "<<std::endl;
  //m_slotPtrs.clear ();
}

void LiFiApMac::Start (void)
{
  NS_LOG_FUNCTION (this);
  // m_activeEpoch = true;
  // StartSuperSessions();
  if (m_activeEpoch)
  {
    //std::cout<<"LiFiApMac::SetMacLow macLow "<<m_centralMac->GetMacLow()->GetPhy()->GetDevice()->GetNode()->GetId()<<std::endl;
    //m_centralMac->StopTransmission(false);
  }
  m_activeEpoch = true;
  frame_number =0;
  //m_activeEpoch = true;
  SendOneBeacon();
  //SendControlPackets ();
  //TransmitControlFrames(0);
  //std::cout<<"void LiFiApMac::Start (void): void LiFiApMac::DistributeSlots() m_slotPtrs.size() transmissionsVector "<<transmissionsVector.size()<<std::endl;
  Simulator::Schedule (MicroSeconds (50),&LiFiApMac::DistributeSlots, this);
 
}

void  LiFiApMac::ResetDataStructures()
{
  
 // std::cout<<"void  LiFiApMac::ResetDataStructures() "<<transmissionsVector.size()<<std::endl;
  m_slotPtrs_vector.clear();
  m_slotInfosMaps.clear();
  m_slotPtrs.clear();
  
}   

Time LiFiApMac::GetDefaultSlotTime (void)
{
  return MicroSeconds (35);
}

Time LiFiApMac::GetDefaultGuardTime (void)
{
  return MicroSeconds (2);
}

DataRate LiFiApMac::GetDefaultDataRate (void)
{
  NS_LOG_DEBUG ("Setting default");
  return DataRate ("400000000b/s");
}

void LiFiApMac::SetRemoteStationManager (ns3::Ptr<RemoteStationManager> remoteStationManager)
{
  m_remoteStationManager = remoteStationManager;
}

void
LiFiApMac::SetBeaconGeneration (bool enable)
{
  NS_LOG_FUNCTION (this << enable);
  if (!enable)
    {
      m_beaconEvent.Cancel ();
    }
  else if (enable && !m_enableBeaconGeneration)
    {
     // m_beaconEvent = Simulator::ScheduleNow (&LiFiApMac::SendOneBeacon, this);
    }
  m_enableBeaconGeneration = enable;
}

Time
LiFiApMac::GetBeaconInterval (void) const
{
  NS_LOG_FUNCTION (this);
  return m_beaconInterval;
}

void
LiFiApMac::SetBeaconInterval (Time interval) 
{
  NS_LOG_FUNCTION (this);
  NS_LOG_FUNCTION (this << interval);
  if ((interval.GetMicroSeconds () % 1024) != 0)
    {
      NS_FATAL_ERROR ("beacon interval should be multiple of 1024us (802.11 time unit), see IEEE Std. 802.11-2012");
    }
  if (interval.GetMicroSeconds () > (1024 * 65535))
    {
      NS_FATAL_ERROR ("beacon interval should be smaller then or equal to 65535 * 1024us (802.11 time unit)");
    }
  m_beaconInterval = interval;
}

void LiFiApMac::StartSuperSessions (void)
{
 NS_LOG_FUNCTION (this);
 //std::cout<<"void LiFiApMac::StartSuperSessions (void)"<<std::endl;
  // m_activeEpoch = true;
  // StartSuperSessions();
  if (m_activeEpoch)
  {
    //std::cout<<"LiFiApMac::SetMacLow macLow "<<m_centralMac->GetMacLow()->GetPhy()->GetDevice()->GetNode()->GetId()<<std::endl;
    //m_centralMac->StopTransmission(false);
  }
  m_activeEpoch = true;
  frame_number =0;
  //m_activeEpoch = true;
  SendOneBeacon();
  //SendControlPackets ();
  //TransmitControlFrames(0);
  //std::cout<<"void LiFiApMac::Start (void): void LiFiApMac::DistributeSlots() m_slotPtrs.size() transmissionsVector "<<transmissionsVector.size()<<std::endl;
  Simulator::Schedule (MicroSeconds (100),&LiFiApMac::DistributeSlots, this);
}

void LiFiApMac::SendOneBeacon (void)
{
  //std::cout<<"void LiFiApMac::SendOneBeacon (void) "<<std::endl;
    NS_LOG_FUNCTION (this);
    WifiMacHeader hdr;
    hdr.SetType (WIFI_MAC_MGT_BEACON);
    hdr.SetAddr1 (Mac48Address::GetBroadcast ());
    hdr.SetAddr2 (GetAddress());
   // hdr.SetAddr3 (m_centralMac->GetAddress ());
    hdr.SetDsNotFrom ();
    hdr.SetDsNotTo ();
    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader(hdr);
    MgtBeaconHeader beacon;
    //beacon.SetSsid (GetSsid ());
    //beacon.SetSupportedRates (GetSupportedRates ());
    beacon.SetBeaconIntervalUs (GetBeaconInterval ().GetMicroSeconds ());
    packet->AddHeader(beacon);
    //The beacon has it's own special queue, so we load it in there
    m_beaconTxop->Queue (packet, hdr);
    m_beaconTxop->StartTransmission (30);
    for (size_t i = 0; i < m_slotPtrs_vector.size(); i++)
    {
      m_lastepoch_slotPtrs_vector.push_back(m_slotPtrs_vector[i]);
    }


    m_slotPtrs_vector.clear();
}

void LiFiApMac::SendControlPackets ()
{
  // Vector of int to store values
  std::vector<MacResourcesTag > macResourceTagVector;  
  macResourceTagVector.reserve(m_slotInfosMaps.size());
  //std::cout<<"void LiFiApMac::SendControlPackets () "<<macResourceTagVector.size()<<" m_slotInfosMaps "<<m_slotInfosMaps.size()<<std::endl;
  std::transform (m_slotInfosMaps.begin(), m_slotInfosMaps.end(),back_inserter(macResourceTagVector), &getSecond<uint32_t, MacResourcesTag>);
  for (std::map<uint32_t, MacResourcesTag>::iterator it = m_slotInfosMaps.begin(); it != m_slotInfosMaps.end(); ++it)
  {
    MacResourcesTag     macTransmissionUnit = it->second; 
    Ptr<Packet> packet = Create<Packet> ();
  
    WifiMacHeader hdr;
    hdr.SetType ( WIFI_MAC_MGT_ASSOCIATION_RESPONSE);
    hdr.SetAddr1 (macTransmissionUnit.GetDestinationAddress());
    hdr.SetAddr2 (GetAddress ());
   // hdr.SetDsNotFrom ();
    //hdr.SetDsNotTo ();
    packet->AddHeader (hdr); 
 //   std::cout<<"Destination Address "<<macTransmissionUnit.GetDestinationAddress()<<" GetTotalUplinklinkSlots "<<macTransmissionUnit.GetTotalUplinklinkSlots()<<" GetTotalDownlinkSlots "<<macTransmissionUnit.GetTotalDownlinkSlots()<<std::endl;
    packet->AddPacketTag(macTransmissionUnit);                                             
    m_beaconTxop->Queue (packet, hdr);
  }
  m_beaconTxop->StopTransmission(true);
  TransmitControlFrames(0);
  if (transmissionsVector.size() > 0)
  {
   // std::cout<<"void LiFiApMac::SendControlPackets () transmissionsVector.size() "<<transmissionsVector.size()<<" Time Now "<<ns3::Simulator::Now().GetSeconds()<<std::endl;
    Simulator::Schedule (MicroSeconds (120),&LiFiApMac::TransmitDataFrame,this,transmissionsVector[0]);
  }
}

Ptr<PointToPointNetDevice> LiFiApMac::GetPointerToP2PNetDevice ()
{
  Ptr<Node> node = GetNode();
  //std::cout<<"node "<<node<<std::endl;
  uint32_t num_devs = node->GetNDevices();

  for (size_t i = 0; i < num_devs; i++)
  {
    Ptr<NetDevice> netD = node->GetDevice(i);
    Ptr<PointToPointNetDevice> netdev = ns3::DynamicCast <PointToPointNetDevice>(netD);
    if (netdev)
    {
      return netdev;
    }
    
  }

  return 0;
}

void LiFiApMac::SetCCMacAddress (Address address)
{
  m_cc_address = address;
}

Address LiFiApMac::GetCCMacAddress (void)
{
  return m_cc_address;
}

void LiFiApMac::SendRecordToCentralizedControllers (uint32_t index)
{

  for (size_t i = 0; i < m_slotPtrs_vector.size(); i++)
  {
    Ptr<LiFiUserDeviceInfos> udinfos = m_slotPtrs_vector[i];
    Ptr<PointToPointNetDevice> p2pnetdev = GetPointerToP2PNetDevice();
    MeasurementReport measurementReport;
    measurementReport.SetDeviceID(udinfos->GetDeviceID());
    std::vector<double> snrV;
    snrV.push_back(udinfos->GetSinrLevel());
    measurementReport.SetSNR(snrV);
    Ptr<Packet> packet = Create<Packet> ();
    packet->AddPacketTag(measurementReport);
    uint16_t protoc = 0x800;
    p2pnetdev->Send(packet,GetCCMacAddress(),protoc);
    //std::cout<<"p2pnetdev p2pnetdev  packet "<<packet<<" p2pnetdev "<<p2pnetdev<<std::endl;
  }

  for (size_t i = 0; i < m_lastepoch_slotPtrs_vector.size(); i++)
  {

    Ptr<LiFiUserDeviceInfos> udinfos = m_lastepoch_slotPtrs_vector[i];
    Ptr<PointToPointNetDevice> p2pnetdev = GetPointerToP2PNetDevice();
    MeasurementReport measurementReport;
    measurementReport.SetDeviceID(udinfos->GetDeviceID());
    std::vector<double> snrV;
    snrV.push_back(0);
    measurementReport.SetSNR(snrV);
    Ptr<Packet> packet = Create<Packet> ();
    packet->AddPacketTag(measurementReport);
    uint16_t protoc = 0x800;
    p2pnetdev->Send(packet,GetCCMacAddress(),protoc);
  
  }
  
}

void LiFiApMac::TransmitControlFrames(uint32_t index)
{
  NS_LOG_FUNCTION (this);
  Time transmissionSlot = Time(MicroSeconds(20));
  Time totalTransmissionTimeUs =  MicroSeconds(4) + transmissionSlot;
 
  m_beaconTxop->StartTransmission (totalTransmissionTimeUs.GetMicroSeconds ());
  if (index == m_slotPtrs_vector.size())
  {
    return;
  }
  else
  {
   //std::cout<<"void LiFiApMac totalTransmissionTimeUs "<<totalTransmissionTimeUs.GetSeconds()<<" GetControlGuardSlotTime "<<GetControlGuardSlotTime()<<" m_slotPtrs_vector "<<m_slotPtrs_vector.size()<<std::endl;
    Simulator::Schedule (totalTransmissionTimeUs, &LiFiApMac::TransmitControlFrames, this, index);
    index++;

  }
}

void LiFiApMac::SetControlGuardSlotTime (uint32_t time)
{
  m_controlGuardTime = time;
}

Time LiFiApMac::GetControlGuardSlotTime ()
{
  return MicroSeconds(m_controlGuardTime);
}

void LiFiApMac::StartFrameTransmission (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  frame_number = frame_number + 1;
  
  //if(transmissionsVector.size() > 0)
  TransmitDataFrame (transmissionsVector[0]);
}

void LiFiApMac::DistributeSlots()
{

  if (m_slotPtrs_vector.size() == 0)
  {
    ns3::Simulator::Schedule (MilliSeconds(30), &LiFiApMac::StartSuperSessions, this);
    Simulator::ScheduleNow (&LiFiApMac::SendRecordToCentralizedControllers,this,0);
    return;
  }
  
   Simulator::ScheduleNow (&LiFiApMac::SendRecordToCentralizedControllers,this,0);

  transmissionsVector.clear();
  m_totalSlotsAllowed = 0;
  //m_totalSlotsAllowed = 0;
  uint16_t slot_number = 0;
 // std::cout<<"void LiFiApMac::DistributeSlots() m_slotPtrs.size() "<<this->m_slotPtrs.size()<<std::endl;
  ///downlink
  for(std::map<uint32_t, Ptr<LiFiUserDeviceInfos>  >::iterator it = m_slotPtrs.begin(); it != m_slotPtrs.end(); ++it)
  {
    Ptr<LiFiUserDeviceInfos> udInfos = it->second;   
    uint16_t* leng_arr = new uint16_t[2];
    uint16_t ac_index = 0;
    uint8_t trafficType = udInfos->GeetServiceType();
    if (trafficType == 0 ||  trafficType == 2)
    {
        ac_index = GetNumberOfSlotsForAccessCategory (udInfos->GetQoSRequirements());
    } 
    else 
    {
      ac_index = 1;
    }
     
    leng_arr[0] = ac_index;
    leng_arr [1] = 0;

//    std::cout<<"ac_index  "<<ac_index<<" leng_arr [0] "<<leng_arr[0]<<" leng_arr [1] "<<leng_arr[1]<<std::endl;
    uint16_t* downlink_arr = new uint16_t[ac_index];
    //std::cout<<" ac_index  ac_index "<<ac_index<<std::endl;
    for (size_t i = 0; i < ac_index; i++)
    {
        
        MacTransmissionUnit macTransmissionUnit;
        macTransmissionUnit.slot_id = slot_number;
        macTransmissionUnit.node_id = udInfos->GetDeviceID();
        macTransmissionUnit.destinationAddress = udInfos->GetDestinationAddress();
        
        macTransmissionUnit.transmissionType = "Downlink";   
        transmissionsVector.push_back(macTransmissionUnit);  
        m_totalSlotsAllowed ++;
        
        downlink_arr[i] = slot_number;
        slot_number ++;
         // downlinkSlotVecotrs.push(macTransmissionUnit);
      }
     // slotVectors[0] = downlinkSlotVecotrs;
      //std::cout<<"download m_totalSlotsAllowed "<<m_totalSlotsAllowed<<std::endl;
      MacResourcesTag macReourceTag;
      macReourceTag.SetDestinationAddress (udInfos->GetDestinationAddress());
      macReourceTag.SetDeviceID(udInfos->GetDeviceID());
      macReourceTag.SetTotalDownlinkSlots(ac_index);
      macReourceTag.SetDownlinkSlots(downlink_arr);
      m_slotInfosMaps.insert(std::make_pair(udInfos->GetDeviceID(), macReourceTag));
     // delete downlink_arr;
     // delete leng_arr;
  }
  
 // leng_arr[0] = (transmissionsVector.size()-1);
  for(std::map<uint32_t, Ptr<LiFiUserDeviceInfos>  >::iterator it = m_slotPtrs.begin(); it != m_slotPtrs.end(); ++it)
  {
    Ptr<LiFiUserDeviceInfos> udInfos = it->second;
    uint16_t ac_index = 0;
    uint8_t trafficType = udInfos->GeetServiceType();
    if (trafficType == 1 ||  trafficType == 2)
    {
        ac_index = GetNumberOfSlotsForAccessCategory (udInfos->GetQoSRequirements());
    } 
    else 
    {
      ac_index = 1;
    }
         //std::cout<<"uplink  ac_index  ac_index "<<ac_index<<std::endl;

    std::map<uint32_t, MacResourcesTag >:: iterator it1;
    it1 = m_slotInfosMaps.find(udInfos->GetDeviceID());
    //uint16_t* leng_arr ;
    uint16_t* uplink_arr;
    MacResourcesTag macReourceTag;
    if (it1 != m_slotInfosMaps.end())
    {
      macReourceTag = it1->second;
    }
    
    uplink_arr = new uint16_t[ac_index];
    for (size_t i = 0; i < ac_index; i++)
    {
      MacTransmissionUnit macTransmissionUnit;
      macTransmissionUnit.slot_id = slot_number;
      macTransmissionUnit.node_id = udInfos->GetDeviceID();
      macTransmissionUnit.destinationAddress = udInfos->GetDestinationAddress();
      macTransmissionUnit.transmissionType = "Uplink";  
      transmissionsVector.push_back(macTransmissionUnit);  
      uplink_arr[i] = slot_number;
      m_totalSlotsAllowed++;
      //std::cout<<"uplink m_totalSlotsAllowed "<<m_totalSlotsAllowed<<std::endl;

      slot_number ++;
      // uplinkSlotVecotrs.push(macTransmissionUnit);
    }


      //std::cout<<"total uplink slots "<<ac_index<<" Uplink slot number "<<uplink_arr[0]<<std::endl;
      macReourceTag.SetTotalUplinklinkSlots(ac_index);
      macReourceTag.SetUplinkSlots(uplink_arr);

      std::map<uint32_t, MacResourcesTag >:: iterator itr;
      itr = m_slotInfosMaps.find (it->first);             // by iterator (b), leaves acdefghi.
      m_slotInfosMaps.erase (udInfos->GetDeviceID());
      //macReourceTag.SetDeviceID(udInfos->GetDeviceID());
      //macReourceTag.SetDestinationAddress (udInfos->GetDestinationAddress());
      m_slotInfosMaps.insert(std::make_pair(udInfos->GetDeviceID(), macReourceTag));
      //delete uplink_arr;
      //delete leng_arr;

  }
  static int count =0;
  if (count<=100000)
  {
    //std::cout<<"Distributed Slots Number of Users m_slotInfosMaps "<<m_slotInfosMaps.size()<<" transmissionsVector size "<<transmissionsVector.size()<<" count "<<count<<" m_totalSlotsAllowed "<<m_totalSlotsAllowed<<std::endl;
    count++;
  }

  // std::cout<<"void LiFiApMac::DistributeSlots() m_slotPtrs.size() "<<m_slotPtrs.size()<<std::endl;
  Simulator::Schedule (MicroSeconds (30),&LiFiApMac::SendControlPackets, this);

}

void LiFiApMac::UpdateUserRegistration()
{
  bool isUserRemoved = false;
  std::map<uint32_t, Ptr<LiFiUserDeviceInfos> > userInfosMap(m_slotPtrs);
  std::vector<Ptr<Node> > removedNodesVect;

  for(std::map<uint32_t, Ptr<LiFiUserDeviceInfos>  >::iterator it = userInfosMap.begin(); it != userInfosMap.end(); ++it)
  {
      Ptr<LiFiUserDeviceInfos> udInfos = it->second;
      std::map<uint32_t, ns3::Ptr<Node> >::iterator it1;
      it1 = markedUeNodesMap.find(udInfos->GetDeviceID());
      if (it1 != markedUeNodesMap.end())
      {
          Ptr<Node> node = it1->second;
          std::map<uint32_t, Ptr<LiFiUserDeviceInfos> >::iterator itr1;
          itr1 = m_slotPtrs.find (it->first);             // by iterator (b), leaves acdefghi.
          m_slotPtrs.erase (itr1->first);
          markedUeNodesMap.erase(it1->first);
          removedNodesVect.push_back(it1->second);
          isUserRemoved = true;
          m_totalSlotsAllowed--;
      }
  }

  if (isUserRemoved)
  {    
    ReAssignSlots();
    m_CompleteHandoverllback(this, removedNodesVect);
  } 
}

void LiFiApMac::AddUserDeviceInfos(Ptr<LiFiUserDeviceInfos > userDeviceInfos)
{
  m_slotPtrs_vector.push_back(userDeviceInfos);
}

void  LiFiApMac::ReAssignSlots()
{
  std::vector<uint32_t> keys;
  std::vector<Ptr<LiFiUserDeviceInfos> > valueVects;

  std::map<uint32_t, Ptr<LiFiUserDeviceInfos> >::iterator it;
  for (it=m_slotPtrs.begin(); it!=m_slotPtrs.end(); ++it) 
  {
    keys.push_back(it->first);
    valueVects.push_back(it->second);
  } 

  m_slotPtrs.clear();
  for (uint32_t i = 0; i < keys.size(); i++)
  {  
      AddTdmaSlot(i,valueVects[i]); 
  }
}

void LiFiApMac::AddTdmaSlot (uint32_t slotPos, Ptr<LiFiUserDeviceInfos> macPtr)
{
  NS_LOG_FUNCTION (slotPos << macPtr);
  std::pair<std::map<uint32_t, Ptr<LiFiUserDeviceInfos> >::iterator, bool> result = m_slotPtrs.insert (std::make_pair (slotPos,macPtr));
  if (result.second == true)
  {
      NS_LOG_DEBUG ("Added mac : " << macPtr << " in slot " << slotPos);
  }
  else
  {
      NS_LOG_WARN ("Could not add mac: " << macPtr << " to slot " << slotPos);
  }
  //std::cout<<"void LiFiApMac::AddTdmaSlot (uint32_t slotPos, Ptr<LiFiUserDeviceInfos> macPtr) m_slotPtrs "<<m_slotPtrs.size()<<std::endl;
}

void LiFiApMac::AddTdmaSlots (uint32_t slotPos, Ptr<LiFiUserDeviceInfos> macPtr)
{
  NS_LOG_FUNCTION (slotPos << macPtr);
  std::pair<std::map<uint32_t, Ptr<LiFiUserDeviceInfos> >::iterator, bool> result = m_slotPtrs.insert (std::make_pair (slotPos,macPtr));

  if (result.second == true)
    {
      NS_LOG_DEBUG ("Added mac : " << macPtr << " in slot " << slotPos);
    }
  else
    {
      NS_LOG_WARN ("Could not add mac: " << macPtr << " to slot " << slotPos);
    }
}

void LiFiApMac::SetSlotTime (Time slotTime)
{
  NS_LOG_FUNCTION (this << slotTime);
  m_slotTime = 15.0;
  m_slotTime = slotTime.GetMicroSeconds ();
}

void LiFiApMac::MarkNodeForHandoverNodes(Ptr<Node> node)
{
    markedUeNodesMap.insert(std::make_pair(node->GetId(), node));
    markedUeNodesVector.push_back(node);
}

Time LiFiApMac::GetSlotTime (void) const
{
  return MicroSeconds (30);
}

void LiFiApMac::SetDataRate (DataRate bps)
{
  NS_LOG_FUNCTION (this << bps);
  m_bps = bps;
}

DataRate LiFiApMac::GetDataRate (void) const
{
  return m_bps;
}

void LiFiApMac::SetChannel (Ptr<LiFiChannel> c)
{
  NS_LOG_FUNCTION (this << c);
  m_channel = c;
}


Ptr<LiFiChannel> LiFiApMac::GetChannel (void) 
{
  NS_LOG_FUNCTION (this);
  return m_channel;
}

void LiFiApMac::SetGuardTime (Time guardTime)
{
  NS_LOG_FUNCTION (this << guardTime);
  if (m_channel != 0)
    {
      m_guardTime = guardTime.GetMicroSeconds();
    }
  else
    {
      m_guardTime = guardTime.GetMicroSeconds ();
    }
}

Time LiFiApMac::GetGuardTime (void) const
{
  return MicroSeconds (m_guardTime);
}

void LiFiApMac::SetInterFrameTimeInterval (Time interFrameTime)
{
  NS_LOG_FUNCTION (interFrameTime);
  m_tdmaInterFrameTime = interFrameTime.GetMicroSeconds ();
}

Time LiFiApMac::GetInterFrameTimeInterval (void) const
{
  return MicroSeconds (m_tdmaInterFrameTime);
}

void LiFiApMac::SetTotalSlotsAllowed (uint32_t slotsAllowed)
{
  m_totalSlotsAllowed = slotsAllowed;
  //m_totalSlotsAllowed = 60;
// std::cout<<"void LiFiApMac::SetTotalSlotsAllowed (uint32_t slotsAllowed) m_totalSlotsAllowed "<<m_totalSlotsAllowed<<std::endl;
  //m_slotPtrs.clear ();
}

uint32_t LiFiApMac::GetTotalSlotsAllowed (void) const
{
  return m_totalSlotsAllowed;
}

void LiFiApMac::SetMacLow(ns3::Ptr<LiFiMacLow> macLow)
{
  //m_MacLow = macLow;
  //m_MacLow->SetForwardUpManagmentCallback (MakeCallback (&LiFiApMac::Receive, this));
}

ns3::Ptr<LiFiMacLow> LiFiApMac::GetMacLow()
{
  return m_low;
}

void LiFiApMac::ReigsterUserDevice(Ptr<Packet> packet, WifiMacHeader machHeader)
{ 

//std::cout<<"void LiFiApMac::ReigsterUserDevice(Ptr<Packet> packet, WifiMacHeader machHeader) "<<std::endl;
  //Ptr<LiFiUserDeviceInfos> lifiUserDeviceInfos = CreateObject <LiFiUserDeviceInfos>();
  //m_slotPtrs_vector.push_back(lifiUserDeviceInfos);
  ns3::BeaconResponseTag responseTag;
  packet->RemovePacketTag(responseTag);
  uint32_t id             = responseTag.GetDeviceID();
  uint8_t acc_cat         = responseTag.GetAccessCateogy();
  uint8_t traffic_type    = responseTag.GetTrafficType();
  Mac48Address macAddress = machHeader.GetAddr2();

  //std::cout<<"BeaconResponseTag GetSnrLevel "<<10*log(responseTag.GetSnrLevel())<<std::endl;
  ns3::Ptr<LiFiUserDeviceInfos> lifiUserDeviceInfos = CreateObject <LiFiUserDeviceInfos> ();
  lifiUserDeviceInfos->SetDeviceID(id);
  lifiUserDeviceInfos->SetQoSRequirements(acc_cat);
  lifiUserDeviceInfos->SetServiceType(traffic_type);
  lifiUserDeviceInfos->SetDestinationAddress(macAddress);
  double snr = 10*log(responseTag.GetSnrLevel());
  lifiUserDeviceInfos->SetSinrLevel(snr);
  UpdateUserRegistration(lifiUserDeviceInfos);
  m_slotPtrs.insert (std::make_pair (lifiUserDeviceInfos->GetDeviceID(),lifiUserDeviceInfos));
  //AddTdmaSlots(lifiUserDeviceInfos->GetDeviceID(),lifiUserDeviceInfos);
}

void LiFiApMac::UpdateUserRegistration ( ns3::Ptr<LiFiUserDeviceInfos> lfUserDeviceInfos)
{   bool elementFound = false;
    for (size_t i = 0; i < m_slotPtrs_vector.size(); i++)
    {
      if (lfUserDeviceInfos->GetDeviceID() == m_slotPtrs_vector[i]->GetDeviceID())
      {
        elementFound = true;
        m_slotPtrs_vector[i] = lfUserDeviceInfos;
        break;
      }
    }

    if (!elementFound)
    {
      m_slotPtrs_vector.push_back(lfUserDeviceInfos);
    }
    
    
    uint32_t index = -1;
    bool found = false;
    for (size_t i = 0; i < m_lastepoch_slotPtrs_vector.size(); i++)
    {
      if (m_lastepoch_slotPtrs_vector[i]->GetDeviceID() == lfUserDeviceInfos->GetDeviceID())
      {
        index = i;
        found = true;
        break;
      }
    }

    if (found)
    {
      //std::cout<<"void LiFiApMac::UpdateUserRegistration ( ns3::Ptr<LiFiUserDeviceInfos> lfUserDeviceInfos) index "<<index<<" m_lastepoch_slotPtrs_vector "<<m_lastepoch_slotPtrs_vector.size()<<std::endl;
      m_lastepoch_slotPtrs_vector.erase(m_lastepoch_slotPtrs_vector.begin() + index);
      //std::cout<<"after void LiFiApMac::UpdateUserRegistration ( ns3::Ptr<LiFiUserDeviceInfos> lfUserDeviceInfos) index "<<index<<" m_lastepoch_slotPtrs_vector "<<m_lastepoch_slotPtrs_vector.size()<<std::endl;
    }

}

void LiFiApMac::TransmitDataFrame (MacTransmissionUnit userDevice)
{
  //std::cout<<"void LiFiApMac::TransmitDataFrame (MacTransmissionUnit userDevice) "<<std::endl;
  Time transmissionSlot = Time(GetSlotTime ().GetNanoSeconds());
  Time totalTransmissionTimeUs =  GetGuardTime () + transmissionSlot;
  if (frame_number == 60)
  {
    NS_LOG_DEBUG ("Starting over all sessions again");//StartFrameTransmission
    Simulator::Schedule ((totalTransmissionTimeUs + GetInterFrameTimeInterval ())*2+GetGuardTime (), &LiFiApMac::StartSuperSessions, this);
    m_current_slot = 0;
    m_current_device_index = 0;

    //std::cout<<"  if (frame_number == 10) "<<frame_number<<" Simulator Time "<<Simulator::Now().GetSeconds()<<std::endl;
  }
  else if (m_current_slot >= GetTotalSlotsAllowed ())
  {
  // std::cout<<"   frame_number  "<<frame_number<<" GetTotalSlotsAllowed () "<<GetTotalSlotsAllowed ()<<" totalTransmissionTimeUs "<<totalTransmissionTimeUs.GetMicroSeconds()<<std::endl;
    NS_LOG_DEBUG ("Starting over all sessions again");//StartFrameTransmission
  //std::cout<<"void LiFiApMac::TransmitDataFrame (MacTransmissionUnit userDevice) total transmission time "<<(totalTransmissionTimeUs + GetInterFrameTimeInterval ())<<std::endl;
    Simulator::Schedule ((totalTransmissionTimeUs + GetInterFrameTimeInterval ()), &LiFiApMac::StartFrameTransmission, this);
    m_current_slot = 0;
    m_current_device_index = 0;
    //std::cout<<"Restart Frame "<<std::endl;
  }
  else
  {
    NS_LOG_DEBUG ("Scheduling next session");
   
    for (size_t i = 0; i < transmissionsVector.size(); i++)
    {
      //std::cout<<"transmissionsVector[m_current_slot].transmissionType == Uplink: "<<transmissionsVector[i].transmissionType<<std::endl;
    }

    
  MacTransmissionUnit macunit = transmissionsVector[m_current_slot];
   //  std::cout<<"///////////////// m_current_slot "<<m_current_slot<<" transmissionsVector "<<transmissionsVector.size()<<" macunit.transmissionType "<<macunit.transmissionType<<" GetTotalSlotsAllowed () "<<GetTotalSlotsAllowed ()<<std::endl;

  if (macunit.transmissionType == "Uplink")
  {
      //std::cout<<"1------>transmissionsVector "<<"GetTotalSlotsAllowed () "<<GetTotalSlotsAllowed ()<<" "<<transmissionsVector.size()<<" m_current_slot "<<m_current_slot<<std::endl;  

   // std::cout<<"transmissionsVector[m_current_slot].transmissionType ==  Uplink "<<std::endl;
      Ptr<LiFiRegularMac> mac= m_remoteStationManager->GetMac(userDevice.destinationAddress);
      Ptr<LiFiTxMac> txMac = mac->GetLiFiTxMac();
      //std::cout<<"transmissionsVector[m_current_slot].transmissionType ==  Uplink transmissionSlot.GetMicroSeconds () "<<transmissionSlot.GetMicroSeconds ()<<std::endl;
      txMac->StartTransmission (transmissionSlot.GetMicroSeconds ());
  }
  else 
  {
    //std::cout<<"transmissionsVector[m_current_slot].transmissionType ==  transmissionSlot.GetMicroSeconds () "<<transmissionSlot.GetMicroSeconds ()<<std::endl;
     m_txMac->StartTransmission (transmissionSlot.GetMicroSeconds ());
  }
    //userDevice->UpdateCurrentSlot();  
    Simulator::Schedule (totalTransmissionTimeUs, &LiFiApMac::TransmitDataFrame, this, transmissionsVector[m_current_slot]);
    m_current_slot ++;
  }
}

bool LiFiApMac::IsUserAssociated(Mac48Address to)
{
 for (std::map<uint32_t, Ptr<LiFiUserDeviceInfos>  >::iterator it = m_slotPtrs.begin(); it != m_slotPtrs.end(); ++it)  
 {
   Ptr<LiFiUserDeviceInfos> udInfos = it->second;
   if (udInfos->GetDestinationAddress() == to)
   {
     return true;
   }
  }

  return false;
}

bool LiFiApMac::SupportsSendFrom (void) const
{
  return false;
}

void
LiFiApMac::Enqueue (Ptr<Packet> packet, Mac48Address to, Mac48Address from)
{
  NS_LOG_FUNCTION (this << packet << to << from);
  if (IsUserAssociated(to) || to.IsBroadcast() )
    {
      ForwardDown (packet, from, to);
    }
  else
    {
      //std::cout<<"LiFiApMac::Enqueue ---> Packet Dropped "<<std::endl;
      NotifyTxDrop (packet);
    }
}

void
LiFiApMac::Enqueue (Ptr<Packet> packet, Mac48Address to)
{
  NS_LOG_FUNCTION (this << packet << to);
  //std::cout<<"LiFiApMac::Enqueue (Ptr<Packet> packet, Mac48Address to) "<<std::endl;
  //We're sending this packet with a from address that is our own. We
  //get that address from the lower MAC and make use of the
  //from-spoofing Enqueue() method to avoid duplicated code.
  Enqueue (packet, to, m_low->GetAddress ());
}

void
LiFiApMac::ForwardDown (Ptr<Packet> packet, Mac48Address from, Mac48Address to)
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
}

uint8_t LiFiApMac::GetNumberOfSlotsForAccessCategory (uint8_t ac)
{
  uint8_t  numOfSlotsAllotted = 10;
  switch (ac)
  {
    case 0:
      numOfSlotsAllotted = 60;

      break;
    case 1:
      numOfSlotsAllotted = 12;

      break;
    case 2:
      numOfSlotsAllotted = 15;
      break;
    default:

      numOfSlotsAllotted = 20;
      break;
   }

  return numOfSlotsAllotted;
}

void LiFiApMac::Receive (Ptr< Packet> packet,WifiMacHeader *hdr)
{
  //NS_LOG_FUNCTION (snrLevel);

  //WifiMacHeader hdr;
  //packet->RemoveHeader (hdr);
  if (hdr->IsProbeResp ())
  { 

  }
  else if (hdr->IsData ())
  {
      ns3::LlcSnapHeader llc;
      packet->RemoveHeader (llc);
      m_forwardUp(packet,llc.GetType(),hdr->GetAddr1 (),hdr->GetAddr2 ());
      return;
  }
  
  else 
  {

      NS_LOG_DEBUG ("Need to drop frame as it is not either data or management frame");
  }
        ReigsterUserDevice(packet,*hdr);

  // std::cout<<"void LiFiApMac::ReigsterUserDevice(Ptr<Packet> packet, WifiMacHeader machHeader) GetDeviceID() "<<m_slotPtrs.size()<<std::endl;
}

Time LiFiApMac::CalculateTxTime (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION (packet);
  //std::cout<<"packet->GetSize () "<<packet->GetSize ()<<std::endl;
  NS_ASSERT_MSG (packet->GetSize () < 1500,"PacketSize must be less than 1500B, it is: " << packet->GetSize ());
 // std::cout<<"Time LiFiApMac::CalculateTxTime (Ptr<const Packet> packet) "<<packet->GetSize ()<<" tx time "<<m_bps.CalculateBytesTxTime (packet->GetSize ())<<std::endl;
  return m_bps.CalculateBytesTxTime (packet->GetSize ());
}

} // namespace ns3

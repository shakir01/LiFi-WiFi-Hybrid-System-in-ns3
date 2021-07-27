#include "ns3/lifi-mgt-header.h"
namespace ns3 {

/***********************************************************
 *          Probe Request
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED (LiFiMgtProbeRequestHeader);

LiFiMgtProbeRequestHeader::~LiFiMgtProbeRequestHeader ()
{
}

void
LiFiMgtProbeRequestHeader::SetSsid (Ssid ssid)
{
  m_ssid = ssid;
}

Ssid
LiFiMgtProbeRequestHeader::GetSsid (void) const
{
  return m_ssid;
}

uint32_t
LiFiMgtProbeRequestHeader::GetSerializedSize (void) const
{
  uint32_t size = 0;
  size += m_ssid.GetSerializedSize ();  
 // size += m_heCapability.GetSerializedSize ();
  return size;
}

TypeId
LiFiMgtProbeRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiMgtProbeRequestHeader")
    .SetParent<Header> ()
    .SetGroupName ("Wifi")
    .AddConstructor<LiFiMgtProbeRequestHeader> ()
  ;
  return tid;
}

TypeId
LiFiMgtProbeRequestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
LiFiMgtProbeRequestHeader::Print (std::ostream &os) const
{
  os << "ssid=" << m_ssid << ", "
     << "rates=" << m_rates;
}

void
LiFiMgtProbeRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i = m_ssid.Serialize (i);
  i = m_rates.Serialize (i);
}

uint32_t
LiFiMgtProbeRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  i = m_ssid.Deserialize (i);
  return i.GetDistanceFrom (start);
}


/***********************************************************
 *          Probe Response
 ***********************************************************/

NS_OBJECT_ENSURE_REGISTERED (LiFiMgtProbeResponseHeader);

LiFiMgtProbeResponseHeader::LiFiMgtProbeResponseHeader ()
{
}

LiFiMgtProbeResponseHeader::~LiFiMgtProbeResponseHeader ()
{
}

uint64_t
LiFiMgtProbeResponseHeader::GetTimestamp ()
{
  return m_timestamp;
}

Ssid
LiFiMgtProbeResponseHeader::GetSsid (void) const
{
  return m_ssid;
}

uint64_t
LiFiMgtProbeResponseHeader::GetBeaconIntervalUs (void) const
{
  return m_beaconInterval;
}

/*void
LiFiMgtProbeResponseHeader::SetCapabilities (CapabilityInformation capabilities)
{
  m_capability = capabilities;
}

CapabilityInformation
LiFiMgtProbeResponseHeader::GetCapabilities (void) const
{
  return m_capability;
}
*/
void
LiFiMgtProbeResponseHeader::SetSsid (Ssid ssid)
{
  m_ssid = ssid;
}

void
LiFiMgtProbeResponseHeader::SetBeaconIntervalUs (uint64_t us)
{
  m_beaconInterval = us;
}

/*void
LiFiMgtProbeResponseHeader::SetEdcaParameterSet (EdcaParameterSet edcaParameters)
{
  m_edcaParameterSet = edcaParameters;
}

EdcaParameterSet
LiFiMgtProbeResponseHeader::GetEdcaParameterSet (void) const
{
  return m_edcaParameterSet;
}*/

TypeId
LiFiMgtProbeResponseHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiMgtProbeResponseHeader")
    .SetParent<Header> ()
    .SetGroupName ("Wifi")
    .AddConstructor<LiFiMgtProbeResponseHeader> ()
  ;
  return tid;
}

TypeId
LiFiMgtProbeResponseHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
LiFiMgtProbeResponseHeader::GetSerializedSize (void) const
{
  uint32_t size = 0;
  size += 8; //timestamp
  size += 2; //beacon interval
  size += m_capability.GetSerializedSize ();
  size += m_ssid.GetSerializedSize ();
  size += m_edcaParameterSet.GetSerializedSize ();

  return size;
}

void LiFiMgtProbeResponseHeader::Print (std::ostream &os) const
{
  os << "ssid=" << m_ssid << ", "
     << "rates=" << m_rates;
}

void LiFiMgtProbeResponseHeader::Serialize (Buffer::Iterator start) const
{
  //timestamp
  //beacon interval
  //capability information
  //SSID
  //supported rates
  //FH parameter set
  //DS parameter set
  //CF parameter set
  //IBSS parameter set
  Buffer::Iterator i = start;
  i.WriteHtolsbU64 (Simulator::Now ().GetMicroSeconds ());
  i.WriteHtolsbU16 (static_cast<uint16_t> (m_beaconInterval / 1024));
//  i = m_capability.Serialize (i);
  i = m_ssid.Serialize (i);
 // i = m_edcaParameterSet.Serialize (i);
}

uint32_t LiFiMgtProbeResponseHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_timestamp = i.ReadLsbtohU64 ();
  m_beaconInterval = i.ReadLsbtohU16 ();
  m_beaconInterval *= 1024;
 // i = m_capability.Deserialize (i);
  i = m_ssid.Deserialize (i);
  //i = m_edcaParameterSet.DeserializeIfPresent (i);
  return i.GetDistanceFrom (start);
}

/***********************************************************
 *          Beacons
 ***********************************************************/
NS_OBJECT_ENSURE_REGISTERED (LiFiMgtBeaconHeader);
/* static */
TypeId LiFiMgtBeaconHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiMgtBeaconHeader")
    .SetParent<LiFiMgtProbeResponseHeader> ()
    .SetGroupName ("Wifi")
    .AddConstructor<LiFiMgtBeaconHeader> ()
  ;
  return tid;
}
}

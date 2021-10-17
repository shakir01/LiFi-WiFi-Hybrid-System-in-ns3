#include "ns3/beacon-response-tag.h"

namespace ns3 {
   TypeId 
 BeaconResponseTag::GetTypeId (void)
 {
   static TypeId tid = TypeId ("ns3::BeaconResponseTag")
     .SetParent<Tag> ()
     .AddConstructor<BeaconResponseTag> ()
   ;
   return tid;
 }

 BeaconResponseTag::BeaconResponseTag()
 {

 }

 void BeaconResponseTag::Print (std::ostream &os) const
 {
 //  os << "v=" << (uint32_t)m_simpleValue;
 }

TypeId BeaconResponseTag::GetInstanceTypeId (void) const
 {
   return GetTypeId ();
 }
 uint32_t BeaconResponseTag::GetSerializedSize (void) const
 {
   return sizeof(m_deviceId) + sizeof(m_accessCategory) + sizeof(m_trafficDirectionality) + sizeof(m_snrLevel) + sizeof(m_error) + sizeof(m_pkt_drop);
 }

 void BeaconResponseTag::Serialize (TagBuffer i) const
 {
   i.WriteU32 (m_deviceId);
   i.WriteU8 (m_accessCategory);
   i.WriteU8(m_trafficDirectionality);
   i.WriteDouble(m_snrLevel);
   i.WriteDouble(m_error);
   i.WriteU32(m_pkt_drop);
 }

 void BeaconResponseTag::Deserialize (TagBuffer i)
 {
   m_deviceId       = i.ReadU32 ();
  // std::cout<<"BeaconResponseTag::Deserialize (TagBuffer i) m_deviceId "<<m_deviceId<<std::endl;
   m_accessCategory = i.ReadU8 ();
   m_trafficDirectionality = i.ReadU8();
   m_snrLevel = i.ReadDouble();
   m_error = i.ReadDouble();
	 m_pkt_drop = i.ReadU32 ();
 }

 void BeaconResponseTag::SetDeviceID (uint32_t id)
 {
  m_deviceId = id;
 }

uint32_t BeaconResponseTag::GetDeviceID ()
{
  return m_deviceId;
}

void BeaconResponseTag::SetAccessCateogy (uint8_t ac) 
{
  m_accessCategory = ac;
}

uint8_t BeaconResponseTag::GetAccessCateogy (void)
{
  return m_accessCategory;
} 

void BeaconResponseTag::SetSnrLevel (double snr)
{
  m_snrLevel = snr;
}

double BeaconResponseTag::GetSnrLevel (void)
{
  return m_snrLevel;
}

void BeaconResponseTag::SetError (double error)
{
  m_error = error;
}

double BeaconResponseTag::GetError ()
{
  return m_error;
}

void BeaconResponseTag::SetPktDrop (double pktDrop)
{
  m_pkt_drop = pktDrop;
}

double BeaconResponseTag::GetPktDrop (void)
{
  return m_pkt_drop;
}

void BeaconResponseTag::SetTrafficType(uint8_t traffictype)
{
  m_trafficDirectionality = traffictype;
}

uint8_t BeaconResponseTag::GetTrafficType(void)
{
  return m_trafficDirectionality;
}

}
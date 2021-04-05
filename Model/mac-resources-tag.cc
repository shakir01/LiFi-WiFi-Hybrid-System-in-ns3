#include "mac-resources-tag.h"

namespace ns3 {
TypeId 
 MacResourcesTag::GetInstanceTypeId (void) const
 {
   return GetTypeId ();
 }
 uint32_t 
 MacResourcesTag::GetSerializedSize (void) const
 {
   return sizeof(m_deviceId) + sizeof(uint8_t)*2 +sizeof(uint8_t)*slotsArray[0] + sizeof(uint8_t)*slotsArray[1];
 }

 void MacResourcesTag::Serialize (TagBuffer i) const
 {
  i.WriteU32 (m_deviceId);
  i.Write (slotsArray,2);
  i.Write (downlinkSlotsArray,slotsArray[0]);
  i.Write (uplinkSlotsArray,slotsArray[1]);

  delete slotsArray;
  delete downlinkSlotsArray;
  delete uplinkSlotsArray;
 }

 void MacResourcesTag::Deserialize (TagBuffer i)
 {
  m_deviceId      = i.ReadU32 ();
  slotsArray = new uint8_t[2];
  i.Read(slotsArray, 2);
  downlinkSlotsArray =  new uint8_t[slotsArray[0]];
  i.Read(downlinkSlotsArray, slotsArray[0]);
  uplinkSlotsArray =  new uint8_t[slotsArray[1]];
  i.Read(uplinkSlotsArray, slotsArray[1]);
   /*downlinkSlotInfos.slots_array = i.ReadU16 ();
   m_downlinkSlots = i.ReadU16 ();
   m_uplinkslots   = i.ReadU16 ();
   uint32_t dataLength = GetLength () - 2;
   uint8_t* data = new uint8_t[dataLength];
   i.Read (data, dataLength);*/
 }

/* void 
 MacResourcesTag::Print (std::ostream &os) const
 {
   os << "v=" << (uint32_t)m_simpleValue;
 }*/
 
void MacResourcesTag::SetDeviceID (uint32_t id)
{
  m_deviceId = id;
}

uint32_t MacResourcesTag::GetDeviceID ()
{
   return m_deviceId;
}

void  MacResourcesTag::SetTotalSlots (uint8_t* slots)
{
  slotsArray = slots;
}

void MacResourcesTag::SetDestinationAddress(Mac48Address destinationaddress)
{
  m_destinationAddress = destinationaddress;
}

Mac48Address MacResourcesTag::GetDestinationAddress(void)
{
  return m_destinationAddress;
}


uint8_t* MacResourcesTag::GetTotalSlots (void)
{
    return slotsArray;
}

void MacResourcesTag::SetDownlinkSlots (uint8_t* slots)
{
  downlinkSlotsArray = slots;
}

void MacResourcesTag::SetUplinkSlots (uint8_t* slots) 
{
  uplinkSlotsArray = slots;
}

uint8_t* MacResourcesTag::GetDownlinkSlots (void)
{
  return downlinkSlotsArray;
}

uint8_t* MacResourcesTag::GetUplinkSlots (void)
{
  return uplinkSlotsArray;
}

}


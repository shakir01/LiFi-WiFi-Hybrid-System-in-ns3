#include "ns3/mac-resources-tag.h"
namespace ns3 {
 TypeId 
 MacResourcesTag::GetTypeId (void)
 {
   static TypeId tid = TypeId ("ns3::MacResourcesTag")
     .SetParent<Tag> ()
     .AddConstructor<MacResourcesTag> ()
   ;
   return tid;
 }

MacResourcesTag::MacResourcesTag()
 {
  totalDownlinkSlots = 0;
	totalUplinkSlots   = 0;
 }

 MacResourcesTag::~MacResourcesTag()
{
//delete downlinkSlotsArray;
//	delete uplinkSlotsArray;
}

TypeId 
 MacResourcesTag::GetInstanceTypeId (void) const
 {
   return GetTypeId ();
 }
 uint32_t 
 MacResourcesTag::GetSerializedSize (void) const
 {
   return sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t)*totalDownlinkSlots + sizeof(uint16_t)*totalUplinkSlots;
 }

 void MacResourcesTag::Serialize (TagBuffer i) const
 {
  i.WriteU32 (m_deviceId);
  /*i.Write (slotsArray,2);
  if (slotsArray[0] > 0)
    i.Write (downlinkSlotsArray,slotsArray[0]);
  
  std::cout<<"slotsArray[1] "<<slotsArray[1]<<" slotsArray[0] "<<slotsArray[0]<<std::endl;
  if (slotsArray[1] > 0)
    i.Write (uplinkSlotsArray,slotsArray[1]);*/
       // std::cout<<" void MacResourcesTag::Serialize (TagBuffer i) const uplink slots "<<totalUplinkSlots<<" total downlink slots "<<totalDownlinkSlots<<std::endl;   

  if (totalDownlinkSlots > 0)
  {    
      i.WriteU16 (totalDownlinkSlots);
      for (size_t ii = 0; ii < totalDownlinkSlots; ii++)
      {
        i.WriteU16 (downlinkSlotsArray[ii]);
      }
      //delete downlinkSlotsArray;
  }

  if (totalUplinkSlots > 0)
  {

      i.WriteU16 (totalUplinkSlots);
      for (size_t ii = 0; ii < totalUplinkSlots; ii++)
      {
        i.WriteU16 (uplinkSlotsArray[ii]);
       // delete uplinkSlotsArray;

      }
  }
  
  /*uint8_t mac[6];
  m_destinationAddress.CopyTo (mac);
  for (size_t ii = 0; ii < 6; ii++)
  {
    std::cout<<"address digit  "<<mac[ii]<<std::endl;
     i.WriteU8(mac[ii]);
  }*/

  //delete slotsArray;
  
 }

 void MacResourcesTag::Deserialize (TagBuffer i)
 {
  m_deviceId        = i.ReadU32 ();
  totalDownlinkSlots= i.ReadU16();
  if (totalDownlinkSlots > 0)
  {    
      downlinkSlotsArray = new uint16_t [totalDownlinkSlots];
      for (size_t ii = 0; ii < totalDownlinkSlots; ii++)
      {
        downlinkSlotsArray[ii] = i.ReadU16 ();
      }
  }

  totalUplinkSlots= i.ReadU16();
  if (totalUplinkSlots > 0 && totalUplinkSlots < 20)
  {    
      uplinkSlotsArray = new uint16_t [totalUplinkSlots];
      for (size_t ii = 0; ii < totalUplinkSlots; ii++)
      {
           // std::cout<<"void MacResourcesTag::Deserialize (TagBuffer i) totalUplinkSlots "<<totalUplinkSlots<<" ii "<<ii<<std::endl;

        uplinkSlotsArray[ii] = i.ReadU16 ();
      }
  }
  
  /*slotsArray = new uint8_t[2];
  i.Read(slotsArray, 2);
  if (slotsArray[0] > 0)
  {
    downlinkSlotsArray =  new uint8_t[slotsArray[0]];
    i.Read(downlinkSlotsArray, slotsArray[0]);
  }

  if (slotsArray[1] > 0)
  {
    uplinkSlotsArray =  new uint8_t[slotsArray[1]];
    i.Read(uplinkSlotsArray, slotsArray[1]);
  }*/

 /* uint8_t mac[6];
 // 
  for (size_t ii = 0; ii < 6; ii++)
  {
    uint8_t digit = i.ReadU8 ();
    mac[ii] = digit;
  }
  m_destinationAddress.CopyTo (mac);*/
   /*downlinkSlotInfos.slots_array = i.ReadU16 ();
   m_downlinkSlots = i.ReadU16 ();
   m_uplinkslots   = i.ReadU16 ();
   uint32_t dataLength = GetLength () - 2;
   uint8_t* data = new uint8_t[dataLength];
   i.Read (data, dataLength);*/
 }

 void 
 MacResourcesTag::Print (std::ostream &os) const
 {
  // os << "v=" << (uint32_t)m_simpleValue;
 }
 
void MacResourcesTag::SetDeviceID (uint32_t id)
{
  m_deviceId = id;
}

uint32_t MacResourcesTag::GetDeviceID ()
{
   return m_deviceId;
}

void  MacResourcesTag::SetTotalSlots (uint16_t* slots)
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


uint16_t* MacResourcesTag::GetTotalSlots (void)
{
    return slotsArray;
}

void MacResourcesTag::SetDownlinkSlots (uint16_t* slots)
{
  downlinkSlotsArray = slots;
}

void MacResourcesTag::SetUplinkSlots (uint16_t* slots) 
{
  uplinkSlotsArray = slots;
}

uint16_t* MacResourcesTag::GetDownlinkSlots (void)
{
  return downlinkSlotsArray;
}

uint16_t* MacResourcesTag::GetUplinkSlots (void)
{
  return uplinkSlotsArray;
}

void MacResourcesTag::SetTotalDownlinkSlots(uint16_t dlSlots)
{
  totalDownlinkSlots = dlSlots;
}

void MacResourcesTag::SetTotalUplinklinkSlots(uint16_t ulSlots)
{
  //std::cout<<"void MacResourcesTag::SetTotalUplinklinkSlots(uint16_t ulSlots) "<<ulSlots<<std::endl;
  totalUplinkSlots = ulSlots;
}

uint16_t MacResourcesTag::GetTotalDownlinkSlots(void)
{
  return totalDownlinkSlots;
}

uint16_t MacResourcesTag::GetTotalUplinklinkSlots(void)
{
  return totalUplinkSlots;
}

}


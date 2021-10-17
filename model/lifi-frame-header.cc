#include "lifi-frame-header.h"
namespace ns3 {
TypeId LiFiFrameHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t LiFiFrameHeader::GetSerializedSize (void) const
{
  return sizeof(m_currentSlot);
}

void LiFiFrameHeader::Serialize (TagBuffer i) const
{
  i.WriteU8(m_currentSlot);
}

void LiFiFrameHeader::Deserialize (TagBuffer i)
{
  m_currentSlot = i.ReadU8 ();
}

void LiFiFrameHeader::SetCurrentSlot (uint8_t slots)
{
  m_currentSlot = slots;
}

uint8_t LiFiFrameHeader::GetCurrentSlot (void)
{
  return m_currentSlot;
}

}


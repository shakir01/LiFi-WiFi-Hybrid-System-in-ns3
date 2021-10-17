
#include "lifi-capability-information.h"

namespace ns3 {

LiFiCapabilityInformation::LiFiCapabilityInformation ()
  : m_capability (0)
{
}

void
LiFiCapabilityInformation::SetEss (void)
{
  Set (0);
  Clear (1);
}

void
LiFiCapabilityInformation::SetIbss (void)
{
  Clear (0);
  Set (1);
}

void
LiFiCapabilityInformation::SetShortSlotTime (bool shortSlotTime)
{
  if (shortSlotTime)
    {
      Set (10);
    }
}

bool
LiFiCapabilityInformation::IsEss (void) const
{
  return Is (0);
}

bool
LiFiCapabilityInformation::IsIbss (void) const
{
  return Is (1);
}

bool
LiFiCapabilityInformation::IsShortSlotTime (void) const
{
  return Is (10);
}

void
LiFiCapabilityInformation::Set (uint8_t n)
{
  uint32_t mask = 1 << n;
  m_capability |= mask;
}

void
LiFiCapabilityInformation::Clear (uint8_t n)
{
  uint32_t mask = 1 << n;
  m_capability &= ~mask;
}

bool
LiFiCapabilityInformation::Is (uint8_t n) const
{
  uint32_t mask = 1 << n;
  return (m_capability & mask) == mask;
}

uint32_t
LiFiCapabilityInformation::GetSerializedSize (void) const
{
  return 2;
}

Buffer::Iterator
LiFiCapabilityInformation::Serialize (Buffer::Iterator start) const
{
  start.WriteHtolsbU16 (m_capability);
  return start;
}

Buffer::Iterator
LiFiCapabilityInformation::Deserialize (Buffer::Iterator start)
{
  m_capability = start.ReadLsbtohU16 ();
  return start;
}

} //namespace ns3

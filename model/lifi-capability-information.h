
#ifndef LIFI_CAPABILITY_INFORMATION_H
#define LIFI_CAPABILITY_INFORMATION_H

#include "ns3/buffer.h"

namespace ns3 {

/**
 * \ingroup wifi
 *
 * Capability information
 */
class LiFiCapabilityInformation
{
public:
  LiFiCapabilityInformation ();

  /**
   * Set the Extended Service Set (ESS) bit
   * in the capability information field.
   */
  void SetEss (void);
  /**
   * Set the Independent BSS (IBSS) bit
   * in the capability information field.
   */
  void SetIbss (void);
  
  /**
   * Set the short slot time bit
   * in the capability information field.
   *
   * \param shortSlotTime the short preamble bit
   *
   */
  void SetShortSlotTime (bool shortSlotTime);
 

  /**
   * Check if the Extended Service Set (ESS) bit
   * in the capability information field is set to 1.
   *
   * \return ESS bit in the capability information
   *         field is set to 1
   */
  bool IsEss (void) const;
  /**
   * Check if the Independent BSS (IBSS) bit
   * in the capability information field is set to 1.
   *
   * \return IBSS bit in the capability information
   *         field is set to 1
   */
  bool IsIbss (void) const;
 
  /**
   * Check if the short slot time
   * in the capability information field is set to 1.
   *
   * \return short slot time bit in the capability information
   *         field is set to 1
   */
  bool IsShortSlotTime (void) const;

  /**
   * Return the serialized size of capability
   * information.
   *
   * \return the serialized size
   */
  uint32_t GetSerializedSize (void) const;
  /**
   * Serialize capability information to the given buffer.
   *
   * \param start an iterator to a buffer
   * \return an iterator to a buffer after capability information
   *         was serialized
   */
  Buffer::Iterator Serialize (Buffer::Iterator start) const;
  /**
   * Deserialize capability information from the given buffer.
   *
   * \param start an iterator to a buffer
   * \return an iterator to a buffer after capability information
   *         was deserialized
   */
  Buffer::Iterator Deserialize (Buffer::Iterator start);


private:
  /**
   * Check if bit n is set to 1.
   *
   * \param n the bit position
   *
   * \return true if bit n is set to 1,
   *         false otherwise
   */
  bool Is (uint8_t n) const;
  /**
   * Set bit n to 1.
   *
   * \param n the bit position
   */
  void Set (uint8_t n);
  /**
   * Set bit n to 0.
   *
   * \param n the bit position
   */
  void Clear (uint8_t n);

  uint16_t m_capability; ///< capability
};

} //namespace ns3

#endif /* CAPABILITY_INFORMATION_H */

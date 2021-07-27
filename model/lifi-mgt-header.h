#ifndef LIFI_MGT_HEADERS_H
#define LIFI_MGT_HEADERS_H

#include "ns3/mac48-address.h"
#include "status-code.h"
#include "capability-information.h"
#include "supported-rates.h"
#include "ssid.h"
#include "edca-parameter-set.h"

namespace ns3
{
/**
 * \ingroup wifi
 * Implement the header for management frames of type probe request.
 */
class LiFiMgtProbeRequestHeader : public Header
{
public:
  ~LiFiMgtProbeRequestHeader ();

  /**
   * Set the Service Set Identifier (SSID).
   *
   * \param ssid SSID
   */
  void SetSsid (Ssid ssid);
 
  /**
   * Return the Service Set Identifier (SSID).
   *
   * \return SSID
   */
  Ssid GetSsid (void) const;
  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  TypeId GetInstanceTypeId (void) const;
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);


private:
  Ssid m_ssid;                     //!< Service Set ID (SSID)
  SupportedRates m_rates;          //!< List of supported rates
};


/**
 * \ingroup wifi
 * Implement the header for management frames of type probe response.
 */
class LiFiMgtProbeResponseHeader : public Header
{
public:
  LiFiMgtProbeResponseHeader ();
  ~LiFiMgtProbeResponseHeader ();

  /**
   * Return the Service Set Identifier (SSID).
   *
   * \return SSID
   */
  Ssid GetSsid (void) const;
  /**
   * Return the beacon interval in microseconds unit.
   *
   * \return beacon interval in microseconds unit
   */
  uint64_t GetBeaconIntervalUs (void) const;

  /**
   * Return the Capability information.
   *
   * \return Capability information
   */
 // CapabilityInformation GetCapabilities (void) const;
 /**
   * Set the EDCA Parameter Set.
   *
   * \param edcaParameterSet the EDCA Parameter Set
   */
 // void SetEdcaParameterSet (EdcaParameterSet edcaParameterSet);
  /**
   * Return the EDCA Parameter Set.
   *
   * \return the EDCA Parameter Set
   */
  //EdcaParameterSet GetEdcaParameterSet (void) const;
 
  /**
   * Set the Capability information.
   *
   * \param capabilities Capability information
   */
 // void SetCapabilities (CapabilityInformation capabilities);
  
  /**
   * Set the Service Set Identifier (SSID).
   *
   * \param ssid SSID
   */
  void SetSsid (Ssid ssid);
  /**
   * Set the beacon interval in microseconds unit.
   *
   * \param us beacon interval in microseconds unit
   */
  void SetBeaconIntervalUs (uint64_t us);
  /**
   * Return the time stamp.
   *
   * \return time stamp
   */
  uint64_t GetTimestamp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  TypeId GetInstanceTypeId (void) const;
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);


private:
  uint64_t m_timestamp;                //!< Timestamp
  Ssid m_ssid;                         //!< Service set ID (SSID)
  uint64_t m_beaconInterval;           //!< Beacon interval
  CapabilityInformation m_capability;  //!< Capability information
  EdcaParameterSet m_edcaParameterSet; //!< EDCA Parameter Set
};


/**
 * \ingroup wifi
 * Implement the header for management frames of type beacon.
 */
class LiFiMgtBeaconHeader : public LiFiMgtProbeResponseHeader
{
public:
  /** Register this type. */
  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
};
}
#endif

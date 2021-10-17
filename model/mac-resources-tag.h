#ifndef MAC_RESOURCES_TAG_H
#define MAC_RESOURCES_TAG_H

#include "ns3/object.h"
#include <string>
#include "ns3/address.h"
#include <iostream>
#include "ns3/log.h"
#include "ns3/tag.h"
#include "ns3/mac48-address.h"

namespace ns3 
{
class MacResourcesTag : public Tag	// This class is a subclass of Object class
{
public:
	// Public methods
   	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;  
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
	MacResourcesTag();
	~MacResourcesTag();
     // virtual void Print (std::ostream &os) const;  
	void SetDeviceID (uint32_t);
	uint32_t GetDeviceID ();  
	void Print (std::ostream &os) const;
	void  SetTotalSlots (uint16_t*);
	uint16_t* GetTotalSlots (void);
	uint16_t* GetDownlinkSlots (void);
	uint16_t* GetUplinkSlots (void);  
	void SetDownlinkSlots (uint16_t* slots);
	void SetUplinkSlots (uint16_t* slots);
	void SetDestinationAddress(Mac48Address);
	Mac48Address GetDestinationAddress(void);

	void SetTotalDownlinkSlots(uint16_t);
	void SetTotalUplinklinkSlots(uint16_t);
	uint16_t GetTotalDownlinkSlots(void);
	uint16_t GetTotalUplinklinkSlots(void);

private:
	uint32_t m_deviceId;
	uint16_t totalDownlinkSlots;
	uint16_t totalUplinkSlots;
	uint16_t* slotsArray;
	uint16_t* downlinkSlotsArray;
	uint16_t* uplinkSlotsArray;
	Mac48Address m_destinationAddress;
};

} // namespace ns3
#endif
 
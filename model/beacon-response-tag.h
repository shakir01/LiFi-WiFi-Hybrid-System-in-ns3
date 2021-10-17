#ifndef BEACON_RESPONSE_TAG_H
#define BEACON_RESPONSE_TAG_H
#include "ns3/object.h"
#include <string>
#include "ns3/address.h"
#include <iostream>
#include "ns3/log.h"
 #include "ns3/tag.h"
 #include "ns3/commons.h"
namespace ns3 {
class BeaconResponseTag: public Tag	// This class is a subclass of Object class
{
public:
	  // Public methods
   	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	 BeaconResponseTag();
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
    // virtual void Print (std::ostream &os) const;
	virtual void SetDeviceID (uint32_t);
	virtual uint32_t GetDeviceID ();
    virtual void SetAccessCateogy(uint8_t);
    virtual uint8_t GetAccessCateogy(void);
	virtual void SetSnrLevel (double);
	virtual double GetSnrLevel ();
	virtual void SetError (double);
	virtual double GetError ();
	virtual void SetPktDrop (double);
	virtual double GetPktDrop ();
 	virtual void Print (std::ostream &os) const;
	virtual void SetTrafficType(uint8_t);
	virtual uint8_t GetTrafficType(void);
private:
    uint32_t m_deviceId;
    double m_snrLevel;
	uint8_t m_accessCategory;
	uint8_t m_trafficDirectionality;
	double m_error;
	uint32_t m_pkt_drop;
};

} 
#endif 
 
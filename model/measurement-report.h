#ifndef MEASUREMENT_REPORTS_H
#define MEASUREMENT_REPORTS_H
#include "ns3/tag.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include <iostream>
namespace ns3
{

class MeasurementReport : public Tag	// This class is a subclass of Object class
{
public:
	// Public methods
   	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;  
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
	MeasurementReport();
	~MeasurementReport();
     // virtual void Print (std::ostream &os) const;  
	void SetDeviceID (uint32_t);
	uint32_t GetDeviceID ();  
	void Print (std::ostream &os) const;
	void  SetSNR (std::vector<double> );
	std::vector<double>  GetSNR (void);
	void SetErrorRate (std::vector<double>);
	std::vector<double> GetErrorRate (void);
    void SetDroppedPackets (std::vector<uint32_t>);
	std::vector<uint32_t> GetDroppedPackets (void);
	
private:
	uint32_t m_device_id;
	uint16_t m_no_measurements;
	std::vector<double> m_snr_vector;
	std::vector<double> m_error_vector;
	std::vector<uint32_t> m_dropped_vector;
};
}
#endif

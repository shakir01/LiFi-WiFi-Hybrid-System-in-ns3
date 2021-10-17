#include "measurement-report.h"

   
namespace ns3 {
 TypeId 
 MeasurementReport::GetTypeId (void)
 {
   static TypeId tid = TypeId ("ns3::MeasurementReport")
     .SetParent<Tag> ()
     .AddConstructor<MeasurementReport> ()
   ;
   return tid;
 }

MeasurementReport::MeasurementReport()
 {
    m_total_devices = 0;
 }

 MeasurementReport::~MeasurementReport()
{
//delete downlinkSlotsArray;
//	delete uplinkSlotsArray;
}

TypeId 
 MeasurementReport::GetInstanceTypeId (void) const
 {
   return GetTypeId ();
 }
 uint32_t 
 MeasurementReport::GetSerializedSize (void) const
 {
   return sizeof(uint16_t)+sizeof(uint32_t)*m_total_devices + sizeof(double)*m_total_devices + sizeof(double)*m_total_devices + sizeof(uint32_t)*m_total_devices;
 }

 void MeasurementReport::Serialize (TagBuffer i) const
 {
  i.WriteU16 (m_total_devices);
  if (m_total_devices > 0)
  {    
      for (size_t ii = 0; ii < m_total_devices; ii++)
      {
        i.WriteU32 (m_ids_vector[ii]);
       
      }

      for (size_t ii = 0; ii < m_total_devices; ii++)
      {
        i.WriteDouble (m_snr_vector[ii]);
       
      }

      for (size_t ii = 0; ii < m_total_devices; ii++)
      {
        i.WriteDouble (m_error_vector[ii]);
       
      }

      for (size_t ii = 0; ii < m_total_devices; ii++)
      {
        i.WriteU32 (m_dropped_vector[ii]);
      }     
  }
  //delete slotsArray;
  
 }

 void MeasurementReport::Deserialize (TagBuffer i)
 {
  m_total_devices        = i.ReadU32 ();
  if (m_total_devices > 0)
  {    
      for (size_t ii = 0; ii < m_total_devices; ii++)
      {
        m_ids_vector[ii] = i.ReadU32 ();
      }

       for (size_t ii = 0; ii < m_total_devices; ii++)
      {
        m_snr_vector[ii] = i.ReadDouble ();
      }

       for (size_t ii = 0; ii < m_total_devices; ii++)
      {
        m_error_vector[ii] = i.ReadDouble ();
      }

       for (size_t ii = 0; ii < m_total_devices; ii++)
      {
        m_dropped_vector[ii] = i.ReadU32 ();
      }
  }

 }

 void 
 MeasurementReport::Print (std::ostream &os) const
 {
  // os << "v=" << (uint32_t)m_simpleValue;
 }
 
  // virtual void Print (std::ostream &os) const;  
	void MeasurementReport::SetDeviceID (std::vector<uint32_t> idsVect)
  {
    m_ids_vector = idsVect;
  }

	std::vector<uint32_t> MeasurementReport::GetDeviceID ()
  {
    return m_ids_vector;
  } 

	void MeasurementReport::SetSNR (std::vector<double> snrV)
  {
    m_snr_vector = snrV;
  }

	std::vector<double>  MeasurementReport::GetSNR (void)
  {
    return m_snr_vector;
  }

	void MeasurementReport::SetErrorRate (std::vector<double> errorV)
  {
    m_error_vector = errorV;
  }

	std::vector<double> MeasurementReport::GetErrorRate (void)
  {
    return m_error_vector;
  }

  void MeasurementReport::SetDroppedPackets (std::vector<uint32_t> droppedPacketV)
  {
    m_dropped_vector = droppedPacketV;
  }

	std::vector<uint32_t> MeasurementReport::GetDroppedPackets (void)
  {
    return m_dropped_vector;
  }

}


#ifndef LIFI_PHY_H
#define LIFI_PHY_H
#include "ns3/lifi-mobility-model.h"
#include "ns3/pointer.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/object.h"
#include "ns3/type-id.h"
#include <cstring>
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/simple-net-device.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/ptr.h"
#include "ns3/mac48-address.h"
#include "ns3/error-model.h"
#include "ns3/queue.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/llc-snap-header.h"
#include "ns3/wifi-mac-trailer.h"
#include "ns3/lifi-snr.h"
#include "ns3/lifi-error-model.h"
#include "ns3/lifi-propagation-loss-model.h"
#include "commons.h"
namespace ns3 {

class LiFiChannel;
class TdmaController;
class LiFiMacLow;
class LiFiSnr;
class LiFiNetDevice;


class LiFiPhy : public Object
{
public:
  static TypeId GetTypeId (void);

  LiFiPhy ();
  virtual ~LiFiPhy ();
  /**
   * \param channel The channel this device is attached to
   */
  void SetChannel (Ptr<LiFiChannel> channel);

  //returns the semiangle of the TX device
	double GetSemiangle();

	//sets the semianle of the TX device
	void SetSemiangle(double angle);

  //sets the angle of radiance of the TX device
	void SetAngleOfRadiance(double angle);

	//returns the angle of radiance of the TX device
	double GetAngleOfRadiance();

	//returns the lambertian order of the TX device
	double GetLambertianOrder();

	//computes and then sets the lambertian order of the TX device
	void SetLambertainOrder();

	//returns the gain of the TX device
	double GetTXGain();

	//computes and sets the gain of the TX device
	void SetTXGain();

    void SetMobilityModel();
	//sets the filter gain of the RX device
	double GetFilterGain();

	//returns the filter gain of the RX device
	void SetFilterGain(double fgain);
	//returns the photodetector area of the RX device
	double GetPhotoDetectorArea();
	//sets the photo detector area of the RX device
	void SetPhotoDectectorArea(double pArea);
	//returns the FOV angle of the RX device
	double GetFOVAngle();
	//sets the FOV angle of the RX device
	void SetFOVAngle(double angle);
	//returns the refractive index of the RX device
	double GetRefractiveIndex();
	//sets the refractive index of the RX device
	void SetRefractiveIndex(double angle);
	//returns the concentration gain of the RX device
	double GetConcentrationGain();
	//sets the concentration gain of the RX device
	void SetConcentrationGain();
	void SetErrorRateModel(Ptr<LiFiErrorModel> errorModel);
	//returns the total gain of the RX device
	double GetRXGain();
	//sets the total gain of the RX device using internal parameters
	void SetRXGain();
	void SetLossModel(Ptr<LiFiPropagationLossModel>);
	void SetMacLow(Ptr<LiFiMacLow>);
	Ptr<LiFiMacLow> GetMacLow();
	double CalculateErrorRateForErrorModel(); 
	//sets the incidence angle of the RX device
	void SetIncidenceAngle(double angle);
	//returns the concentration gain of the RX device
	double GetIncidenceAngle();
  	void SetDevice(Ptr<LiFiNetDevice>);
  	Ptr<LiFiNetDevice> GetDevice() const;
  	void SetSNR(Ptr<LiFiSnr>);
 	 Ptr<LiFiSnr> GetSNR(void) const;
  	// methods for PAM
	int GetModulationOrder(void) const;	// returns the modulation order (M)
	void SetModulationOrder(int m_order);	// sets the modulation order value
   	Ptr<Channel> GetChannel (void) const;
  	void Send (Ptr<const Packet> packet, Ptr<LiFiNetDevice>);
  	ns3::Ptr<ns3::LiFiErrorModel> GetErrorModel();
	void Receive (Ptr<Packet> packet, Ptr<MobilityModel> txMobility);
	void SetDeviceType(std::string);
	std::string GetDeviceType(void);
	void ReportMeasurements();
	std::vector <ChannelStats >  GetChannelData();
	typedef void (* MonitorRxDataCallback)(Ptr<LiFiPhy> channel, uint32_t apId,std::vector <ChannelStats>  maps);

private:
std::vector<double> m_RXOpticalPower;
	std::vector<double> m_signal;
	ns3::Ptr<ns3::LiFiErrorModel> m_error;
	Ptr<LiFiMacLow> m_mac;
	Ptr<LiFiChannel> m_channel;
	Ptr<LiFiNetDevice> m_lifiNetDevice;
	Ptr<LiFiSnr> m_LifiSNR;
	double m_semiangle;
	double m_lOrder;
	double m_photodetectorArea;
	double m_FOVangle;
	double m_refIndex;
	double m_concentrationGain;
	double m_RXGain;
	double m_filterGain;
	Ptr<MobilityModel> m_mobilityModel;
	Ptr<LiFiNetDevice> m_LifiNetDevice;
	Ptr<LiFiPhy> m_PhyList;
	Ptr<LiFiPropagationLossModel> m_propgationLossModel;
	std::string m_deviceType;
	ChannelStats m_lastMeasurementReport;
	TracedCallback<Ptr<LiFiPhy>, uint32_t,std::vector <ChannelStats> > m_monitorRxDataCallback;
    std::vector <ChannelStats >  channelStatsVector;
	bool  startStatsCollection;
	  
};//end class 

}//end namespace
#endif /* TDMA_NET_DEVICE_H */

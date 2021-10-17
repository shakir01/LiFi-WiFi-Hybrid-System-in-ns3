#ifndef LIFI_TX_NET_DEVICET_H
#define LIFI_TX_NET_DEVICET_H

#include "ns3/core-module.h"
#include "ns3/lifi-net-device.h"
#include "ns3/ptr.h"
#include "ns3/lifi-channel.h"
#include <cmath>
////////////////
#include <cstring>
#include "ns3/address.h"
#include "ns3/node.h"
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
#include <algorithm>
#include "lifi-user-device-infos.h"

namespace ns3 {

class LiFiChannel;

class LiFiTxNetDevice: public LiFiNetDevice 
{

public:
	LiFiTxNetDevice();

	static TypeId GetTypeId(void);

	virtual ~LiFiTxNetDevice();

	//adds a signal instant to the TX optical power signal
	void AddTXOpticalPowerSignal(double power);

	//returns the TX Optical power signal in the form of std::vector of doubles
	std::vector<double>& GetTXOpticalPowerSignal();

	//returns the TX power signal at instant time, time has to be lesser than the capacity of std::vector of signal
	double GetOpticalPowerSignalAtInstant(int time);

	//sets the TX power signal from a std::vector
	void SetTXOpticalPowerSignal(std::vector<double> &powerSignal);

	//reserves the capacity of signal vector
	void SetCapacity(int size);

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

	//adds a signal component to the signal vector
	void AddSignal(double signal);

	//returns the signal vector of the TX device
	std::vector<double>& GetSignal();

	//returns the magnitude of the signal at a particular instant
	double GetSignalAtInstant(int time);

	//sets the signal vector of the TX device
	void SetSignal(std::vector<double> &signal);

	//sets the bias voltage of the TX device
	void SetBias(double bias);

	//returns the bias voltage of the TX device
	double GetBias();

	//calculates the optical power signal after biasing it by m_bias
	void BoostSignal();

	//returns the Maximum TX power that can be transmitted
	double GetTXPowerMax();

	//returns the average of TX Power signal
	double GetAveragePowerSignalPower();

	//returns the average of the TX signal
	double GetAverageSignal();

	double GetAveragePower();
	void SetAveragePower(double val);

	void EnqueueDataPacket(Ptr<Packet> p);

    bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
	bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
	//virtual bool SupportsSendFrom (void) const;
    void AddReceiverModel (Ptr<LiFiUserDeviceInfos> recieverModel);
	/**
	   * \param controller The tdma controller this device is attached to
	*/
  	/*void SetTdmaController (Ptr<TdmaController> controller);
  	Ptr<TdmaController>  GetTdmaController (void) const;

	virtual	void SetChannel (Ptr<LiFiChannel> channel);
	virtual	Ptr<Channel> GetChannel (void) const;
	virtual	Ptr<LiFiChannel> DoGetChannel (void) const;

	virtual	void SetAddress (Address address);
	virtual Address GetAddress (void) const;

  	virtual void DoDispose (void);
  	virtual void DoInitialize (void);*/

  	//virtual void CompleteConfig (void);
     //   virtual uint32_t GetIfIndex (void) const;

 	/**
   	    * \param mac the mac layer to use.
   	*/
  	//void SetMac (Ptr<TdmaMac> mac);
  	/**
   	   * \returns the mac we are currently using.
   	*/
       /* Ptr<TdmaMac> GetMac (void) const;

   	void SetQueueStateChangeCallback (Callback<void,uint32_t> callback);
	uint32_t GetQueueState (uint32_t index);
	uint32_t GetNQueues (void);
        void AddReceiverModel (Ptr<LiFiUserDeviceInfos> recieverModel);
        Ptr<LiFiUserDeviceInfos> SearchForReceiverModel (Address address);*/

	void RemoveRxDevice (uint32_t index);

private:
	//bool TxQueueStart (uint32_t index);
	//bool TxQueueStop (uint32_t index);
	std::vector<double> m_TXOpticalPower;
	std::vector<double> m_signal; 
	const double m_TMAX;
	double m_semiangle;
	double m_angleOfRadiance;
	double m_lOrder;
	double m_TXGain;
	double m_bias;
  	/*TracedCallback<uint32_t> m_queueStateChanges;
  	Ptr<TdmaController> m_tdmaController;
  	Ptr<TdmaMac> m_mac;

  	Ptr<LiFiChannel> m_channel;
        
	bool m_linkUp;
  	bool m_configComplete;
        */
};

} /* namespace vlc */

#endif /* VLCNETDEVICETX_H_ */

#ifndef LiFi_ERROR_MODEL_H
#define LiFi_ERROR_MODEL_H

#include <list>
#include "ns3/error-model.h"
#include "ns3/random-variable-stream.h"
#include <cmath>
#include <math.h>
#include <stdexcept>

namespace ns3 {

class Packet;

class LiFiErrorModel: public RateErrorModel// This class is a subclass of RateErrorModel class
{
public:
	// Public methods
	static TypeId GetTypeId(void); // returns meta-information about VlcErrorModel class 

	LiFiErrorModel();	// constructor
	virtual ~LiFiErrorModel();	//destructor

	enum ModScheme	// enumeration for the two modulation schemes
	{
		PAM4, OOK, VPPM , PSK4, PSK16, QAM4 , QAM16,PAM8,PAM16
	};

	LiFiErrorModel::ModScheme GetScheme(void) const;	// returns the modulation scheme used

	void SetScheme(ModScheme scheme);// sets the value of the modulation scheme used

	void SetRandomVariable(Ptr<RandomVariableStream> ranVar);// assigns a random variable stream to be used by this model

	int64_t AssignStreams(int64_t stream);// assigns a fixed stream number to the random variables used by this model

	double GetSNR(void) const;		// returns the signal-to-noise ratio (SNR)

	void SetSNR(double snr);		// sets the SNR value

	double CalculateErrorRate(void);// calculates the error rate value according to modulation scheme

	bool IsCorrupt(Ptr<Packet> pkt);// determines if the packet is corrupted according to the error model

	// methods for PAM
	int GetModulationOrder(void) const;	// returns the modulation order (M)
	void SetModulationOrder(int m_order);	// sets the modulation order value

	// methods for VPPM
	double GetAlpha(void) const;	// returns alpha value
	void SetAlpha(double a);		// sets alpha value

	double GetBeta(void) const;		// returns beta value
	void SetBeta(double b);		// sets beta value

	double ComputeBER();

	bool CorruptPacket(Ptr<Packet> p, double ber);

	//static function defination
	double GetAveragePower(double pmax,double pmin,double alpha);

private:
	// Private methods
	virtual void DoReset(void);	// overrides DoReset method from RateErrorModel class
	double CalculateSER(void);	// calculates the SER value (for OOK and PAM)
	double CalculateBER(void);		// calculates the BER value (for VPPM)
	double GenerateQfunction(double a);

	// Private class members
	int mod_order;	// the modulation order (M) of PAM scheme
	double alpha;	// the duty cycle of the VPPM signal
	double beta;// a factor relating noise bandwidth to signal bandwidth in VPPM
	double SNR;		// signal-to-noise ratio

	Ptr<RandomVariableStream> m_ranvar;		// random variable stream
	LiFiErrorModel::ModScheme mod_scheme;	// the modulation scheme used
};

} // namespace ns3
#endif

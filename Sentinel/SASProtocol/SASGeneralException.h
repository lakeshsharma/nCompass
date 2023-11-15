#pragma once
#include "SASProtocol/SASGeneralPoll.h"


class CSASGeneralException
{
public:

	/// <summary>
	/// Default description for unrecognized GPE
	/// </summary>
	static const std::string UnknownGPE;

	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CSASGeneralException"/> class.
	/// </summary>
	CSASGeneralException();

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CSASGeneralException"/> class.
	/// </summary>
	~CSASGeneralException();

	/// <summary>
	/// Gets the description for the general poll exception if the description is found, returns an empty string otherwise.
	/// </summary>
	/// <param name="gpe">IN - The general poll exception.</param>
	/// <returns>The description for the general poll exception if the description is found, returns an empty string otherwise.</returns>
	std::string GetDescriptionFor(GeneralPollException gpe) const;

private:
	/// <summary>
	/// STL collection for bool configuration name-value pairs
	/// </summary>
	typedef std::map<GeneralPollException, std::string> GPEDescriptionMap;

	typedef std::pair<GeneralPollException, std::string> GPEKeyPair;


	GPEDescriptionMap m_eventDescMap;

	void LoadGPEDescriptions();

};


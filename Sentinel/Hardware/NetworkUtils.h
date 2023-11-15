#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "Connection.h"
#include "Logging/LogString.h"
#include "DirectoryManager.h"

using namespace std;
class NetworkUtils
{
    public:
        /// <summary> 
        /// Gets a reference to the global instance of NetworkUtils.
		/// </summary>
        /// <returns> 
        /// A reference to the global NetworkUtils instance. 
		/// </returns>
		static NetworkUtils &Instance()
		{
			static NetworkUtils m_instance;
			return m_instance;
		}

		/// <summary> 
		/// Checks if network cable is changed and restart the network service if 
		///ethernet cable is changed
		/// </summary>
		/// <returns> 
		/// Ture if network service restarted 
		/// </returns> 
         bool NetWorkSvcRestartIfNetworkCableChanged(void);

		 /// <summary> 
		 /// Gets the network cable connectivity toggle state
		 /// </summary>
		 /// <returns> 
		 /// True if network cable goes from connected->disconnected state
		 /// </returns> 
         bool GetNetworkCableToggleState();

		 /// <summary> 
		 /// Get network service restarted state
		 /// </summary>
		 /// <returns> 
		 ///True if network service restarted
		 /// </returns> 
         bool GetNetworkServiceRestarted();

		 /// <summary> 
		 /// Get network cable unplugged state
		 /// </summary>
		 /// <returns> 
		 /// True if network cable is unplugged
		 /// </returns> 
         bool GetNetworkCableUnPlugged();

		 /// <summary> 
		 /// Set network cable unplugged state
		 /// </summary>
		 ///<param name="NetworkCableUnPlugged"> True if network cable is unplugged </param>
		 void SetNetworkCableUnPlugged(bool NetworkCableUnPlugged);

		 /// <summary> 
		 /// Sets the network cable connectivity toggle state
		 /// </summary>
		 ///<param name="NetworkCableToggle">True if network cable goes from connected->disconnected state</param>
		 void SetNetworkCableToggleState(bool NetworkCableToggle);

		 /// <summary> 
		 /// Set network service restarted state
		 /// </summary>
		 ///<param name="NetworkCableToggle">True if network service restarted</param>
		 void SetNetworkServiceRestarted(bool NetworkServiceRestarted);

		 /// <summary> 
		 /// Get the command for network cable state
		 /// </summary>
		 /// <returns> 
		 /// The command for getting the state of network cable 
		 /// </returns> 
		 virtual string GetCmdForNetworkCableState(void);

	//To derive the mocked classes constructor and destrucyot are made protected
		NetworkUtils(void);
		virtual ~NetworkUtils() {}
		NetworkUtils(NetworkUtils const&) = default;
		NetworkUtils& operator=(NetworkUtils const&) = default;

    private:

		static std::recursive_mutex m_criticalSection;
    	bool m_bNetworkCableUnPlugged;
    	bool m_bNetworkCableToggle;
    	bool m_bNetworkServiceRestarted;

		friend class NetworkUtilsTestsMock;
};

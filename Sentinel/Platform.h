#pragma once

#include "DirectoryManager.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class Platform; }
namespace SentinelNativeUnitTests { class DeviceInfoSet; }



	class CPlatform
	{
		// Grant "friend" access for testing.
		friend class SentinelUnitTests::Platform;
		friend class SentinelNativeUnitTests::DeviceInfoSet;

	public:
		static CPlatform & Instance();

		static void CleanupInstance();

		static CPlatform & MockInstance(LPCTSTR osVersion, UINT platformID, LPCTSTR platformName, LPCTSTR upgradeSMDFilename);

		const std::string &GetOSImageVersionNumber()
		{
			return m_osImageVersionNumber;
		}


		void GetOSImageVersionNumberChars(char *outChars, int length)
		{
			CUtilities::ConvertCharsToChars(GetOSImageVersionNumber(), outChars, length);
		}

		UINT GetPlatformID() const
		{
			return m_platformID;
		}

		const std::string& GetPlatformName();

		bool IsLiteDevice() const;
		bool IsNexusDevice() const;

		/// <summary>
		/// Returns whether or not the hardware platform uses the OS upgrade flag.
		/// </summary>
		/// <returns>true if the hardware platform uses the OS upgrade flag, false otherwise.</returns>
		bool PlatformUsesOSUpgradeFlag() const;

		const std::string &GetUpgradeSMDfilename()
		{
			return m_upgradeSMDfilename;
		}

		/// <summary>
		/// Gets the Touchscreen type
		/// </summary>
		///// <param name="ethernetType">OUT - ethernet type.</param>
		///// <param name="mcuType">OUT - MCU type.</param>
		///// <param name="touchscreenType">OUT - touchscreen type.</param>
		void GetHardwareComponents(std::string& ethernetType, std::string& mcuType, std::string& touchscreenType);

		/// <summary>
		/// Checks if touch screen works with the current OS
		/// </summary>
		bool CheckIfTouchScreenWorksWithTheCurrentOS();

		void WaitForNeededServices();
		long GetCPUTemperature();

		static const UINT AXIOMTEK_PLATFORMID;
		static const UINT BRAEMAC_BLUE_PLATFORM_ID;

		std::string GetResolution();
		bool GetLandscape();
		bool GetTouchScreen();
		std::string GetProcessorType();
		std::string GetNumberOfCores();
		std::string GetBiosVersion();
		std::string GetTotalMemoryInKB();
		std::string GetBeaconCapability();
		std::string GetBeaconFirmware();
		std::string GetCarrierFirmwareVersion();
		std::string GetVideoMixerFirmwareVersion();

	private:
		static const UINT SERVICE_WAIT_TIME = 30;

		// The singleton instance
		static CPlatform *m_instance;
		static std::recursive_mutex m_instanceCS;

		CPlatform();
		CPlatform(LPCTSTR osVersion, UINT platformID, LPCTSTR platformName, LPCTSTR upgradeSMDFilename);
		~CPlatform();

		void GetOSImageInformation();

		CDirectoryManager& m_directoryManager;

		std::string m_osImageVersionNumber;

		//Platform ID
		UINT m_platformID;

		//Platform Name
		std::string m_platformName;

		//CPU Temperature
		std::string m_CPUTemp;

		//Upgrade SMD filename
		std::string m_upgradeSMDfilename;

		std::string m_ethernetType;
		std::string m_mcuType;
		std::string m_touchscreenType;
		std::string m_resolution;
		std::string m_biosVersion;
		std::string m_processorType;
		std::string m_totalMemoryInKB;
		std::string m_numberOfCores;
		std::string m_beaconCapability;
		std::string m_beaconFirmware;
		std::string m_carrierFirmwareVersion;
		std::string m_videoMixerFirmwareVersion;
	};


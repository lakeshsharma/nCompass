#pragma once
#include "SASPollMessage.h" 

#define XB7_POLLSIZE 38
#define XB7_RESPONSESIZE 39
const static byte MAX_FLOORLOCATIONSIZE = 28;
#define LP_B7_VALUE  0xB7
#define EGM_SUPPORT_ASSET_NUMBER_UPDATE_BY_HOST  0x01
#define EGM_SUPPORT_FLOOR_LOCATION_UPDATE_BY_HOST  0x02

class CLongPollB7Message :
	public CSASPollMessage
{
public:
	CLongPollB7Message(byte gameAddress);
    
	~CLongPollB7Message(void);
	
	virtual BYTE GetPollValue();
    virtual bool SwapPollValue();
    void ResponseReceived(void);
    void Fill();
    
    void SetAssetNumber(DWORD assetNum);
    DWORD GetAssetNumber();

    void SetFloorLocation(byte *floorLoc, byte floorLocLength);
    byte* GetFloorLocation(byte &length);
	
    bool ResponseHostControlAssetNumber();
    bool ResponseHostControlFloorLocation();
	
private:
	byte m_length;
	DWORD m_assetNumber;
	byte m_floorLocationLength;
	byte m_floorLocation[MAX_FLOORLOCATIONSIZE];
	byte m_controlFlag;

};



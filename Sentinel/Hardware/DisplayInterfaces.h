#pragma once

#include "VideoMode.h"

class IDisplay
{
public:
	virtual const CVideoMode& GetMatchingVideoMode() const = 0;
	virtual bool GetResolution(SIZE& resolution) const = 0;
	virtual bool Updated() const = 0;
	virtual bool Update() = 0;
};

class IVideoDisplay : public IDisplay
{
public:
	virtual byte GetVideoModeId() const = 0;
	virtual bool SetVideoModeId(byte modeId) = 0;
	virtual bool GetAnalogOffsets(POINT& offsets) const = 0;
	virtual bool SetAnalogOffsets(POINT offsets) = 0;
};

class IDesktopDisplay : public IDisplay
{
public:
	virtual bool GetOrientation(DesktopOrientationType& orientation) const = 0;
	virtual bool GetPosition(POINT& position) const = 0;
	virtual bool SetDesktop(const CVideoTiming* timing, DesktopOrientationType orientation, const POINT* position) = 0;
};

#include "stdafx.h"
#include "ProgressiveWinnerPick.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"

using namespace std;
using namespace rapidjson;

CProgressiveWinnerPick::CProgressiveWinnerPick(DWORD receivedTickCount, const Document& doc)
	: m_levelId(JsonGetIntEx(doc, pKeyLevelId))
	, m_levelName(JsonGetCStringEx(doc, pKeyLevelName))
	, m_receivedTickCount(receivedTickCount)
{
}

#include "stdafx.h"
#include "ProgressiveTotalizerAnnouncement.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "Util/Currency.h"

using namespace rapidjson;


CProgressiveTotalizerAnnouncement::CProgressiveTotalizerAnnouncement(const Document& doc) :
	m_Id(JsonGetInt64Ex(doc, pKeyId)),
	m_Name(JsonGetStringEx(doc, pKeyName)),
	m_MetricName(JsonGetStringEx(doc, pKeyMetricName)),
	m_MetricValue(Currency::ToString(JsonGetDoubleEx(doc, pKeyMetricValue)))
{
}
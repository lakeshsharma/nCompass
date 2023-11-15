#pragma once
#include "ConfigField.h"
#include "../InternalMsgContent.h"


class CConfigFields :
	public CInternalMsgContent<INT_MSG_CONTENT_CONFIGFIELDS>
{
public:
	CConfigFields(void);
public:
	~CConfigFields(void);

public:
	void AddField(CConfigField* field);
	CConfigField * GetFieldByIndex(int index);
	CConfigField * GetFieldByType(FieldTypes type);

	int GetCount();


private:
	std::vector<CConfigField *> m_fieldsArray;

};


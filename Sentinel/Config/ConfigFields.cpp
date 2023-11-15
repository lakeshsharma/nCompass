#include "stdafx.h"
#include "ConfigFields.h"

CConfigFields::CConfigFields(void)
{
}

CConfigFields::~CConfigFields(void)
{
	for (std::vector<CConfigField *>::iterator it = m_fieldsArray.begin(); it != m_fieldsArray.end(); ++it)
	{
		delete *it;
	}

	m_fieldsArray.clear();
}


void CConfigFields::AddField( CConfigField* field )
{
	
	m_fieldsArray.push_back(field);
}

CConfigField * CConfigFields::GetFieldByIndex(int index)
{
	CConfigField *retVal = nullptr;

	if (index < (int)m_fieldsArray.size())
		retVal = m_fieldsArray[index];

	return retVal;
}

CConfigField * CConfigFields::GetFieldByType(FieldTypes type)
{
	
	CConfigField *retVal = nullptr;
	for (std::vector<CConfigField *>::iterator it = m_fieldsArray.begin(); retVal == nullptr && it != m_fieldsArray.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			retVal = *it;
		}
	}

	return retVal;
}

int CConfigFields::GetCount()
{
    return m_fieldsArray.size();
}


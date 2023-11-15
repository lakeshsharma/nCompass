#include "stdafx.h"
#include "ByteArrayEx.h"

#if 0
void CByteArrayEx::Append(const void *data, size_t dataLength)
{
	INT_PTR index(GetSize());
	SetSize(index + dataLength);
	memcpy(GetData() + index, data, dataLength);
}

void CByteArrayEx::Append(BYTE data)
{
	Append(&data, sizeof(data));
}

void CByteArrayEx::Append(WORD data)
{
	Append(&data, sizeof(data));
}
#endif
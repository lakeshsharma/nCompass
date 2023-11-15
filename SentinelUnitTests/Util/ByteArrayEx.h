#pragma once
class CByteArrayEx 
{
public:
	/// <summary>
	/// Appends the specified data to the array.
	/// </summary>
	/// <param name="data">IN - The data.</param>
	void Append(const void *data, size_t dataLength);

	/// <summary>
	/// Appends the specified data to the array.
	/// </summary>
	/// <param name="data">IN - The data.</param>
	void Append(BYTE data);

	/// <summary>
	/// Appends the specified data to the array, least significant byte first.
	/// </summary>
	/// <param name="data">IN - The data.</param>
	void Append(WORD data);

	/// <summary>
	/// Get the size of the byte array.
	/// </summary>
	uint GetSize();
	
private:
	std::vector<char> data;
};


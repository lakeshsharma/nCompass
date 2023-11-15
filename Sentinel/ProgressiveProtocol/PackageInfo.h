#pragma once
#include "stdafx.h"

class CPackageInfo
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CPackageInfo();

	/// <summary>
	/// Parameterized constructor.
	/// </summary>
	///	<param name="id">package id</param>
	///	<param name="name">package name</param>
	///	<param name="version">package version</param>
	///	<param name="revision">package revision</param>
	///	<param name="mediaCount">package media count</param>
	CPackageInfo(DWORD id, const std::string &name, const std::string &version, DWORD revision, DWORD mediaCount = 0);

	/// <summary>
	/// Overloaded comparison operator.
	/// </summary>
	///	<returns>true if contents matched, else returns false</returns>
	bool operator==(CPackageInfo &info);

	/// <summary>
	/// Gets the package name.
	/// </summary>
	///	<returns>the package name</returns>
	std::string GetName() const;

	/// <summary>
	/// Gets the package id.
	/// </summary>
	///	<returns>the package id</returns>
	DWORD GetId() const;

	/// <summary>
	/// Gets the package version.
	/// </summary>
	///	<returns>the package version</returns>
	std::string GetVersion() const;

	/// <summary>
	/// Gets the package revision.
	/// </summary>
	///	<returns>the package revision</returns>
	DWORD GetRevision() const;

	/// <summary>
	/// Gets the package pending media count to download.
	/// </summary>
	///	<returns>the package pending media count</returns>
	DWORD GetPendingMediaCount() const;

	/// <summary>
	/// Decrements the pending media count of package.
	/// </summary>
	void DecrementPendingMediaCount();

private:
	std::string m_name;
	std::string m_version;
	DWORD m_id;
	DWORD m_revision;
	DWORD m_pendingMediaCount;
};


#include "stdafx.h"
#include "PersonalBankerAutoUpload.h"
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{
		/// <summary>
		/// Test the implementation of the default constructor:
		///     CPersonalBankerAutoUpload::CPersonalBankerAutoUpload(void).
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, PersonalBankerAutoUploadDefaultTest)
		{
			// Test data.
			const bool autoInProcessExpected = false;
			const bool autoNcepOutPendingExpected = false;
			const bool autoStartedOnceExpected = false;
			const bool startUploadOverExpected = false;
			const CCard * const cardExpected = NULL;
			const bool uploadTimerExpiredExpected = true;
			const DWORD pendingAutoNcepAmountExpected = 0;

			// Setup.
			const CPersonalBankerAutoUpload pbau;
			CPersonalBankerAutoUpload varPbau(pbau);

			// Test #1.
			const bool autoInProcessResult = pbau.IsAutoInProcess();

			{
				// Report #1.
				bool areEqual = autoInProcessResult == autoInProcessExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoInProcessResult=%d expected=%d in %s"),
					(int)autoInProcessResult,
					(int)autoInProcessExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
                ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			const bool autoNcepOutPendingResult = pbau.IsAutoUploadPending();

			{
				// Report #2.
				bool areEqual = autoNcepOutPendingResult == autoNcepOutPendingExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
					(int)autoNcepOutPendingResult,
					(int)autoNcepOutPendingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			const bool autoStartedOnceResult = pbau.IsAutoStartedOnce();

			{
				// Report #3.
				bool areEqual = autoStartedOnceResult == autoStartedOnceExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoStartedOnceResult=%d expected=%d in %s"),
					(int)autoStartedOnceResult,
					(int)autoStartedOnceExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			const bool startUploadOverResult = pbau.IsStartUploadOver();

			{
				// Report #4.
				bool areEqual = startUploadOverResult == startUploadOverExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED startUploadOverResult=%d expected=%d in %s"),
					(int)startUploadOverResult,
					(int)startUploadOverExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			CCard const * const cardResult = varPbau.GetCard();

			{
				// Report #5.
				bool areEqual = cardResult == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=%p in %s"),
					(void *)cardResult,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(CUtilities::GetTickCount());
			// No requirements for GetUploadTime().
			ASSERT_TRUE(true) << _T("uploadTimeResult unexpected");
			{
				// Report #6.
				bool areEqual = uploadTimerExpiredResult == uploadTimerExpiredExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d in %s"),
					(int)uploadTimerExpiredResult,
					(int)uploadTimerExpiredExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			const DWORD pendingAutoNcepAmountResult = pbau.GetPendingAutoNcepAmount();

			{
				// Report #7.
				bool areEqual = pendingAutoNcepAmountResult == pendingAutoNcepAmountExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pendingAutoNcepAmountResult=%u expected=%u in %s"),
					(int)pendingAutoNcepAmountResult,
					(int)pendingAutoNcepAmountExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of the default constructor and copy constructor:
		///     CPersonalBankerAutoUpload::CPersonalBankerAutoUpload(void).
		///     CPersonalBankerAutoUpload::CPersonalBankerAutoUpload(const CPersonalBankerAutoUpload &).
		/// Copy constructor should only be used if m_pbtOrigCard is NULL.
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, PersonalBankerAutoUploadDefaultCopyTest)
		{
			// Test data.
			const bool autoInProcessExpected = false;
			const bool autoNcepOutPendingExpected = false;
			const bool autoStartedOnceExpected = false;
			const bool startUploadOverExpected = false;
			const CCard * const cardExpected = NULL;
			const bool uploadTimerExpiredExpected = true;
			const DWORD pendingAutoNcepAmountExpected = 0;

			// Setup.
			const CPersonalBankerAutoUpload pbau1;
			const CPersonalBankerAutoUpload pbau(pbau1);
			CPersonalBankerAutoUpload varPbau(pbau);

			// Test #1.
			const bool autoInProcessResult = pbau.IsAutoInProcess();

			{
				// Report #1.
				bool areEqual = autoInProcessResult == autoInProcessExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoInProcessResult=%d expected=%d in %s"),
					(int)autoInProcessResult,
					(int)autoInProcessExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			const bool autoNcepOutPendingResult = pbau.IsAutoUploadPending();

			{
				// Report #2.
				bool areEqual = autoNcepOutPendingResult == autoNcepOutPendingExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
					(int)autoNcepOutPendingResult,
					(int)autoNcepOutPendingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			const bool autoStartedOnceResult = pbau.IsAutoStartedOnce();

			{
				// Report #3.
				bool areEqual = autoStartedOnceResult == autoStartedOnceExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoStartedOnceResult=%d expected=%d in %s"),
					(int)autoStartedOnceResult,
					(int)autoStartedOnceExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			const bool startUploadOverResult = pbau.IsStartUploadOver();

			{
				// Report #4.
				bool areEqual = startUploadOverResult == startUploadOverExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED startUploadOverResult=%d expected=%d in %s"),
					(int)startUploadOverResult,
					(int)startUploadOverExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			CCard const * const cardResult = varPbau.GetCard();

			{
				// Report #5.
				bool areEqual = cardResult == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=%p in %s"),
					(void *)cardResult,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(CUtilities::GetTickCount());
			// No requirements for GetUploadTime().
			ASSERT_TRUE(true) << _T("uploadTimeResult unexpected");
			{
				// Report #6.
				bool areEqual = uploadTimerExpiredResult == uploadTimerExpiredExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d in %s"),
					(int)uploadTimerExpiredResult,
					(int)uploadTimerExpiredExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			const DWORD pendingAutoNcepAmountResult = pbau.GetPendingAutoNcepAmount();

			{
				// Report #7.
				bool areEqual = pendingAutoNcepAmountResult == pendingAutoNcepAmountExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pendingAutoNcepAmountResult=%u expected=%u in %s"),
					(int)pendingAutoNcepAmountResult,
					(int)pendingAutoNcepAmountExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of the default constructor and copy constructor:
		///     CPersonalBankerAutoUpload::CPersonalBankerAutoUpload(void).
		///     CPersonalBankerAutoUpload::operator=(const CPersonalBankerAutoUpload &)
		/// Assignment operator should only be used if m_pbtOrigCard is NULL.
		/// </summary>
		/// <remarks>
		/// 2013-06-10 The compiler calls copy constructor instead of the assignment operator.
		/// </remarks>
		TEST(PersonalBankerAutoUploadTests, PersonalBankerAutoUploadDefaultAssignTest)
		{
			// Test data.
			const bool autoInProcessExpected = false;
			const bool autoNcepOutPendingExpected = false;
			const bool autoStartedOnceExpected = false;
			const bool startUploadOverExpected = false;
			const CCard * const cardExpected = NULL;
			const bool uploadTimerExpiredExpected = true;
			const DWORD pendingAutoNcepAmountExpected = 0;

			// Setup.
			const CPersonalBankerAutoUpload pbau1;
			const CPersonalBankerAutoUpload pbau = pbau1;
			CPersonalBankerAutoUpload varPbau(pbau);

			// Test #1.
			const bool autoInProcessResult = pbau.IsAutoInProcess();

			{
				// Report #1.
				bool areEqual = autoInProcessResult == autoInProcessExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoInProcessResult=%d expected=%d in %s"),
					(int)autoInProcessResult,
					(int)autoInProcessExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			const bool autoNcepOutPendingResult = pbau.IsAutoUploadPending();

			{
				// Report #2.
				bool areEqual = autoNcepOutPendingResult == autoNcepOutPendingExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
					(int)autoNcepOutPendingResult,
					(int)autoNcepOutPendingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			const bool autoStartedOnceResult = pbau.IsAutoStartedOnce();

			{
				// Report #3.
				bool areEqual = autoStartedOnceResult == autoStartedOnceExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoStartedOnceResult=%d expected=%d in %s"),
					(int)autoStartedOnceResult,
					(int)autoStartedOnceExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			const bool startUploadOverResult = pbau.IsStartUploadOver();

			{
				// Report #4.
				bool areEqual = startUploadOverResult == startUploadOverExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED startUploadOverResult=%d expected=%d in %s"),
					(int)startUploadOverResult,
					(int)startUploadOverExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			CCard const * const cardResult = varPbau.GetCard();

			{
				// Report #5.
				bool areEqual = cardResult == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=%p in %s"),
					(void *)cardResult,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(CUtilities::GetTickCount());
			// No requirements for GetUploadTime().
			ASSERT_TRUE(true) << _T("uploadTimeResult unexpected");
			{
				// Report #6.
				bool areEqual = uploadTimerExpiredResult == uploadTimerExpiredExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d in %s"),
					(int)uploadTimerExpiredResult,
					(int)uploadTimerExpiredExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			const DWORD pendingAutoNcepAmountResult = pbau.GetPendingAutoNcepAmount();

			{
				// Report #7.
				bool areEqual = pendingAutoNcepAmountResult == pendingAutoNcepAmountExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pendingAutoNcepAmountResult=%u expected=%u in %s"),
					(int)pendingAutoNcepAmountResult,
					(int)pendingAutoNcepAmountExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of the default constructor and copy constructor:
		///     CPersonalBankerAutoUpload::CPersonalBankerAutoUpload(void).
		///     CPersonalBankerAutoUpload::operator=(const CPersonalBankerAutoUpload &)
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, PersonalBankerAutoUploadDefaultAssign0Test)
		{
			// Test data.
			const bool autoInProcessExpected = false;
			const bool autoNcepOutPendingExpected = false;
			const bool autoStartedOnceExpected = false;
			const bool startUploadOverExpected = false;
			const CCard * const cardExpected = NULL;
			const bool uploadTimerExpiredExpected = true;
			const DWORD pendingAutoNcepAmountExpected = 0;

			// Setup.
			CPersonalBankerAutoUpload varPbau;
			CPersonalBankerAutoUpload varPbau1;
			varPbau1.CreateCard();
			CPersonalBankerAutoUpload varPbau2;
			varPbau2.CreateCard();
			varPbau1 = varPbau2;
			const CPersonalBankerAutoUpload pbau(varPbau1);

			// Test #1.
			const bool autoInProcessResult = pbau.IsAutoInProcess();

			{
				// Report #1.
				bool areEqual = autoInProcessResult == autoInProcessExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoInProcessResult=%d expected=%d in %s"),
					(int)autoInProcessResult,
					(int)autoInProcessExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			const bool autoNcepOutPendingResult = pbau.IsAutoUploadPending();

			{
				// Report #2.
				bool areEqual = autoNcepOutPendingResult == autoNcepOutPendingExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
					(int)autoNcepOutPendingResult,
					(int)autoNcepOutPendingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			const bool autoStartedOnceResult = pbau.IsAutoStartedOnce();

			{
				// Report #3.
				bool areEqual = autoStartedOnceResult == autoStartedOnceExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoStartedOnceResult=%d expected=%d in %s"),
					(int)autoStartedOnceResult,
					(int)autoStartedOnceExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			const bool startUploadOverResult = pbau.IsStartUploadOver();

			{
				// Report #4.
				bool areEqual = startUploadOverResult == startUploadOverExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED startUploadOverResult=%d expected=%d in %s"),
					(int)startUploadOverResult,
					(int)startUploadOverExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			CCard const * const cardResult = varPbau.GetCard();

			{
				// Report #5.
				bool areEqual = cardResult == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=%p in %s"),
					(void *)cardResult,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(CUtilities::GetTickCount());
			// No requirements for GetUploadTime().
			ASSERT_TRUE(true) <<  _T("uploadTimeResult unexpected");
			{
				// Report #6.
				bool areEqual = uploadTimerExpiredResult == uploadTimerExpiredExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d in %s"),
					(int)uploadTimerExpiredResult,
					(int)uploadTimerExpiredExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			const DWORD pendingAutoNcepAmountResult = pbau.GetPendingAutoNcepAmount();

			{
				// Report #7.
				bool areEqual = pendingAutoNcepAmountResult == pendingAutoNcepAmountExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pendingAutoNcepAmountResult=%u expected=%u in %s"),
					(int)pendingAutoNcepAmountResult,
					(int)pendingAutoNcepAmountExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of the default constructor and assigment operator:
		///     CPersonalBankerAutoUpload::CPersonalBankerAutoUpload(void).
		///     CPersonalBankerAutoUpload::operator=(const CPersonalBankerAutoUpload &)
		/// Assignment operator should only be used if m_pbtOrigCard is NULL.
		/// </summary>
		/// <remarks>
		/// Modify the target of the assignment before assignment so the assignment operator is called instead of the copy constructor.
		/// </remarks>
		TEST(PersonalBankerAutoUploadTests, PersonalBankerAutoUploadDefaultAssign2Test)
		{
			// Test data.
			const bool autoInProcessExpected = false;
			const bool autoNcepOutPendingExpected = false;
			const bool autoStartedOnceExpected = false;
			const bool startUploadOverExpected = false;
			const CCard * const cardExpected = NULL;
			const bool uploadTimerExpiredExpected = true;
			const DWORD pendingAutoNcepAmountExpected = 0;

			// Setup.
			const CPersonalBankerAutoUpload pbau1;
			CPersonalBankerAutoUpload varPbau;
			varPbau.SetAutoInProcess(true);
			varPbau = pbau1;
			const CPersonalBankerAutoUpload pbau(varPbau);

			// Test #1.
			const bool autoInProcessResult = pbau.IsAutoInProcess();

			{
				// Report #1.
				bool areEqual = autoInProcessResult == autoInProcessExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoInProcessResult=%d expected=%d in %s"),
					(int)autoInProcessResult,
					(int)autoInProcessExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			const bool autoNcepOutPendingResult = pbau.IsAutoUploadPending();

			{
				// Report #2.
				bool areEqual = autoNcepOutPendingResult == autoNcepOutPendingExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
					(int)autoNcepOutPendingResult,
					(int)autoNcepOutPendingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			const bool autoStartedOnceResult = pbau.IsAutoStartedOnce();

			{
				// Report #3.
				bool areEqual = autoStartedOnceResult == autoStartedOnceExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoStartedOnceResult=%d expected=%d in %s"),
					(int)autoStartedOnceResult,
					(int)autoStartedOnceExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			const bool startUploadOverResult = pbau.IsStartUploadOver();

			{
				// Report #4.
				bool areEqual = startUploadOverResult == startUploadOverExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED startUploadOverResult=%d expected=%d in %s"),
					(int)startUploadOverResult,
					(int)startUploadOverExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			CCard const * const cardResult = varPbau.GetCard();

			{
				// Report #5.
				bool areEqual = cardResult == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=%p in %s"),
					(void *)cardResult,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(CUtilities::GetTickCount());
			// No requirements for GetUploadTime().
			ASSERT_TRUE(true) <<  _T("uploadTimeResult unexpected");
			{
				// Report #6.
				bool areEqual = uploadTimerExpiredResult == uploadTimerExpiredExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d in %s"),
					(int)uploadTimerExpiredResult,
					(int)uploadTimerExpiredExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			const DWORD pendingAutoNcepAmountResult = pbau.GetPendingAutoNcepAmount();

			{
				// Report #7.
				bool areEqual = pendingAutoNcepAmountResult == pendingAutoNcepAmountExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pendingAutoNcepAmountResult=%u expected=%u in %s"),
					(int)pendingAutoNcepAmountResult,
					(int)pendingAutoNcepAmountExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of the default constructor and copy constructor:
		///     CPersonalBankerAutoUpload::CPersonalBankerAutoUpload(void).
		///     CPersonalBankerAutoUpload::operator=(const CPersonalBankerAutoUpload &)
		/// Assignment operator should only be used if m_pbtOrigCard is NULL.
		/// </summary>
		/// <remarks>
		/// Modify the target of the assignment before assignment so the assignment operator is called instead of the copy constructor.
		/// </remarks>
		TEST(PersonalBankerAutoUploadTests, PersonalBankerAutoUploadDefaultAssign3Test)
		{
			// Test data.
			const bool autoInProcessExpected = false;
			const bool autoNcepOutPendingExpected = false;
			const bool autoStartedOnceExpected = false;
			const bool startUploadOverExpected = false;
			const CCard * const cardExpected = NULL;
			const bool uploadTimerExpiredExpected = true;
			const DWORD pendingAutoNcepAmountExpected = 0;

			// Setup.
			const CPersonalBankerAutoUpload pbau1;
			CPersonalBankerAutoUpload varPbau;
			varPbau.SetAutoInProcess(true);
			varPbau = pbau1;
			const CPersonalBankerAutoUpload pbau(varPbau);

			// Test #1.
			const bool autoInProcessResult = pbau.IsAutoInProcess();

			{
				// Report #1.
				bool areEqual = autoInProcessResult == autoInProcessExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoInProcessResult=%d expected=%d in %s"),
					(int)autoInProcessResult,
					(int)autoInProcessExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			const bool autoNcepOutPendingResult = pbau.IsAutoUploadPending();

			{
				// Report #2.
				bool areEqual = autoNcepOutPendingResult == autoNcepOutPendingExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoNcepOutPendingResult=%d expected=%d in %s"),
					(int)autoNcepOutPendingResult,
					(int)autoNcepOutPendingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			const bool autoStartedOnceResult = pbau.IsAutoStartedOnce();

			{
				// Report #3.
				bool areEqual = autoStartedOnceResult == autoStartedOnceExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED autoStartedOnceResult=%d expected=%d in %s"),
					(int)autoStartedOnceResult,
					(int)autoStartedOnceExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			const bool startUploadOverResult = pbau.IsStartUploadOver();

			{
				// Report #4.
				bool areEqual = startUploadOverResult == startUploadOverExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED startUploadOverResult=%d expected=%d in %s"),
					(int)startUploadOverResult,
					(int)startUploadOverExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #5.
			CCard const * const cardResult = varPbau.GetCard();

			{
				// Report #5.
				bool areEqual = cardResult == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=%p in %s"),
					(void *)cardResult,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #6.
			const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(CUtilities::GetTickCount());
			// No requirements for GetUploadTime().
			ASSERT_TRUE(true) << _T("uploadTimeResult unexpected");
			{
				// Report #6.
				bool areEqual = uploadTimerExpiredResult == uploadTimerExpiredExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d in %s"),
					(int)uploadTimerExpiredResult,
					(int)uploadTimerExpiredExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #7.
			const DWORD pendingAutoNcepAmountResult = pbau.GetPendingAutoNcepAmount();

			{
				// Report #7.
				bool areEqual = pendingAutoNcepAmountResult == pendingAutoNcepAmountExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pendingAutoNcepAmountResult=%u expected=%u in %s"),
					(int)pendingAutoNcepAmountResult,
					(int)pendingAutoNcepAmountExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetAutoInProcess(), and
		///     CPersonalBankerAutoUpload::IsAutoInProcess().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, SetAutoInProcessTest)
		{
			typedef struct
			{
				// Inputs.
				const bool m_autoInProcess;

				// Expected results.
				const bool m_autoInProcessExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerAutoUpload varPbau;
 
				// Perform operation(s) to be tested.
				varPbau.SetAutoInProcess(TEST_DATA[testIndex].m_autoInProcess);
				const CPersonalBankerAutoUpload &pbau(varPbau);

				// Test.
				const bool autoInProcessResult = pbau.IsAutoInProcess();

				{
					// Report #1.
					bool areEqual = autoInProcessResult == TEST_DATA[testIndex].m_autoInProcessExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetAutoInProcess() autoInProcessResult=%d expected=%d testIndex=%d in %s"),
						(int)autoInProcessResult,
						(int)TEST_DATA[testIndex].m_autoInProcessExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetAutoUploadPending(), and
		///     CPersonalBankerAutoUpload::IsAutoUploadPending().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, SetAutoNcepOutPendingTest)
		{
			typedef struct
			{
				// Inputs.
				const bool m_autoNcepOutPending;

				// Expected results.
				const bool m_autoNcepOutPendingExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerAutoUpload varPbau;
 
				// Perform operation(s) to be tested.
				varPbau.SetAutoUploadPending(TEST_DATA[testIndex].m_autoNcepOutPending);
				const CPersonalBankerAutoUpload &pbau(varPbau);

				// Test #1.
				const bool autoNcepOutPendingResult = pbau.IsAutoUploadPending();

				{
					// Report #1.
					bool areEqual = autoNcepOutPendingResult == TEST_DATA[testIndex].m_autoNcepOutPendingExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetAutoUploadPending() autoNcepOutPendingResult=%d expected=%d testIndex=%d in %s"),
						(int)autoNcepOutPendingResult,
						(int)TEST_DATA[testIndex].m_autoNcepOutPendingExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetAutoStartedOnce(), and
		///     CPersonalBankerAutoUpload::IsAutoStartedOnce().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, SetAutoStartedOnceTest)
		{
			typedef struct
			{
				// Inputs.
				const bool m_autoStartedOnce;

				// Expected results.
				const bool m_autoStartedOnceExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerAutoUpload varPbau;
 
				// Perform operation(s) to be tested.
				varPbau.SetAutoStartedOnce(TEST_DATA[testIndex].m_autoStartedOnce);
				const CPersonalBankerAutoUpload &pbau(varPbau);

				// Test #1.
				const bool autoStartedOnceResult = pbau.IsAutoStartedOnce();

				{
					// Report #1.
					bool areEqual = autoStartedOnceResult == TEST_DATA[testIndex].m_autoStartedOnceExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetAutoStartedOnce() autoStartedOnceResult=%d expected=%d testIndex=%d in %s"),
						(int)autoStartedOnceResult,
						(int)TEST_DATA[testIndex].m_autoStartedOnceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetStartUploadOver(), and
		///     CPersonalBankerAutoUpload::IsStartUploadOver().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, SetStartUploadOverTest)
		{
			typedef struct
			{
				// Inputs.
				const bool m_startUploadOver;

				// Expected results.
				const bool m_startUploadOverExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerAutoUpload varPbau;
 
				// Perform operation(s) to be tested.
				varPbau.SetStartUploadOver(TEST_DATA[testIndex].m_startUploadOver);
				const CPersonalBankerAutoUpload &pbau(varPbau);

				// Test #1.
				const bool startUploadOverResult = pbau.IsStartUploadOver();

				{
					// Report #1.
					bool areEqual = startUploadOverResult == TEST_DATA[testIndex].m_startUploadOverExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetStartUploadOver() startUploadOverResult=%d expected=%d testIndex=%d in %s"),
						(int)startUploadOverResult,
						(int)TEST_DATA[testIndex].m_startUploadOverExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::CreateCard(void), and
		///     CPersonalBankerAutoUpload::GetCard().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, CreateCardDefault1Test)
		{
			// No test data.

			// Setup.
			CPersonalBankerAutoUpload pbau;

			// Perform operation(s) to be tested.
			CCard const * const cardResult1 = pbau.CreateCard();
			// Don't use CPersonalBankerAutoUpload copy constructor if card is NOT NULL.

			{
				// Report #1.
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult1=%p expected=NOT NULL in %s"),
					(void *)cardResult1,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(cardResult1 != NULL) << failedMsg;
			}

			// Test #2.
			CCard const * const cardResult2 = pbau.GetCard();

			{
				// Report #2.
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult2=%p expected=NOT NULL in %s"),
					(void *)cardResult2,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(cardResult2 != NULL) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::CreateCard(void)
		///     CPersonalBankerAutoUpload::CreateCard(void)
		///     CPersonalBankerAutoUpload::GetCard()
		/// </summary>
		/// <remarks>
		/// Create another card first.
		/// </remarks>
		TEST(PersonalBankerAutoUploadTests, CreateCardDefault2Test)
		{
			// No test data.

			// Setup.
			CPersonalBankerAutoUpload pbau;
			pbau.CreateCard();

			// Perform operation(s) to be tested.
			CCard const * const cardResult1 = pbau.CreateCard();
			// Don't use CPersonalBankerAutoUpload copy constructor if card is NOT NULL.

			{
				// Report #1.
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult1=%p expected=NOT NULL in %s"),
					(void *)cardResult1,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(cardResult1 != NULL) << failedMsg;
			}

			// Test #2.
			CCard const * const cardResult2 = pbau.GetCard();

			{
				// Report #2.
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult2=%p expected=NOT NULL in %s"),
					(void *)cardResult2,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(cardResult2 != NULL) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::CreateCard(void), and
		///     CPersonalBankerAutoUpload::GetCard().
		/// Update the card returned by CreateCard() and verify that GetCard() returns the updated card.
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, CreateCardDefaultUpdateTest)
		{
			typedef struct
			{
				// Inputs.
				CardType m_type;

				// Expected results.
				CardType m_cardTypeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ CARD_UNKNOWN, CARD_UNKNOWN },
				{ CARD_PLAYER, CARD_PLAYER },
				{ CARD_MECHANIC, CARD_MECHANIC },
				{ CARD_GLOBAL, CARD_GLOBAL },
				{ CARD_FLOOR, CARD_FLOOR },
				{ CARD_CURRENCY, CARD_CURRENCY },
				{ CARD_METER, CARD_METER },
				{ CARD_TEMP, CARD_TEMP },
				{ (CardType)0, (CardType)0 },
				{ (CardType)1, (CardType)1 },
				{ (CardType)(INT_MAX - 1), (CardType)(INT_MAX - 1) },
				{ (CardType)INT_MAX, (CardType)INT_MAX },
				{ (CardType)INT_MIN, (CardType)INT_MIN },
				{ (CardType)(INT_MIN + 1), (CardType)(INT_MIN + 1) },
				{ (CardType)-2, (CardType)-2 },
				{ (CardType)-1, (CardType)-1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{

				// Setup.
				CPersonalBankerAutoUpload pbau;

				// Perform operation(s) to be tested.
				CCard * const cardResult1 = pbau.CreateCard();
				cardResult1->SetCardType(TEST_DATA[testIndex].m_type);
				// Don't use CPersonalBankerAutoUpload copy constructor if card is NOT NULL.

				// Test #1.  Verify that GetCard() return value is the same as CreateCard().
				CCard const * const cardResult2 = pbau.GetCard();

				{
					// Report #1.
					bool areEqual = cardResult2 == cardResult1;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cardResult2=%p expected=%p testIndex=%d in %s"),
						(void *)cardResult2,
						(void *)cardResult1,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.  Verify value of GetCard()->GetCardType().
				const CardType cardTypeResult = cardResult2->GetCardType();

				{
					// Report #2.
					bool areEqual = cardTypeResult == TEST_DATA[testIndex].m_cardTypeExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cardTypeResult=%d expected=%d testIndex=%d in %s"),
						(int)cardTypeResult,
						(int)TEST_DATA[testIndex].m_cardTypeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::CreateCard(const CCard*)
		///     CPersonalBankerAutoUpload::GetCard()
		/// Test using a NULL CCard pointer.
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, CreateCardNull1Test)
		{
			// Test data.
			const CCard *card = NULL;
			const CCard *cardExpected = NULL;

			// Setup.
			CPersonalBankerAutoUpload varPbau;

			// Perform operation(s) to be tested.
			CCard const * const cardResult1 = varPbau.CreateCard(card);
			const CPersonalBankerAutoUpload &pbau(varPbau);

			{
				// Report #1.
				bool areEqual = cardResult1 == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult1=%p expected=%p in %s"),
					(void *)cardResult1,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.  Verify that GetCard() return value has NOT changed.
			CCard const * const cardResult2 = varPbau.GetCard();

			{
				// Report #2.
				bool areEqual = cardResult2 == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult2=%p expected=%p in %s"),
					(void *)cardResult2,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::CreateCard(void)
		///     CPersonalBankerAutoUpload::CreateCard(const CCard*)
		///     CPersonalBankerAutoUpload::GetCard()
		/// Test using a NULL CCard pointer for 2nd CreateCard() call.
		/// Verify that GetCard() returns the card from the 1st CreateCard() call.
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, CreateCardNull2Test)
		{
			// Test data.
			const CCard *card = NULL;

			// Setup.
			CPersonalBankerAutoUpload pbau;
			CCard const * const cardExpected = pbau.CreateCard();

			// Perform operation(s) to be tested.
			CCard const * const cardResult1 = pbau.CreateCard(card);
			pbau.CreateCard(card);
			// Don't use CPersonalBankerAutoUpload copy constructor if card is NOT NULL.

			{
				// Report #1.
				bool areEqual = cardResult1 == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult1=%p expected=%p in %s"),
					(void *)cardResult1,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.  Verify that GetCard() return value has NOT changed.
			CCard const * const cardResult2 = pbau.GetCard();

			{
				// Report #2.
				bool areEqual = cardResult2 == cardExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult2=%p expected=%p in %s"),
					(void *)cardResult2,
					(void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::CreateCard(const CCard*)
		///     CPersonalBankerAutoUpload::GetCard()
		/// Test using a NOT NULL CCard pointer.
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, CreateCard0Test)
		{
			// Test data.
			const CCard card(time_t(0), time_t(0));

			// Setup.
			CPersonalBankerAutoUpload pbau;

			// Perform operation(s) to be tested.
			CCard const * const cardResult1 = pbau.CreateCard(&card);
			// Don't use CPersonalBankerAutoUpload copy constructor if card is NOT NULL.

			{
				// Report #1.
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult1=%p expected=NOT NULL in %s"),
					(void *)cardResult1,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(cardResult1 != NULL) << failedMsg;
			}

			// Test #2.  Verify that GetCard() return value has NOT changed.
			CCard const * const cardResult2 = pbau.GetCard();

			{
				// Report #2.
				bool areEqual = cardResult2 == cardResult1;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult2=%p expected=%p in %s"),
					(void *)cardResult2,
					(void *)cardResult1,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

				// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::CreateCard(const CCard*)
		///     CPersonalBankerAutoUpload::GetCard()
		/// Test using a NOT NULL CCard pointer.
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, CreateCard1Test)
		{
			// Test data.
			const CCard card(time_t(0), time_t(0));

			// Setup.
			CPersonalBankerAutoUpload pbau;

			// Perform operation(s) to be tested.
			CCard const * const cardResult1 = pbau.CreateCard(&card);
			// Don't use CPersonalBankerAutoUpload copy constructor if card is NOT NULL.

			{
				// Report #1.
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult1=%p expected=NOT NULL in %s"),
					(void *)cardResult1,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(cardResult1 != NULL) << failedMsg;
			}

			// Test #2.  Verify that GetCard() return value has NOT changed.
			CCard const * const cardResult2 = pbau.GetCard();

			{
				// Report #2.
				bool areEqual = cardResult2 == cardResult1;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult2=%p expected=%p in %s"),
					(void *)cardResult2,
					(void *)cardResult1,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

				// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::CreateCard(void), and
		///     CPersonalBankerAutoUpload::GetCard().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, CreateCard2Test)
		{
			typedef struct
			{
				// Inputs.
				CardType m_type;

				// Expected results.
				CardType m_cardTypeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ CARD_UNKNOWN, CARD_UNKNOWN },
				{ CARD_PLAYER, CARD_PLAYER },
				{ CARD_MECHANIC, CARD_MECHANIC },
				{ CARD_GLOBAL, CARD_GLOBAL },
				{ CARD_FLOOR, CARD_FLOOR },
				{ CARD_CURRENCY, CARD_CURRENCY },
				{ CARD_METER, CARD_METER },
				{ CARD_TEMP, CARD_TEMP },
				{ (CardType)0, (CardType)0 },
				{ (CardType)1, (CardType)1 },
				{ (CardType)(INT_MAX - 1), (CardType)(INT_MAX - 1) },
				{ (CardType)INT_MAX, (CardType)INT_MAX },
				{ (CardType)INT_MIN, (CardType)INT_MIN },
				{ (CardType)(INT_MIN + 1), (CardType)(INT_MIN + 1) },
				{ (CardType)-2, (CardType)-2 },
				{ (CardType)-1, (CardType)-1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{

				// Setup.
				CPersonalBankerAutoUpload pbau;
				CCard varCard(time_t(1), time_t(1));
				varCard.SetCardType(TEST_DATA[testIndex].m_type);
				const CCard card(varCard);

				// Perform operation(s) to be tested.
				CCard * const cardResult1 = pbau.CreateCard(&card);
				// Don't use CPersonalBankerAutoUpload copy constructor if card is NOT NULL.

				// Test #1.  Verify that GetCard() return value is the same as CreateCard().
				CCard const * const cardResult2 = pbau.GetCard();

				{
					// Report #1.
					bool areEqual = cardResult2 == cardResult1;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cardResult2=%p expected=%p testIndex=%d in %s"),
						(void *)cardResult2,
						(void *)cardResult1,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.  Verify value of GetCard()->GetCardType().
				const CardType cardTypeResult = cardResult2->GetCardType();

				{
					// Report #2.
					bool areEqual = cardTypeResult == TEST_DATA[testIndex].m_cardTypeExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cardTypeResult=%d expected=%d testIndex=%d in %s"),
						(int)cardTypeResult,
						(int)TEST_DATA[testIndex].m_cardTypeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::CreateCard(void), and
		///     CPersonalBankerAutoUpload::GetCard().
		/// Update the card returned by CreateCard() and verify that GetCard() returns the updated card.
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, CreateCardUpdateTest)
		{
			typedef struct
			{
				// Inputs.
				CardType m_type;

				// Expected results.
				CardType m_cardTypeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ CARD_UNKNOWN, CARD_UNKNOWN },
				{ CARD_PLAYER, CARD_PLAYER },
				{ CARD_MECHANIC, CARD_MECHANIC },
				{ CARD_GLOBAL, CARD_GLOBAL },
				{ CARD_FLOOR, CARD_FLOOR },
				{ CARD_CURRENCY, CARD_CURRENCY },
				{ CARD_METER, CARD_METER },
				{ CARD_TEMP, CARD_TEMP },
				{ (CardType)0, (CardType)0 },
				{ (CardType)1, (CardType)1 },
				{ (CardType)(INT_MAX - 1), (CardType)(INT_MAX - 1) },
				{ (CardType)INT_MAX, (CardType)INT_MAX },
				{ (CardType)INT_MIN, (CardType)INT_MIN },
				{ (CardType)(INT_MIN + 1), (CardType)(INT_MIN + 1) },
				{ (CardType)-2, (CardType)-2 },
				{ (CardType)-1, (CardType)-1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{

				// Setup.
				CPersonalBankerAutoUpload pbau;
				const CCard card(time_t(1), time_t(1));

				// Perform operation(s) to be tested.
				CCard * const cardResult1 = pbau.CreateCard(&card);
				cardResult1->SetCardType(TEST_DATA[testIndex].m_type);
				// Don't use CPersonalBankerAutoUpload copy constructor if card is NOT NULL.

				// Test #1.  Verify that GetCard() return value is the same as CreateCard().
				CCard const * const cardResult2 = pbau.GetCard();

				{
					// Report #1.
					bool areEqual = cardResult2 == cardResult1;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cardResult2=%p expected=%p testIndex=%d in %s"),
						(void *)cardResult2,
						(void *)cardResult1,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.  Verify value of GetCard()->GetCardType().
				const CardType cardTypeResult = cardResult2->GetCardType();

				{
					// Report #2.
					bool areEqual = cardTypeResult == TEST_DATA[testIndex].m_cardTypeExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cardTypeResult=%d expected=%d testIndex=%d in %s"),
						(int)cardTypeResult,
						(int)TEST_DATA[testIndex].m_cardTypeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::IsAutoUploadInProgress().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, IsAutoUploadInProgressDefaultTest)
		{

			// Test Data.
			bool autoUploadInProgressExpected = false;

			// Setup.
			CPersonalBankerAutoUpload varPbau;
 
			// Perform operation(s) to be tested.
			const CPersonalBankerAutoUpload &pbau(varPbau);

			// Test #1.
			const bool autoUploadInProgressResult = pbau.IsAutoUploadInProgress();

			{
				// Report #1.
				bool areEqual = autoUploadInProgressResult == autoUploadInProgressExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d in %s"),
					(int)autoUploadInProgressResult,
					(int)autoUploadInProgressExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetAutoUploadPending(), and
		///     CPersonalBankerAutoUpload::IsAutoUploadInProgress().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, IsAutoUploadInProgressFalseTest)
		{

			// Test Data.
			const bool autoNcepOutPending = false;
			const bool autoInProcess = false;
			const bool autoUploadInProgressExpected = false;

			// Setup.
			CPersonalBankerAutoUpload varPbau;
 
			// Perform operation(s) to be tested.
			varPbau.SetAutoUploadPending(autoNcepOutPending);
			varPbau.SetAutoInProcess(autoInProcess);
			const CPersonalBankerAutoUpload &pbau(varPbau);

			// Test #1.
			const bool autoUploadInProgressResult = pbau.IsAutoUploadInProgress();

			{
				// Report #1.
				bool areEqual = autoUploadInProgressResult == autoUploadInProgressExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d testindex=n/a (%d, %d) in %s"),
					(int)autoUploadInProgressResult,
					(int)autoUploadInProgressExpected,
					(int)autoNcepOutPending,
					(int)autoInProcess,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetAutoUploadPending(), and
		///     CPersonalBankerAutoUpload::IsAutoUploadInProgress().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, IsAutoUploadInProgressTrue1Test)
		{

			// Test Data.
			const bool autoNcepOutPending = true;
			const bool autoInProcess = false;
			const bool autoUploadInProgressExpected = true;

			// Setup.
			CPersonalBankerAutoUpload varPbau;
 
			// Perform operation(s) to be tested.
			varPbau.SetAutoUploadPending(autoNcepOutPending);
			varPbau.SetAutoInProcess(autoInProcess);
			const CPersonalBankerAutoUpload &pbau(varPbau);

			// Test #1.
			const bool autoUploadInProgressResult = pbau.IsAutoUploadInProgress();

			{
				// Report #1.
				bool areEqual = autoUploadInProgressResult == autoUploadInProgressExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d testindex=n/a (%d, %d) in %s"),
					(int)autoUploadInProgressResult,
					(int)autoUploadInProgressExpected,
					(int)autoNcepOutPending,
					(int)autoInProcess,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetAutoUploadPending(), and
		///     CPersonalBankerAutoUpload::IsAutoUploadInProgress().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, IsAutoUploadInProgressTrue2Test)
		{

			// Test Data.
			const bool autoNcepOutPending = false;
			const bool autoInProcess = true;
			const bool autoUploadInProgressExpected = true;

			// Setup.
			CPersonalBankerAutoUpload varPbau;
 
			// Perform operation(s) to be tested.
			varPbau.SetAutoUploadPending(autoNcepOutPending);
			varPbau.SetAutoInProcess(autoInProcess);
			const CPersonalBankerAutoUpload &pbau(varPbau);

			// Test #1.
			const bool autoUploadInProgressResult = pbau.IsAutoUploadInProgress();

			{
				// Report #1.
				bool areEqual = autoUploadInProgressResult == autoUploadInProgressExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d testindex=n/a (%d, %d) in %s"),
					(int)autoUploadInProgressResult,
					(int)autoUploadInProgressExpected,
					(int)autoNcepOutPending,
					(int)autoInProcess,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetAutoUploadPending(), and
		///     CPersonalBankerAutoUpload::IsAutoUploadInProgress().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, IsAutoUploadInProgressTrue3Test)
		{

			// Test Data.
			const bool autoNcepOutPending = true;
			const bool autoInProcess = true;
			const bool autoUploadInProgressExpected = true;

			// Setup.
			CPersonalBankerAutoUpload varPbau;
 
			// Perform operation(s) to be tested.
			varPbau.SetAutoUploadPending(autoNcepOutPending);
			varPbau.SetAutoInProcess(autoInProcess);
			const CPersonalBankerAutoUpload &pbau(varPbau);

			// Test #1.
			const bool autoUploadInProgressResult = pbau.IsAutoUploadInProgress();

			{
				// Report #1.
				bool areEqual = autoUploadInProgressResult == autoUploadInProgressExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d testindex=n/a (%d, %d) in %s"),
					(int)autoUploadInProgressResult,
					(int)autoUploadInProgressExpected,
					(int)autoNcepOutPending,
					(int)autoInProcess,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetUploadTimerExpired(), and
		///     CPersonalBankerAutoUpload::IsUploadTimerExpired().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, SetUploadTimerExpiredTest)
		{

			// Test Data.
			bool uploadTimerExpiredExpected = true;

			// Setup.
			CPersonalBankerAutoUpload varPbau;
 
			// Perform operation(s) to be tested.
			varPbau.SetUploadTimerExpired();
			const CPersonalBankerAutoUpload &pbau(varPbau);

			// Test #1.
			const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(CUtilities::GetTickCount());

			{
				// Report #1.
				bool areEqual = uploadTimerExpiredResult == uploadTimerExpiredExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d in %s"),
					(int)uploadTimerExpiredResult,
					(int)uploadTimerExpiredExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::IsUploadTimerExpired().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, IsUploadTimerExpiredTest)
		{

			// Test Data.
			bool uploadTimerExpiredExpected = true;

			// Setup.
			CPersonalBankerAutoUpload varPbau;
 
			// Perform operation(s) to be tested.
			varPbau.SetUploadTimerExpired();
			const CPersonalBankerAutoUpload &pbau(varPbau);

			// Test #1.
			const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(CUtilities::GetTickCount());

			{
				// Report #1.
				bool areEqual = uploadTimerExpiredResult == uploadTimerExpiredExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d in %s"),
					(int)uploadTimerExpiredResult,
					(int)uploadTimerExpiredExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetUploadDelay(), and
		///     CPersonalBankerAutoUpload::IsUploadTimerExpired().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, SetUploadDelayHexTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_uploadTickCount;
				const DWORD m_uploadDelayPeriod;
				const DWORD m_testTickCount;

				// Expected results.
				const bool m_uploadTimerExpiredExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, 0, 0, true },
				{ 0, 0, 1, true },
				{ 0, 0, 2, true },
				{ 0, 0, 0x7ffffffe, true },
				{ 0, 0, 0x7fffffff, true },
				{ 0, 0, 0x80000000, true },
				{ 0, 0, 0x80000001, true },
				{ 0, 0, 0xfffffffe, true },
				{ 0, 0, 0xffffffff, true },

				{ 0, 0, 0, true },
				{ 1, 0, 0, true },
				{ 2, 0, 0, true },
				{ 0x7ffffffe, 0, 0, true },
				{ 0x7fffffff, 0, 0, true },
				{ 0x80000000, 0, 0, true },
				{ 0x80000001, 0, 0, true },
				{ 0xfffffffe, 0, 0, true },
				{ 0xffffffff, 0, 0, true },

				{ 0, 0, 0, true },
				{ 1, 0, 1, true },
				{ 2, 0, 2, true },
				{ 0x7ffffffe, 0, 0x7ffffffe, true },
				{ 0x7fffffff, 0, 0x7fffffff, true },
				{ 0x80000000, 0, 0x80000000, true },
				{ 0x80000001, 0, 0x80000001, true },
				{ 0xfffffffe, 0, 0xfffffffe, true },
				{ 0xffffffff, 0, 0xffffffff, true },

				{ 0, 1, 0, false },
				{ 0, 1, 1, true },
				{ 0, 1, 2, true },
				{ 0, 1, 0xffffffff, true },

				{ 1, 1, 0, true },
				{ 1, 1, 1, false },
				{ 1, 1, 2, true },
				{ 1, 1, 0xffffffff, true },

				{ 2, 1, 0, true },
				{ 2, 1, 1, true },
				{ 2, 1, 2, false },
				{ 2, 1, 3, true },
				{ 2, 1, 0xffffffff, true },

				{ 0xffffffff, 1, 0, true },
				{ 0xffffffff, 1, 1, true },
				{ 0xffffffff, 1, 0xfffffffe, true },
				{ 0xffffffff, 1, 0xffffffff, false },

				{ 0, 0x80000000, 0, false },
				{ 0, 0x80000000, 1, false },
				{ 0, 0x80000000, 2, false },
				{ 0, 0x80000000, 0x7fffffff, false },
				{ 0, 0x80000000, 0x80000000, true },
				{ 0, 0x80000000, 0x80000001, true },
				{ 0, 0x80000000, 0xfffffffe, true },
				{ 0, 0x80000000, 0xffffffff, true },

				{ 1, 0x80000000, 0, true },
				{ 1, 0x80000000, 1, false },
				{ 1, 0x80000000, 2, false },
				{ 1, 0x80000000, 0x7fffffff, false },
				{ 1, 0x80000000, 0x80000000, false },
				{ 1, 0x80000000, 0x80000001, true },
				{ 1, 0x80000000, 0xfffffffe, true },
				{ 1, 0x80000000, 0xffffffff, true },

				{ 0x7fffffff, 0x80000000, 0, true },
				{ 0x7fffffff, 0x80000000, 1, true },
				{ 0x7fffffff, 0x80000000, 2, true },
				{ 0x7fffffff, 0x80000000, 0x7fffffff, false },
				{ 0x7fffffff, 0x80000000, 0x80000000, false },
				{ 0x7fffffff, 0x80000000, 0x80000001, false },
				{ 0x7fffffff, 0x80000000, 0xfffffffe, false },
				{ 0x7fffffff, 0x80000000, 0xffffffff, true },

				{ 0x80000000, 0x80000000, 0, true },
				{ 0x80000000, 0x80000000, 1, true },
				{ 0x80000000, 0x80000000, 2, true },
				{ 0x80000000, 0x80000000, 0x7fffffff, true },
				{ 0x80000000, 0x80000000, 0x80000000, false },
				{ 0x80000000, 0x80000000, 0x80000001, false },
				{ 0x80000000, 0x80000000, 0xfffffffe, false },
				{ 0x80000000, 0x80000000, 0xffffffff, false },

				{ 0x80000001, 0x80000000, 0, false },
				{ 0x80000001, 0x80000000, 1, true },
				{ 0x80000001, 0x80000000, 2, true },
				{ 0x80000001, 0x80000000, 0x7fffffff, true },
				{ 0x80000001, 0x80000000, 0x80000000, true },
				{ 0x80000001, 0x80000000, 0x80000001, false },
				{ 0x80000001, 0x80000000, 0xfffffffe, false },
				{ 0x80000001, 0x80000000, 0xffffffff, false },

				{ 0xfffffffe, 0x80000000, 0, false },
				{ 0xfffffffe, 0x80000000, 1, false },
				{ 0xfffffffe, 0x80000000, 2, false },
				{ 0xfffffffe, 0x80000000, 0x7ffffffe, true },
				{ 0xfffffffe, 0x80000000, 0x7fffffff, true },
				{ 0xfffffffe, 0x80000000, 0x80000000, true },
				{ 0xfffffffe, 0x80000000, 0x80000001, true },
				{ 0xfffffffe, 0x80000000, 0xfffffffe, false },
				{ 0xfffffffe, 0x80000000, 0xffffffff, false },

				{ 0xffffffff, 0x80000000, 0, false },
				{ 0xffffffff, 0x80000000, 1, false },
				{ 0xffffffff, 0x80000000, 2, false },
				{ 0xffffffff, 0x80000000, 0x7ffffffe, false },
				{ 0xffffffff, 0x80000000, 0x7fffffff, true },
				{ 0xffffffff, 0x80000000, 0x80000000, true },
				{ 0xffffffff, 0x80000000, 0x80000001, true },
				{ 0xffffffff, 0x80000000, 0xfffffffe, true },
				{ 0xffffffff, 0x80000000, 0xffffffff, false },

				{ 0, 0xffffffff, 0, false },
				{ 0, 0xffffffff, 1, false },
				{ 0, 0xffffffff, 2, false },
				{ 0, 0xffffffff, 0xfffffffe, false },
				{ 0, 0xffffffff, 0xffffffff, true },

				{ 1, 0xffffffff, 0, true },
				{ 1, 0xffffffff, 1, false },
				{ 1, 0xffffffff, 2, false },
				{ 1, 0xffffffff, 0xfffffffe, false },
				{ 1, 0xffffffff, 0xffffffff, false },

				{ 2, 0xffffffff, 0, false },
				{ 2, 0xffffffff, 1, true },
				{ 2, 0xffffffff, 2, false },
				{ 2, 0xffffffff, 3, false },
				{ 2, 0xffffffff, 0xfffffffe, false },
				{ 2, 0xffffffff, 0xffffffff, false },

				{ 0xfffffffe, 0xffffffff, 0, false },
				{ 0xfffffffe, 0xffffffff, 1, false },
				{ 0xfffffffe, 0xffffffff, 0xfffffffc, false },
				{ 0xfffffffe, 0xffffffff, 0xfffffffd, true },
				{ 0xfffffffe, 0xffffffff, 0xfffffffe, false },
				{ 0xfffffffe, 0xffffffff, 0xffffffff, false },

				{ 0xffffffff, 0xffffffff, 0, false },
				{ 0xffffffff, 0xffffffff, 1, false },
                
				{ 0xffffffff, 0xffffffff, 0xfffffffc, false },
				{ 0xffffffff, 0xffffffff, 0xfffffffd, false },
				{ 0xffffffff, 0xffffffff, 0xfffffffe, true },
				{ 0xffffffff, 0xffffffff, 0xffffffff, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Test Data.
				WORD uploadDelaySeconds = WORD_MAX; // Test should complete before timer expires.
				bool uploadTimerExpiredExpected = false;

				// Setup.
				CPersonalBankerAutoUpload varPbau;
 
				// Perform operation(s) to be tested.
				varPbau.SetUploadDelay(TEST_DATA[testIndex].m_uploadTickCount, TEST_DATA[testIndex].m_uploadDelayPeriod);
				const CPersonalBankerAutoUpload &pbau(varPbau);

				// Test #1.
				const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(TEST_DATA[testIndex].m_testTickCount);

				{
					// Report #1.
					bool areEqual = uploadTimerExpiredResult == TEST_DATA[testIndex].m_uploadTimerExpiredExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d testIndex=%d (%8.8x, %8.8x, %8.8x) in %s"),
						(int)uploadTimerExpiredResult,
						(int)TEST_DATA[testIndex].m_uploadTimerExpiredExpected,
						(int)testIndex,
						(unsigned)TEST_DATA[testIndex].m_uploadTickCount,
						(unsigned)TEST_DATA[testIndex].m_uploadDelayPeriod,
						(unsigned)TEST_DATA[testIndex].m_testTickCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetUploadDelay(), and
		///     CPersonalBankerAutoUpload::IsUploadTimerExpired().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, SetUploadDelayDecimalTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_uploadTickCount;
				const DWORD m_uploadDelayPeriod;
				const DWORD m_testTickCount;

				// Expected results.
				const bool m_uploadTimerExpiredExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, 1000, 0, false },
				{ 0, 1000, 1, false },
				{ 0, 1000, 999, false },
				{ 0, 1000, 1000, true },
				{ 0, 1000, 1001, true },
				{ 0, 1000, 0x7ffffffe, true },
				{ 0, 1000, 0x80000000, true },
				{ 0, 1000, 0xfffffffe, true },
				{ 0, 1000, 0xffffffff, true },

				{ 1000, 1000, 0, true },
				{ 1000, 1000, 1, true },
				{ 1000, 1000, 999, true },
				{ 1000, 1000, 1000, false },
				{ 1000, 1000, 1001, false },
				{ 1000, 1000, 1999, false },
				{ 1000, 1000, 2000, true },
				{ 1000, 1000, 2001, true },
				{ 1000, 1000, 0x7ffffffe, true },
				{ 1000, 1000, 0x80000000, true },
				{ 1000, 1000, 0xfffffffe, true },
				{ 1000, 1000, 0xffffffff, true },

				{ (DWORD)-500, 1000, 0, false },
				{ (DWORD)-500, 1000, 1, false },
				{ (DWORD)-500, 1000, 499, false },
				{ (DWORD)-500, 1000, 500, true },
				{ (DWORD)-500, 1000, 501, true },
				{ (DWORD)-500, 1000, 0x7ffffffe, true },
				{ (DWORD)-500, 1000, 0x80000000, true },
				{ (DWORD)-500, 1000, (DWORD)-501, true },
				{ (DWORD)-500, 1000, (DWORD)-500, false },
				{ (DWORD)-500, 1000, (DWORD)-499, false },
				{ (DWORD)-500, 1000, 0xfffffffe, false },
				{ (DWORD)-500, 1000, 0xffffffff, false },

				{ (DWORD)-1, 1000, 0, false },
				{ (DWORD)-1, 1000, 1, false },
				{ (DWORD)-1, 1000, 998, false },
				{ (DWORD)-1, 1000, 999, true },
				{ (DWORD)-1, 1000, 1000, true },
				{ (DWORD)-1, 1000, 0x7ffffffe, true },
				{ (DWORD)-1, 1000, 0x80000000, true },
				{ (DWORD)-1, 1000, (DWORD)-2, true },
				{ (DWORD)-1, 1000, (DWORD)-1, false },
				{ (DWORD)-1, 1000, 0xfffffffe, true },
				{ (DWORD)-1, 1000, 0xffffffff, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Test Data.
				WORD uploadDelaySeconds = WORD_MAX; // Test should complete before timer expires.
				bool uploadTimerExpiredExpected = false;

				// Setup.
				CPersonalBankerAutoUpload varPbau;
 
				// Perform operation(s) to be tested.
				varPbau.SetUploadDelay(TEST_DATA[testIndex].m_uploadTickCount, TEST_DATA[testIndex].m_uploadDelayPeriod);
				const CPersonalBankerAutoUpload &pbau(varPbau);

				// Test #1.
				const bool uploadTimerExpiredResult = pbau.IsUploadTimerExpired(TEST_DATA[testIndex].m_testTickCount);

				{
					// Report #1.
					bool areEqual = uploadTimerExpiredResult == TEST_DATA[testIndex].m_uploadTimerExpiredExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED uploadTimerExpiredResult=%d expected=%d testIndex=%d (%d, %d, %d) in %s"),
						(int)uploadTimerExpiredResult,
						(int)TEST_DATA[testIndex].m_uploadTimerExpiredExpected,
						(int)testIndex,
						(int)TEST_DATA[testIndex].m_uploadTickCount,
						(int)TEST_DATA[testIndex].m_uploadDelayPeriod,
						(int)TEST_DATA[testIndex].m_testTickCount,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
		/// <summary>
		/// Test the implementation of:
		///     CPersonalBankerAutoUpload::SetPendingAutoNcepAmount(), and
		///     CPersonalBankerAutoUpload::GetPendingAutoNcepAmount().
		/// </summary>
		TEST(PersonalBankerAutoUploadTests, SetPendingAutoNcepAmountTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_amount;

				// Expected results.
				const DWORD m_pendingAutoNcepAmountExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, 0 },
				{ 1, 1 },
				{ 0x7ffffffe, 0x7ffffffe },
				{ 0x7fffffff, 0x7fffffff },
				{ 0x80000000, 0x80000000 },
				{ 0x80000001, 0x80000001 },
				{ 0xfffffffe, 0xfffffffe },
				{ 0xffffffff, 0xffffffff },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBankerAutoUpload varPbau;
 
				// Perform operation(s) to be tested.
				varPbau.SetPendingAutoNcepAmount(TEST_DATA[testIndex].m_amount);
				const CPersonalBankerAutoUpload &pbau(varPbau);

				// Test #1.
				const DWORD pendingAutoNcepAmountResult = pbau.GetPendingAutoNcepAmount();

				{
					// Report #1.
					bool areEqual = pendingAutoNcepAmountResult == TEST_DATA[testIndex].m_pendingAutoNcepAmountExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetPendingAutoNcepAmount() pendingAutoNcepAmountResult=%u expected=%u testIndex=%d in %s"),
						(unsigned)pendingAutoNcepAmountResult,
						(unsigned)TEST_DATA[testIndex].m_pendingAutoNcepAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
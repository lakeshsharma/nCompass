#include "stdafx.h"
#include "HttpGet.h"
// Sentinel includes.
#include "Hardware/WatchDog.h"
#include "SentinelException.h"
#ifndef _UNIT_TEST
#include <curl/curl.h>
#endif

using namespace std;

static bool curlInitDone = false;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    CWatchDog::Instance().Checkin();

    size_t realsize = size * nmemb;
    if (stream != NULL) 
    {
        ((std::ostream *)stream)->write((const char *)ptr, realsize);
    }
    return realsize;
}
 
bool HttpGet(
	std::string srcFile,
	std::ostream* pStream,
	long&	statusCode,
	bool useCert,
	long timeout,
	progress_callback callback)
{
    bool result = false;

    #ifndef _UNIT_TEST
    if (!curlInitDone) {
        curl_global_init(CURL_GLOBAL_ALL);
        curlInitDone = true;
    }
    #endif

	if (srcFile != "")
	{
        #ifndef _UNIT_TEST
        /* init the curl session */ 
        CURL* curl_handle = curl_easy_init();
 
        /* set URL to get here */ 
        curl_easy_setopt(curl_handle, CURLOPT_URL, srcFile.c_str());
 
		if (callback)
		{
			/* enable progress meter */
			curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, callback);
		}
		else
		{
			/* disable progress meter */
			curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
		}

        /* send all data to this function  */ 
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

        /* write the page body to this file handle */ 
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pStream);
 
        /* do not validate certificates */ 
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, useCert ? 1L : 0L);

        /* do not validate certificates */ 
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, useCert ? 1L : 0L);

		if (timeout)
		{
			/* timeout interval in seconds */
			curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout);
		}

        /* get it! */ 
        CURLcode res = curl_easy_perform(curl_handle);
 
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &statusCode);

        result = res == CURLE_OK;

        /* cleanup curl stuff */ 
        curl_easy_cleanup(curl_handle);
        #endif
	}

    return result;
}


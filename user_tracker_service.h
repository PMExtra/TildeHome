#pragma once

#include <fstream>

#include "service_base.h"

class TildeHomeService final : public ServiceBase
{
public:
	TildeHomeService(const TildeHomeService& other) = delete;
	TildeHomeService& operator=(const TildeHomeService& other) = delete;

	TildeHomeService(TildeHomeService&& other) = delete;
	TildeHomeService& operator=(TildeHomeService&& other) = delete;

	TildeHomeService()
		: ServiceBase(_T("TildeHome"),
		              _T("Tilde Home Support"),
		              SERVICE_DEMAND_START,
		              SERVICE_ERROR_NORMAL,
		              SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SESSIONCHANGE)
	{
	}

private:
	void OnStart(DWORD argc, TCHAR* argv[]) override;
	void OnStop() override;

#ifdef UNICODE
	using tofstream = std::wofstream;
#else
    using tofstream = std::ofstream;
#endif

	tofstream m_logFile;
};

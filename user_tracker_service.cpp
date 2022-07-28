#include "user_tracker_service.h"

void TildeHomeService::OnStart(DWORD /*argc*/, TCHAR** /*argv[]*/)
{
	
}

void TildeHomeService::OnStop()
{
	m_logFile.close();
}

#include "service_installer.h"

#include <Windows.h>
#include <atlstr.h>

namespace
{
	class ServiceHandle
	{
	public:
		ServiceHandle(SC_HANDLE handle) : m_handle(handle)
		{
		}

		~ServiceHandle()
		{
			if (m_handle)
			{
				CloseServiceHandle(m_handle);
			}
		}

		operator SC_HANDLE() const
		{
			return m_handle;
		}

	private:
		SC_HANDLE m_handle = nullptr;
	};
}

//static
bool ServiceInstaller::Install(const ServiceBase& service)
{
	CString escapedPath;
	TCHAR* modulePath = escapedPath.GetBufferSetLength(MAX_PATH);

	if (::GetModuleFileName(nullptr, modulePath, MAX_PATH) == 0)
	{
		_tprintf(_T("Couldn't get module file name: %d\n"), GetLastError());
		return false;
	}

	escapedPath.ReleaseBuffer();
	escapedPath.Remove(_T('\"'));

	escapedPath = _T('\"') + escapedPath + _T('\"');

	ServiceHandle svcControlManager = ::OpenSCManager(nullptr, nullptr,
	                                                  SC_MANAGER_CONNECT |
	                                                  SC_MANAGER_CREATE_SERVICE);
	if (!svcControlManager)
	{
		_tprintf(_T("Couldn't open service control manager: %d\n"), GetLastError());
		return false;
	}

	const CString& depends = service.GetDependencies();
	const CString& acc = service.GetAccount();
	const CString& pass = service.GetPassword();

	const ServiceHandle serviceHandle = ::CreateService(svcControlManager,
	                                                    service.GetName(),
	                                                    service.GetDisplayName(),
	                                                    SERVICE_QUERY_STATUS,
	                                                    SERVICE_WIN32_OWN_PROCESS,
	                                                    service.GetStartType(),
	                                                    service.GetErrorControlType(),
	                                                    escapedPath,
	                                                    nullptr,
	                                                    nullptr,
	                                                    (depends.IsEmpty() ? nullptr : depends.GetString()),
	                                                    (acc.IsEmpty() ? nullptr : acc.GetString()),
	                                                    (pass.IsEmpty() ? nullptr : pass.GetString()));
	if (!serviceHandle)
	{
		_tprintf(_T("Couldn't create service: %d\n"), GetLastError());
		return false;
	}

	return true;
}

//static
bool ServiceInstaller::Uninstall(const ServiceBase& service)
{
	ServiceHandle svcControlManager = ::OpenSCManager(nullptr, nullptr,
	                                                  SC_MANAGER_CONNECT);

	if (!svcControlManager)
	{
		_tprintf(_T("Couldn't open service control manager: %d\n"), GetLastError());
		return false;
	}

	ServiceHandle serviceHandle = ::OpenService(svcControlManager, service.GetName(),
	                                            SERVICE_QUERY_STATUS |
	                                            SERVICE_STOP |
	                                            DELETE);

	if (!serviceHandle)
	{
		_tprintf(_T("Couldn't open service control manager: %d\n"), GetLastError());
		return false;
	}

	SERVICE_STATUS serviceStatus = {};
	if (ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus))
	{
		_tprintf(_T("Stoping service %s\n"), service.GetName());

		while (QueryServiceStatus(serviceHandle, &serviceStatus))
		{
			if (serviceStatus.dwCurrentState != SERVICE_STOP_PENDING)
			{
				break;
			}
		}

		if (serviceStatus.dwCurrentState != SERVICE_STOPPED)
		{
			_tprintf(_T("Failed to stop the service\n"));
		}
		else
		{
			_tprintf(_T("Service stopped\n"));
		}
	}
	else
	{
		_tprintf(_T("Didn't control service: %d\n"), GetLastError());
	}

	if (!DeleteService(serviceHandle))
	{
		_tprintf(_T("Failed to delete the service: %d\n"), GetLastError());
		return false;
	}

	return true;
}

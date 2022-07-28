#pragma once

#include "service_base.h"

class ServiceInstaller
{
public:
	static bool Install(const ServiceBase& service);
	static bool Uninstall(const ServiceBase& service);
private:
	ServiceInstaller()
	{
	}
};

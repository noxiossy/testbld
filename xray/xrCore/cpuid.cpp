#include "stdafx.h"
#include "cpuid.h"
#include <intrin.h>

_processor_info::_processor_info()
{
	int cpinfo[4];
	// detect cpu vendor
	__cpuid(cpinfo, 0);
	memcpy(vendor, &(cpinfo[1]), sizeof(int));
	memcpy(vendor + sizeof(int), &(cpinfo[3]), sizeof(int));
	memcpy(vendor + 2 * sizeof(int), &(cpinfo[2]), sizeof(int));

	// detect cpu model
	__cpuid(cpinfo, 0x80000002);
	memcpy(brand, cpinfo, sizeof(cpinfo));
	__cpuid(cpinfo, 0x80000003);
	memcpy(brand + sizeof(cpinfo), cpinfo, sizeof(cpinfo));
	__cpuid(cpinfo, 0x80000004);
	memcpy(brand + 2 * sizeof(cpinfo), cpinfo, sizeof(cpinfo));

	// detect cpu main features
	__cpuid(cpinfo, 1);
	stepping = cpinfo[0] & 0xf;
	model = (u8)((cpinfo[0] >> 4) & 0xf) | ((u8)((cpinfo[0] >> 16) & 0xf) << 4);
	family = (u8)((cpinfo[0] >> 8) & 0xf) | ((u8)((cpinfo[0] >> 20) & 0xff) << 4);
	m_f1_ECX = cpinfo[2];
	m_f1_EDX = cpinfo[3];

	__cpuid(cpinfo, 7);
	m_f7_EBX = cpinfo[1];
	m_f7_ECX = cpinfo[2];

	// and check 3DNow! support
	__cpuid(cpinfo, 0x80000001);
	m_f81_ECX = cpinfo[2];
	m_f81_EDX = cpinfo[3];

	// get version of OS
	DWORD dwMajorVersion = 0;
	DWORD dwVersion = 0;
	dwVersion = GetVersion();

	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

	if (dwMajorVersion <= 5)		// XP don't support SSE3+ instruction sets
	{
		m_f1_ECX[0] = 0;
		m_f1_ECX[9] = 0;
		m_f1_ECX[19] = 0;
		m_f1_ECX[20] = 0;
		m_f81_ECX[6] = 0;
		m_f1_ECX[28] = 0;
		m_f7_EBX[5] = 0;
	}

	// Number of processors
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	threadCount = siSysInfo.dwNumberOfProcessors;
}

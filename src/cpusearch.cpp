/*
 * brwatch - PVI diagnosis tool for B&R PLCs
 *
 * Repository: https://github.com/hilch/brwatch
 *
 * SPDX-License-Identifier: GPL-3.0-only
 * Licensed under the GNU General Public License version 3.
 * See LICENSE for details.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

// cpusearch.cpp
//
#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "main.h"
#include "cpusearch.h"
#include <time.h>
#include <algorithm>

static bool ScanCancelled(const CpuScanControl *control)
{
	return control != nullptr && control->cancelRequested.load();
}

static void ReportDiscoveryError(const char *message, CpuScanControl *control)
{
	if (control != nullptr)
		control->errors.emplace_back(message);
	else
		MessageBoxA(MainWindowGetHandle(), message, "CPU scan", MB_OK | MB_ICONERROR);
}

#ifdef __DEBUG__
static void PrintEthernetAdapters(const struct stAdapterInfo *adapters, int count)
{
	printf("CPU scan adapters (IPv4 addresses): %d\n", count);
	for (int i = 0; i < count; ++i)
	{
		printf("Adapter: %.*s\n  IP address: %.*s\n  Subnet mask: %.*s\n",
			   static_cast<int>(sizeof(adapters[i].description)), adapters[i].description,
			   static_cast<int>(sizeof(adapters[i].ipAddress)), adapters[i].ipAddress,
			   static_cast<int>(sizeof(adapters[i].subnetMask)), adapters[i].subnetMask);
	}
	fflush(stdout);
}
#endif

/* -----------------------------------------------------------------------------------------
	start searching CPUs via TCP port 11169
	----------------------------------------------------------------------------------------
*/
static bool IsCpuScanTarget(const CpuPortScanPlan &plan, std::uint32_t address)
{
	return plan.loopback || (address != 0 && address != plan.localAddress &&
		address != plan.subnetNetwork && address != plan.subnetBroadcast);
}

bool GetCpuPortScanPlan(const stAdapterInfo &adapter, unsigned hosts, CpuPortScanPlan &plan, int &error,
	bool allowBeyondSubnet)
{
	plan = {};
	error = WSAEINVAL;
	if (hosts != 256 && hosts != 512 && hosts != 1024 && hosts != 2048 && hosts != 16384)
		return false;
	IN_ADDR ip = {}, mask = {};
	if (InetPtonA(AF_INET, adapter.ipAddress, &ip) != 1 ||
		InetPtonA(AF_INET, adapter.subnetMask, &mask) != 1)
		return false;
	std::uint32_t localIp = ntohl(ip.s_addr);
	std::uint32_t subnetMask = ntohl(mask.s_addr);
	std::uint32_t inverseMask = ~subnetMask;
	if ((inverseMask & (inverseMask + 1)) != 0 || localIp == 0 || localIp >= 0xe0000000u)
		return false;
	plan.localAddress = localIp;
	plan.subnetNetwork = localIp & subnetMask;
	plan.subnetBroadcast = plan.subnetNetwork | inverseMask;
	if (localIp == INADDR_LOOPBACK)
	{
		plan.firstAddress = plan.lastAddress = localIp;
		plan.targetCount = 1;
		plan.loopback = true;
	}
	else
	{
		std::uint32_t blockFirst = localIp & ~(hosts - 1u);
		std::uint32_t blockLast = blockFirst + hosts - 1;
		bool exceedsSubnet = blockFirst < plan.subnetNetwork || blockLast > plan.subnetBroadcast;
		plan.limitedBySubnet = !allowBeyondSubnet && exceedsSubnet;
		plan.extendsBeyondSubnet = allowBeyondSubnet && exceedsSubnet;
		plan.firstAddress = allowBeyondSubnet ? blockFirst : (std::max)(blockFirst, plan.subnetNetwork + 1);
		plan.lastAddress = allowBeyondSubnet ? blockLast : (std::min)(blockLast, plan.subnetBroadcast - 1);
		for (std::uint32_t address = plan.firstAddress; address <= plan.lastAddress; ++address)
		{
			if (IsCpuScanTarget(plan, address))
				++plan.targetCount;
		}
	}
	error = 0;
	return true;
}

std::uint64_t CpuPortScanTimeoutBudgetMs(const CpuPortScanPlan &plan)
{
	return ((plan.targetCount + CpuScanBatchSize - 1) / CpuScanBatchSize) *
		static_cast<std::uint64_t>(CpuScanBatchTimeoutMs);
}

static bool IsOfflineCpuError(int error)
{
	return error == WSAECONNREFUSED || error == WSAETIMEDOUT ||
		error == WSAEHOSTUNREACH || error == WSAECONNRESET;
}

static bool IsCpuScanRejection(int error)
{
	return error == WSAENETUNREACH || error == WSAEACCES || error == WSAEADDRNOTAVAIL;
}

CpuPortScanResult SearchCpuViaPortScan(const stAdapterInfo &adapter,
	stEthernetCpuInfo *ethernetCpuInfo, int maxEntries, unsigned hosts, bool allowBeyondSubnet, CpuScanControl *control)
{
	CpuPortScanResult scan;
	if (ScanCancelled(control))
		return scan;
	CpuPortScanPlan plan;
	if (!GetCpuPortScanPlan(adapter, hosts, plan, scan.error, allowBeyondSubnet) || ethernetCpuInfo == nullptr || maxEntries <= 0)
	{
		scan.error = WSAEINVAL;
		scan.operation = "address range or output buffer";
		return scan;
	}
	WSADATA wsa;
	scan.error = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (scan.error != 0)
	{
		scan.operation = "WSAStartup";
		return scan;
	}
	bool abortAdapter = false;
	auto recordError = [&](const char *operation, int error, std::uint32_t address, bool stopAdapter = true) {
		scan.operation = operation;
		scan.error = error;
		scan.address = address;
		abortAdapter = stopAdapter;
	};
	auto recordConnectionError = [&](int error, std::uint32_t address) {
		bool outsideSubnet = address < plan.subnetNetwork || address > plan.subnetBroadcast;
		if (allowBeyondSubnet && outsideSubnet && IsCpuScanRejection(error))
		{
			++scan.rejectedTargets;
			if (scan.error == 0)
				recordError("connect", error, address, false);
		}
		else
			recordError("connect", error, address);
	};
	auto addCpu = [&](IN_ADDR address) {
		auto &cpu = ethernetCpuInfo[scan.entries++];
		memset(&cpu, 0, sizeof(cpu));
		strcpy(cpu.ipAddress, inet_ntoa(address));
		if (address.s_addr == htonl(plan.localAddress) ||
			(ntohl(address.s_addr) > plan.subnetNetwork && ntohl(address.s_addr) < plan.subnetBroadcast))
			strcpy(cpu.subnetMask, adapter.subnetMask);
	};

	std::uint32_t target = plan.firstAddress;
	while (plan.targetCount != 0 && target <= plan.lastAddress && scan.entries < maxEntries && !abortAdapter && !ScanCancelled(control))
	{
		SOCKET sockets[CpuScanBatchSize];
		IN_ADDR addresses[CpuScanBatchSize];
		int pending = 0;
		unsigned attempted = 0;
		while (target <= plan.lastAddress && attempted < CpuScanBatchSize && scan.entries < maxEntries && !ScanCancelled(control))
		{
			std::uint32_t targetIp = target++;
			if (!IsCpuScanTarget(plan, targetIp))
				continue;
			++attempted;
			SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (connection == INVALID_SOCKET)
			{
				recordError("socket", WSAGetLastError(), targetIp);
				break;
			}
			SOCKADDR_IN localAddress = {};
			localAddress.sin_family = AF_INET;
			localAddress.sin_addr.s_addr = htonl(plan.localAddress);
			if (bind(connection, reinterpret_cast<SOCKADDR *>(&localAddress), sizeof(localAddress)) == SOCKET_ERROR)
			{
				recordError("bind", WSAGetLastError(), plan.localAddress);
				closesocket(connection);
				break;
			}
			u_long nonblocking = 1;
			if (ioctlsocket(connection, FIONBIO, &nonblocking) == SOCKET_ERROR)
			{
				recordError("ioctlsocket", WSAGetLastError(), targetIp);
				closesocket(connection);
				break;
			}

			SOCKADDR_IN targetAddress = {};
			targetAddress.sin_family = AF_INET;
			targetAddress.sin_port = htons(11169);
			targetAddress.sin_addr.s_addr = htonl(targetIp);
			int result = connect(connection, reinterpret_cast<SOCKADDR *>(&targetAddress), sizeof(targetAddress));
			if (result == 0)
			{
				addCpu(targetAddress.sin_addr);
				closesocket(connection);
			}
			else
			{
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
				{
					sockets[pending] = connection;
					addresses[pending++] = targetAddress.sin_addr;
				}
				else
				{
					closesocket(connection);
					if (!IsOfflineCpuError(error))
					{
						recordConnectionError(error, targetIp);
						if (abortAdapter)
							break;
					}
				}
			}
		}

		DWORD started = GetTickCount();
		while (pending > 0 && scan.entries < maxEntries && !abortAdapter && !ScanCancelled(control))
		{
			DWORD elapsed = GetTickCount() - started;
			if (elapsed >= CpuScanBatchTimeoutMs)
				break;

			fd_set writable, failed;
			FD_ZERO(&writable);
			FD_ZERO(&failed);
			for (int index = 0; index < pending; ++index)
			{
				FD_SET(sockets[index], &writable);
				FD_SET(sockets[index], &failed);
			}
			DWORD waitMs = CpuScanBatchTimeoutMs - elapsed;
			if (control != nullptr)
				waitMs = (std::min)(waitMs, DWORD(50));
			struct timeval timeout = {0, static_cast<long>(waitMs * 1000)};
			int ready = select(0, nullptr, &writable, &failed, &timeout);
			if (ready == SOCKET_ERROR)
			{
				recordError("select", WSAGetLastError(), 0);
				break;
			}
			if (ready == 0)
			{
				if (control != nullptr)
					continue;
				break;
			}

			for (int index = pending - 1; index >= 0 && scan.entries < maxEntries; --index)
			{
				if (!FD_ISSET(sockets[index], &writable) && !FD_ISSET(sockets[index], &failed))
					continue;
				int error = 0;
				int errorSize = sizeof(error);
				if (getsockopt(sockets[index], SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&error), &errorSize) == SOCKET_ERROR)
				{
					recordError("getsockopt", WSAGetLastError(), ntohl(addresses[index].s_addr));
				}
				else if (error == 0 && FD_ISSET(sockets[index], &writable))
				{
					addCpu(addresses[index]);
				}
				else if (error != 0 && !IsOfflineCpuError(error))
				{
					recordConnectionError(error, ntohl(addresses[index].s_addr));
				}
				closesocket(sockets[index]);
				--pending;
				sockets[index] = sockets[pending];
				addresses[index] = addresses[pending];
				if (abortAdapter)
					break;
			}
		}
		for (int index = 0; index < pending; ++index)
			closesocket(sockets[index]);
		if (control != nullptr)
			control->completed.fetch_add(attempted);
	}

	WSACleanup();
	return scan;
}

/* -----------------------------------------------------------------------------------------
	get a list of available ethernet adapters
	----------------------------------------------------------------------------------------
*/
unsigned long ListCpuScanAdapters(std::vector<stAdapterInfo> &adapters)
{
	adapters = {{"127.0.0.1", "255.0.0.0", "Loopback (localhost)"}};
	DWORD size = 0;
	DWORD result = GetAdaptersInfo(nullptr, &size);
	if (result == ERROR_NO_DATA)
		return ERROR_SUCCESS;
	if (result != ERROR_BUFFER_OVERFLOW)
		return result;
	std::vector<unsigned char> buffer;
	for (unsigned attempt = 0; attempt < 3; ++attempt)
	{
		buffer.resize(size);
		result = GetAdaptersInfo(reinterpret_cast<IP_ADAPTER_INFO *>(buffer.data()), &size);
		if (result != ERROR_BUFFER_OVERFLOW)
			break;
	}
	if (result == ERROR_NO_DATA)
		return ERROR_SUCCESS;
	if (result != ERROR_SUCCESS)
		return result;
	for (auto next = reinterpret_cast<IP_ADAPTER_INFO *>(buffer.data()); next != nullptr; next = next->Next)
	{
		if (next->Type != MIB_IF_TYPE_ETHERNET)
			continue;
		for (auto ipaddr = &next->IpAddressList; ipaddr != nullptr; ipaddr = ipaddr->Next)
		{
			if (strcmp(ipaddr->IpAddress.String, "0.0.0.0") != 0)
			{
				stAdapterInfo adapter = {};
				snprintf(adapter.ipAddress, sizeof(adapter.ipAddress), "%s", ipaddr->IpAddress.String);
				snprintf(adapter.subnetMask, sizeof(adapter.subnetMask), "%s", ipaddr->IpMask.String);
				snprintf(adapter.description, sizeof(adapter.description), "%s", next->Description);
				adapters.push_back(adapter);
			}
		}
	}
	return ERROR_SUCCESS;
}

/* -----------------------------------------------------------------------------------------
	PVI Callback for SNMP line
	----------------------------------------------------------------------------------------
*/
static void WINAPI PviSnmpProc(WPARAM /* wParam */, LPARAM /* lParam */, LPVOID /* pData */, DWORD /* DataLen */, T_RESPONSE_INFO * /*pInfo*/) {

};

/* -----------------------------------------------------------------------------------------
	start searching CPUs via SNMP protocol
	----------------------------------------------------------------------------------------
*/
struct SnmpLink {
	DWORD id = 0;
	~SnmpLink()
	{
		if (id != 0)
			PviUnlink(id);
	}
};

int SearchCpuViaSnmp(struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries, CpuScanControl *control)
{
	if (ScanCancelled(control))
		return 0;
	if (ethernetCpuInfo == nullptr || maxEntries <= 0)
	{
		ReportDiscoveryError("Invalid SNMP scan output buffer.", control);
		return -1;
	}
	int entries = 0;
	LPVOID pviSnmpCallback = reinterpret_cast<LPVOID>(PviSnmpProc);
	memset(ethernetCpuInfo, 0, sizeof(struct stEthernetCpuInfo) * maxEntries);
	auto failed = [&](const char *operation, int error) {
		if (!ScanCancelled(control))
		{
			char message[180];
			snprintf(message, sizeof(message), "SNMP/PVI discovery failed during %s (error %d).", operation, error);
			ReportDiscoveryError(message, control);
		}
	};
	auto create = [&](SnmpLink &link, const char *name, DWORD type, const char *description) {
		if (ScanCancelled(control))
			return false;
		int result = PviCreate(&link.id, name, type, description, pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
		if (result != 0)
		{
			link.id = 0;
			failed(name, result);
			return false;
		}
		return !ScanCancelled(control);
	};
	auto read = [&](DWORD link, DWORD access, void *buffer, LONG size) {
		if (ScanCancelled(control))
			return false;
		int result = PviRead(link, access, nullptr, 0, buffer, size);
		if (result != 0)
		{
			failed("PviRead", result);
			return false;
		}
		return !ScanCancelled(control);
	};
	SnmpLink line, device;
	if (!create(line, "@Pvi/LnSNMP", POBJ_LINE, "CD=\"LNSNMP\"") ||
		!create(device, "@Pvi/LnSNMP/Device", POBJ_DEVICE, "CD=\"/IF=snmp /RT=3000\""))
		return entries;
	std::vector<char> buffer(65536);
	if (!read(device.id, POBJ_ACC_LIST_EXTERN, buffer.data(), static_cast<LONG>(buffer.size())))
		return entries;
	buffer.back() = '\0';
	for (char *entry = buffer.data(); entry != nullptr && *entry != '\0' &&
		entries < maxEntries && !ScanCancelled(control);)
	{
		char *next = strchr(entry, '\t');
		if (next != nullptr)
			*next++ = '\0';
		if (strstr(entry, KWDESC_OBJTYPE "=" KWOBJTYPE_STATION) != nullptr)
		{
			auto &cpu = ethernetCpuInfo[entries];
			snprintf(cpu.macAddress, sizeof(cpu.macAddress), "%.17s", entry);
			char description[64];
			snprintf(description, sizeof(description), "CD=\"/CN=%s\"", cpu.macAddress);
			SnmpLink station;
			if (!create(station, "@Pvi/LnSNMP/Device/Station", POBJ_STATION, description))
				break;
			int arState = 0;
			struct Property { const char *name; const char *value; void *buffer; LONG size; };
			Property properties[] = {
				{"Ip", "ipAddress", cpu.ipAddress, sizeof(cpu.ipAddress)},
				{"Subnet", "subnetMask", cpu.subnetMask, sizeof(cpu.subnetMask)},
				{"Gateway", "defaultGateway", cpu.gateway, sizeof(cpu.gateway)},
				{"snmpMode", "snmpMode", &cpu.SNMP_mode, sizeof(cpu.SNMP_mode)},
				{"inaActivated", "inaActivated", &cpu.INA_activated, sizeof(cpu.INA_activated)},
				{"inaNodeNumber", "inaNodeNumber", &cpu.INA_nodeNumber, sizeof(cpu.INA_nodeNumber)},
				{"inaPortNumber", "inaPortNumber", &cpu.INA_portNumber, sizeof(cpu.INA_portNumber)},
				{"ipMethod", "ipMethod", &cpu.ipMethod, sizeof(cpu.ipMethod)},
				{"arState", "arState", &arState, sizeof(arState)},
				{"targetTypeDescription", "targetTypeDescription", cpu.targetTypeDescription, sizeof(cpu.targetTypeDescription)},
				{"arVersion", "arVersion", cpu.arVersion, sizeof(cpu.arVersion)}
			};
			bool complete = true;
			for (const auto &property : properties)
			{
				std::string name = std::string("@Pvi/LnSNMP/Device/Station/") + property.name;
				std::string descriptor = std::string("CD=") + property.value;
				SnmpLink variable;
				if (!create(variable, name.c_str(), POBJ_PVAR, descriptor.c_str()) ||
					!read(variable.id, POBJ_ACC_DATA, property.buffer, property.size))
				{
					complete = false;
					break;
				}
			}
			if (!complete)
				break;
			static const char states[][sizeof(cpu.arState)] = {"(unknown)", "BOOT", "DIAG", "SERVICE", "RUN"};
			strcpy(cpu.arState, states[arState >= 1 && arState <= 4 ? arState : 0]);
			++entries;
		}
		entry = next;
	}
	return entries;
}

/* -----------------------------------------------------------------------------------------
	start searching CPUs connected by Ethernet
	----------------------------------------------------------------------------------------
*/
static void ReportCpuScanError(const stAdapterInfo &adapter, const CpuPortScanResult &scan, CpuScanControl *control)
{
	char systemMessage[256] = {};
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
		static_cast<DWORD>(scan.error), 0, systemMessage, sizeof(systemMessage), nullptr);
	char message[720];
	char address[20];
	snprintf(address, sizeof(address), "%u.%u.%u.%u", (scan.address >> 24) & 255,
		(scan.address >> 16) & 255, (scan.address >> 8) & 255, scan.address & 255);
	snprintf(message, sizeof(message),
		"Could not complete the TCP scan on %s (%s).\nOperation: %s\nAddress: %s\nError %d: %s\n"
		"Rejected out-of-subnet targets: %u.\n"
		"Check the adapter address, subnet, routing, and firewall permissions. Other adapters will still be scanned.",
		adapter.description, adapter.ipAddress, scan.operation,
		scan.address == 0 ? adapter.ipAddress : address, scan.error, systemMessage, scan.rejectedTargets);
	if (IsCpuScanRejection(scan.error) &&
		(strcmp(scan.operation, "bind") == 0 || strcmp(scan.operation, "connect") == 0))
	{
		OutputDebugStringA(message);
		return;
	}
	ReportDiscoveryError(message, control);
}

int SearchEthernetCpus(stEthernetCpuInfo *ethernetCpuInfo, int maxEntries, unsigned hosts, bool allowBeyondSubnet,
	CpuScanControl *control)
{
	if (ScanCancelled(control))
		return 0;
	if (ethernetCpuInfo == nullptr || maxEntries <= 0)
	{
		ReportDiscoveryError("Invalid CPU scan output buffer.", control);
		return -1;
	}
	std::vector<stAdapterInfo> adapters;
	unsigned long error = ListCpuScanAdapters(adapters);
	if (error != ERROR_SUCCESS)
	{
		char message[160];
		snprintf(message, sizeof(message), "Could not enumerate Ethernet adapters (error %lu).", error);
		ReportDiscoveryError(message, control);
		return -1;
	}
	CpuPortScanPlan validationPlan;
	int validationError;
	if (!GetCpuPortScanPlan(adapters.front(), hosts, validationPlan, validationError, allowBeyondSubnet))
	{
		ReportDiscoveryError("Invalid CPU scan address-block size.", control);
		return -1;
	}

#ifdef __DEBUG__
	PrintEthernetAdapters(adapters.data(), static_cast<int>(adapters.size()));
#endif
	if (control != nullptr)
	{
		unsigned total = 0;
		for (const auto &adapter : adapters)
		{
			CpuPortScanPlan plan;
			int error;
			if (GetCpuPortScanPlan(adapter, hosts, plan, error, allowBeyondSubnet))
				total += plan.targetCount;
		}
		control->total.store(total);
		control->phase.store(CpuScanPhase::Snmp);
	}
	int noOfCPUs = SearchCpuViaSnmp(ethernetCpuInfo, maxEntries, control);
	if (ScanCancelled(control) || noOfCPUs < 0)
		return noOfCPUs;
	if (control != nullptr)
		control->phase.store(CpuScanPhase::Tcp);
	std::vector<stEthernetCpuInfo> candidates(maxEntries);
	for (const auto &adapter : adapters)
	{
		if (noOfCPUs >= maxEntries || ScanCancelled(control))
			break;
		unsigned started = control != nullptr ? control->completed.load() : 0;
		CpuPortScanResult scan = SearchCpuViaPortScan(adapter, candidates.data(), maxEntries, hosts, allowBeyondSubnet, control);
		if (ScanCancelled(control))
			break;
		if (scan.error != 0)
			ReportCpuScanError(adapter, scan, control);
		if (control != nullptr)
		{
			CpuPortScanPlan plan;
			int error;
			if (GetCpuPortScanPlan(adapter, hosts, plan, error, allowBeyondSubnet))
				control->completed.store(started + plan.targetCount);
		}
		for (int i = 0; i < scan.entries; ++i)
		{
#ifdef __DEBUG__
			printf("%s / %s\n", candidates[i].ipAddress, candidates[i].subnetMask);
#endif
			bool found = false;
			for (int n = 0; n < noOfCPUs; ++n)
			{
				if (strcmp(candidates[i].ipAddress, ethernetCpuInfo[n].ipAddress) == 0)
				{
					found = true;
					break;
				}
			}
			if (!found && noOfCPUs < maxEntries)
				ethernetCpuInfo[noOfCPUs++] = candidates[i];
		}
	}
	return noOfCPUs;
}

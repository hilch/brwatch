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

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "main.h"
#include "watch_listview.h"
#include "object_treeview.h"
#include "dlg_about.h"
#include "dlg_cpuscan.h"
#include "resource.h"
#include "settings.h"
#include <shellapi.h>
#include <type_traits>
#include <vector>
#include <string>
#include <map>

static_assert(std::is_standard_layout<PviObject>::value, "Keep the Win32-facing data layout");
static_assert(std::is_trivially_copyable<PviObject>::value, "Object descriptions remain value-copyable");

typedef void(WINAPI *PviTestCallback)(WPARAM, LPARAM, LPVOID, DWORD, T_RESPONSE_INFO *);

static DWORD nextLinkId = 1;
static PviTestCallback callback;
static const char *readResponse;
static unsigned dataNotifications;
static unsigned errorNotifications;
static DWORD detachedLinkId;
static unsigned expectedErrorMessages;
static unsigned createRequests;
static const char *expectedMessageCaption = "Error";
static std::string lastMessage;
static unsigned browserLaunches;
static INT_PTR browserResult = 33;
static unsigned editDialogs;
static UINT editDialogResource;
static PviObject *editDialogObject;
static HMENU lastPopupMenu;
static HWND lastPopupOwner;
static std::vector<stAdapterInfo> testAdapters;
static ULONG adapterEnumerationError;
static unsigned adapterOverflowRetries;
static bool secondaryAdapterAddress;
static bool wirelessAdapter;
static unsigned scanDialogs;
static INT_PTR scanDialogResult = IDCANCEL;
static unsigned scanDialogHosts = CpuScanDefaultHosts;
static bool scanDialogBeyondSubnet;
static unsigned startupCalls;
static unsigned cleanupCalls;
static unsigned selectCalls;
static int socketLastError;
static int socketFailureError = WSAEADDRNOTAVAIL;
static const char *socketFailure;
static unsigned connectionsBeforeFailure;
static int pendingConnectError;
static std::uint32_t failingBindAddress;
static std::uint32_t failedTargetFirst;
static std::uint32_t failedTargetLast;
static int targetRoutingError;
static std::map<SOCKET, std::uint32_t> socketTargets;
static std::vector<SOCKET> openSockets;
static std::vector<std::uint32_t> scanTargets;
static std::vector<std::uint32_t> bindAddresses;
enum class SocketMode { Timeout, Immediate, Pending, Refused, PendingRefused };
static SocketMode socketMode = SocketMode::Timeout;
static DWORD mockTicks;
static unsigned readRequests;
static unsigned unlinkRequests;
static CpuScanControl *cancelOnRead;
static unsigned cancelReadRequest;
static CpuScanControl *cancelOnConnect;
static CpuScanControl *cancelOnSelect;
static HANDLE readEntered;
static HANDLE readRelease;
static HANDLE selectEntered;
static HANDLE selectRelease;
static bool snmpPropertyResponses;
static int snmpArState = 4;
static std::map<DWORD, std::string> snmpProperties;
static DWORD testUiThread = GetCurrentThreadId();

static void Check(bool condition, const char *expression, int line)
{
	if (!condition)
	{
		fprintf(stderr, "Line %d: %s\n", line, expression);
		exit(EXIT_FAILURE);
	}
}

#define CHECK(expression) Check((expression), #expression, __LINE__)

static int WINAPI TestMessageBoxA(HWND, LPCSTR text, LPCSTR caption, UINT)
{
	CHECK(GetCurrentThreadId() == testUiThread);
	CHECK(expectedErrorMessages != 0);
	CHECK(strcmp(caption, expectedMessageCaption) == 0);
	lastMessage = text;
	--expectedErrorMessages;
	return IDOK;
}

static HINSTANCE WINAPI TestShellExecuteW(HWND, LPCWSTR operation, LPCWSTR url, LPCWSTR, LPCWSTR, INT show)
{
	CHECK(lstrcmpW(operation, L"open") == 0);
	CHECK(lstrcmpW(url, L"https://github.com/hilch/brwatch") == 0);
	CHECK(show == SW_SHOWNORMAL);
	++browserLaunches;
	return reinterpret_cast<HINSTANCE>(browserResult);
}

static INT_PTR WINAPI TestDialogBoxParamA(HINSTANCE, LPCSTR resource, HWND, DLGPROC, LPARAM data)
{
	CHECK(IS_INTRESOURCE(resource));
	if (reinterpret_cast<ULONG_PTR>(resource) == DLG_CPU_SCAN)
	{
		++scanDialogs;
		auto scan = reinterpret_cast<CpuScanDialogData *>(data);
		CHECK(!scan->adapters.empty());
		scan->hosts = scanDialogHosts;
		scan->allowBeyondSubnet = scanDialogBeyondSubnet;
		return scanDialogResult;
	}
	++editDialogs;
	editDialogResource = static_cast<UINT>(reinterpret_cast<ULONG_PTR>(resource));
	editDialogObject = reinterpret_cast<PviObject *>(data);
	return IDCANCEL;
}

static BOOL WINAPI TestTrackPopupMenu(HMENU menu, UINT, int, int, int, HWND owner, const RECT *)
{
	lastPopupMenu = menu;
	lastPopupOwner = owner;
	return TRUE;
}

extern "C" ULONG WINAPI GetAdaptersInfo(PIP_ADAPTER_INFO info, PULONG size)
{
	if (adapterEnumerationError != 0)
		return adapterEnumerationError;
	if (testAdapters.empty())
		return ERROR_NO_DATA;
	ULONG required = static_cast<ULONG>(testAdapters.size() * sizeof(IP_ADAPTER_INFO) +
		(secondaryAdapterAddress ? sizeof(IP_ADDR_STRING) : 0));
	if (info != nullptr && adapterOverflowRetries != 0)
	{
		--adapterOverflowRetries;
		*size += sizeof(IP_ADAPTER_INFO);
		return ERROR_BUFFER_OVERFLOW;
	}
	if (info == nullptr || *size < required)
	{
		*size = required;
		return ERROR_BUFFER_OVERFLOW;
	}
	memset(info, 0, required);
	for (size_t i = 0; i < testAdapters.size(); ++i)
	{
		info[i].Next = i + 1 == testAdapters.size() ? nullptr : &info[i + 1];
		info[i].Type = wirelessAdapter && i + 1 == testAdapters.size() ? IF_TYPE_IEEE80211 : MIB_IF_TYPE_ETHERNET;
		strcpy(info[i].Description, testAdapters[i].description);
		strcpy(info[i].IpAddressList.IpAddress.String, testAdapters[i].ipAddress);
		strcpy(info[i].IpAddressList.IpMask.String, testAdapters[i].subnetMask);
	}
	if (secondaryAdapterAddress)
	{
		auto secondary = reinterpret_cast<IP_ADDR_STRING *>(info + testAdapters.size());
		strcpy(secondary->IpAddress.String, "10.2.3.4");
		strcpy(secondary->IpMask.String, "255.255.255.0");
		info[0].IpAddressList.Next = secondary;
	}
	return ERROR_SUCCESS;
}

static bool FailSocketOperation(const char *operation)
{
	if (socketFailure != nullptr && strcmp(socketFailure, operation) == 0 &&
		scanTargets.size() >= connectionsBeforeFailure)
	{
		socketLastError = socketFailureError;
		return true;
	}
	return false;
}

static int WSAAPI TestWSAStartup(WORD version, LPWSADATA)
{
	CHECK(version == MAKEWORD(2, 2));
	++startupCalls;
	return FailSocketOperation("WSAStartup") ? socketFailureError : 0;
}

static int WSAAPI TestWSACleanup()
{
	CHECK(openSockets.empty());
	++cleanupCalls;
	return 0;
}

static int WSAAPI TestWSAGetLastError()
{
	return socketLastError;
}

static SOCKET WSAAPI TestSocket(int family, int type, int protocol)
{
	CHECK(family == AF_INET && type == SOCK_STREAM && protocol == IPPROTO_TCP);
	if (FailSocketOperation("socket"))
		return INVALID_SOCKET;
	static SOCKET nextSocket = 100;
	SOCKET connection = nextSocket++;
	openSockets.push_back(connection);
	CHECK(openSockets.size() <= CpuScanBatchSize);
	return connection;
}

static int WSAAPI TestCloseSocket(SOCKET connection)
{
	for (auto it = openSockets.begin(); it != openSockets.end(); ++it)
	{
		if (*it == connection)
		{
			openSockets.erase(it);
			socketTargets.erase(connection);
			return 0;
		}
	}
	CHECK(false);
	return SOCKET_ERROR;
}

static int WSAAPI TestBind(SOCKET, const sockaddr *address, int size)
{
	CHECK(size == sizeof(sockaddr_in));
	auto local = reinterpret_cast<const sockaddr_in *>(address);
	CHECK(local->sin_family == AF_INET && local->sin_port == 0);
	bindAddresses.push_back(ntohl(local->sin_addr.s_addr));
	return (failingBindAddress == 0 || failingBindAddress == bindAddresses.back()) &&
		FailSocketOperation("bind") ? SOCKET_ERROR : 0;
}

static int WSAAPI TestIoctlSocket(SOCKET, long command, u_long *value)
{
	CHECK(command == static_cast<long>(FIONBIO) && *value == 1);
	return FailSocketOperation("ioctlsocket") ? SOCKET_ERROR : 0;
}

static int WSAAPI TestConnect(SOCKET connection, const sockaddr *address, int size)
{
	CHECK(size == sizeof(sockaddr_in));
	auto target = reinterpret_cast<const sockaddr_in *>(address);
	CHECK(target->sin_family == AF_INET && ntohs(target->sin_port) == 11169);
	bool fail = FailSocketOperation("connect");
	scanTargets.push_back(ntohl(target->sin_addr.s_addr));
	socketTargets[connection] = scanTargets.back();
	if (cancelOnConnect != nullptr)
		cancelOnConnect->cancelRequested.store(true);
	if (fail)
		return SOCKET_ERROR;
	if (targetRoutingError != 0 && scanTargets.back() >= failedTargetFirst &&
		scanTargets.back() <= failedTargetLast && socketMode != SocketMode::Pending)
	{
		socketLastError = targetRoutingError;
		return SOCKET_ERROR;
	}
	if (socketMode == SocketMode::Immediate)
		return 0;
	socketLastError = socketMode == SocketMode::Refused ? WSAECONNREFUSED : WSAEWOULDBLOCK;
	return SOCKET_ERROR;
}

static int WSAAPI TestSelect(int, fd_set *readable, fd_set *writable, fd_set *failed, const timeval *timeout)
{
	CHECK(readable == nullptr);
	CHECK(writable->fd_count == failed->fd_count && writable->fd_count <= CpuScanBatchSize);
	CHECK(timeout->tv_sec == 0 && timeout->tv_usec > 0 &&
		timeout->tv_usec <= static_cast<long>(CpuScanBatchTimeoutMs * 1000));
	++selectCalls;
	if (selectEntered != nullptr && selectCalls == 2)
	{
		CHECK(SetEvent(selectEntered));
		CHECK(WaitForSingleObject(selectRelease, 5000) == WAIT_OBJECT_0);
	}
	if (cancelOnSelect != nullptr)
		cancelOnSelect->cancelRequested.store(true);
	if (FailSocketOperation("select"))
		return SOCKET_ERROR;
	failed->fd_count = 0;
	if (socketMode == SocketMode::Timeout)
	{
		mockTicks += CpuScanBatchTimeoutMs;
		return 0;
	}
	return static_cast<int>(writable->fd_count);
}

static int WSAAPI TestGetSockOpt(SOCKET connection, int level, int option, char *value, int *size)
{
	CHECK(level == SOL_SOCKET && option == SO_ERROR && *size == sizeof(int));
	if (FailSocketOperation("getsockopt"))
		return SOCKET_ERROR;
	*reinterpret_cast<int *>(value) = pendingConnectError != 0 ? pendingConnectError :
		(socketMode == SocketMode::PendingRefused ? WSAECONNREFUSED : 0);
	if (targetRoutingError != 0 && socketTargets.at(connection) >= failedTargetFirst &&
		socketTargets.at(connection) <= failedTargetLast)
		*reinterpret_cast<int *>(value) = targetRoutingError;
	return 0;
}

static DWORD WINAPI TestGetTickCount()
{
	return mockTicks;
}

extern "C"
{
	decltype(&MessageBoxA) __imp_MessageBoxA = TestMessageBoxA;
	decltype(&ShellExecuteW) __imp_ShellExecuteW = TestShellExecuteW;
	decltype(&DialogBoxParamA) __imp_DialogBoxParamA = TestDialogBoxParamA;
	decltype(&TrackPopupMenu) __imp_TrackPopupMenu = TestTrackPopupMenu;
	decltype(&WSAStartup) __imp_WSAStartup = TestWSAStartup;
	decltype(&WSACleanup) __imp_WSACleanup = TestWSACleanup;
	decltype(&WSAGetLastError) __imp_WSAGetLastError = TestWSAGetLastError;
	decltype(&socket) __imp_socket = TestSocket;
	decltype(&closesocket) __imp_closesocket = TestCloseSocket;
	decltype(&bind) __imp_bind = TestBind;
	decltype(&ioctlsocket) __imp_ioctlsocket = TestIoctlSocket;
	decltype(&connect) __imp_connect = TestConnect;
	decltype(&select) __imp_select = TestSelect;
	decltype(&getsockopt) __imp_getsockopt = TestGetSockOpt;
	decltype(&GetTickCount) __imp_GetTickCount = TestGetTickCount;
}

extern "C" INT WINAPI PviInitialize(LONG, LONG, LPCSTR, LPVOID)
{
	return 0;
}

extern "C" INT WINAPI PviDeinitialize()
{
	return 0;
}

extern "C" VOID WINAPI PviGetVersion(LPSTR buffer, DWORD size)
{
	snprintf(buffer, size, "PVI test");
}

extern "C" INT WINAPI PviSetGlobEventMsg(DWORD, LPVOID, DWORD, LPARAM)
{
	return 0;
}

extern "C" INT WINAPI PviCreate(LPDWORD link, LPCSTR name, DWORD, LPCSTR descriptor,
							   LPVOID eventCallback, DWORD, LPARAM, LPCSTR)
{
	++createRequests;
	*link = nextLinkId++;
	if (snmpPropertyResponses && strstr(name, "@Pvi/LnSNMP/Device/Station/") == name)
		snmpProperties[*link] = descriptor;
	callback = reinterpret_cast<PviTestCallback>(eventCallback);
	return 0;
}

extern "C" INT WINAPI PviRead(DWORD link, DWORD access, LPVOID, LONG, LPVOID buffer, LONG size)
{
	++readRequests;
	if (readEntered != nullptr)
	{
		CHECK(SetEvent(readEntered));
		CHECK(WaitForSingleObject(readRelease, 5000) == WAIT_OBJECT_0);
	}
	if (cancelOnRead != nullptr && readRequests == cancelReadRequest)
		cancelOnRead->cancelRequested.store(true);
	if (snmpPropertyResponses && access == POBJ_ACC_DATA)
	{
		memset(buffer, 0, size);
		const auto &property = snmpProperties.at(link);
		const char *text = nullptr;
		if (property == "CD=ipAddress") text = "192.168.10.2";
		else if (property == "CD=subnetMask") text = "255.255.255.0";
		else if (property == "CD=defaultGateway") text = "192.168.10.1";
		else if (property == "CD=targetTypeDescription") text = "Test CPU";
		else if (property == "CD=arVersion") text = "1.0";
		else if (property == "CD=arState")
		{
			CHECK(size == sizeof(int));
			*static_cast<int *>(buffer) = snmpArState;
		}
		if (text != nullptr)
			snprintf(static_cast<char *>(buffer), size, "%s", text);
		return 0;
	}
	CHECK(readResponse != nullptr);
	CHECK(strlen(readResponse) < static_cast<size_t>(size));
	strcpy(static_cast<char *>(buffer), readResponse);
	return 0;
}

extern "C" INT WINAPI PviReadRequest(DWORD, DWORD, LPVOID, DWORD, LPARAM)
{
	return 0;
}

extern "C" INT WINAPI PviDelete(LPCSTR)
{
	return 0;
}

extern "C" INT WINAPI PviUnlink(DWORD linkid)
{
	++unlinkRequests;
	snmpProperties.erase(linkid);
	if (detachedLinkId != 0 && linkid == detachedLinkId)
	{
		CHECK(PviObject::FindByLinkId(linkid) == nullptr);
		T_RESPONSE_INFO info = {};
		info.nType = POBJ_EVENT_ERROR;
		info.LinkID = linkid;
		info.ErrCode = 123;
		unsigned previousErrors = errorNotifications;
		callback(0, 0, nullptr, 0, &info);
		CHECK(errorNotifications == previousErrors);
	}
	return 0;
}

static void DataChanged(PviObject *)
{
	++dataNotifications;
}

static void WatchDataChanged(PviObject *object)
{
	watchListView.UpdateValue(object);
}

static void ErrorChanged(PviObject *, int)
{
	++errorNotifications;
}

static PviObject *AddObject(const char *name, const char *descriptor, T_POBJ_TYPE type)
{
	PviObject description;
	strcpy(description.name, name);
	strcpy(description.descriptor, descriptor);
	description.type = type;
	PviObject *object = description.Add(FALSE);
	CHECK(object != nullptr);
	CHECK(object != &description);
	CHECK(PviObject::FindByName(name) == object);
	return object;
}

static void CheckRegistryGrowth(PviObject *watched, PviObject *cpu, PviObject *task, PviObject *axisName)
{
	const char *retainedName = watched->name;
	int previousCount = PviObject::Count();
	std::vector<PviObject *> added;
	for (unsigned i = 0; i < 2048; ++i)
	{
		char name[80];
		snprintf(name, sizeof(name), "@Pvi/Stress/%u", i);
		added.push_back(AddObject(name, "Stress", POBJ_TASK));
	}
	CHECK(PviObject::Count() == previousCount + 2048);
	CHECK(PviObject::FindByName(retainedName) == watched);
	CHECK(PviObject::FindByLinkId(watched->linkid) == watched);
	CHECK(PviObject::FindByName("@Pvi/Device/CPU") == cpu);
	CHECK(task->ex.task.cpu == cpu);
	CHECK(axisName->ex.pv.cpu == cpu && axisName->ex.pv.task == task);

	for (unsigned i : {0u, 1024u, 2047u})
	{
		added[i]->Remove();
		char name[80];
		snprintf(name, sizeof(name), "@Pvi/Stress/%u", i);
		CHECK(PviObject::FindByName(name) == nullptr);
	}
	CHECK(PviObject::Count() == previousCount + 2045);
	CHECK(PviObject::FindByName("@Pvi/Stress/1025") == added[1025]);
	CHECK(PviObject::FindByName(retainedName) == watched);
	CHECK(axisName->ex.pv.cpu == cpu && axisName->ex.pv.task == task);

	T_RESPONSE_INFO info = {};
	info.nType = POBJ_EVENT_DATA;
	info.LinkID = watched->linkid;
	LONG value = 84;
	callback(0, 0, &value, sizeof(value), &info);
	CHECK(*static_cast<LONG *>(watched->ex.pv.pvalue) == value);
}

static void CheckCursorRemoval()
{
	PviObject *parent = AddObject("@Pvi/Cursor", "Cursor", POBJ_TASK);
	PviObject *first = AddObject("@Pvi/Cursor/First", "First", POBJ_TASK);
	PviObject *second = AddObject("@Pvi/Cursor/Second", "Second", POBJ_TASK);
	PviObject *third = AddObject("@Pvi/Cursor/Third", "Third", POBJ_TASK);
	CHECK(parent->FindChild(TRUE) == first);
	PviObject::GetNext(TRUE);
	PviObject *object;
	while ((object = PviObject::GetNext(FALSE)) != first)
		CHECK(object != nullptr);
	CHECK(first->Remove() == parent);
	CHECK(PviObject::GetNext(FALSE) == second);
	CHECK(parent->FindChild(FALSE) == second);
	second->Remove();
	CHECK(PviObject::GetNext(FALSE) == third);
	CHECK(parent->FindChild(FALSE) == third);
	CHECK(PviObject::GetNext(FALSE) == nullptr);
	CHECK(parent->FindChild(FALSE) == nullptr);
	third->Remove();
	parent->Remove();
}

static void CheckStringCallbacks(PVIPVARTYPE type)
{
	PviObject *object = AddObject("@Pvi/StringBounds", "StringBounds", POBJ_PVAR);
	object->ex.pv.type = type;
	object->ex.pv.length = 32;
	object->ex.pv.dimension = 1;
	CHECK(object->Watch(TRUE) == object);
	object->ex.pv.length = 8;
	auto bytes = static_cast<unsigned char *>(object->ex.pv.pvalue);
	memset(bytes, 0x7e, 32);
	T_RESPONSE_INFO info = {};
	info.nType = POBJ_EVENT_DATA;
	info.LinkID = object->linkid;

	char oversized[16];
	memset(oversized, 'X', sizeof(oversized));
	unsigned previousNotifications = dataNotifications;
	expectedErrorMessages = 1;
	callback(0, 0, oversized, sizeof(oversized), &info);
	for (unsigned i = 8; i < 32; ++i)
		CHECK(bytes[i] == 0x7e);
	for (unsigned i = 0; i < 8; ++i)
		CHECK(bytes[i] == 0x7e);
	CHECK(expectedErrorMessages == 0);
	CHECK(dataNotifications == previousNotifications);

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	auto pages = static_cast<char *>(VirtualAlloc(nullptr, systemInfo.dwPageSize * 2,
												MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	CHECK(pages != nullptr);
	DWORD previousProtection;
	CHECK(VirtualProtect(pages + systemInfo.dwPageSize, systemInfo.dwPageSize,
						 PAGE_NOACCESS, &previousProtection) != 0);
	DWORD unitSize = type == BR_WSTRING ? sizeof(WCHAR) : sizeof(char);
	DWORD shortLength = unitSize * 2;
	char *shortData = pages + systemInfo.dwPageSize - shortLength;
	memset(shortData, 0, shortLength);
	shortData[0] = 'A';
	callback(0, 0, shortData, shortLength, &info);
	CHECK(bytes[0] == 'A');
	for (unsigned i = 1; i < object->ex.pv.length; ++i)
		CHECK(bytes[i] == 0);
	CHECK(VirtualFree(pages, 0, MEM_RELEASE) != 0);

	char unterminated[8];
	memset(unterminated, 'X', sizeof(unterminated));
	callback(0, 0, unterminated, sizeof(unterminated), &info);
	for (unsigned i = 0; i < 8 - unitSize; ++i)
		CHECK(bytes[i] == 'X');
	for (unsigned i = 8 - unitSize; i < 8; ++i)
		CHECK(bytes[i] == 0);
	for (unsigned i = 8; i < 32; ++i)
		CHECK(bytes[i] == 0x7e);
	object->Remove();
}

static void CheckStringListInsertion(PviObject *task)
{
	InitCommonControls();
	HWND parent = CreateWindowExA(0, "STATIC", "", WS_OVERLAPPEDWINDOW,
								 0, 0, 640, 480, nullptr, nullptr, GetModuleHandleA(nullptr), nullptr);
	CHECK(parent != nullptr);
	CHECK(watchListView.Create(GetModuleHandleA(nullptr), parent) != nullptr);

	PviObject *array = AddObject("@Pvi/StringList/errorTexts", "gAxisHandling.ax[0].errorText", POBJ_PVAR);
	array->ex.pv.type = BR_STRING;
	array->ex.pv.pdatatype = "STRING";
	array->ex.pv.dimension = 15;
	array->ex.pv.length = 121;
	array->ex.pv.task = task;
	array->watchsort = 0;
	unsigned previousCreates = createRequests;
	expectedMessageCaption = "Watch list";
	expectedErrorMessages = 1;
	watchListView.InsertPVIObjects(array);
	CHECK(ListView_GetItemCount(watchListView.GetHandle()) == 0);
	CHECK(expectedErrorMessages == 0);
	CHECK(createRequests == previousCreates);
	CHECK(array->watchsort == -1 && array->linkid == 0);
	CHECK(array->ex.pv.pvalue == nullptr);
	CHECK(array->ex.pv.dimension == 15 && array->ex.pv.length == 121);

	expectedErrorMessages = 1;
	CHECK(array->Watch(TRUE) == nullptr);
	CHECK(expectedErrorMessages == 0);
	CHECK(createRequests == previousCreates);
	array->Expand();
	PviObject *element = array->FindChild(TRUE);
	CHECK(element != nullptr && element->ex.pv.dimension == 1);
	CHECK(element->ex.pv.length == 121);
	element->watchsort = 0;
	watchListView.InsertPVIObjects(element);
	CHECK(ListView_GetItemCount(watchListView.GetHandle()) == 1);
	CHECK(element->ex.pv.pvalue != nullptr);
	CHECK(ListView_DeleteAllItems(watchListView.GetHandle()) != 0);
	element->Remove();

	array->ex.pv.type = BR_STRUCT;
	array->ex.pv.dimension = 1;
	previousCreates = createRequests;
	expectedErrorMessages = 1;
	CHECK(array->Watch(TRUE) == nullptr);
	CHECK(expectedErrorMessages == 0);
	CHECK(createRequests == previousCreates);
	CHECK(array->ex.pv.pvalue == nullptr);
	array->Remove();
	expectedMessageCaption = "Error";

	PviObject *object = AddObject("@Pvi/StringList/errorText", "gAxisHandling.ax[0].errorText", POBJ_PVAR);
	object->ex.pv.type = BR_STRING;
	object->ex.pv.pdatatype = "STRING";
	object->ex.pv.dimension = 1;
	object->ex.pv.length = 4097;
	object->ex.pv.task = task;
	object->watchsort = 0;
	watchListView.InsertPVIObjects(object);
	CHECK(ListView_GetItemCount(watchListView.GetHandle()) == 1);
	CHECK(object->ex.pv.pvalue != nullptr);
	pvi_interface_notify.cbdata_changed = WatchDataChanged;
	T_RESPONSE_INFO info = {};
	info.nType = POBJ_EVENT_DATA;
	info.LinkID = object->linkid;
	std::vector<char> data(object->ex.pv.length, '\1');
	data.back() = 0;
	std::vector<char> displayed(4096 * 5 + 1);
	for (BOOL oem : {FALSE, TRUE})
	{
		object->gui_info.interpret_as_oem = oem;
		callback(0, 0, data.data(), static_cast<DWORD>(data.size()), &info);
		LVITEMA item = {};
		item.iSubItem = 3;
		item.pszText = displayed.data();
		item.cchTextMax = static_cast<int>(displayed.size());
		CHECK(SendMessageA(watchListView.GetHandle(), LVM_GETITEMTEXTA, 0,
						   reinterpret_cast<LPARAM>(&item)) == 4096 * 5);
		for (unsigned i = 0; i < 4096; ++i)
			CHECK(memcmp(displayed.data() + i * 5, "\\x001", 5) == 0);
		CHECK(displayed.back() == 0);
	}
	pvi_interface_notify.cbdata_changed = DataChanged;
	CHECK(DestroyWindow(parent) != 0);
	object->Remove();
}

static void CheckObjectTreeView(PviObject *cpu, PviObject *task, const char *testDirectory)
{
	HINSTANCE instance = GetModuleHandleA(nullptr);
	HWND parent = CreateWindowExA(0, "STATIC", "", WS_OVERLAPPEDWINDOW,
								 0, 0, 640, 480, nullptr, nullptr, instance, nullptr);
	CHECK(parent != nullptr);
	ObjectTreeView first;
	ObjectTreeView second;
	CHECK(first.GetHandle() == nullptr && second.GetHandle() == nullptr);
	CHECK(first.Create(instance, parent) != nullptr);
	CHECK(second.Create(instance, parent) != nullptr);
	CHECK(first.GetHandle() != second.GetHandle());
	CHECK(MoveWindow(first.GetHandle(), 0, 0, 300, 240, FALSE) != 0);
	CHECK(MoveWindow(second.GetHandle(), 300, 0, 300, 240, FALSE) != 0);
	CHECK(watchListView.Create(instance, parent) != nullptr);

	PviObject *array = AddObject("@Pvi/Tree/Values", "Values", POBJ_PVAR);
	array->ex.pv.type = BR_DINT;
	array->ex.pv.pdatatype = "DINT";
	array->ex.pv.dimension = 3;
	array->ex.pv.length = sizeof(LONG);
	array->ex.pv.task = task;
	array->ex.pv.cpu = cpu;
	HTREEITEM root = first.AddItem(TVI_ROOT, array->descriptor, reinterpret_cast<LPARAM>(array->name), 0);
	CHECK(root != nullptr);
	CHECK(second.AddItem(TVI_ROOT, array->descriptor, reinterpret_cast<LPARAM>(array->name), 0) != nullptr);
	NMTREEVIEWA selection = {};
	selection.hdr.hwndFrom = first.GetHandle();
	selection.hdr.code = TVN_SELCHANGEDA;
	selection.itemNew.hItem = root;
	selection.itemNew.lParam = reinterpret_cast<LPARAM>(array->name);
	CHECK(second.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 1);
	CHECK(first.HandleMessages(WM_SIZE, 0, 0) == 1);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	CHECK(TreeView_GetCount(first.GetHandle()) == 4);
	CHECK(TreeView_GetCount(second.GetHandle()) == 1);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	CHECK(TreeView_GetCount(first.GetHandle()) == 4);
	char label[80];
	TVITEMA item = {};
	item.mask = TVIF_TEXT;
	item.hItem = root;
	item.pszText = label;
	item.cchTextMax = sizeof(label);
	CHECK(SendMessageA(first.GetHandle(), TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item)) != 0);
	CHECK(strcmp(label, "Values   :DINT[3]") == 0);

	NMHDR doubleClick = {};
	doubleClick.hwndFrom = first.GetHandle();
	doubleClick.code = NM_DBLCLK;
	CHECK(TreeView_SelectItem(first.GetHandle(), root) != 0);
	unsigned previousCreates = createRequests;
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&doubleClick)) == 0);
	CHECK(ListView_GetItemCount(watchListView.GetHandle()) == 0);
	CHECK(array->watchsort == -1 && createRequests == previousCreates);
	HTREEITEM childItem = TreeView_GetChild(first.GetHandle(), root);
	CHECK(childItem != nullptr);
	PviObject *element = PviObject::FindByName("@Pvi/Tree/Values[0]");
	CHECK(element != nullptr);
	CHECK(TreeView_SelectItem(first.GetHandle(), childItem) != 0);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&doubleClick)) == 0);
	CHECK(ListView_GetItemCount(watchListView.GetHandle()) == 1);
	CHECK(element->watchsort == 0 && element->ex.pv.pvalue != nullptr);
	LVITEMA listItem = {};
	listItem.mask = LVIF_PARAM;
	CHECK(ListView_GetItem(watchListView.GetHandle(), &listItem) != 0);
	CHECK(reinterpret_cast<const char *>(listItem.lParam) == element->name);

	NMTREEVIEWA drag = {};
	drag.hdr.hwndFrom = first.GetHandle();
	drag.hdr.code = TVN_BEGINDRAGA;
	drag.itemNew.hItem = childItem;
	drag.itemNew.lParam = reinterpret_cast<LPARAM>(element->name);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&drag)) == 0);
	CHECK(g_draggedPVIObject == element);
	CHECK(GetCapture() == parent);
	CHECK(TreeView_GetDropHilight(first.GetHandle()) == childItem);
	CHECK(TreeView_GetDropHilight(second.GetHandle()) == nullptr);
	ImageList_EndDrag();
	ImageList_DragLeave(parent);
	ReleaseCapture();
	g_draggedPVIObject = nullptr;

	char originalSettings[MAX_PATH];
	strcpy(originalSettings, SettingsGetFileName());
	char settingsFile[MAX_PATH];
	snprintf(settingsFile, sizeof(settingsFile), "%s\\tree-settings.ini", testDirectory);
	strcpy(SettingsGetFileName(), settingsFile);
	HMENU menu = CreatePopupMenu();
	CHECK(menu != nullptr);
	CHECK(AppendMenuA(menu, MF_STRING, IDM_STORE_CPU_SETTINGS, "Store") != 0);
	CHECK(AppendMenuA(menu, MF_STRING, IDM_REMOVE_CPU_SETTINGS, "Remove") != 0);
	second.UpdateCpuSettingsMenu(menu);
	CHECK((GetMenuState(menu, IDM_STORE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) != 0);
	CHECK((GetMenuState(menu, IDM_REMOVE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) != 0);
	HTREEITEM cpuItem = second.AddItem(TVI_ROOT, cpu->descriptor, reinterpret_cast<LPARAM>(cpu->name), 0);
	CHECK(cpuItem != nullptr);
	CHECK(TreeView_SelectItem(second.GetHandle(), cpuItem) != 0);
	second.UpdateCpuSettingsMenu(menu);
	CHECK((GetMenuState(menu, IDM_STORE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) == 0);
	CHECK((GetMenuState(menu, IDM_REMOVE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) != 0);
	second.CpuSettingsCommand(IDM_STORE_CPU_SETTINGS);
	CHECK(SettingsFindCpuSection(cpu->descriptor) != 0);
	second.UpdateCpuSettingsMenu(menu);
	CHECK((GetMenuState(menu, IDM_STORE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) != 0);
	CHECK((GetMenuState(menu, IDM_REMOVE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) == 0);
	second.CpuSettingsCommand(IDM_REMOVE_CPU_SETTINGS);
	CHECK(SettingsFindCpuSection(cpu->descriptor) == 0);
	first.CpuSettingsCommand(IDM_STORE_CPU_SETTINGS);
	CHECK(SettingsFindCpuSection(cpu->descriptor) == 0);
	CHECK(DestroyMenu(menu) != 0);
	CHECK(DeleteFileA(settingsFile) != 0);
	strcpy(SettingsGetFileName(), originalSettings);

	second.ClearAll();
	CHECK(TreeView_GetCount(second.GetHandle()) == 0);
	CHECK(TreeView_GetCount(first.GetHandle()) == 4);
	first.ClearAll();
	CHECK(TreeView_GetCount(first.GetHandle()) == 0);
	CHECK(DestroyWindow(parent) != 0);
	for (unsigned i = 0; i < 3; ++i)
	{
		char name[80];
		snprintf(name, sizeof(name), "@Pvi/Tree/Values[%u]", i);
		PviObject *child = PviObject::FindByName(name);
		CHECK(child != nullptr);
		child->Remove();
	}
	array->Remove();
}

static void CheckWatchListView(const char *testDirectory)
{
	HINSTANCE instance = GetModuleHandleA(nullptr);
	HWND parent = CreateWindowExA(0, "STATIC", "", WS_OVERLAPPEDWINDOW,
								 0, 0, 640, 480, nullptr, nullptr, instance, nullptr);
	CHECK(parent != nullptr);
	WatchListView first;
	WatchListView second;
	CHECK(first.GetHandle() == nullptr && second.GetHandle() == nullptr);
	CHECK(first.Create(instance, parent) != nullptr);
	CHECK(second.Create(instance, parent) != nullptr);
	CHECK(first.GetHandle() != second.GetHandle());
	CHECK(MoveWindow(first.GetHandle(), 0, 0, 300, 240, FALSE) != 0);
	CHECK(MoveWindow(second.GetHandle(), 300, 0, 300, 240, FALSE) != 0);
	CHECK(Header_GetItemCount(ListView_GetHeader(first.GetHandle())) == 4);
	CHECK(ListView_GetColumnWidth(first.GetHandle(), 0) == 100);
	CHECK(ListView_GetColumnWidth(first.GetHandle(), 3) == 600);

	PviObject *number = AddObject("@Pvi/Watch/Number", "Number", POBJ_PVAR);
	number->ex.pv.type = BR_DINT;
	number->ex.pv.pdatatype = "DINT";
	number->ex.pv.dimension = 1;
	number->ex.pv.length = sizeof(LONG);
	number->watchsort = 0;
	first.InsertPVIObjects(number);
	unsigned previousCreates = createRequests;
	first.InsertPVIObjects(number);
	CHECK(createRequests == previousCreates);
	CHECK(ListView_GetItemCount(first.GetHandle()) == 1);
	CHECK(ListView_GetItemCount(second.GetHandle()) == 0);
	*static_cast<LONG *>(number->ex.pv.pvalue) = 42;
	number->gui_info.display_as_hex = 1;
	number->gui_info.display_as_binary = 1;
	first.UpdateValue(number);
	char text[160];
	LVITEMA item = {};
	item.iSubItem = 3;
	item.pszText = text;
	item.cchTextMax = sizeof(text);
	CHECK(SendMessageA(first.GetHandle(), LVM_GETITEMTEXTA, 0, reinterpret_cast<LPARAM>(&item)) != 0);
	CHECK(strstr(text, "16#0000002A") != nullptr);
	CHECK(strstr(text, "2#0000_0000_0000_0000_0000_0000_0010_1010") != nullptr);

	const WCHAR wideText[] = L"Wide \u03a9";
	PviObject *wide = AddObject("@Pvi/Watch/Wide", "Wide", POBJ_PVAR);
	wide->ex.pv.type = BR_WSTRING;
	wide->ex.pv.pdatatype = "WSTRING";
	wide->ex.pv.dimension = 1;
	wide->ex.pv.length = sizeof(wideText);
	wide->watchsort = 0;
	second.InsertPVIObjects(wide);
	memcpy(wide->ex.pv.pvalue, wideText, sizeof(wideText));
	second.UpdateValue(wide);
	WCHAR wideDisplay[80];
	LVITEMW wideItem = {};
	wideItem.iSubItem = 3;
	wideItem.pszText = wideDisplay;
	wideItem.cchTextMax = 80;
	CHECK(SendMessageW(second.GetHandle(), LVM_GETITEMTEXTW, 0, reinterpret_cast<LPARAM>(&wideItem)) != 0);
	CHECK(lstrcmpW(wideDisplay, wideText) == 0);
	CHECK(ListView_GetItemCount(first.GetHandle()) == 1);

	PviObject *real = AddObject("@Pvi/Watch/Real", "Real", POBJ_PVAR);
	real->ex.pv.type = BR_REAL;
	real->ex.pv.pdatatype = "REAL";
	real->ex.pv.dimension = 1;
	real->ex.pv.length = sizeof(float);
	real->watchsort = 1;
	second.InsertPVIObjects(real);
	*static_cast<float *>(real->ex.pv.pvalue) = 42.5f;
	second.UpdateValue(real);
	CHECK(SendMessageA(second.GetHandle(), LVM_GETITEMTEXTA, 1, reinterpret_cast<LPARAM>(&item)) != 0);
	CHECK(strcmp(text, "42.5") == 0);
	PviObject *cpu = AddObject("@Pvi/Watch/CPU", "/IP=127.0.0.1", POBJ_CPU);
	cpu->watchsort = 2;
	cpu->ex.cpu.running = TRUE;
	second.InsertPVIObjects(cpu);
	second.UpdateValue(cpu);
	PviObject *task = AddObject("@Pvi/Watch/CPU/Task", "Task", POBJ_TASK);
	task->ex.task.cpu = cpu;
	strcpy(task->ex.task.status, "RUN");
	task->watchsort = 3;
	second.InsertPVIObjects(task);
	second.UpdateValue(task);
	CHECK(SendMessageA(second.GetHandle(), LVM_GETITEMTEXTA, 3, reinterpret_cast<LPARAM>(&item)) != 0);
	CHECK(strcmp(text, "RUN") == 0);

	char originalSettings[MAX_PATH];
	strcpy(originalSettings, SettingsGetFileName());
	char settingsFile[MAX_PATH];
	snprintf(settingsFile, sizeof(settingsFile), "%s\\watch-settings.ini", testDirectory);
	strcpy(SettingsGetFileName(), settingsFile);
	CHECK(WritePrivateProfileStringA("General", "WriteAllowed", "0", settingsFile) != 0);
	NMLISTVIEW notification = {};
	notification.hdr.hwndFrom = first.GetHandle();
	notification.hdr.code = NM_DBLCLK;
	notification.iItem = 0;
	unsigned previousDialogs = editDialogs;
	CHECK(second.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&notification)) == 1);
	CHECK(first.HandleMessages(WM_SIZE, 0, 0) == 1);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&notification)) == 0);
	CHECK(editDialogs == previousDialogs);
	CHECK(WritePrivateProfileStringA("General", "WriteAllowed", "1", settingsFile) != 0);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&notification)) == 0);
	CHECK(editDialogs == previousDialogs + 1);
	CHECK(editDialogResource == DLG_WRITE_NUMBERS && editDialogObject == number);
	notification.hdr.hwndFrom = second.GetHandle();
	notification.iItem = 2;
	second.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&notification));
	CHECK(editDialogResource == DLG_EDIT_CPU && editDialogObject == cpu);
	notification.iItem = 3;
	second.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&notification));
	CHECK(editDialogResource == DLG_EDIT_TASK && editDialogObject == task);
	ListView_SetItemState(first.GetHandle(), 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	NMLVKEYDOWN key = {};
	key.hdr.hwndFrom = first.GetHandle();
	key.hdr.code = LVN_KEYDOWN;
	key.wVKey = VK_RETURN;
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&key)) == 0);
	CHECK(editDialogResource == DLG_WRITE_NUMBERS && editDialogObject == number);
	notification.hdr.hwndFrom = first.GetHandle();
	notification.hdr.code = LVN_ITEMACTIVATE;
	notification.iItem = 0;
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&notification)) == 0);
	notification.hdr.code = NM_RCLICK;
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&notification)) == 0);
	CHECK(g_selectedPVIObject == number);
	CHECK(lastPopupOwner == parent && lastPopupMenu != nullptr);
	CHECK(GetMenuState(lastPopupMenu, IDM_DELETE_OBJECT_FROM_LIST, MF_BYCOMMAND) != static_cast<UINT>(-1));
	CHECK(DestroyMenu(lastPopupMenu) != 0);
	lastPopupMenu = nullptr;

	notification.hdr.hwndFrom = second.GetHandle();
	notification.hdr.code = LVN_BEGINDRAG;
	notification.iItem = 1;
	CHECK(second.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&notification)) == 0);
	CHECK(g_draggedPVIObject == real && GetCapture() == parent);
	CHECK(real->ex.pv.pvalue == nullptr && real->watchsort == -1);
	CHECK(ListView_GetItemCount(second.GetHandle()) == 3);
	CHECK(ListView_GetItemCount(first.GetHandle()) == 1);
	ImageList_EndDrag();
	ImageList_DragLeave(parent);
	ReleaseCapture();
	g_draggedPVIObject = nullptr;
	real->watchsort = 1;
	first.InsertPVIObjects(real);
	CHECK(ListView_GetItemCount(first.GetHandle()) == 2 && real->ex.pv.pvalue != nullptr);
	key.wVKey = VK_DELETE;
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&key)) == 0);
	CHECK(number->ex.pv.pvalue == nullptr && number->watchsort == -1 && number->linkid == 0);
	CHECK(ListView_GetItemCount(first.GetHandle()) == 1);
	CHECK(ListView_GetItemCount(second.GetHandle()) == 3);
	CHECK(DeleteFileA(settingsFile) != 0);
	strcpy(SettingsGetFileName(), originalSettings);
	CHECK(DestroyWindow(parent) != 0);
	g_selectedPVIObject = nullptr;
	for (PviObject *object : {number, wide, real, task, cpu})
		object->Remove();
}

static void ResetSocketTest()
{
	CHECK(openSockets.empty());
	CHECK(socketTargets.empty());
	scanTargets.clear();
	bindAddresses.clear();
	startupCalls = cleanupCalls = selectCalls = 0;
	socketLastError = pendingConnectError = 0;
	failingBindAddress = 0;
	targetRoutingError = 0;
	failedTargetFirst = failedTargetLast = 0;
	socketFailure = nullptr;
	connectionsBeforeFailure = 0;
	socketMode = SocketMode::Timeout;
	cancelOnRead = cancelOnConnect = cancelOnSelect = nullptr;
}

static void CheckCpuScanCancellation()
{
	ResetSocketTest();
	CpuScanControl preCancelled;
	preCancelled.cancelRequested.store(true);
	stEthernetCpuInfo results[4] = {};
	unsigned previousCreates = createRequests;
	CHECK(SearchEthernetCpus(results, 4, 512, true, &preCancelled) == 0);
	CHECK(createRequests == previousCreates && startupCalls == 0);

	readResponse = "";
	CpuScanControl snmpCancel;
	cancelOnRead = &snmpCancel;
	cancelReadRequest = readRequests + 1;
	unsigned previousUnlinks = unlinkRequests;
	CHECK(SearchEthernetCpus(results, 4, 512, true, &snmpCancel) == 0);
	CHECK(snmpCancel.cancelRequested.load() && startupCalls == 0 && snmpCancel.errors.empty());
	CHECK(unlinkRequests == previousUnlinks + 2);
	ResetSocketTest();
	readResponse = "11:22:33:44:55:66 " KWDESC_OBJTYPE "=" KWOBJTYPE_STATION;
	snmpPropertyResponses = true;
	CpuScanControl propertyCancel;
	cancelOnRead = &propertyCancel;
	cancelReadRequest = readRequests + 2;
	previousCreates = createRequests;
	previousUnlinks = unlinkRequests;
	CHECK(SearchEthernetCpus(results, 4, 512, true, &propertyCancel) == 0);
	CHECK(propertyCancel.cancelRequested.load() && startupCalls == 0);
	CHECK(createRequests == previousCreates + 4 && unlinkRequests == previousUnlinks + 4 && snmpProperties.empty());
	ResetSocketTest();
	CpuScanControl snmpComplete;
	CHECK(SearchCpuViaSnmp(results, 4, &snmpComplete) == 1);
	CHECK(strcmp(results[0].ipAddress, "192.168.10.2") == 0 && strcmp(results[0].arState, "RUN") == 0);
	CHECK(snmpProperties.empty() && snmpComplete.errors.empty());
	for (int state : {0, 5, -1})
	{
		snmpArState = state;
		CHECK(SearchCpuViaSnmp(results, 4, &snmpComplete) == 1);
		CHECK(strcmp(results[0].arState, "(unknown)") == 0 && snmpComplete.errors.empty());
	}
	snmpArState = 4;
	snmpPropertyResponses = false;
	readResponse = "";
	stAdapterInfo adapter = {"192.168.11.8", "255.255.255.0", "Cancellation Ethernet"};
	CpuScanControl tcpCancel;
	cancelOnConnect = &tcpCancel;
	CHECK(SearchCpuViaPortScan(adapter, results, 4, 512, true, &tcpCancel).error == 0);
	CHECK(scanTargets.size() == 1 && tcpCancel.completed.load() == 1 && cleanupCalls == 1 && openSockets.empty());
	ResetSocketTest();
	CpuScanControl waitCancel;
	cancelOnSelect = &waitCancel;
	CHECK(SearchCpuViaPortScan(adapter, results, 4, 512, true, &waitCancel).error == 0);
	CHECK(scanTargets.size() == 32 && selectCalls == 1 && cleanupCalls == 1 && openSockets.empty());
	ResetSocketTest();
	testAdapters = {adapter};
	CpuScanControl complete;
	CHECK(SearchEthernetCpus(results, 4, 512, true, &complete) == 0);
	CHECK(complete.phase.load() == CpuScanPhase::Tcp && complete.completed.load() == 510 && complete.total.load() == 510);
	CHECK(complete.errors.empty());
	ResetSocketTest();
	socketFailure = "socket";
	CpuScanControl failure;
	CHECK(SearchEthernetCpus(results, 4, 512, true, &failure) == 0);
	CHECK(failure.errors.size() == 2 && failure.errors[0].find("socket") != std::string::npos);
	CHECK(expectedErrorMessages == 0);
	testAdapters.clear();
	ResetSocketTest();
}

static void CheckCpuScanPlansAndSockets()
{
	stAdapterInfo adapter = {"10.12.34.56", "255.255.0.0", "Test Ethernet"};
	stEthernetCpuInfo results[3] = {};
	memset(&results[2], 0x5a, sizeof(results[2]));
	stEthernetCpuInfo canary = results[2];
	CpuPortScanPlan plan;
	int error;
	for (unsigned hosts : {256u, 512u, 1024u, 2048u, 16384u})
	{
		CHECK(GetCpuPortScanPlan(adapter, hosts, plan, error));
		CHECK(error == 0 && !plan.limitedBySubnet && !plan.loopback);
		CHECK(plan.localAddress == 0x0a0c2238u);
		std::uint32_t blockFirst = plan.localAddress & ~(hosts - 1);
		bool startsAtSubnetNetwork = blockFirst == 0x0a0c0000u;
		CHECK(plan.firstAddress == blockFirst + (startsAtSubnetNetwork ? 1 : 0));
		CHECK(plan.lastAddress == blockFirst + hosts - 1);
		CHECK(plan.targetCount == hosts - (startsAtSubnetNetwork ? 2 : 1));
		CHECK(CpuPortScanTimeoutBudgetMs(plan) == ((plan.targetCount + 31) / 32) * 300u);
		ResetSocketTest();
		CpuPortScanResult scan = SearchCpuViaPortScan(adapter, results, 2, hosts);
		CHECK(scan.error == 0 && scan.entries == 0);
		CHECK(scanTargets.size() == plan.targetCount);
		CHECK(selectCalls == (plan.targetCount + 31) / 32);
		CHECK(scanTargets.front() == plan.firstAddress && scanTargets.back() == plan.lastAddress);
		for (size_t i = 0; i < scanTargets.size(); ++i)
		{
			CHECK(scanTargets[i] != plan.localAddress);
			CHECK(bindAddresses[i] == plan.localAddress);
			if (i != 0)
				CHECK(scanTargets[i] > scanTargets[i - 1]);
		}
		CHECK(startupCalls == 1 && cleanupCalls == 1 && openSockets.empty());
	}
	stAdapterInfo clipped = {"192.168.10.8", "255.255.255.0", "Small subnet"};
	for (unsigned hosts : {256u, 512u, 1024u, 2048u, 16384u})
	{
		CHECK(GetCpuPortScanPlan(clipped, hosts, plan, error));
		CHECK(plan.firstAddress == 0xc0a80a01u && plan.lastAddress == 0xc0a80afeu);
		CHECK(plan.targetCount == 253 && plan.limitedBySubnet == (hosts != 256));
		CHECK(CpuPortScanTimeoutBudgetMs(plan) == 2400);
		ResetSocketTest();
		CpuPortScanResult scan = SearchCpuViaPortScan(clipped, results, 2, hosts);
		CHECK(scan.error == 0 && scanTargets.size() == 253 && selectCalls == 8);
		CHECK(scanTargets.front() == plan.firstAddress && scanTargets.back() == plan.lastAddress);
	}
	stAdapterInfo subnet23 = {"192.168.11.8", "255.255.254.0", "Subnet /23"};
	CHECK(GetCpuPortScanPlan(subnet23, 512, plan, error));
	CHECK(plan.firstAddress == 0xc0a80a01u && plan.lastAddress == 0xc0a80bfeu);
	CHECK(plan.targetCount == 509 && !plan.limitedBySubnet);
	CHECK(CpuPortScanTimeoutBudgetMs(plan) == 4800);
	stAdapterInfo unrestricted = {"10.12.98.56", "255.255.0.0", "Unrestricted block"};
	CHECK(GetCpuPortScanPlan(unrestricted, 16384, plan, error));
	CHECK(plan.firstAddress == 0x0a0c4000u && plan.lastAddress == 0x0a0c7fffu);
	CHECK(plan.targetCount == 16383 && CpuPortScanTimeoutBudgetMs(plan) == 153600);
	stAdapterInfo loopback = {"127.0.0.1", "255.0.0.0", "Loopback"};
	CHECK(GetCpuPortScanPlan(loopback, 16384, plan, error));
	CHECK(plan.loopback && plan.targetCount == 1 && plan.firstAddress == INADDR_LOOPBACK &&
		plan.lastAddress == INADDR_LOOPBACK && CpuPortScanTimeoutBudgetMs(plan) == 300);
	ResetSocketTest();
	CHECK(SearchCpuViaPortScan(loopback, results, 2, 16384).error == 0);
	CHECK(scanTargets.size() == 1 && scanTargets[0] == INADDR_LOOPBACK && selectCalls == 1);
	for (const char *mask : {"255.255.255.254", "255.255.255.255"})
	{
		strcpy(clipped.subnetMask, mask);
		CHECK(GetCpuPortScanPlan(clipped, 256, plan, error) && plan.targetCount == 0);
		ResetSocketTest();
		CHECK(SearchCpuViaPortScan(clipped, results, 2, 256).error == 0);
		CHECK(scanTargets.empty());
	}
	strcpy(clipped.subnetMask, "255.0.255.0");
	CHECK(!GetCpuPortScanPlan(clipped, 256, plan, error) && error == WSAEINVAL);
	strcpy(clipped.ipAddress, "not-an-address");
	CHECK(!GetCpuPortScanPlan(clipped, 256, plan, error));
	CHECK(!GetCpuPortScanPlan(adapter, 4096, plan, error));
	ResetSocketTest();
	CHECK(SearchCpuViaPortScan(adapter, results, 0, 256).error == WSAEINVAL);
	CHECK(SearchCpuViaPortScan(adapter, nullptr, 2, 256).error == WSAEINVAL);
	CHECK(startupCalls == 0);

	for (SocketMode mode : {SocketMode::Immediate, SocketMode::Pending, SocketMode::Refused, SocketMode::PendingRefused})
	{
		ResetSocketTest();
		socketMode = mode;
		CpuPortScanResult scan = SearchCpuViaPortScan(adapter, results, 2, 256);
		CHECK(scan.error == 0 && openSockets.empty());
		if (mode == SocketMode::Immediate || mode == SocketMode::Pending)
		{
			CHECK(scan.entries == 2);
			CHECK(scanTargets.size() == (mode == SocketMode::Immediate ? 2 : 32));
			CHECK(results[0].ipAddress[0] != '\0');
			CHECK(strcmp(results[0].subnetMask, adapter.subnetMask) == 0);
		}
		else
			CHECK(scan.entries == 0 && scanTargets.size() == 255);
		CHECK(memcmp(&results[2], &canary, sizeof(canary)) == 0);
	}
	for (const char *operation : {"WSAStartup", "socket", "bind", "ioctlsocket", "connect", "select", "getsockopt"})
	{
		ResetSocketTest();
		socketFailure = operation;
		socketMode = SocketMode::Pending;
		CpuPortScanResult scan = SearchCpuViaPortScan(adapter, results, 2, 256);
		CHECK(scan.error == socketFailureError && strcmp(scan.operation, operation) == 0);
		CHECK(scan.entries == 0 && openSockets.empty());
		CHECK(startupCalls == 1 && cleanupCalls == (strcmp(operation, "WSAStartup") == 0 ? 0u : 1u));
		if (strcmp(operation, "bind") == 0)
			CHECK(scan.address == 0x0a0c2238u);
	}
	ResetSocketTest();
	socketMode = SocketMode::Pending;
	pendingConnectError = WSAEACCES;
	CpuPortScanResult denied = SearchCpuViaPortScan(adapter, results, 2, 256);
	CHECK(denied.error == WSAEACCES && strcmp(denied.operation, "connect") == 0);
	CHECK(cleanupCalls == 1 && openSockets.empty());
	ResetSocketTest();
	socketMode = SocketMode::Immediate;
	socketFailure = "connect";
	connectionsBeforeFailure = 1;
	CpuPortScanResult partial = SearchCpuViaPortScan(adapter, results, 2, 256);
	CHECK(partial.error == socketFailureError && partial.entries == 1 && scanTargets.size() == 2);
	CHECK(strcmp(results[0].ipAddress, "10.12.34.0") == 0);
	CHECK(cleanupCalls == 1 && openSockets.empty());
	ResetSocketTest();

	stAdapterInfo expanded = {"192.168.11.8", "255.255.255.0", "Expanded subnet"};
	for (unsigned hosts : {256u, 512u, 1024u, 2048u, 16384u})
	{
		CHECK(GetCpuPortScanPlan(expanded, hosts, plan, error, true));
		CHECK(!plan.limitedBySubnet && plan.extendsBeyondSubnet == (hosts != 256));
		CHECK(plan.firstAddress == (plan.localAddress & ~(hosts - 1)));
		CHECK(plan.lastAddress == plan.firstAddress + hosts - 1);
		CHECK(plan.targetCount == hosts - 3);
		ResetSocketTest();
		CpuPortScanResult scan = SearchCpuViaPortScan(expanded, results, 2, hosts, true);
		CHECK(scan.error == 0 && scanTargets.size() == plan.targetCount);
		CHECK(selectCalls * CpuScanBatchTimeoutMs == CpuPortScanTimeoutBudgetMs(plan));
		for (auto address : scanTargets)
			CHECK(address != plan.localAddress && address != plan.subnetNetwork && address != plan.subnetBroadcast);
	}
	CHECK(GetCpuPortScanPlan(loopback, 16384, plan, error, true) && plan.targetCount == 1);
	CHECK(!plan.extendsBeyondSubnet && CpuPortScanTimeoutBudgetMs(plan) == 300);
	ResetSocketTest();
	CHECK(SearchCpuViaPortScan(loopback, results, 2, 16384, true).error == 0 && scanTargets.size() == 1);
	for (const char *mask : {"255.255.255.254", "255.255.255.255"})
	{
		strcpy(expanded.subnetMask, mask);
		CHECK(GetCpuPortScanPlan(expanded, 512, plan, error, true));
		CHECK(plan.targetCount == (strcmp(mask, "255.255.255.255") == 0 ? 511u : 510u));
	}
	strcpy(expanded.subnetMask, "255.255.255.0");
	for (int routingError : {WSAENETUNREACH, WSAEACCES, WSAEADDRNOTAVAIL})
	{
		for (SocketMode mode : {SocketMode::Timeout, SocketMode::Pending})
		{
			ResetSocketTest();
			socketMode = mode;
			pendingConnectError = WSAECONNREFUSED;
			targetRoutingError = routingError;
			failedTargetFirst = 0xc0a80a00u;
			failedTargetLast = 0xc0a80affu;
			CpuPortScanResult scan = SearchCpuViaPortScan(expanded, results, 2, 512, true);
			CHECK(scan.error == routingError && scan.rejectedTargets == 256 && scan.entries == 0);
			CHECK(scanTargets.size() == 509 && scanTargets.back() == 0xc0a80bfeu);
			CHECK(scan.address >= failedTargetFirst && scan.address <= failedTargetLast);
			CHECK(strcmp(scan.operation, "connect") == 0 && cleanupCalls == 1 && openSockets.empty());
		}
	}
	ResetSocketTest();
	socketMode = SocketMode::Immediate;
	CpuPortScanResult remote = SearchCpuViaPortScan(expanded, results, 2, 512, true);
	CHECK(remote.error == 0 && remote.entries == 2);
	CHECK(strcmp(results[0].ipAddress, "192.168.10.0") == 0 && results[0].subnetMask[0] == '\0');
	CHECK(memcmp(&results[2], &canary, sizeof(canary)) == 0);
	ResetSocketTest();
}

static void CheckCpuScanAdapters()
{
	std::vector<stAdapterInfo> adapters;
	CHECK(ListCpuScanAdapters(adapters) == ERROR_SUCCESS && adapters.size() == 1);
	CHECK(strcmp(adapters[0].ipAddress, "127.0.0.1") == 0);
	for (unsigned i = 0; i < 24; ++i)
	{
		stAdapterInfo adapter = {};
		snprintf(adapter.ipAddress, sizeof(adapter.ipAddress), "10.0.%u.2", i);
		strcpy(adapter.subnetMask, "255.255.255.0");
		strcpy(adapter.description, "Ethernet adapter");
		testAdapters.push_back(adapter);
	}
	strcpy(testAdapters[22].ipAddress, "0.0.0.0");
	secondaryAdapterAddress = wirelessAdapter = true;
	adapterOverflowRetries = 2;
	CHECK(ListCpuScanAdapters(adapters) == ERROR_SUCCESS);
	CHECK(adapterOverflowRetries == 0 && adapters.size() == 24);
	CHECK(strcmp(adapters[2].ipAddress, "10.2.3.4") == 0);
	for (const auto &adapter : adapters)
		CHECK(strcmp(adapter.ipAddress, "0.0.0.0") != 0 && adapter.description[39] == '\0');
	adapterOverflowRetries = 3;
	CHECK(ListCpuScanAdapters(adapters) == ERROR_BUFFER_OVERFLOW);
	adapterEnumerationError = ERROR_ACCESS_DENIED;
	CHECK(ListCpuScanAdapters(adapters) == ERROR_ACCESS_DENIED);
	adapterEnumerationError = 0;
	secondaryAdapterAddress = wirelessAdapter = false;
	testAdapters.clear();
}

static HWND CreateHiddenDialog(UINT resourceId, DLGPROC procedure, LPARAM data)
{
	HMODULE module = GetModuleHandleA(nullptr);
	HRSRC resource = FindResource(module, MAKEINTRESOURCE(resourceId), RT_DIALOG);
	CHECK(resource != nullptr);
	DWORD size = SizeofResource(module, resource);
	CHECK(size >= 26);
	HGLOBAL loaded = LoadResource(module, resource);
	CHECK(loaded != nullptr);
	const void *dialogTemplate = LockResource(loaded);
	CHECK(dialogTemplate != nullptr);
	std::vector<WORD> hiddenTemplate((size + sizeof(WORD) - 1) / sizeof(WORD));
	memcpy(hiddenTemplate.data(), dialogTemplate, size);
	CHECK(hiddenTemplate[1] == 0xffff);
	// DIALOGEX stores its style DWORD at byte offset 12.
	DWORD style;
	memcpy(&style, hiddenTemplate.data() + 6, sizeof(style));
	style &= ~WS_VISIBLE;
	memcpy(hiddenTemplate.data() + 6, &style, sizeof(style));
	HWND dialog = CreateDialogIndirectParamA(module,
		reinterpret_cast<const DLGTEMPLATE *>(hiddenTemplate.data()), nullptr, procedure, data);
	CHECK(dialog != nullptr);
	CHECK(!IsWindowVisible(dialog));
	return dialog;
}

static void CheckCpuScanDialog()
{
	CpuScanDialogData data;
	data.adapters = {
		{"127.0.0.1", "255.0.0.0", "Loopback"},
		{"192.168.10.8", "255.255.255.0", "Small subnet"},
		{"10.12.34.56", "255.255.0.0", "Large subnet"}
	};
	HWND dialog = CreateHiddenDialog(DLG_CPU_SCAN, CpuScanDlgProc, reinterpret_cast<LPARAM>(&data));
	CHECK(SendDlgItemMessageA(dialog, IDC_CPU_SCAN_HOSTS, CB_GETCOUNT, 0, 0) == 5);
	CHECK(IsDlgButtonChecked(dialog, IDC_CPU_SCAN_BEYOND_SUBNET) == BST_UNCHECKED);
	unsigned index = 0;
	for (unsigned hosts : {256u, 512u, 1024u, 2048u, 16384u})
	{
		char text[2400], expected[160];
		SendDlgItemMessageA(dialog, IDC_CPU_SCAN_HOSTS, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(text));
		snprintf(expected, sizeof(expected), "%u", hosts);
		CHECK(strcmp(text, expected) == 0);
		SendDlgItemMessageA(dialog, IDC_CPU_SCAN_HOSTS, CB_SETCURSEL, index++, 0);
		SendMessageA(dialog, WM_COMMAND, MAKEWPARAM(IDC_CPU_SCAN_HOSTS, CBN_SELCHANGE), 0);
		CHECK(data.hosts == hosts);
		GetDlgItemTextA(dialog, IDC_CPU_SCAN_TIMEOUT, text, sizeof(text));
		unsigned budget = 300 + 2400 + ((hosts - 1 + 31) / 32) * 300;
		snprintf(expected, sizeof(expected), "Maximum TCP timeout budget: %.1f seconds", budget / 1000.0);
		CHECK(strstr(text, expected) != nullptr);
		GetDlgItemTextA(dialog, IDC_CPU_SCAN_DETAILS, text, sizeof(text));
		CHECK(strstr(text, "127.0.0.1 - 127.0.0.1: 1 TCP targets") != nullptr);
		CHECK(strstr(text, "192.168.10.1 - 192.168.10.254: 253 TCP targets") != nullptr);
		CHECK((strstr(text, "WARNING:") != nullptr) == (hosts != 256));
		CHECK(IsWindowEnabled(GetDlgItem(dialog, IDOK)));
	}
	CheckDlgButton(dialog, IDC_CPU_SCAN_BEYOND_SUBNET, BST_CHECKED);
	SendMessageA(dialog, WM_COMMAND, MAKEWPARAM(IDC_CPU_SCAN_BEYOND_SUBNET, BN_CLICKED), 0);
	CHECK(data.allowBeyondSubnet);
	char expandedText[2400];
	GetDlgItemTextA(dialog, IDC_CPU_SCAN_DETAILS, expandedText, sizeof(expandedText));
	CHECK(strstr(expandedText, "192.168.0.0 - 192.168.63.255: 16381 TCP targets") != nullptr);
	CHECK(strstr(expandedText, "route/gateway") != nullptr && strstr(expandedText, "only in-subnet") == nullptr);
	GetDlgItemTextA(dialog, IDC_CPU_SCAN_TIMEOUT, expandedText, sizeof(expandedText));
	CHECK(strstr(expandedText, "307.5 seconds") != nullptr);
	CheckDlgButton(dialog, IDC_CPU_SCAN_BEYOND_SUBNET, BST_UNCHECKED);
	SendMessageA(dialog, WM_COMMAND, MAKEWPARAM(IDC_CPU_SCAN_BEYOND_SUBNET, BN_CLICKED), 0);
	CHECK(!data.allowBeyondSubnet);
	GetDlgItemTextA(dialog, IDC_CPU_SCAN_TIMEOUT, expandedText, sizeof(expandedText));
	CHECK(strstr(expandedText, "156.3 seconds") != nullptr);
	data.adapters = {{"192.168.10.8", "255.0.255.0", "Invalid mask"}};
	SendMessageA(dialog, WM_COMMAND, MAKEWPARAM(IDC_CPU_SCAN_HOSTS, CBN_SELCHANGE), 0);
	char details[500];
	GetDlgItemTextA(dialog, IDC_CPU_SCAN_DETAILS, details, sizeof(details));
	CHECK(strstr(details, "ERROR:") != nullptr && !IsWindowEnabled(GetDlgItem(dialog, IDOK)));
	strcpy(data.adapters[0].subnetMask, "255.255.255.255");
	SendMessageA(dialog, WM_COMMAND, MAKEWPARAM(IDC_CPU_SCAN_HOSTS, CBN_SELCHANGE), 0);
	GetDlgItemTextA(dialog, IDC_CPU_SCAN_DETAILS, details, sizeof(details));
	CHECK(strstr(details, "No other usable addresses") != nullptr && !IsWindowEnabled(GetDlgItem(dialog, IDOK)));
	CHECK(DestroyWindow(dialog));
	unsigned hosts = 512;
	bool allowBeyondSubnet = false;
	scanDialogHosts = 16384;
	scanDialogBeyondSubnet = true;
	scanDialogResult = IDCANCEL;
	CHECK(!DlgCpuScanShowDialog(nullptr, hosts, allowBeyondSubnet) && hosts == 512 && !allowBeyondSubnet);
	scanDialogResult = IDOK;
	CHECK(DlgCpuScanShowDialog(nullptr, hosts, allowBeyondSubnet) && hosts == 16384 && allowBeyondSubnet);
	expectedMessageCaption = "CPU scan";
	expectedErrorMessages = 1;
	scanDialogResult = -1;
	CHECK(!DlgCpuScanShowDialog(nullptr, hosts, allowBeyondSubnet) && expectedErrorMessages == 0);
	adapterEnumerationError = ERROR_ACCESS_DENIED;
	expectedErrorMessages = 1;
	CHECK(!DlgCpuScanShowDialog(nullptr, hosts, allowBeyondSubnet) && expectedErrorMessages == 0);
	adapterEnumerationError = 0;
	expectedMessageCaption = "Error";
	scanDialogResult = IDCANCEL;
	scanDialogBeyondSubnet = false;
}

static void WaitForCpuScan(ObjectTreeView &tree)
{
	ULONGLONG deadline = GetTickCount64() + 5000;
	while (tree.GetCpuScanDialogHandle() != nullptr)
	{
		MSG message;
		while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
		{
			CHECK(message.message != WM_QUIT);
			HWND dialog = tree.GetCpuScanDialogHandle();
			if (dialog == nullptr || !IsDialogMessageA(dialog, &message))
			{
				TranslateMessage(&message);
				DispatchMessageA(&message);
			}
		}
		CHECK(GetTickCount64() < deadline);
		Sleep(1);
	}
}

static void CheckCpuScanProgress()
{
	auto savedCallback = callback;
	const char *savedResponse = readResponse;
	readResponse = "";
	scanDialogResult = IDOK;
	scanDialogHosts = 512;
	ResetSocketTest();
	HINSTANCE instance = GetModuleHandleA(nullptr);
	HWND parent = CreateWindowExA(0, "STATIC", "", WS_OVERLAPPEDWINDOW,
		0, 0, 640, 480, nullptr, nullptr, instance, nullptr);
	CHECK(parent != nullptr);
	ObjectTreeView tree;
	CHECK(tree.Create(instance, parent) != nullptr);
	PviObject *device = AddObject("@Pvi/ProgressDevice", "/IF=tcpip", POBJ_DEVICE);
	HTREEITEM root = tree.AddItem(TVI_ROOT, device->descriptor, reinterpret_cast<LPARAM>(device->name), 0);
	NMTREEVIEWA selection = {};
	selection.hdr.hwndFrom = tree.GetHandle();
	selection.hdr.code = TVN_SELCHANGEDA;
	selection.itemNew.hItem = root;
	selection.itemNew.lParam = reinterpret_cast<LPARAM>(device->name);
	for (UINT cancelMessage : {WM_COMMAND, WM_CLOSE})
	{
		readEntered = CreateEventA(nullptr, TRUE, FALSE, nullptr);
		readRelease = CreateEventA(nullptr, TRUE, FALSE, nullptr);
		CHECK(readEntered != nullptr && readRelease != nullptr);
		CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
		HWND dialog = tree.GetCpuScanDialogHandle();
		CHECK(dialog != nullptr && IsWindowEnabled(parent));
		CHECK(GetDlgItem(dialog, IDC_CPU_SCAN_PROGRESS) != nullptr);
		CHECK((GetWindowLongPtrA(GetDlgItem(dialog, IDC_CPU_SCAN_PROGRESS), GWL_STYLE) & PBS_MARQUEE) != 0);
		CHECK(WaitForSingleObject(readEntered, 5000) == WAIT_OBJECT_0);
		SendMessageA(dialog, cancelMessage, IDCANCEL, 0);
		CHECK(!IsWindowEnabled(GetDlgItem(dialog, IDCANCEL)));
		char status[180];
		GetDlgItemTextA(dialog, IDC_CPU_SCAN_STATUS, status, sizeof(status));
		CHECK(strstr(status, "Cancelling") != nullptr);
		CHECK(SetEvent(readRelease));
		WaitForCpuScan(tree);
		CHECK(!IsWindow(dialog) && TreeView_GetChild(tree.GetHandle(), root) == nullptr && startupCalls == 0);
		CHECK(CloseHandle(readEntered) && CloseHandle(readRelease));
		readEntered = readRelease = nullptr;
	}
	testAdapters = {{"192.168.10.8", "255.255.255.0", "Progress Ethernet"}};
	selectEntered = CreateEventA(nullptr, TRUE, FALSE, nullptr);
	selectRelease = CreateEventA(nullptr, TRUE, FALSE, nullptr);
	CHECK(selectEntered != nullptr && selectRelease != nullptr);
	CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	CHECK(WaitForSingleObject(selectEntered, 5000) == WAIT_OBJECT_0);
	HWND tcpDialog = tree.GetCpuScanDialogHandle();
	SendMessageA(tcpDialog, WM_TIMER, 1, 0);
	HWND progress = GetDlgItem(tcpDialog, IDC_CPU_SCAN_PROGRESS);
	CHECK((GetWindowLongPtrA(progress, GWL_STYLE) & PBS_MARQUEE) == 0);
	CHECK(SendMessageA(progress, PBM_GETPOS, 0, 0) > 0);
	char status[180];
	GetDlgItemTextA(tcpDialog, IDC_CPU_SCAN_STATUS, status, sizeof(status));
	CHECK(strstr(status, "1 / 254") != nullptr);
	SendMessageA(tcpDialog, WM_COMMAND, IDCANCEL, 0);
	CHECK(SetEvent(selectRelease));
	WaitForCpuScan(tree);
	CHECK(scanTargets.size() == 33 && startupCalls == 2 && cleanupCalls == 2 && openSockets.empty());
	CHECK(TreeView_GetChild(tree.GetHandle(), root) == nullptr);
	CHECK(CloseHandle(selectEntered) && CloseHandle(selectRelease));
	selectEntered = selectRelease = nullptr;
	testAdapters.clear();
	ResetSocketTest();
	socketFailure = "socket";
	expectedMessageCaption = "CPU scan";
	expectedErrorMessages = 1;
	CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	WaitForCpuScan(tree);
	CHECK(expectedErrorMessages == 0 && lastMessage.find("socket") != std::string::npos);
	expectedMessageCaption = "Error";
	ResetSocketTest();
	readEntered = CreateEventA(nullptr, TRUE, FALSE, nullptr);
	readRelease = CreateEventA(nullptr, TRUE, FALSE, nullptr);
	CHECK(readEntered != nullptr && readRelease != nullptr);
	CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	CHECK(WaitForSingleObject(readEntered, 5000) == WAIT_OBJECT_0);
	PviObject *manual = AddObject("@Pvi/ProgressDevice/Manual", "/IP=192.168.10.3", POBJ_CPU);
	HTREEITEM manualItem = tree.AddItem(root, manual->descriptor, reinterpret_cast<LPARAM>(manual->name), 0);
	CHECK(manualItem != nullptr);
	CHECK(SetEvent(readRelease));
	WaitForCpuScan(tree);
	CHECK(TreeView_GetCount(tree.GetHandle()) == 2);
	CHECK(CloseHandle(readEntered) && CloseHandle(readRelease));
	readEntered = readRelease = nullptr;
	CHECK(TreeView_DeleteItem(tree.GetHandle(), manualItem));
	manual->Remove();
	// Clearing the tree must cancel/join the worker before discarding its target.
	CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	tree.ClearAll();
	CHECK(tree.GetCpuScanDialogHandle() == nullptr && TreeView_GetCount(tree.GetHandle()) == 0);
	CHECK(DestroyWindow(parent));
	device->Remove();
	ResetSocketTest();
	scanDialogResult = IDCANCEL;
	callback = savedCallback;
	readResponse = savedResponse;
}

static void CheckCpuScanTree()
{
	auto savedCallback = callback;
	const char *savedResponse = readResponse;
	readResponse = "";
	ResetSocketTest();
	testAdapters = {{"10.12.34.56", "255.255.0.0", "Test Ethernet"}};
	HINSTANCE instance = GetModuleHandleA(nullptr);
	HWND parent = CreateWindowExA(0, "STATIC", "", WS_OVERLAPPEDWINDOW,
		0, 0, 640, 480, nullptr, nullptr, instance, nullptr);
	CHECK(parent != nullptr);
	ObjectTreeView tree;
	CHECK(tree.Create(instance, parent) != nullptr);
	PviObject *device = AddObject("@Pvi/CpuScanTreeDevice", "/IF=tcpip", POBJ_DEVICE);
	HTREEITEM root = tree.AddItem(TVI_ROOT, device->descriptor, reinterpret_cast<LPARAM>(device->name), 0);
	CHECK(root != nullptr);
	NMTREEVIEWA selection = {};
	selection.hdr.hwndFrom = tree.GetHandle();
	selection.hdr.code = TVN_SELCHANGEDA;
	selection.itemNew.hItem = root;
	selection.itemNew.lParam = reinterpret_cast<LPARAM>(device->name);
	unsigned previousDialogs = scanDialogs;
	unsigned previousCreates = createRequests;
	scanDialogResult = IDCANCEL;
	CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	CHECK(scanDialogs == previousDialogs + 1 && createRequests == previousCreates);
	CHECK(TreeView_GetChild(tree.GetHandle(), root) == nullptr && startupCalls == 0);
	scanDialogHosts = 512;
	scanDialogResult = IDOK;
	CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	WaitForCpuScan(tree);
	CHECK(scanDialogs == previousDialogs + 2 && scanTargets.size() == 512);
	CHECK(startupCalls == 2 && cleanupCalls == 2);
	CHECK(scanTargets[0] == INADDR_LOOPBACK && scanTargets[1] == 0x0a0c2200u &&
		scanTargets.back() == 0x0a0c23ffu);
	ResetSocketTest();
	testAdapters = {{"192.168.11.8", "255.255.255.0", "Beyond subnet Ethernet"}};
	scanDialogBeyondSubnet = true;
	CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	WaitForCpuScan(tree);
	CHECK(scanTargets.size() == 510 && scanTargets[1] == 0xc0a80a00u && scanTargets.back() == 0xc0a80bfeu);
	CHECK(scanDialogs == previousDialogs + 3);
	scanDialogBeyondSubnet = false;
	testAdapters.clear();
	ResetSocketTest();
	socketMode = SocketMode::Immediate;
	CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	WaitForCpuScan(tree);
	CHECK(TreeView_GetChild(tree.GetHandle(), root) != nullptr);
	CHECK(scanDialogs == previousDialogs + 4 && scanTargets.size() == 1);
	CHECK(tree.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	CHECK(scanDialogs == previousDialogs + 4 && scanTargets.size() == 1);
	tree.ClearAll();
	CHECK(DestroyWindow(parent));
	while (PviObject *child = device->FindChild(TRUE))
		child->Remove();
	device->Remove();
	ResetSocketTest();
	socketMode = SocketMode::Immediate;
	socketFailure = "bind";
	failingBindAddress = 0x0a0c6238u;
	testAdapters = {
		{"10.12.98.56", "255.255.0.0", "Unavailable Ethernet"},
		{"192.168.10.8", "255.255.255.0", "Working Ethernet"}
	};
	expectedMessageCaption = "CPU scan";
	stEthernetCpuInfo results[3] = {};
	memset(&results[2], 0x5a, sizeof(results[2]));
	stEthernetCpuInfo canary = results[2];
	for (int rejection : {WSAEADDRNOTAVAIL, WSAEACCES, WSAENETUNREACH})
	{
		ResetSocketTest();
		socketMode = SocketMode::Immediate;
		socketFailure = "bind";
		failingBindAddress = 0x0a0c6238u;
		socketFailureError = rejection;
		CHECK(expectedErrorMessages == 0);
		CHECK(SearchEthernetCpus(results, 2, 512) == 2);
		CHECK(strcmp(results[0].ipAddress, "127.0.0.1") == 0);
		CHECK(strcmp(results[1].ipAddress, "192.168.10.1") == 0);
		CHECK(startupCalls == 3 && cleanupCalls == 3 && memcmp(&results[2], &canary, sizeof(canary)) == 0);
	}
	ResetSocketTest();
	socketMode = SocketMode::Immediate;
	socketFailure = "bind";
	failingBindAddress = 0x0a0c6238u;
	socketFailureError = WSAENETDOWN;
	expectedErrorMessages = 1;
	CHECK(SearchEthernetCpus(results, 2, 512) == 2 && expectedErrorMessages == 0);
	CHECK(lastMessage.find("bind") != std::string::npos && lastMessage.find("10050") != std::string::npos);
	socketFailureError = WSAEADDRNOTAVAIL;
	ResetSocketTest();
	socketMode = SocketMode::Immediate;
	testAdapters = {
		{"10.0.0.1", "255.255.255.252", "First Ethernet"},
		{"10.0.0.1", "255.255.255.252", "Duplicate address"}
	};
	CHECK(SearchEthernetCpus(results, 3, 512) == 2);
	CHECK(strcmp(results[0].ipAddress, "127.0.0.1") == 0 && strcmp(results[1].ipAddress, "10.0.0.2") == 0);
	CHECK(startupCalls == 3 && cleanupCalls == 3 && scanTargets.size() == 3);
	ResetSocketTest();
	expectedErrorMessages = 1;
	previousCreates = createRequests;
	CHECK(SearchEthernetCpus(results, 3, 4096) == -1 && expectedErrorMessages == 0);
	CHECK(startupCalls == 0 && createRequests == previousCreates);
	testAdapters = {{"192.168.11.8", "255.255.255.0", "No external route"}};
	for (int rejection : {WSAENETUNREACH, WSAEACCES, WSAEADDRNOTAVAIL})
	{
		for (SocketMode mode : {SocketMode::Timeout, SocketMode::Pending})
		{
			ResetSocketTest();
			socketMode = mode;
			pendingConnectError = WSAECONNREFUSED;
			targetRoutingError = rejection;
			failedTargetFirst = 0xc0a80a00u;
			failedTargetLast = 0xc0a80affu;
			CHECK(expectedErrorMessages == 0);
			CHECK(SearchEthernetCpus(results, 3, 512, true) == 0);
			CHECK(scanTargets.size() == 510 && startupCalls == 2 && cleanupCalls == 2);
		}
	}
	ResetSocketTest();
	testAdapters.clear();
	expectedMessageCaption = "Error";
	scanDialogResult = IDCANCEL;
	callback = savedCallback;
	readResponse = savedResponse;
}

static void CheckAboutLink()
{
	INITCOMMONCONTROLSEX controls = {sizeof(INITCOMMONCONTROLSEX), ICC_LINK_CLASS};
	CHECK(InitCommonControlsEx(&controls) != 0);
	HWND dialog = CreateHiddenDialog(DLG_ABOUT, AboutDlgProc, 0);
	HWND link = GetDlgItem(dialog, IDC_LINK_PROJECT);
	CHECK(link != nullptr);
	CHECK((GetWindowLongPtrA(link, GWL_STYLE) & WS_TABSTOP) != 0);
	NMLINK notification = {};
	notification.hdr.hwndFrom = link;
	notification.hdr.idFrom = IDC_LINK_PROJECT;
	notification.item.mask = LIF_ITEMINDEX | LIF_URL;
	notification.item.iLink = 0;
	CHECK(SendMessageW(link, LM_GETITEM, 0, reinterpret_cast<LPARAM>(&notification.item)) != 0);
	CHECK(lstrcmpW(notification.item.szUrl, L"https://github.com/hilch/brwatch") == 0);
	for (UINT code : {NM_CLICK, NM_RETURN})
	{
		notification.hdr.code = code;
		unsigned previousLaunches = browserLaunches;
		SendMessageA(dialog, WM_NOTIFY, IDC_LINK_PROJECT, reinterpret_cast<LPARAM>(&notification));
		CHECK(browserLaunches == previousLaunches + 1);
	}
	browserResult = SE_ERR_NOASSOC;
	expectedErrorMessages = 1;
	SendMessageA(dialog, WM_NOTIFY, IDC_LINK_PROJECT, reinterpret_cast<LPARAM>(&notification));
	CHECK(expectedErrorMessages == 0);
	browserResult = 33;
	CHECK(DestroyWindow(dialog) != 0);
}

int main()
{
	CheckCpuScanPlansAndSockets();
	CheckCpuScanCancellation();
	CheckCpuScanAdapters();
	CheckCpuScanDialog();
	char temporaryPath[MAX_PATH];
	char testDirectory[MAX_PATH];
	CHECK(GetTempPathA(sizeof(temporaryPath), temporaryPath) != 0);
	CHECK(GetTempFileNameA(temporaryPath, "pvo", 0, testDirectory) != 0);
	CHECK(DeleteFileA(testDirectory) != 0);
	CHECK(CreateDirectoryA(testDirectory, nullptr) != 0);
	strcpy(GetApplicationPath(), testDirectory);

	PviObject empty;
	CHECK(empty.linkid == 0);
	CHECK(empty.name[0] == '\0' && empty.descriptor[0] == '\0');
	CHECK(empty.ex.pv.pvalue == nullptr && empty.ex.pv.cpu == nullptr);
	CHECK(empty.gui_info.has_childs == 0);
	CHECK(PviObject::Start() == 0);
	CHECK(PviObject::Count() == 0);
	CHECK(strcmp(PviObject::GetNext(TRUE)->name, "@Pvi") == 0);

	PviObject *device = AddObject("@Pvi/Device", "/IF=tcpip", POBJ_DEVICE);
	PviObject duplicate = *device;
	CHECK(duplicate.Add(FALSE) == device);
	CHECK(PviObject::Count() == 1);
	PviObject *cpu = AddObject("@Pvi/Device/CPU", "/IP=127.0.0.1", POBJ_CPU);
	PviObject *task = AddObject("@Pvi/Device/CPU/Task", "Task", POBJ_TASK);
	task->ex.task.cpu = cpu;
	CHECK(device->FindChild(TRUE) == cpu);
	CHECK(device->FindChild(FALSE) == nullptr);
	CHECK(device->IsParentOf(task));
	CHECK(!task->IsParentOf(device));
	CHECK(!task->IsParentOf(nullptr));
	CHECK(!task->IsParentOf(task));

	PviObject *structure = AddObject("@Pvi/Device/CPU/Task/AxisHandling", "AxisHandling", POBJ_PVAR);
	structure->ex.pv.type = BR_STRUCT;
	structure->ex.pv.dimension = 1;
	structure->ex.pv.cpu = cpu;
	structure->ex.pv.task = task;
	readResponse =
		"VT=struct SN=AxisHandlingType VL=78056 VN=1 "
		"{.ax VT=struct SN=AxisHandlingAxisType VL=19240 VN=2 VO=0} "
		"{.ax.name VT=string VL=33 VN=1 VO=0} "
		"{.axGroupCnc VT=struct SN=AxisHandlingAxesGroupType VL=19788 VN=1 VO=38480} "
		"{.axGroupAux VT=struct SN=AxisHandlingAxesGroupType VL=19788 VN=1 VO=58268} "
		"{.unnamed VT=struct VL=4 VN=1 VO=0} "
		"{.state VT=i32 SN=StateEnum VL=4 VN=1 VO=0}";
	structure->Expand();
	CHECK(structure->gui_info.has_childs == 1);
	CHECK(strcmp(structure->ex.pv.pdatatype, "AxisHandlingType") == 0);
	const char *childNames[] = {".ax", ".axGroupCnc", ".axGroupAux", ".unnamed", ".state"};
	const char *childTypes[] = {"AxisHandlingAxisType", "AxisHandlingAxesGroupType",
							   "AxisHandlingAxesGroupType", "DATATYPE(STRUCT)", "DINT"};
	PviObject *child = structure->FindChild(TRUE);
	for (unsigned i = 0; i < sizeof(childNames) / sizeof(childNames[0]); ++i)
	{
		CHECK(child != nullptr);
		CHECK(strcmp(child->name + strlen(structure->name), childNames[i]) == 0);
		CHECK(strcmp(child->ex.pv.pdatatype, childTypes[i]) == 0);
		CHECK(child->ex.pv.dimension == (i == 0 ? 2 : 1));
		CHECK(child->ex.pv.cpu == cpu && child->ex.pv.task == task);
		child = structure->FindChild(FALSE);
	}
	CHECK(child == nullptr);

	PviObject *axisArray = PviObject::FindByName("@Pvi/Device/CPU/Task/AxisHandling.ax");
	CHECK(axisArray != nullptr);
	readResponse =
		"VT=struct SN=AxisHandlingAxisType VL=19240 VN=2 "
		"{.name VT=string VL=33 VN=1 VO=0}";
	axisArray->Expand();
	CHECK(axisArray->gui_info.has_childs == 1);
	PviObject *axis = axisArray->FindChild(TRUE);
	CHECK(axis != nullptr && axis->ex.pv.dimension == 1);
	CHECK(strcmp(axis->name, "@Pvi/Device/CPU/Task/AxisHandling.ax[0]") == 0);
	axis = axisArray->FindChild(FALSE);
	CHECK(axis != nullptr && axis->ex.pv.dimension == 1);
	CHECK(strcmp(axis->name, "@Pvi/Device/CPU/Task/AxisHandling.ax[1]") == 0);
	CHECK(axisArray->FindChild(FALSE) == nullptr);
	PviObject *axisName = PviObject::FindByName("@Pvi/Device/CPU/Task/AxisHandling.ax[1].name");
	CHECK(axisName != nullptr && axisName->ex.pv.type == BR_STRING);
	CHECK(axisName->ex.pv.cpu == cpu && axisName->ex.pv.task == task);

	PviObject *variable = AddObject("@Pvi/Device/CPU/Task/Value", "Value", POBJ_PVAR);
	variable->ex.pv.type = BR_DINT;
	variable->ex.pv.length = sizeof(LONG);
	variable->ex.pv.dimension = 1;
	variable->ex.pv.pdatatype = "DINT";
	CHECK(variable->Watch(TRUE) == variable);
	CHECK(variable->ex.pv.pvalue != nullptr);
	CHECK(*static_cast<LONG *>(variable->ex.pv.pvalue) == 0);
	CHECK(PviObject::FindByLinkId(variable->linkid) == variable);
	pvi_interface_notify.cbdata_changed = DataChanged;
	pvi_interface_notify.cberror_changed = ErrorChanged;
	T_RESPONSE_INFO info = {};
	info.nType = POBJ_EVENT_DATA;
	info.LinkID = variable->linkid;
	LONG value = 42;
	CHECK(callback != nullptr);
	callback(0, 0, &value, sizeof(value), &info);
	CHECK(*static_cast<LONG *>(variable->ex.pv.pvalue) == value);
	CHECK(dataNotifications == 1 && variable->ex.pv.value_changed == 1);
	info.nType = POBJ_EVENT_ERROR;
	info.ErrCode = 123;
	callback(0, 0, nullptr, 0, &info);
	CHECK(variable->error == 123 && errorNotifications == 1);
	info.nType = POBJ_EVENT_DATA;
	callback(0, 0, &value, sizeof(value), &info);
	CHECK(variable->error == 0 && errorNotifications == 2);
	CheckRegistryGrowth(variable, cpu, task, axisName);
	CheckCursorRemoval();
	CheckStringCallbacks(BR_STRING);
	CheckStringCallbacks(BR_WSTRING);
	CheckStringListInsertion(task);
	CheckObjectTreeView(cpu, task, testDirectory);
	CheckWatchListView(testDirectory);
	CheckAboutLink();
	CheckCpuScanTree();
	CheckCpuScanProgress();
	CHECK(variable->Watch(FALSE) == variable);
	CHECK(variable->ex.pv.pvalue == nullptr && variable->linkid == 0);
	CHECK(variable->watchsort == -1);

	char watchFile[MAX_PATH];
	snprintf(watchFile, sizeof(watchFile), "%s\\watch.wtc", testDirectory);
	variable->watchsort = 3;
	CHECK(variable->AddToWatchFile(watchFile) == 0);
	readResponse = "VT=i32 VL=4 VN=1 SC=g";
	CHECK(PviObject::LoadFromWatchFile(POBJ_PVAR, watchFile) == 1);
	CHECK(variable->watchsort == 3 && variable->gui_info.loaded_from_file == 1);
	CHECK(variable->ex.pv.type == BR_DINT);
	CHECK(variable->ex.pv.cpu == cpu && variable->ex.pv.task == task);
	CHECK(PviObject::FindByName(variable->name) == variable);

	int previousCount = PviObject::Count();
	detachedLinkId = variable->linkid;
	variable->Remove();
	detachedLinkId = 0;
	CHECK(PviObject::Count() == previousCount - 1);
	CHECK(PviObject::FindByName("@Pvi/Device/CPU/Task/Value") == nullptr);
	CHECK(PviObject::Stop() == 0);
	CHECK(PviObject::Count() == 0);
	CHECK(PviObject::GetNext(TRUE)->type == POBJ_PVI);
	CHECK(PviObject::GetNext(FALSE) == nullptr);
	CHECK(DeleteFileA(watchFile) != 0);
	char logFile[MAX_PATH];
	snprintf(logFile, sizeof(logFile), "%s\\pvilog.txt", testDirectory);
	CHECK(DeleteFileA(logFile) != 0);
	CHECK(RemoveDirectoryA(testDirectory) != 0);
	puts("PASS: PviObject lifecycle, vector growth, cursor removal, expansion, bounded string callbacks, scalar-only watches, tree/list views, About link, CPU scan plans/sockets/dialog/tree integration, and watch files");
	return 0;
}

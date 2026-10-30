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

#ifndef CPUSEARCH_H
#define CPUSEARCH_H

#include <cstdint>
#include <vector>
#include <atomic>
#include <string>

struct stEthernetCpuInfo;

enum class CpuScanPhase { Snmp, Tcp };

struct CpuScanControl {
	std::atomic<bool> cancelRequested{false};
	std::atomic<CpuScanPhase> phase{CpuScanPhase::Snmp};
	std::atomic<unsigned> completed{0};
	std::atomic<unsigned> total{0};
	// Written by discovery; read only after the worker has stopped.
	std::vector<std::string> errors;
};

constexpr unsigned CpuScanDefaultHosts = 256;
constexpr unsigned CpuScanBatchSize = 32;
constexpr unsigned CpuScanBatchTimeoutMs = 300;

struct stAdapterInfo {
	char ipAddress[20];
	char subnetMask[20];
	char description[40];
};

struct CpuPortScanPlan {
	std::uint32_t localAddress = 0;
	std::uint32_t subnetNetwork = 0;
	std::uint32_t subnetBroadcast = 0;
	std::uint32_t firstAddress = 0;
	std::uint32_t lastAddress = 0;
	unsigned targetCount = 0;
	bool limitedBySubnet = false;
	bool extendsBeyondSubnet = false;
	bool loopback = false;
};

struct CpuPortScanResult {
	int entries = 0;
	int error = 0;
	const char *operation = nullptr;
	std::uint32_t address = 0;
	unsigned rejectedTargets = 0;
};

unsigned long ListCpuScanAdapters(std::vector<stAdapterInfo> &adapters);
bool GetCpuPortScanPlan(const stAdapterInfo &adapter, unsigned hosts, CpuPortScanPlan &plan, int &error,
	bool allowBeyondSubnet = false);
std::uint64_t CpuPortScanTimeoutBudgetMs(const CpuPortScanPlan &plan);
CpuPortScanResult SearchCpuViaPortScan(const stAdapterInfo &adapter,
	stEthernetCpuInfo *ethernetCpuInfo, int maxEntries, unsigned hosts, bool allowBeyondSubnet = false,
	CpuScanControl *control = nullptr);
int SearchEthernetCpus(stEthernetCpuInfo *ethernetCpuInfo, int maxEntries,
	unsigned hosts = CpuScanDefaultHosts, bool allowBeyondSubnet = false, CpuScanControl *control = nullptr);
int SearchCpuViaSnmp(struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries, CpuScanControl *control = nullptr);

#endif // CPUSEARCH_H

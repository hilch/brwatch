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

#ifndef __LOGGER_H
#define __LOGGER_H

INT_PTR CALLBACK LoggerConfigDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Logger( void );
BOOL IsLoggerRunning( void );
void LoggerDataChanged( PviObject *object );

#define UTF8BOM	"\xef\xbb\xbf" 					
#define LOGFILE_HEADER				UTF8BOM "cycle@rtc(yyyy/mm/dd-hh:mm:ss);ticks(ms)"
#define LOGFILE_HEADER_BY_CHANGE	UTF8BOM "event@rtc(yyyy/mm/dd-hh:mm:ss);ticks(ms);name;value"

#endif


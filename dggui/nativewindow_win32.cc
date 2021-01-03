/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_win32.cc
 *
 *  Fri Dec 28 18:45:52 CET 2012
 *  Copyright 2012 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "nativewindow_win32.h"

#include <cstring>
#include <algorithm>
#include <commctrl.h>

#include "window.h"

namespace dggui
{

static BOOL trackMouse(HWND hwnd)
{
	TRACKMOUSEEVENT ev{};
	ev.cbSize = sizeof(ev);
	ev.dwFlags = TME_HOVER | TME_LEAVE;
	ev.hwndTrack = hwnd;
	ev.dwHoverTime = 1;
	return TrackMouseEvent(&ev);
}

LRESULT CALLBACK NativeWindowWin32::dialogProc(HWND hwnd, UINT msg,
                                               WPARAM wp, LPARAM lp)
{
	NativeWindowWin32* native =
		(NativeWindowWin32*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	// NOTE: 'native' is nullptr intil the WM_CREATE message has been handled.
	if(!native)
	{
		return DefWindowProc(hwnd, msg, wp, lp);
	}

	Window& window = native->window;

	switch(msg)
	{
	case WM_SIZE:
		if(wp > 4)
		{
			// Bogus value - ignore
			break;
		}
		{
			auto resizeEvent = std::make_shared<ResizeEvent>();
			resizeEvent->width = LOWORD(lp);
			resizeEvent->height = HIWORD(lp);
			native->event_queue.push_back(resizeEvent);
		}
		break;

	case WM_MOVE:
		{
			auto moveEvent = std::make_shared<MoveEvent>();
			moveEvent->x = (short)LOWORD(lp);
			moveEvent->y = (short)HIWORD(lp);
			native->event_queue.push_back(moveEvent);
		}
		break;

	case WM_CLOSE:
		{
			auto closeEvent = std::make_shared<CloseEvent>();
			native->event_queue.push_back(closeEvent);
		}
		return 0; // Do not call DefWindowProc for this event.
//		HWND child, old;
//		old	= 0;

//		numDialogs--;

//		while(old != (child = GetNextDlgGroupItem(hwnd, hwnd, false))) {
//			old = child;
//			EndDialog(child, 0);
//		}

//		if(numDialogs) EndDialog(hwnd, 0);
//		else PostQuitMessage(0);
//		return 0;
	case WM_MOUSEMOVE:
		{
			trackMouse(native->m_hwnd);
			auto mouseMoveEvent = std::make_shared<MouseMoveEvent>();
			mouseMoveEvent->x = (short)LOWORD(lp);
			mouseMoveEvent->y = (short)HIWORD(lp);
			native->last_mouse_position = { mouseMoveEvent->x, mouseMoveEvent->y };

			if(!native->mouse_in_window)
			{
				auto enterEvent = std::make_shared<MouseEnterEvent>();
				enterEvent->x = native->last_mouse_position.first;
				enterEvent->y = native->last_mouse_position.second;
				native->event_queue.push_back(enterEvent);
				native->mouse_in_window = true;
			}
			native->event_queue.push_back(mouseMoveEvent);
		}
		break;

	case WM_MOUSEWHEEL:
		{
			auto scrollEvent = std::make_shared<ScrollEvent>();

			// NOTE: lp is coordinates in screen space, not client space.
			POINT p;
			p.x = (short)LOWORD(lp);
			p.y = (short)HIWORD(lp);
			ScreenToClient(hwnd, &p);

			scrollEvent->x = p.x;
			scrollEvent->y = p.y;
			scrollEvent->delta = -1 * (short)HIWORD(wp) / 60.0f;
			native->event_queue.push_back(scrollEvent);
		}
		break;

	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
		{
			auto buttonEvent = std::make_shared<ButtonEvent>();

			buttonEvent->x = (short)LOWORD(lp);
			buttonEvent->y = (short)HIWORD(lp);

			if(msg == WM_LBUTTONUP ||
			   msg == WM_LBUTTONDBLCLK ||
			   msg == WM_LBUTTONDOWN)
			{
				buttonEvent->button = MouseButton::left;
			}
			else if(msg == WM_MBUTTONUP ||
			        msg == WM_MBUTTONDBLCLK ||
			        msg == WM_MBUTTONDOWN)
			{
				buttonEvent->button = MouseButton::middle;
			}
			else if(msg == WM_RBUTTONUP ||
			        msg == WM_RBUTTONDBLCLK ||
			        msg == WM_RBUTTONDOWN)
			{
				buttonEvent->button = MouseButton::right;
			}
			else
			{
				break; // unknown button
			}

			// Double-clicking the a mouse button actually generates a sequence
			// of four messages: WM_xBUTTONDOWN, WM_xBUTTONUP, WM_xBUTTONDBLCLK, and
			// WM_xBUTTONUP. In other words the second WM_xBUTTONDOWN is replaced by a
			// WM_xBUTTONDBLCLK. We simply 'return it' as a WM_xBUTTONDOWN but set the
			// doubleClick boolean hint accordingly.
			if(msg == WM_LBUTTONUP ||
			   msg == WM_RBUTTONUP ||
			   msg == WM_MBUTTONUP)
			{
				buttonEvent->direction = Direction::up;
			}
			else if(msg == WM_LBUTTONDOWN ||
			        msg == WM_RBUTTONDOWN ||
			        msg == WM_MBUTTONDOWN ||
			        msg == WM_LBUTTONDBLCLK ||
			        msg == WM_RBUTTONDBLCLK ||
			        msg == WM_MBUTTONDBLCLK)
			{
				buttonEvent->direction = Direction::down;
			}

			buttonEvent->doubleClick = (msg == WM_LBUTTONDBLCLK ||
			                            msg == WM_RBUTTONDBLCLK ||
			                            msg == WM_MBUTTONDBLCLK);

			native->event_queue.push_back(buttonEvent);
		}
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			auto keyEvent = std::make_shared<KeyEvent>();

			switch(wp) {
			case VK_LEFT:   keyEvent->keycode = Key::left;      break;
			case VK_RIGHT:  keyEvent->keycode = Key::right;     break;
			case VK_UP:     keyEvent->keycode = Key::up;        break;
			case VK_DOWN:   keyEvent->keycode = Key::down;      break;
			case VK_BACK:   keyEvent->keycode = Key::backspace; break;
			case VK_DELETE: keyEvent->keycode = Key::deleteKey; break;
			case VK_HOME:   keyEvent->keycode = Key::home;      break;
			case VK_END:    keyEvent->keycode = Key::end;       break;
			case VK_PRIOR:  keyEvent->keycode = Key::pageUp;    break;
			case VK_NEXT:   keyEvent->keycode = Key::pageDown;  break;
			case VK_RETURN: keyEvent->keycode = Key::enter;     break;
			default:        keyEvent->keycode = Key::unknown;   break;
			}

			keyEvent->text = "";
			keyEvent->direction =
				(msg == WM_KEYDOWN) ? Direction::down : Direction::up;

			native->event_queue.push_back(keyEvent);
		}
		break;

	case WM_CHAR:
		{
			if(wp >= ' ') // Filter control chars.
			{
				auto keyEvent = std::make_shared<KeyEvent>();
				keyEvent->keycode = Key::character;
				keyEvent->text += (char)wp;
				keyEvent->direction = Direction::up;
				native->event_queue.push_back(keyEvent);
			}
		}
		break;

	case WM_PAINT:
		{
			RECT rect;
			GetUpdateRect(hwnd, &rect, FALSE);

			// Bypass partial update, which is apparrently broken.
			rect.left = 0;
			rect.top = 0;
			rect.right = window.wpixbuf.width;
			rect.bottom = window.wpixbuf.height;

			auto repaintEvent = std::make_shared<RepaintEvent>();
			repaintEvent->x = rect.left;
			repaintEvent->y = rect.top;
			repaintEvent->width = rect.right - rect.left;
			repaintEvent->height = rect.bottom - rect.top;
			native->event_queue.push_back(repaintEvent);

			// Move to window.h (in class)
			HDC pDC;
			HBITMAP old;
			HBITMAP ourbitmap;
			int* framebuf;
			PixelBuffer& px = window.wpixbuf;

			{ // Create bitmap
				HDC hDC;
				BITMAPINFO bitmapinfo;
				hDC = CreateCompatibleDC(nullptr);
				bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bitmapinfo.bmiHeader.biWidth = px.width;
				bitmapinfo.bmiHeader.biHeight = -px.height; // top-down
				bitmapinfo.bmiHeader.biPlanes = 1;
				bitmapinfo.bmiHeader.biBitCount = 32;
				bitmapinfo.bmiHeader.biCompression = BI_RGB;
				bitmapinfo.bmiHeader.biSizeImage = 0;
				bitmapinfo.bmiHeader.biClrUsed = 256;
				bitmapinfo.bmiHeader.biClrImportant = 256;
				ourbitmap = CreateDIBSection(hDC, &bitmapinfo,
				                             DIB_RGB_COLORS, (void**)&framebuf, 0, 0);
				pDC=CreateCompatibleDC(nullptr);
				old = (HBITMAP__*)SelectObject(pDC, ourbitmap);
				DeleteDC(hDC);
			}

			int from_x = rect.left;
			int to_x = std::min(rect.right, (long)px.width);
			int from_y = rect.top;
			int to_y = std::min(rect.bottom, (long)px.height);
			{ // Copy PixelBuffer to framebuffer
				int idx = 0;
				for(int y = from_y; y < to_y; ++y)
				{
					for(int x = from_x; x < to_x; ++x)
					{
						*(framebuf + idx) = RGB(px.buf[(x + y * px.width) * 3 + 2],
						                        px.buf[(x + y * px.width) * 3 + 1],
						                        px.buf[(x + y * px.width) * 3 + 0]);
						++idx;
					}
				}
			}

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(native->m_hwnd, &ps);
			BitBlt(hdc, from_x, from_y, to_x, to_y, pDC, from_x, from_y, SRCCOPY);
			EndPaint(native->m_hwnd, &ps);

			{ // Destroy bitmap (move to window.cc)
				SelectObject(pDC,old);
				DeleteDC(pDC);
				DeleteObject(ourbitmap);
			}
		}
		break;

	case WM_MOUSELEAVE:
		{
			auto leaveEvent = std::make_shared<MouseLeaveEvent>();
			leaveEvent->x = native->last_mouse_position.first;
			leaveEvent->y = native->last_mouse_position.second;
			native->event_queue.push_back(leaveEvent);
			native->mouse_in_window = false;
		}
		break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT CALLBACK NativeWindowWin32::subClassProc(HWND hwnd, UINT msg,
                                                 WPARAM wp, LPARAM lp,
                                                 UINT_PTR id, DWORD_PTR data)
{
	NativeWindowWin32* native = (NativeWindowWin32*)data;

	// NOTE: 'native' is nullptr intil the WM_CREATE message has been handled.
	if(!native)
	{
		return DefWindowProc(hwnd, msg, wp, lp);
	}

	switch(msg)
	{
	case WM_SIZE:
		if(wp > 4)
		{
			// Bogus value - ignore
			break;
		}
		{
			// Parent window size changed, replicate this size in inner window.
			int width = LOWORD(lp);
			int height = HIWORD(lp);
			SetWindowPos(native->m_hwnd, nullptr, -1, -1, width, height, SWP_NOMOVE);
		}
		break;
	}

	return DefSubclassProc(hwnd, msg, wp, lp);
}

NativeWindowWin32::NativeWindowWin32(void* native_window, Window& window)
	: window(window)
{
	WNDCLASSEX wcex{};

	//Time to register a window class.
	//Generic flags and everything. cbWndExtra is the size of a pointer to an
	// object - we need this in the wndproc handler.

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;//class_style;
	wcex.lpfnWndProc = (WNDPROC)dialogProc;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// Set data:
	wcex.cbWndExtra = sizeof(NativeWindowWin32*); // Size of data.
	wcex.hInstance = GetModuleHandle(nullptr);

	//	if(ex_style && WS_EX_TRANSPARENT == WS_EX_TRANSPARENT) {
	//		wcex.hbrBackground = nullptr;
	//	} else {
	wcex.hbrBackground = nullptr;//(HBRUSH) COLOR_BACKGROUND + 1;
	//	}

	const char* name = "DrumGizmoClass";
	char* c_name = (char*)malloc(strlen(name) + 1);
	strcpy(c_name, name);
	wcex.lpszClassName = m_className = c_name;

	RegisterClassEx(&wcex);

	parent_window = (HWND)native_window;

	int width = 1, height = 1;
	if(parent_window)
	{
		// Listen in on parent size changes.
		SetWindowSubclass(parent_window, subClassProc, 42, (LONG_PTR)this);

		// Resize newly created window to fit into parent.
		RECT rect;
		GetClientRect(parent_window, &rect);

		auto resizeEvent = std::make_shared<ResizeEvent>();
		width = resizeEvent->width = rect.right - rect.left;
		height = resizeEvent->height = rect.bottom - rect.top;
		event_queue.push_back(resizeEvent);
	}

	m_hwnd = CreateWindowEx(0/*ex_style*/, m_className,
	                        "DGBasisWidget",
	                        (native_window?WS_CHILD:WS_OVERLAPPEDWINDOW) |
	                        (native_window?WS_VISIBLE:0),
	                        0, 0,
	                        width, height,
	                        parent_window, nullptr,
	                        GetModuleHandle(nullptr), nullptr);

	SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);

	// Set up initial tracking of the mouse leave events
	trackMouse(m_hwnd);
}

NativeWindowWin32::~NativeWindowWin32()
{
	if(parent_window)
	{
		RemoveWindowSubclass(parent_window, subClassProc, 42);
	}
	DestroyWindow(m_hwnd);
	UnregisterClass(m_className, GetModuleHandle(nullptr));
	free(m_className);
}

void NativeWindowWin32::setFixedSize(std::size_t width, std::size_t height)
{
	resize(width, height);
	LONG style =  GetWindowLong(m_hwnd, GWL_STYLE);
	style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	SetWindowLong(m_hwnd, GWL_STYLE, style);
}

void NativeWindowWin32::setAlwaysOnTop(bool always_on_top)
{
	this->always_on_top = always_on_top;
	SetWindowPos(m_hwnd, always_on_top ? HWND_TOPMOST : nullptr,
	             0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void NativeWindowWin32::resize(std::size_t width, std::size_t height)
{
	auto hwnd = m_hwnd;
	//if(parent_window)
	//{
	//	hwnd = parent_window;
	//}

	// Set requested size on the window (or parent)
	SetWindowPos(hwnd, always_on_top ? HWND_TOPMOST : nullptr,
	             -1, -1, (int)width, (int)height, SWP_NOMOVE);

	// Ask the client window what size it actually got
	RECT rect;
	GetClientRect(m_hwnd, &rect);
	int w = width - rect.right;
	int h = height - rect.bottom;

	// Set the compensated size on the window (or parent)
	SetWindowPos(hwnd, always_on_top ? HWND_TOPMOST : nullptr,
	             -1, -1, width + w, height + h, SWP_NOMOVE);
}

std::pair<std::size_t, std::size_t> NativeWindowWin32::getSize() const
{
	RECT rect;
	GetClientRect(m_hwnd, &rect);
	return std::make_pair(rect.right - rect.left, rect.bottom - rect.top);
}

void NativeWindowWin32::move(int x, int y)
{
	SetWindowPos(m_hwnd, always_on_top ? HWND_TOPMOST : nullptr,
	             (int)x, (int)y, -1, -1, SWP_NOSIZE);
}

std::pair<int, int> NativeWindowWin32::getPosition() const
{
	RECT rect;
	GetClientRect(m_hwnd, &rect);
	return std::make_pair(rect.left, rect.top);
}

void NativeWindowWin32::show()
{
	ShowWindow(m_hwnd, SW_SHOW);
}

void NativeWindowWin32::hide()
{
	ShowWindow(m_hwnd, SW_HIDE);
}

bool NativeWindowWin32::visible() const
{
	return IsWindowVisible(m_hwnd);
}

void NativeWindowWin32::redraw(const Rect& dirty_rect)
{
	// Send WM_PAINT message. Buffer transfering is handled in MessageHandler.
	if(parent_window == nullptr)
	{
		RECT rect =
			{
				(long)dirty_rect.x1,
				(long)dirty_rect.y1,
				(long)dirty_rect.x2,
				(long)dirty_rect.y2
			};
		RedrawWindow(m_hwnd, &rect, nullptr, RDW_INVALIDATE);
		UpdateWindow(m_hwnd);
	}
	else
	{
		InvalidateRect(m_hwnd, 0, TRUE);
	}
}

void NativeWindowWin32::setCaption(const std::string &caption)
{
	SetWindowText(m_hwnd, caption.c_str());
}

void NativeWindowWin32::grabMouse(bool grab)
{
	if(grab)
	{
		SetCapture(m_hwnd);
	}
	else
	{
		ReleaseCapture();
	}
}

EventQueue NativeWindowWin32::getEvents()
{
	MSG msg;
	while(PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	EventQueue events;
	std::swap(events, event_queue);
	return events;
}

void* NativeWindowWin32::getNativeWindowHandle() const
{
	return (void*)m_hwnd;
}

Point NativeWindowWin32::translateToScreen(const Point& point)
{
	POINT p{ point.x, point.y };
	ClientToScreen(m_hwnd, &p);
	return { p.x, p.y };
}

} // dggui::

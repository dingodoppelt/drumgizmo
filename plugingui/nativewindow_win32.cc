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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "nativewindow_win32.h"

#include "window.h"

#include <cstring>

namespace GUI {

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

	switch(msg) {
	case WM_SIZE:
		{
			static bool first = true;
			if(!first)
			{
				ResizeEvent* resizeEvent = new ResizeEvent();
				resizeEvent->width = LOWORD(lp);
				resizeEvent->height = HIWORD(lp);
				native->event = resizeEvent;
				first = false;
			}
		}
		break;

	case WM_MOVE:
		{
//      MoveEvent* moveEvent = new MoveEvent();
//      moveEvent->x = (short)LOWORD(lp);
//      moveEvent->y = (short)HIWORD(lp);
//      native->event = moveEvent;
		}
		break;

	case WM_CLOSE:
		{
			CloseEvent* closeEvent = new CloseEvent();
			native->event = closeEvent;
		}
		break;
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
			MouseMoveEvent* mouseMoveEvent = new MouseMoveEvent();
			mouseMoveEvent->x = (short)LOWORD(lp);
			mouseMoveEvent->y = (short)HIWORD(lp);
			native->event = mouseMoveEvent;
		}
		break;

	case WM_MOUSEWHEEL:
		{
			ScrollEvent* scrollEvent = new ScrollEvent();

			// NOTE: lp is coordinates in screen space, not client space.
			POINT p;
			p.x = (short)LOWORD(lp);
			p.y = (short)HIWORD(lp);
			ScreenToClient(hwnd, &p);

			scrollEvent->x = p.x;
			scrollEvent->y = p.y;
			scrollEvent->delta = -1 * (short)HIWORD(wp) / 60;
			native->event = scrollEvent;
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
			ButtonEvent* buttonEvent = new ButtonEvent();

			buttonEvent->x = (short)LOWORD(lp);
			buttonEvent->y = (short)HIWORD(lp);

			if(msg == WM_LBUTTONUP ||
			   msg == WM_LBUTTONDBLCLK ||
			   msg == WM_LBUTTONDOWN)
			{
				buttonEvent->button = MouseButton::left;
			}
			else if(msg == WM_RBUTTONUP ||
			        msg == WM_RBUTTONDBLCLK ||
			        msg == WM_RBUTTONDOWN)
			{
				buttonEvent->button = MouseButton::middle;
			}
			else if(msg == WM_MBUTTONUP ||
			        msg == WM_MBUTTONDBLCLK ||
			        msg == WM_MBUTTONDOWN)
			{
				buttonEvent->button = MouseButton::right;
			}
			else
			{
				delete buttonEvent;
				break; // unknown button
			}

			if(msg == WM_LBUTTONUP ||
			   msg == WM_RBUTTONUP ||
			   msg == WM_MBUTTONUP)
			{
				buttonEvent->direction = Direction::up;
			}
			else if(msg == WM_LBUTTONDOWN ||
			        msg == WM_RBUTTONDOWN ||
			        msg == WM_MBUTTONDOWN)
			{
				buttonEvent->direction = Direction::down;
			}
			else
			{
	      delete buttonEvent;
	      break; // unknown direction
			}

			buttonEvent->doubleClick = (msg == WM_LBUTTONDBLCLK ||
			                            msg == WM_RBUTTONDBLCLK ||
			                            msg == WM_MBUTTONDBLCLK);

			native->event = buttonEvent;
		}
		break;

	case WM_KEYDOWN:
		{
			KeyEvent* keyEvent = new KeyEvent();

			switch(wp) {
			case VK_LEFT:   keyEvent->keycode = Key::left;      break;
			case VK_RIGHT:  keyEvent->keycode = Key::right;     break;
			case VK_UP:     keyEvent->keycode = Key::up;        break;
			case VK_DOWN:   keyEvent->keycode = Key::down;      break;
			case VK_BACK:   keyEvent->keycode = Key::backspace; break;
			case VK_DELETE: keyEvent->keycode = Key::deleteKey;    break;
			case VK_HOME:   keyEvent->keycode = Key::home;      break;
			case VK_END:    keyEvent->keycode = Key::end;       break;
			case VK_PRIOR:  keyEvent->keycode = Key::pageUp;    break;
			case VK_NEXT:   keyEvent->keycode = Key::pageDown;  break;
			case VK_RETURN: keyEvent->keycode = Key::enter;     break;
			default:        keyEvent->keycode = Key::unknown;   break;
			}

			keyEvent->text = "";
			keyEvent->direction = Direction::up;

			native->event = keyEvent;
		}
		break;

	case WM_CHAR:
		{
			if(wp >= ' ') // Filter control chars.
			{
				KeyEvent* keyEvent = new KeyEvent();
				keyEvent->keycode = Key::character;
				keyEvent->text += (char)wp;
				keyEvent->direction = Direction::up;
				native->event = keyEvent;
			}
		}
		break;

	case WM_PAINT:
		{
			RepaintEvent* repaintEvent = new RepaintEvent();
			repaintEvent->x = 0;
			repaintEvent->y = 0;
			repaintEvent->width = 100;
			repaintEvent->height = 100;
			native->event = repaintEvent;

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

			{ // Copy PixelBuffer to framebuffer
				int i, j, k;
				for(k = 0, i = 0; i < (int)px.height; ++i)
				{
					for(j = 0; j < (int)px.width; ++j, ++k)
					{
						*(framebuf + k) = RGB(px.buf[(j + i * px.width) * 3 + 2],
						                      px.buf[(j + i * px.width) * 3 + 1],
						                      px.buf[(j + i * px.width) * 3 + 0]);
					}
				}
			}

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(native->m_hwnd, &ps);
			BitBlt(hdc, 0, 0, px.width, px.height, pDC, 0, 0, SRCCOPY);
			EndPaint(native->m_hwnd, &ps);

			{ // Destroy bitmap (move to window.cc)
				SelectObject(pDC,old);
				DeleteDC(pDC);
				DeleteObject(ourbitmap);
			}
		}

		return DefWindowProc(hwnd, msg, wp, lp);
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

NativeWindowWin32::NativeWindowWin32(Window& window)
	: window(window)
{
	WNDCLASSEX wcex;
	WNDID wndId;

	std::memset(&wcex, 0, sizeof(wcex));

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

	wcex.lpszClassName = m_className = strdup("DrumGizmoClass");

	RegisterClassEx(&wcex);

	/*
	if(parent) {
		style = style | WS_CHILD;
		wndId = parent->getWndId();
	} else {
	*/
	//style = style | WS_OVERLAPPEDWINDOW;
	wndId = 0;
	//	}

	m_hwnd = CreateWindowEx(0/*ex_style*/, m_className,
	                        "DGBasisWidget",
	                        (WS_OVERLAPPEDWINDOW | WS_VISIBLE),
	                        window.x(), window.y(),
	                        window.width(), window.height(),
	                        wndId, nullptr,
	                        GetModuleHandle(nullptr), nullptr);

	SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
}

NativeWindowWin32::~NativeWindowWin32()
{
	UnregisterClass(m_className, GetModuleHandle(nullptr));
	free(m_className);
}

void NativeWindowWin32::setFixedSize(int width, int height)
{
	resize(width, height);
	LONG style =  GetWindowLong(m_hwnd, GWL_STYLE);
	style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	SetWindowLong(m_hwnd, GWL_STYLE, style);
}

void NativeWindowWin32::resize(int width, int height)
{
	SetWindowPos(m_hwnd, nullptr, -1, -1, (int)width, (int)height, SWP_NOMOVE);
	RECT r;
	GetClientRect(m_hwnd, &r);
	int w = width - r.right;
	int h = height - r.bottom;

	SetWindowPos(m_hwnd, nullptr, -1, -1, width + w, height + h, SWP_NOMOVE);
}

void NativeWindowWin32::move(int x, int y)
{
	SetWindowPos(m_hwnd, nullptr, (int)x, (int)y, -1, -1, SWP_NOSIZE);
}

void NativeWindowWin32::show()
{
	ShowWindow(m_hwnd, SW_SHOW);
}

void NativeWindowWin32::handleBuffer()
{
}

void NativeWindowWin32::hide()
{
	ShowWindow(m_hwnd, SW_HIDE);
}

void NativeWindowWin32::redraw()
{
	RedrawWindow(m_hwnd, nullptr, nullptr, RDW_ERASE|RDW_INVALIDATE);
	UpdateWindow(m_hwnd);
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

bool NativeWindowWin32::hasEvent()
{
	MSG msg;
	return PeekMessage(&msg, nullptr, 0, 0, 0) != 0;
}

Event* NativeWindowWin32::getNextEvent()
{
	Event* event = nullptr;

	MSG msg;
	if(GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	event = this->event;
	this->event = nullptr;

	return event;
}

} // GUI::

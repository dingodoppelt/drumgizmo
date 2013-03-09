/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            eventhandler.cc
 *
 *  Sun Oct  9 18:58:29 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include "eventhandler.h"

#include "globalcontext.h"

#include <stdio.h>

#ifdef X11
#include <X11/Xutil.h>
#endif/*X11*/

GUI::EventHandler::EventHandler(GlobalContext *gctx)
{
  this->gctx = gctx;
  last_click = 0;
#ifdef WIN32
  this->gctx->eventhandler = this;
  event = NULL;
#endif/*WIN32*/
}

bool GUI::EventHandler::hasEvent()
{
#ifdef X11
  return XPending(gctx->display);
#endif/*X11*/

#ifdef WIN32
	MSG		msg;
  return PeekMessage(&msg, NULL, 0, 0, 0) != 0;
#endif/*WIN32*/
  return false;
}

#ifdef WIN32

extern GUI::Window *gwindow;

#include "window.h"
LRESULT CALLBACK dialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
  static int last_x = 0;
  static int last_y = 0;

  GUI::EventHandler *handler =
    (GUI::EventHandler *) GetWindowLong(hwnd, GWL_USERDATA);

	switch(msg) {
	case WM_SIZE:
    {
      static bool first = true;
      if(!first) {
        GUI::ResizeEvent *e = new GUI::ResizeEvent();
        e->width = LOWORD(lp);
        e->height = HIWORD(lp);
        handler->event = e;
        first = false;
      }
    }
		break;

	case WM_MOVE:
    {
//      GUI::MoveEvent *e = new GUI::MoveEvent();
//      e->x = (int)(short) LOWORD(lp);
//      e->y = (int)(short) HIWORD(lp);
//      handler->event = e;
    }
		break;

	case WM_CLOSE:
    {
      GUI::CloseEvent *e = new GUI::CloseEvent();
      handler->event = e;
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
      
      GUI::MouseMoveEvent *e = new GUI::MouseMoveEvent();
      e->x = (int)(short) LOWORD(lp);
      e->y = (int)(short) HIWORD(lp);
      handler->event = e;

      last_x = e->x;
      last_y = e->y;

      //		xPos = (int)(short) LOWORD(lp);
      //		yPos = (int)(short) HIWORD(lp);
      //		fwKeys = wp;
    }
		break;

	case WM_MOUSEWHEEL:
    {
      GUI::ScrollEvent *e = new GUI::ScrollEvent();

      // NOTE: lp is coordinates in screen space, not client space.
      POINT p;
      p.x = (int)(short) LOWORD(lp);
      p.y = (int)(short) HIWORD(lp);
      ScreenToClient(hwnd, &p);

      e->x = p.x;
      e->y = p.y;
      e->delta = -1 * (short)HIWORD(wp) / 60;
      handler->event = e;
    }
 		break;

	case WM_LBUTTONUP:
    {
      GUI::ButtonEvent *e = new GUI::ButtonEvent();
      e->x = (int)(short) LOWORD(lp);
      e->y = (int)(short) HIWORD(lp);
      e->button = 0;
      e->direction = -1;
      handler->event = e;
    }
    //		xPos = (int)(short) LOWORD(lp);
    //		yPos = (int)(short) HIWORD(lp);
    //		fwKeys = wp;
		break;

	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
    {
      GUI::ButtonEvent *e = new GUI::ButtonEvent();
      e->x = (int)(short) LOWORD(lp);
      e->y = (int)(short) HIWORD(lp);
      e->button = 0;
      e->direction = 1;
      e->doubleclick = (msg == WM_LBUTTONDBLCLK);
      handler->event = e;
    }
    //xPos = (int)(short) LOWORD(lp);
		//yPos = (int)(short) HIWORD(lp);
		//fwKeys = wp;
		break;

	case WM_MBUTTONUP:
    {
      GUI::ButtonEvent *e = new GUI::ButtonEvent();
      e->x = (int)(short) LOWORD(lp);
      e->y = (int)(short) HIWORD(lp);
      e->button = 3;
      e->direction = -1;
      e->doubleclick = 0;
      handler->event = e;
    }
		//xPos = (int)(short) LOWORD(lp);
		//yPos = (int)(short) HIWORD(lp);
		//fwKeys = wp;
		break;

  case WM_MBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
    {
      GUI::ButtonEvent *e = new GUI::ButtonEvent();
      e->x = (int)(short) LOWORD(lp);
      e->y = (int)(short) HIWORD(lp);
      e->button = 3;
      e->direction = 1;
      e->doubleclick = (msg == WM_MBUTTONDBLCLK);
      handler->event = e;
    }
    //		xPos = (int)(short) LOWORD(lp);
    //		yPos = (int)(short) HIWORD(lp);
    //		fwKeys = wp;
		break;

	case WM_RBUTTONUP:
    {
      GUI::ButtonEvent *e = new GUI::ButtonEvent();
      e->x = (int)(short) LOWORD(lp);
      e->y = (int)(short) HIWORD(lp);
      e->button = 1;
      e->direction = -1;
      e->doubleclick = 0;
      handler->event = e;
    }
    //		xPos = (int)(short) LOWORD(lp);
    //		yPos = (int)(short) HIWORD(lp);
    //		fwKeys = wp;
		break;

  case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
    {
      GUI::ButtonEvent *e = new GUI::ButtonEvent();
      e->x = (int)(short) LOWORD(lp);
      e->y = (int)(short) HIWORD(lp);
      e->button = 1;
      e->direction = 1;
      e->doubleclick = (msg == WM_RBUTTONDBLCLK);
      handler->event = e;
    }
    //		xPos = (int)(short) LOWORD(lp);
    //		yPos = (int)(short) HIWORD(lp);
    //		fwKeys = wp;
		break;

	case WM_KEYDOWN:
    {
      GUI::KeyEvent *e = new GUI::KeyEvent();
      switch(wp) {
      case 37: e->keycode = GUI::KeyEvent::KEY_LEFT; break;
      case 39: e->keycode = GUI::KeyEvent::KEY_RIGHT; break;
      case 8: e->keycode = GUI::KeyEvent::KEY_BACKSPACE; break;
      case 46: e->keycode = GUI::KeyEvent::KEY_DELETE; break;
      case 36: e->keycode = GUI::KeyEvent::KEY_HOME; break;
      case 35: e->keycode = GUI::KeyEvent::KEY_END; break;
      default: e->keycode = GUI::KeyEvent::KEY_UNKNOWN; break;
      }
      e->text = "";
      e->direction = -1;
      handler->event = e;
    }
    //		xPos = (int)(short) LOWORD(lp);
    //		yPos = (int)(short) HIWORD(lp);
    //		fwKeys = wp;
		break;

	case WM_CHAR:
    {
      printf("WM_CHAR %d %d\n", (int)lp, (int)wp);
      if(wp >= ' ') { // Filter control chars.
        GUI::KeyEvent *e = new GUI::KeyEvent();
        e->keycode = GUI::KeyEvent::KEY_CHARACTER;
        e->text += (char)wp;
        e->direction = -1;
        handler->event = e;
      }
    }
    //		xPos = (int)(short) LOWORD(lp);
    //		yPos = (int)(short) HIWORD(lp);
    //		fwKeys = wp;
		break;

	case WM_PAINT:
    {
      GUI::RepaintEvent *e = new GUI::RepaintEvent();
      e->x = 0;
      e->y = 0;
      e->width = 100;
      e->height = 100;
      handler->event = e;


#if 1
      // Move to window.h (in class)
      HDC pDC;
      HBITMAP old;
      HBITMAP ourbitmap;
      int * framebuf;
      GUI::PixelBuffer &px = gwindow->wpixbuf;

      { // Create bitmap (move to window.cc)

        HDC hDC;
        BITMAPINFO bitmapinfo;
        hDC=CreateCompatibleDC(NULL);
        bitmapinfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bitmapinfo.bmiHeader.biWidth=px.width;
        bitmapinfo.bmiHeader.biHeight=-px.height; /* top-down */
        bitmapinfo.bmiHeader.biPlanes=1;
        bitmapinfo.bmiHeader.biBitCount=32;
        bitmapinfo.bmiHeader.biCompression=BI_RGB;
        bitmapinfo.bmiHeader.biSizeImage=0;
        bitmapinfo.bmiHeader.biClrUsed=256;
        bitmapinfo.bmiHeader.biClrImportant=256;
        ourbitmap=CreateDIBSection(hDC,&bitmapinfo,DIB_RGB_COLORS,(void**)&framebuf,0,0);
        pDC=CreateCompatibleDC(NULL);
        old=(HBITMAP__*)SelectObject(pDC,ourbitmap);
        DeleteDC(hDC);
        
      }


      { // Copy GUI::PixelBuffer to framebuffer (move to window.cc)

        int i,j,k;
        for (k=0,i=0;i<(int)px.height;i++)
          for (j=0;j<(int)px.width;j++,k++)
            *(framebuf+k)=RGB(px.buf[(j + i * px.width) * 3 + 2],
                              px.buf[(j + i * px.width) * 3 + 1],
                              px.buf[(j + i * px.width) * 3 + 0]);
      }
      
	PAINTSTRUCT	ps;
  //	RECT		rect;
	HDC			hdc;

  //  bool m_state = true;
  //	HBRUSH	m_brush;
  //	HPEN	m_penh, m_pens;		//Outer
  //	HPEN	m_penih, m_penis;	//Inner
  
	hdc = BeginPaint(handler->gctx->m_hwnd, &ps);
  BitBlt(hdc,0,0,px.width,px.height,pDC,0,0,SRCCOPY);
  EndPaint(handler->gctx->m_hwnd, &ps);




  { // Destroy bitmap (move to window.cc)

    SelectObject(pDC,old);
    DeleteDC(pDC);
    DeleteObject(ourbitmap);

  }







#else
	PAINTSTRUCT	ps;
  //	RECT		rect;
	HDC			hdc;
  hdc = BeginPaint(handler->gctx->m_hwnd, &ps);
	if(hdc) {
    //		GetClientRect(handler->gctx->m_hwnd, &rect);

    //Backgound
    //		FillRect(hdc, &rect, m_brush);

    /*
    POINT p[2];
    p[0].x = p[0].y = 10;
    p[1].x = p[1].y = 10;
    SelectObject(hdc, m_penis);
    Polyline(hdc, p, 2);
    */
    GUI::PixelBuffer &px = gwindow->wpixbuf;
    for(size_t y = 0; y < px.height; y++) {
      for(size_t x = 0; x < px.width; x++) {
        int col = *((int*)(&px.buf[(x + y * px.width) * 3])) & 0xffffff;
        SetPixel(hdc, x, y, col);
      }
    }

    /*
		//Edges
		drawHilight(hdc, m_state ? m_pens : m_penh, &rect);
		drawShadow(hdc, m_state ? m_penh : m_pens, &rect);

		//Lav rect 1 mindre (shrink)
		rect.left++;
		rect.right--;
		rect.top++;
		rect.bottom--;

		drawHilight(hdc, m_state ? m_penis : m_penih, &rect);
		drawShadow(hdc, m_state ? m_penih : m_penis, &rect);
    */
		EndPaint(handler->gctx->m_hwnd, &ps);
	}
  //DeleteDC(hdc); 
#endif







    }
		return DefWindowProc(hwnd, msg, wp, lp);
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}
#endif/*WIN32*/

GUI::Event *GUI::EventHandler::getNextEvent()
{
  Event *event = NULL;
#ifdef X11
  XEvent xe;
  XNextEvent(gctx->display, &xe);

  //printf("XEvent ?%d[%d]\n", ConfigureNotify, xe.type);

/**
 * KeyPress                2
 * KeyRelease              3
 * ButtonPress             4
 * ButtonRelease           5
 * MotionNotify            6
 * EnterNotify             7
 * LeaveNotify             8
 * FocusIn                 9
 * FocusOut                10
 * KeymapNotify            11
 * Expose                  12
 * GraphicsExpose          13
 * NoExpose                14
 * VisibilityNotify        15
 * CreateNotify            16
 * DestroyNotify           17
 * UnmapNotify             18
 * MapNotify               19
 * MapRequest              20
 * ReparentNotify          21
 * ConfigureNotify         22
 * ConfigureRequest        23
 * GravityNotify           24
 * ResizeRequest           25
 * CirculateNotify         26
 * CirculateRequest        27
 * PropertyNotify          28
 * SelectionClear          29
 * SelectionRequest        30
 * SelectionNotify         31
 * ColormapNotify          32
 * ClientMessage           33
 * MappingNotify           34
 * GenericEvent            35
 * LASTEvent               36      // must be bigger than any event #
 **/

  if(xe.type == MotionNotify) {
    MouseMoveEvent *e = new MouseMoveEvent();
    e->window_id = xe.xmotion.window;
    e->x = xe.xmotion.x;
    e->y = xe.xmotion.y;
    event = e;
  }

  if(xe.type == Expose && xe.xexpose.count == 0) {
    RepaintEvent *e = new RepaintEvent();
    e->window_id = xe.xexpose.window;
    e->x = xe.xexpose.x;
    e->y = xe.xexpose.y;
    e->width = xe.xexpose.width;
    e->height = xe.xexpose.height;
    event = e;
  }

  if(xe.type == ConfigureNotify) {
    /*
    if(hasEvent()) { // Hack to make sure only the last resize event is played.
      XEvent nxe;
      XPeekEvent(gctx->display, &nxe);
      if(xe.type == ConfigureNotify) return NULL;
    }
    */
    ResizeEvent *e = new ResizeEvent();
    e->window_id = xe.xconfigure.window;
    //    e->x = xe.xconfigure.x;
    //    e->y = xe.xconfigure.y;
    e->width = xe.xconfigure.width;
    e->height = xe.xconfigure.height;
    event = e;
  }

  if(xe.type == ButtonPress || xe.type == ButtonRelease) {
    if(xe.xbutton.button == 4 || xe.xbutton.button == 5) {
      ScrollEvent *e = new ScrollEvent();
      e->window_id = xe.xbutton.window;
      e->x = xe.xbutton.x;
      e->y = xe.xbutton.y;
      e->delta = xe.xbutton.button==4?-1:1;
      event = e;
    } else {
      ButtonEvent *e = new ButtonEvent();
      e->window_id = xe.xbutton.window;
      e->x = xe.xbutton.x;
      e->y = xe.xbutton.y;
      e->button = 0;
      e->direction = xe.type == ButtonPress?1:-1;
      e->doubleclick =
        xe.type == ButtonPress && (xe.xbutton.time - last_click) < 200;
      
      if(xe.type == ButtonPress) last_click = xe.xbutton.time;
      event = e;
    }
  }

  if(xe.type == KeyPress || xe.type == KeyRelease) {
    //    printf("key: %d\n", e.xkey.keycode);
    KeyEvent *e = new KeyEvent();
    e->window_id = xe.xkey.window;

    switch(xe.xkey.keycode) {
    case 113: e->keycode = KeyEvent::KEY_LEFT; break;
    case 114: e->keycode = KeyEvent::KEY_RIGHT; break;
    case 119: e->keycode = KeyEvent::KEY_DELETE; break;
    case 22: e->keycode = KeyEvent::KEY_BACKSPACE; break;
    case 110: e->keycode = KeyEvent::KEY_HOME; break;
    case 115: e->keycode = KeyEvent::KEY_END; break;
    default: e->keycode = KeyEvent::KEY_UNKNOWN; break;
    }

    char buf[1024];
    int sz = XLookupString(&xe.xkey, buf, sizeof(buf),  NULL, NULL);
    if(sz && e->keycode == KeyEvent::KEY_UNKNOWN) {
      e->keycode = KeyEvent::KEY_CHARACTER;
    }
    e->text.append(buf, sz);

    e->direction = xe.type == KeyPress?1:-1;
    event = e;
  }

  if(xe.type == ClientMessage &&
     (unsigned int)xe.xclient.data.l[0] == gctx->wmDeleteMessage) {
    CloseEvent *e = new CloseEvent();
    event = e;
  }

#endif/*X11*/

#ifdef WIN32
	MSG		msg;

	if(GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
	}

  //  printf("Got message: %p\n", this->event);

  event = this->event;
  this->event = NULL;
#endif/*WIN32*/

  return event;
}

void GUI::EventHandler::registerCloseHandler(void (*handler)(void *), void *ptr)
{
  this->closeHandler = handler;
  this->closeHandlerPtr = ptr;
}

void GUI::EventHandler::processEvents(Window *window)
{
  while(hasEvent()) {
    Event *event = getNextEvent();

    if(event == NULL) continue;

    //    Widget *widget = gctx->widgets[event->window_id];
    switch(event->type()) {
    case Event::Repaint:
      //      window->repaint((RepaintEvent*)event);
      window->redraw();
      break;
    case Event::Resize:
      {
        //      window->repaint((RepaintEvent*)event)
        ResizeEvent *re = (ResizeEvent*)event;
        if(re->width != window->width() || re->height != window->height()) {
          window->resized(re->width, re->height);
          window->repaint_r(NULL);
        }
      }
      break;
    case Event::MouseMove:
      {
        MouseMoveEvent *me = (MouseMoveEvent*)event;

        Widget *w = window->find(me->x, me->y);

        if(window->buttonDownFocus()) {
          Widget *w = window->buttonDownFocus();
          /*
          if(me->x < w->x()) me->x = w->x();
          if(me->x > w->x() + w->width()) me->x = w->x() + w->width();
          if(me->y < w->y()) me->y = w->y();
          if(me->y > w->y() + w->height()) me->y = w->y() + w->height();
          */
          me->x -= w->windowX();
          me->y -= w->windowY();

          window->buttonDownFocus()->mouseMoveEvent(me);
          break;
        }

        if(w) {
          me->x -= w->windowX();
          me->y -= w->windowY();
          w->mouseMoveEvent(me);
        }
      }
      break;
    case Event::Button:
      {
        ButtonEvent *be = (ButtonEvent *)event;

        Widget *w = window->find(be->x, be->y);

        if(window->buttonDownFocus()) {
          if(be->direction == -1) {
            Widget *w = window->buttonDownFocus();
            /*
            if(be->x < w->x()) be->x = w->x();
            if(be->x > w->x() + w->width()) be->x = w->x() + w->width();
            if(be->y < w->y()) be->y = w->y();
            if(be->y > w->y() + w->height()) be->y = w->y() + w->height();
            */
            be->x -= w->windowX();
            be->y -= w->windowY();

            w->buttonEvent(be);
            break;
          } else {
            window->setButtonDownFocus(NULL);
          }
        }

        if(w) {
          be->x -= w->windowX();
          be->y -= w->windowY();

          w->buttonEvent(be);

          if(be->direction == 1) {
            if(w->catchMouse()) window->setButtonDownFocus(w);
          } else {
            if(w->isFocusable()) window->setKeyboardFocus(w);
          }
        }
      }
      break;
    case Event::Scroll:
      {
        ScrollEvent *se = (ScrollEvent *)event;

        Widget *w = window->find(se->x, se->y);

        //printf("scroller (%d,%d) %p\n", se->x, se->y, w);

        if(w) {
          se->x -= w->windowX();
          se->y -= w->windowY();

          w->scrollEvent(se);
        }
      }
      break;
    case Event::Key:
      //      window->key((KeyEvent*)event);
      //      lineedit->keyEvent((KeyEvent*)event);
      if(window->keyboardFocus())
        window->keyboardFocus()->keyEvent((KeyEvent*)event);
      break;
    case Event::Close:
      if(closeHandler) closeHandler(closeHandlerPtr);
      //delete window;
      //window = NULL;
      break;
    }
    delete event;
  }
}

#ifdef TEST_EVENTHANDLER
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).

TEST_END;

#endif/*TEST_EVENTHANDLER*/

/* -*- Mode: ObjC; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_cocoa.mm
 *
 *  Fri Dec  2 20:31:03 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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
#include "nativewindow_cocoa.h"

#include "guievent.h"

#include <stdio.h>
#include <unistd.h>

#import <Cocoa/Cocoa.h>

#include "window.h"

#include <Availability.h>

#ifdef __MAC_OS_X_VERSION_MAX_ALLOWED
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 101300 // Before MacOSX 10.13 (High-Sierra)
#define STYLE_MASK                              \
	(NSClosableWindowMask |                       \
	 NSTitledWindowMask |                         \
	 NSResizableWindowMask)
#define IMAGE_FLAGS                             \
	(kCGBitmapByteOrder32Big |                    \
	 kCGImageAlphaPremultipliedLast)
#define EVENT_MASK                              \
	NSAnyEventMask
#else
#define STYLE_MASK                              \
	(NSWindowStyleMaskClosable |                  \
	 NSWindowStyleMaskTitled |                    \
	 NSWindowStyleMaskResizable)
#define IMAGE_FLAGS                             \
	(kCGImageByteOrder32Big |                     \
	 kCGImageAlphaPremultipliedLast)
#define EVENT_MASK                              \
	NSEventMaskAny
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 101400 // Before MacOSX 10.14 (Mojave)
// Nothing here yet...
#endif
#endif

@interface DGListener : NSWindow
{
@public
	NSWindow* window;
	GUI::NativeWindowCocoa* native;
}

- (id) initWithWindow:(NSWindow*)ref
               native:(GUI::NativeWindowCocoa*)_native;
- (void) dealloc;
- (void) windowDidResize;
- (void) windowWillResize;
- (void) windowWillClose;
- (void) unbindNative;
@end

@implementation DGListener
- (id) initWithWindow:(NSWindow*)ref
               native:(GUI::NativeWindowCocoa*)_native
{
	[super init];

	native = _native;
	window = ref;

	[[NSNotificationCenter defaultCenter]
	  addObserver:self
	     selector:@selector(windowDidResize)
	         name:NSWindowDidResizeNotification
	       object:ref];

	[[NSNotificationCenter defaultCenter]
	  addObserver:self
	     selector:@selector(windowWillResize)
	         name:NSWindowWillStartLiveResizeNotification
	       object:ref];

	[[NSNotificationCenter defaultCenter]
	  addObserver:self
	     selector:@selector(windowWillClose)
	         name:NSWindowWillCloseNotification
	       object:ref];

	[self windowWillResize]; // trigger to get the initial size as a size change

	return self;
}

- (void) dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}

- (void)windowDidResize
{
	if(!native)
	{
		return;
	}

	native->resized();
}

- (void)windowWillResize
{
	if(!native)
	{
		return;
	}

	native->resized();
}

- (void) windowWillClose
{
	if(!native)
	{
		return;
	}

	auto closeEvent = std::make_shared<GUI::CloseEvent>();
	native->pushBackEvent(closeEvent);
}

- (void) unbindNative
{
	native = nullptr;
}
@end

@interface DGView : NSView
{
	int colorBits;
	int depthBits;

@private
	GUI::NativeWindowCocoa* native;
	NSTrackingArea* trackingArea;
}

//- (id) initWithFrame:(NSRect)frame
//           colorBits:(int)numColorBits
//           depthBits:(int)numDepthBits;
- (void) updateTrackingAreas;

- (void) mouseEntered:(NSEvent *)event;
- (void) mouseExited:(NSEvent *)event;
- (void) mouseMoved:(NSEvent*)event;
- (void) mouseDown:(NSEvent*)event;
- (void) mouseUp:(NSEvent*)event;
- (void) rightMouseDown:(NSEvent*)event;
- (void) rightMouseUp:(NSEvent*)event;
- (void) otherMouseDown:(NSEvent*)event;
- (void) otherMouseUp:(NSEvent*)event;
- (void) mouseDragged:(NSEvent*)event;
- (void) rightMouseDragged:(NSEvent*)event;
- (void) otherMouseDragged:(NSEvent*)event;
- (void) scrollWheel:(NSEvent*)event;
- (void) keyDown:(NSEvent*)event;
- (void) keyUp:(NSEvent*)event;

- (void) dealloc;
- (void) bindNative:(GUI::NativeWindowCocoa*)native;
- (void) unbindNative;
@end

@implementation DGView
//- (id) initWithFrame:(NSRect)frame
//           colorBits:(int)numColorBits
//           depthBits:(int)numDepthBits
//{
//	[super init];
//	[self updateTrackingAreas];
//	return self;
//}

- (void) updateTrackingAreas
{
	if(trackingArea != nil)
	{
		[self removeTrackingArea:trackingArea];
		[trackingArea release];
	}

	int opts =
		NSTrackingMouseEnteredAndExited |
		NSTrackingMouseMoved |
		NSTrackingActiveAlways;

	trackingArea =
	  [[NSTrackingArea alloc] initWithRect:[self bounds]
	                               options:opts
	                                 owner:self
	                              userInfo:nil];
	[self addTrackingArea:trackingArea];
}

- (void) mouseEntered:(NSEvent *)event
{
	[super mouseEntered:event];
	auto frame = [self frame];
	NSPoint loc = [event locationInWindow];
	auto mouseEnterEvent = std::make_shared<GUI::MouseEnterEvent>();
	mouseEnterEvent->x = loc.x - frame.origin.x;
	mouseEnterEvent->y = frame.size.height - loc.y - frame.origin.y;
	native->pushBackEvent(mouseEnterEvent);
	//[[NSCursor pointingHandCursor] set];
}

- (void) mouseExited:(NSEvent *)event
{
	[super mouseExited:event];
	auto frame = [self frame];
	NSPoint loc = [event locationInWindow];
	auto mouseLeaveEvent = std::make_shared<GUI::MouseLeaveEvent>();
	mouseLeaveEvent->x = loc.x - frame.origin.x;
	mouseLeaveEvent->y = frame.size.height - loc.y - frame.origin.y;
	native->pushBackEvent(mouseLeaveEvent);
	//[[NSCursor arrowCursor] set];
}

- (void) mouseMoved:(NSEvent*)event
{
	auto frame = [self frame];
	NSPoint loc = [event locationInWindow];
	auto mouseMoveEvent = std::make_shared<GUI::MouseMoveEvent>();
	mouseMoveEvent->x = loc.x - frame.origin.x;
	mouseMoveEvent->y = frame.size.height - loc.y - frame.origin.y;
	native->pushBackEvent(mouseMoveEvent);
}

- (void) mouseDown:(NSEvent*)event
{
	auto frame = [self frame];
	NSPoint loc = [event locationInWindow];

	auto buttonEvent = std::make_shared<GUI::ButtonEvent>();
	buttonEvent->x = loc.x - frame.origin.x;
	buttonEvent->y = frame.size.height - loc.y - frame.origin.y;
	switch((int)[event buttonNumber])
	{
	case 0:
		buttonEvent->button = GUI::MouseButton::left;
		break;
	case 1:
		buttonEvent->button = GUI::MouseButton::right;
		break;
	case 2:
		buttonEvent->button = GUI::MouseButton::middle;
		break;
	default:
		return;
	}
	buttonEvent->direction = GUI::Direction::down;
	buttonEvent->doubleClick = [event clickCount] == 2;
	native->pushBackEvent(buttonEvent);

	[super mouseDown: event];
}

- (void) mouseUp:(NSEvent*)event
{
	auto frame = [self frame];
	NSPoint loc = [event locationInWindow];

	auto buttonEvent = std::make_shared<GUI::ButtonEvent>();
	buttonEvent->x = loc.x - frame.origin.x;
	buttonEvent->y = frame.size.height - loc.y - frame.origin.y;
	switch((int)[event buttonNumber])
	{
	case 0:
		buttonEvent->button = GUI::MouseButton::left;
		break;
	case 1:
		buttonEvent->button = GUI::MouseButton::right;
		break;
	case 2:
		buttonEvent->button = GUI::MouseButton::middle;
		break;
	default:
		return;
	}
	buttonEvent->direction = GUI::Direction::up;
	buttonEvent->doubleClick = false;
	native->pushBackEvent(buttonEvent);

	[super mouseUp: event];
}

- (void) rightMouseDown:(NSEvent*)event
{
	[self mouseDown: event];
	[super rightMouseDown: event];
}

- (void) rightMouseUp:(NSEvent*)event
{
	[self mouseUp: event];
	[super rightMouseUp: event];
}

- (void) otherMouseDown:(NSEvent*)event
{
	[self mouseDown: event];
	[super otherMouseDown: event];
}

- (void) otherMouseUp:(NSEvent*)event
{
	[self mouseUp: event];
	[super otherMouseUp: event];
}

- (void) mouseDragged:(NSEvent*)event
{
	[self mouseMoved: event];
	[super mouseDragged: event];
}

- (void) rightMouseDragged:(NSEvent*)event
{
	[self mouseMoved: event];
	[super rightMouseDragged: event];
}

- (void) otherMouseDragged:(NSEvent*)event
{
	[self mouseMoved: event];
	[super otherMouseDragged: event];
}

- (void) scrollWheel:(NSEvent*)event
{
	auto frame = [self frame];
	NSPoint loc = [event locationInWindow];

	auto scrollEvent = std::make_shared<GUI::ScrollEvent>();
	scrollEvent->x = loc.x - frame.origin.x;
	scrollEvent->y = frame.size.height - loc.y - frame.origin.y;
	scrollEvent->delta = [event deltaY] * -1.0f;
	native->pushBackEvent(scrollEvent);

	[super scrollWheel: event];
}

- (void) keyDown:(NSEvent*)event
{
	const NSString* chars = [event characters];
	const char* str = [chars UTF8String];

	auto keyEvent = std::make_shared<GUI::KeyEvent>();

	switch([event keyCode])
	{
	case 123: keyEvent->keycode = GUI::Key::left; break;
	case 124: keyEvent->keycode = GUI::Key::right; break;
	case 126: keyEvent->keycode = GUI::Key::up; break;
	case 125: keyEvent->keycode = GUI::Key::down; break;
	case 117: keyEvent->keycode = GUI::Key::deleteKey; break;
	case 51:  keyEvent->keycode = GUI::Key::backspace; break;
	case 115: keyEvent->keycode = GUI::Key::home; break;
	case 119: keyEvent->keycode = GUI::Key::end; break;
	case 121: keyEvent->keycode = GUI::Key::pageDown; break;
	case 116: keyEvent->keycode = GUI::Key::pageUp; break;
	case 36:  keyEvent->keycode = GUI::Key::enter; break;
	default:  keyEvent->keycode = GUI::Key::unknown; break;
	}

	if(strlen(str) && keyEvent->keycode == GUI::Key::unknown)
	{
		keyEvent->keycode = GUI::Key::character;
	}

	keyEvent->text = str; // TODO: UTF8 decode
	keyEvent->direction = GUI::Direction::down;

	native->pushBackEvent(keyEvent);
	[super keyDown: event];
}

- (void) keyUp:(NSEvent*)event
{
	const NSString* chars = [event characters];
	const char* str = [chars UTF8String];
	auto keyEvent = std::make_shared<GUI::KeyEvent>();

	switch([event keyCode])
	{
	case 123: keyEvent->keycode = GUI::Key::left; break;
	case 124: keyEvent->keycode = GUI::Key::right; break;
	case 126: keyEvent->keycode = GUI::Key::up; break;
	case 125: keyEvent->keycode = GUI::Key::down; break;
	case 117: keyEvent->keycode = GUI::Key::deleteKey; break;
	case 51:  keyEvent->keycode = GUI::Key::backspace; break;
	case 115: keyEvent->keycode = GUI::Key::home; break;
	case 119: keyEvent->keycode = GUI::Key::end; break;
	case 121: keyEvent->keycode = GUI::Key::pageDown; break;
	case 116: keyEvent->keycode = GUI::Key::pageUp; break;
	case 36:  keyEvent->keycode = GUI::Key::enter; break;
	default:  keyEvent->keycode = GUI::Key::unknown; break;
	}

	if(strlen(str) && keyEvent->keycode == GUI::Key::unknown)
	{
		keyEvent->keycode = GUI::Key::character;
	}

	keyEvent->text = str; // TODO: UTF8 decode
	keyEvent->direction = GUI::Direction::up;

	native->pushBackEvent(keyEvent);
	[super keyUp: event];
}

- (void) dealloc
{
	[super dealloc];
}

- (void)bindNative:(GUI::NativeWindowCocoa*)_native
{
	native = _native;
}

- (void) unbindNative
{
	native = nullptr;
}
@end


namespace GUI
{

struct priv
{
	NSWindow* window;
	DGView* view;
	id listener;
	id parent_view;
	std::uint8_t* pixel_buffer{nullptr};
	std::size_t pixel_buffer_width{0};
	std::size_t pixel_buffer_height{0};
};

NativeWindowCocoa::NativeWindowCocoa(void* native_window, Window& window)
	: window(window)
	, priv(new struct priv())
	, native_window(native_window)
{
	[NSAutoreleasePool new];
	[NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

	priv->view = [DGView new];

	[priv->view bindNative:this];

	if(native_window)
	{
		if(sizeof(std::size_t) == sizeof(unsigned int)) // 32 bit machine
		{
			WindowRef ptr = (WindowRef)native_window;
			priv->window = [[[NSWindow alloc] initWithWindowRef:ptr] retain];
			priv->parent_view = [priv->window contentView];
		}
		else  // 64 bit machine
		{
			priv->parent_view = (NSView*)native_window;
			priv->window = [priv->parent_view window];
		}

		[priv->parent_view addSubview:priv->view];
		[priv->view display];
		[priv->parent_view setNeedsDisplay:YES];
	}
	else
	{
		priv->window =
			[[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 10, 10)
			                             styleMask:STYLE_MASK
			                               backing:NSBackingStoreBuffered
			                                 defer:NO]
			  retain];
		[priv->window setLevel:NSStatusWindowLevel];
	}

	priv->listener =
		[[[DGListener alloc] initWithWindow:priv->window
		                             native:this]
		  retain];

	if(native_window)
	{
		[[priv->window contentView] addSubview:priv->view];
	}
	else
	{
		[priv->window setReleasedWhenClosed:NO];
		[priv->window setContentView:priv->view];
	}

	scale = [[NSScreen mainScreen] backingScaleFactor];

	[priv->view setWantsLayer:YES];
	[priv->view setLayerContentsPlacement:NSViewLayerContentsPlacementTopLeft];
	[priv->view updateTrackingAreas];

	if(!native_window)
	{
	  hide();
	}
}

NativeWindowCocoa::~NativeWindowCocoa()
{
	// Make the garbage collector able to collect the ObjC objects:
	if(visible())
	{
		hide();
	}

	[priv->listener unbindNative];
	[priv->listener release];

	[priv->view unbindNative];
	[priv->view release];

	if(native_window)
	{
		if(sizeof(std::size_t) == sizeof(unsigned int)) // 32 bit machine
		{
			[priv->window release];
		}
		else
		{
			// in 64-bit the window was not created by us
		}
	}
	else
	{
		[priv->window release];
	}
}

void NativeWindowCocoa::setFixedSize(std::size_t width, std::size_t height)
{
	resize(width, height);
	[priv->window setMinSize:NSMakeSize(width, height + 22)];
	[priv->window setMaxSize:NSMakeSize(width, height + 22)];
}

void NativeWindowCocoa::setAlwaysOnTop(bool always_on_top)
{
	if(always_on_top)
	{
		[priv->window setLevel: NSStatusWindowLevel];
	}
	else
	{
		[priv->window setLevel: NSNormalWindowLevel];
	}
}

void NativeWindowCocoa::resize(std::size_t width, std::size_t height)
{
	[priv->window setContentSize:NSMakeSize(width, height)];
}

std::pair<std::size_t, std::size_t> NativeWindowCocoa::getSize() const
{
	if(native_window)
	{
		auto frame = [priv->parent_view frame];
		return {frame.size.width, frame.size.height - frame.origin.y};
	}
	else
	{
		NSSize size = [priv->view frame].size;
		return {size.width, size.height};
	}
}

void NativeWindowCocoa::move(int x, int y)
{
	NSRect screen = [[NSScreen mainScreen] frame];
	[priv->window setFrameTopLeftPoint:NSMakePoint(x, screen.size.height - y)];
}

std::pair<int, int> NativeWindowCocoa::getPosition() const
{
	NSRect screen = [[NSScreen mainScreen] frame];
	NSPoint pos = [[priv->window contentView] frame].origin;
	return {pos.x, screen.size.height - pos.y};
}

void NativeWindowCocoa::show()
{
	if(!native_window)
	{
		[priv->window makeKeyAndOrderFront:priv->window];
		[NSApp activateIgnoringOtherApps:YES];
	}
}

void NativeWindowCocoa::hide()
{
	if(!native_window)
	{
		[priv->window orderOut:priv->window];
	}
}

bool NativeWindowCocoa::visible() const
{
	return [priv->window isVisible];
}

void NativeWindowCocoa::redraw(const Rect& dirty_rect)
{
	NSSize size;
	if(native_window)
	{
		size = [priv->parent_view frame].size;
	}
	else
	{
		size = [priv->view frame].size;
	}

	std::size_t width = size.width;
	std::size_t height = size.height;

	if(priv->pixel_buffer == nullptr ||
	   priv->pixel_buffer_width != width ||
	   priv->pixel_buffer_height != height)
	{
		if(priv->pixel_buffer) delete[] priv->pixel_buffer;
		priv->pixel_buffer = new std::uint8_t[width * height * 4];
		priv->pixel_buffer_width = width;
		priv->pixel_buffer_height = height;
	}

	CGColorSpaceRef rgb = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	CGContextRef gc =
		CGBitmapContextCreate(priv->pixel_buffer, width, height,
		                      8, width * 4, rgb,
		                      IMAGE_FLAGS);
	CGColorSpaceRelease(rgb);

	size_t pitch = CGBitmapContextGetBytesPerRow(gc);
	uint8_t *buffer = (uint8_t *)CGBitmapContextGetData(gc);

	struct Pixel
	{
		std::uint8_t red;
		std::uint8_t green;
		std::uint8_t blue;
		std::uint8_t alpha;
	};
	std::uint8_t* pixels = window.wpixbuf.buf;
	for(std::size_t y = dirty_rect.y1; y < std::min(dirty_rect.y2, height); ++y)
	{
		Pixel *row = (Pixel *)(buffer + y * pitch);
		for(std::size_t x = dirty_rect.x1; x < std::min(dirty_rect.x2, width); ++x)
		{
			row[x] = *(Pixel*)&pixels[(y * width + x) * 3];
			row[x].alpha = 0xff;
		}
	}
	CGImageRef image = CGBitmapContextCreateImage(gc);
	CGContextRelease(gc);

	auto nsImage = [[NSImage alloc] initWithCGImage:image size:NSZeroSize];

	id layerContents = [nsImage layerContentsForContentsScale:scale];
	[[priv->view layer] setContents:layerContents];
	updateLayerOffset();
	[[priv->view layer] setContentsScale:scale];
}

void NativeWindowCocoa::setCaption(const std::string &caption)
{
	NSString* title =
		[NSString stringWithCString:caption.data()
		                   encoding:[NSString defaultCStringEncoding]];
	[priv->window setTitle:title];
}

void NativeWindowCocoa::grabMouse(bool grab)
{
}

void NativeWindowCocoa::updateLayerOffset()
{
	if(native_window)
	{
		//auto r1 = [priv->parent_view frame];
		auto r2 = [priv->view frame];

		CATransform3D t = [[priv->view layer] transform];
		if(t.m42 != -r2.origin.y)
		{
			t.m42 = -r2.origin.y; // y
			[[priv->view layer] setTransform:t];
		}
	}
}

EventQueue NativeWindowCocoa::getEvents()
{
	if(first)
	{
		resized();
		first = false;
	}

	// If this is the root window, process the events - event processing will
	// be handled by the hosting window if the window is embedded.
	if(!native_window)
	{
		NSEvent* event = nil;
		do
		{
			event = [NSApp nextEventMatchingMask:EVENT_MASK
			                           untilDate:[NSDate distantPast]
			                              inMode:NSDefaultRunLoopMode
			                             dequeue:YES];
			[NSApp sendEvent:event];
		}
		while(event != nil);
	}

	EventQueue events;
	std::swap(events, event_queue);
	return events;
}

void* NativeWindowCocoa::getNativeWindowHandle() const
{
	if(sizeof(std::size_t) == sizeof(unsigned int)) // 32 bit machine
	{
		return [priv->window windowRef];
	}
	else // 64 bit machine
	{
		return [priv->window contentView];
	}
}

Point NativeWindowCocoa::translateToScreen(const Point& point)
{
	NSRect e = [[NSScreen mainScreen] frame];
	NSRect frame;
	if(native_window)
	{
		frame = [priv->parent_view frame];
	}
	else
	{
		frame = [priv->view frame];
	}

	NSRect rect { { point.x + frame.origin.x,
	                frame.size.height - point.y + frame.origin.y},
	              {0.0, 0.0} };
	rect = [priv->window convertRectToScreen:rect];

	return { (int)rect.origin.x, (int)(e.size.height - rect.origin.y) };
}

Window& NativeWindowCocoa::getWindow()
{
	return window;
}

PixelBuffer& NativeWindowCocoa::getWindowPixbuf()
{
	window.updateBuffer();
	return window.wpixbuf;
}

void NativeWindowCocoa::resized()
{
	if(native_window)
	{
		NSRect frame = [priv->parent_view frame];
		[priv->view setFrame:frame];
		[priv->view updateTrackingAreas];
		updateLayerOffset();
	}

	auto resizeEvent = std::make_shared<GUI::ResizeEvent>();
	resizeEvent->width = 42; // size is not actually used
	resizeEvent->height = 42; // size is not actually used
	pushBackEvent(resizeEvent);
}

void NativeWindowCocoa::pushBackEvent(std::shared_ptr<Event> event)
{
	event_queue.push_back(event);
	redraw({});
}

} // GUI::

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

/*

loop:
http://stackoverflow.com/questions/6732400/cocoa-integrate-nsapplication-into-an-existing-c-mainloop

draw pixels:
http://stackoverflow.com/questions/10955913/how-can-i-display-an-array-of-pixels-on-a-nswindow
*/

#include <stdio.h>
#include <unistd.h>

#import <Cocoa/Cocoa.h>

bool running = true;

@interface MyWindow : NSWindow
{
@public
  //  MyView* my_view;
}

- (id) initWithContentRect:(NSRect)contentRect
                 styleMask:(unsigned int)aStyle
                   backing:(NSBackingStoreType)bufferingType
                     defer:(BOOL)flag;
//- (void) setMyView:(MyView*)view;
- (BOOL) windowShouldClose:(id)sender;
- (void) mouseMoved:(NSEvent*)event;
- (void) drawRect:(NSRect)dirtyRect;
@end

@implementation MyWindow

- (id)initWithContentRect:(NSRect)contentRect
                styleMask:(unsigned int)aStyle
                  backing:(NSBackingStoreType)bufferingType
                    defer:(BOOL)flag
{
        NSWindow* result = [super initWithContentRect:contentRect
                                            styleMask:(NSClosableWindowMask |
                                                       NSTitledWindowMask |
                                                       NSResizableWindowMask)
                                              backing:NSBackingStoreBuffered defer:NO];

        [result setAcceptsMouseMovedEvents:YES];
        [result setLevel: CGShieldingWindowLevel() + 1];

        return result;
}

//- (void)setMyView:(MyView*)view
//{
//  //my_view = view;
//      //[self setContentSize:NSMakeSize(view->width, view->height) ];
//}

- (BOOL)windowShouldClose:(id)sender
{
  printf("closing!\n");
  //if (puglview->closeFunc)
  //            puglview->closeFunc(puglview);
  running = false;
  return YES;
}

#define WIDTH 100
#define HEIGHT 100
#define SIZE (WIDTH*HEIGHT)
#define BYTES_PER_PIXEL 2
#define BITS_PER_COMPONENT 5
#define BITS_PER_PIXEL 16

- (void)drawRect:(NSRect)dirtyRect
{
}
- (void) mouseMoved:(NSEvent*)event
{
  /*
        if (puglview->motionFunc) {
                NSPoint loc = [event locationInWindow];
                puglview->mods = getModifiers(puglview, event);
                puglview->motionFunc(puglview, loc.x, puglview->height - loc.y);
        }
  */
  NSPoint loc = [event locationInWindow];
  printf("mouseMove: %f %f\n", loc.x, loc.y);
}

@end




@interface MyView : NSView
{
        int colorBits;
        int depthBits;
@public
  //PuglView* puglview;

  NSTrackingArea* trackingArea;
}

- (id) initWithFrame:(NSRect)frame
           colorBits:(int)numColorBits
           depthBits:(int)numDepthBits;
- (void) reshape;
- (void) drawRect:(NSRect)rect;

@end
@implementation MyView

- (id) initWithFrame:(NSRect)frame
           colorBits:(int)numColorBits
           depthBits:(int)numDepthBits
{
  colorBits = numColorBits;
  depthBits = numDepthBits;

  //NSOpenGLPixelFormatAttribute pixelAttribs[16] = {
  //  NSOpenGLPFADoubleBuffer,
  //  NSOpenGLPFAAccelerated,
  //  NSOpenGLPFAColorSize,
  //  colorBits,
  //  NSOpenGLPFADepthSize,
  //  depthBits,
  //  0
  //};
  //
  //NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc]
  //            initWithAttributes:pixelAttribs];
  //
  //if (pixelFormat) {
  //  self = [super initWithFrame:frame pixelFormat:pixelFormat];
  //  [pixelFormat release];
  //  if (self) {
  //    [[self openGLContext] makeCurrentContext];
  //    [self reshape];
  //  }
  //} else {
  //  self = nil;
  //}

  return self;
}
- (void) reshape
{
  //[[self openGLContext] update];
  //
  //NSRect bounds = [self bounds];
  //int    width  = bounds.size.width;
  //int    height = bounds.size.height;
  //
  //if (puglview) {
  //  /* NOTE: Apparently reshape gets called when the GC gets around to
  //     deleting the view (?), so we must have reset puglview to NULL when
  //     this comes around.
  //  */
  //  if (puglview->reshapeFunc) {
  //    puglview->reshapeFunc(puglview, width, height);
  //  } else {
  //    puglDefaultReshape(puglview, width, height);
  //  }
  //
  //  puglview->width  = width;
  //  puglview->height = height;
  //}
}
- (void) drawRect:(NSRect)rect
{
  printf("drawRect\n");

  // Get current context
  CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];

  // Colorspace RGB
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

  // Pixel Matrix allocation
  unsigned short *pixels = calloc(SIZE, sizeof(unsigned short));

  // Random pixels will give you a non-organized RAINBOW
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      pixels[i+ j*HEIGHT] = arc4random() % USHRT_MAX;
    }
  }

  // Provider
  CGDataProviderRef provider = CGDataProviderCreateWithData(nil, pixels, SIZE, nil);

  // CGImage
  CGImageRef image = CGImageCreate(WIDTH,
            HEIGHT,
            BITS_PER_COMPONENT,
            BITS_PER_PIXEL,
            BYTES_PER_PIXEL*WIDTH,
            colorSpace,
            kCGImageAlphaNoneSkipFirst,
            // xRRRRRGGGGGBBBBB - 16-bits, first bit is ignored!
            provider,
            nil, //No decode
            NO,  //No interpolation
            kCGRenderingIntentDefault); // Default rendering

  // Draw
  CGContextDrawImage(context, CGRectMake(0, 0, WIDTH, HEIGHT), image);

  // Once everything is written on screen we can release everything
  CGImageRelease(image);
  CGColorSpaceRelease(colorSpace);
  CGDataProviderRelease(provider);
}
@end

#include "window.h"

namespace GUI
{

NativeWindowCocoa::NativeWindowCocoa(void* native_window, Window& window)
	: buffer(nullptr)
	, window(window)
{
  [NSAutoreleasePool new];
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  /*
  id menubar = [[NSMenu new] autorelease];
  id appMenuItem = [[NSMenuItem new] autorelease];
  [menubar addItem:appMenuItem];
  [NSApp setMainMenu:menubar];
  id appMenu = [[NSMenu new] autorelease];
  id appName = [[NSProcessInfo processInfo] processName];
  id quitTitle = [@"Quit " stringByAppendingString:appName];
  id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
    action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
  [appMenu addItem:quitMenuItem];
  [appMenuItem setSubmenu:appMenu];
  */
  id window = [[[MyWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200)
          styleMask:NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO]
  autorelease];
  [window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
  //  [window setTitle:appName];
  [window makeKeyAndOrderFront:nil];

  id my_view       = [MyView new];
  [window setContentView:my_view];
  [NSApp activateIgnoringOtherApps:YES];
  [window makeFirstResponder:my_view];

  //  [NSApp run];
}

NativeWindowCocoa::~NativeWindowCocoa()
{
}

void NativeWindowCocoa::setFixedSize(int width, int height)
{
}

void NativeWindowCocoa::resize(int width, int height)
{
}

void NativeWindowCocoa::move(int x, int y)
{
}

void NativeWindowCocoa::show()
{
}

void NativeWindowCocoa::hide()
{
}

void NativeWindowCocoa::handleBuffer()
{
}

void NativeWindowCocoa::redraw()
{
}

void NativeWindowCocoa::setCaption(const std::string &caption)
{
}

void NativeWindowCocoa::grabMouse(bool grab)
{
}

bool NativeWindowCocoa::hasEvent()
{
	return false;
}

std::shared_ptr<Event> NativeWindowCocoa::getNextEvent()
{
  //[window setNeedsDisplay: YES];

  while(running) {
    NSEvent * event;

    do
      {
        event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefa\
ultRunLoopMode dequeue:YES];


        //Convert the cocoa events to something useful here and add them to your own event queue

        [NSApp sendEvent: event];
      }
    while(event != nil);

    //printf("loop\n");
    usleep(10000);
  }

	return nullptr;
}

std::shared_ptr<Event> NativeWindowCocoa::peekNextEvent()
{
	return nullptr;
}

} // GUI::

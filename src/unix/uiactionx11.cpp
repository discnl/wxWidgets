/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/uiactionx11.cpp
// Purpose:     wxUIActionSimulator implementation
// Author:      Kevin Ollivier, Steven Lamerton, Vadim Zeitlin
// Created:     2010-03-06
// Copyright:   (c) 2010 Kevin Ollivier
//              (c) 2010 Steven Lamerton
//              (c) 2010-2016 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"

#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"
#include "wx/event.h"
#include "wx/evtloop.h"

#include "wx/private/uiaction.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#if wxUSE_XTEST
#include <X11/extensions/XTest.h>
#endif

#include "wx/unix/utilsx11.h"

namespace
{

// Base class for both available X11 implementations.
class wxUIActionSimulatorX11Impl : public wxUIActionSimulatorImpl
{
public:
    // Return the most appopriate implementation to use: if XTest is available,
    // use it, otherwise use plain X11 calls.
    //
    // The returned pointer is owned by the caller.
    static wxUIActionSimulatorImpl* New();

    virtual bool MouseMove(long x, long y);
    virtual bool MouseDown(int button = wxMOUSE_BTN_LEFT);
    virtual bool MouseUp(int button = wxMOUSE_BTN_LEFT);

    virtual bool DoKey(int keycode, int modifiers, bool isDown);

protected:
    // This ctor takes ownership of the display.
    explicit wxUIActionSimulatorX11Impl(wxX11Display& display)
        : m_display(display)
    {
    }

    wxX11Display m_display;

private:
    // Common implementation of Mouse{Down,Up}() which just forwards to
    // DoX11Button() after translating wx button to X button constant.
    bool SendButtonEvent(int button, bool isDown);

    virtual bool DoX11Button(int xbutton, bool isDown) = 0;
    virtual bool DoX11MouseMove(long x, long y) = 0;
    virtual bool DoX11Key(KeyCode xkeycode, int modifiers, bool isDown) = 0;

    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulatorX11Impl);
};

// Implementation using just plain X11 calls.
class wxUIActionSimulatorPlainX11Impl : public wxUIActionSimulatorX11Impl
{
public:
    explicit wxUIActionSimulatorPlainX11Impl(wxX11Display& display)
        : wxUIActionSimulatorX11Impl(display)
    {
    }

private:
    virtual bool DoX11Button(int xbutton, bool isDown);
    virtual bool DoX11MouseMove(long x, long y);
    virtual bool DoX11Key(KeyCode xkeycode, int modifiers, bool isDown);

    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulatorPlainX11Impl);
};

bool wxUIActionSimulatorX11Impl::SendButtonEvent(int button, bool isDown)
{
    if ( !m_display )
        return false;

    int xbutton;
    switch (button)
    {
        case wxMOUSE_BTN_LEFT:
            xbutton = 1;
            break;
        case wxMOUSE_BTN_MIDDLE:
            xbutton = 2;
            break;
        case wxMOUSE_BTN_RIGHT:
            xbutton = 3;
            break;
        default:
            wxFAIL_MSG("Unsupported button passed in.");
            return false;
    }

    return DoX11Button(xbutton, isDown);
}

bool wxUIActionSimulatorPlainX11Impl::DoX11Button(int xbutton, bool isDown)
{
    XEvent event;
    memset(&event, 0x00, sizeof(event));

    event.type = isDown ? ButtonPress : ButtonRelease;
    event.xbutton.button = xbutton;
    event.xbutton.same_screen = True;

    XQueryPointer(m_display, m_display.DefaultRoot(),
                  &event.xbutton.root, &event.xbutton.window,
                  &event.xbutton.x_root, &event.xbutton.y_root,
                  &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    event.xbutton.subwindow = event.xbutton.window;

    while (event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(m_display, event.xbutton.window,
                      &event.xbutton.root, &event.xbutton.subwindow,
                      &event.xbutton.x_root, &event.xbutton.y_root,
                      &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    XSendEvent(m_display, PointerWindow, True, 0xfff, &event);

    return true;
}

bool wxUIActionSimulatorPlainX11Impl::DoX11MouseMove(long x, long y)
{
    Window root = m_display.DefaultRoot();
    XWarpPointer(m_display, None, root, 0, 0, 0, 0, x, y);
    return true;
}

bool
wxUIActionSimulatorPlainX11Impl::DoX11Key(KeyCode xkeycode,
                                          int modifiers,
                                          bool isDown)
{
    int mask, type;

    if ( isDown )
    {
        type = KeyPress;
        mask = KeyPressMask;
    }
    else
    {
        type = KeyRelease;
        mask = KeyReleaseMask;
    }

    Window focus;
    int revert;
    XGetInputFocus(m_display, &focus, &revert);
    if (focus == None)
        return false;

    int mod = 0;

    if (modifiers & wxMOD_SHIFT)
        mod |= ShiftMask;
    //Mod1 is alt in the vast majority of cases
    if (modifiers & wxMOD_ALT)
        mod |= Mod1Mask;
    if (modifiers & wxMOD_CMD)
        mod |= ControlMask;

    XKeyEvent event;
    event.display = m_display;
    event.window = focus;
    event.root = DefaultRootWindow(event.display);
    event.subwindow = None;
    event.time = CurrentTime;
    event.x = 1;
    event.y = 1;
    event.x_root = 1;
    event.y_root = 1;
    event.same_screen = True;
    event.type = type;
    event.state = mod;
    event.keycode = xkeycode;

    XSendEvent(event.display, event.window, True, mask, (XEvent*) &event);

    return true;
}

#if wxUSE_XTEST

// Implementation using XTest extension.
class wxUIActionSimulatorXTestImpl : public wxUIActionSimulatorX11Impl
{
public:
    explicit wxUIActionSimulatorXTestImpl(wxX11Display& display)
        : wxUIActionSimulatorX11Impl(display)
    {
    }

private:
    virtual bool DoX11Button(int xbutton, bool isDown);
    virtual bool DoX11MouseMove(long x, long y);
    virtual bool DoX11Key(KeyCode xkeycode, int modifiers, bool isDown);

    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulatorXTestImpl);
};

bool wxUIActionSimulatorXTestImpl::DoX11Button(int xbutton, bool isDown)
{
    return XTestFakeButtonEvent(m_display, xbutton, isDown, 0) != 0;
}

bool wxUIActionSimulatorXTestImpl::DoX11MouseMove(long x, long y)
{
    return XTestFakeMotionEvent(m_display, -1, x, y, 0) != 0;
}

bool
wxUIActionSimulatorXTestImpl::DoX11Key(KeyCode xkeycode,
                                       int WXUNUSED(modifiers),
                                       bool isDown)
{
    return XTestFakeKeyEvent(m_display, xkeycode, isDown, 0) != 0;
}

#endif // wxUSE_XTEST

wxUIActionSimulatorImpl* wxUIActionSimulatorX11Impl::New()
{
    wxX11Display display;

#if wxUSE_XTEST
    int dummy;
    if ( XTestQueryExtension(display, &dummy, &dummy, &dummy, &dummy) )
        return new wxUIActionSimulatorXTestImpl(display);
#endif // wxUSE_XTEST

    return new wxUIActionSimulatorPlainX11Impl(display);
}

} // anonymous namespace

bool wxUIActionSimulatorX11Impl::MouseDown(int button)
{
    return SendButtonEvent(button, true);
}

bool wxUIActionSimulatorX11Impl::MouseMove(long x, long y)
{
    if ( !m_display )
        return false;

    if ( !DoX11MouseMove(x, y) )
        return false;

    // At least with wxGTK we must always process the pending events before the
    // mouse position change really takes effect, so just do it from here
    // instead of forcing the client code using this function to always use
    // wxYield() which is unnecessary under the other platforms.
    if ( wxEventLoopBase* const loop = wxEventLoop::GetActive() )
    {
        loop->YieldFor(wxEVT_CATEGORY_USER_INPUT);
    }

    return true;
}

bool wxUIActionSimulatorX11Impl::MouseUp(int button)
{
    return SendButtonEvent(button, false);
}

bool wxUIActionSimulatorX11Impl::DoKey(int keycode, int modifiers, bool isDown)
{
    if ( !m_display )
        return false;

    WXKeySym xkeysym = wxCharCodeWXToX(keycode);
    KeyCode xkeycode = XKeysymToKeycode(m_display, xkeysym);
    if ( xkeycode == NoSymbol )
        return false;

    return DoX11Key(xkeycode, modifiers, isDown);
}

wxUIActionSimulator::wxUIActionSimulator()
                   : m_impl(wxUIActionSimulatorX11Impl::New())
{
}

wxUIActionSimulator::~wxUIActionSimulator()
{
    delete m_impl;
}

#endif // wxUSE_UIACTIONSIMULATOR

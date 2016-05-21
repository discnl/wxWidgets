/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/uiactioncmn.cpp
// Purpose:     wxUIActionSimulator common implementation
// Author:      Kevin Ollivier, Steven Lamerton, Vadim Zeitlin
// Created:     2010-03-06
// Copyright:   (c) 2010 Kevin Ollivier
//              (c) 2010 Steven Lamerton
//              (c) 2010-2016 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"

#include "wx/private/uiaction.h"

// ----------------------------------------------------------------------------
// Methods forwarded to wxUIActionSimulatorImpl
// ----------------------------------------------------------------------------

bool wxUIActionSimulator::MouseMove(long x, long y)
{
    return m_impl->MouseMove(x, y);
}

bool wxUIActionSimulator::MouseDown(int button)
{
    return m_impl->MouseDown(button);
}

bool wxUIActionSimulator::MouseUp(int button)
{
    return m_impl->MouseUp(button);
}

bool wxUIActionSimulator::MouseClick(int button)
{
    return m_impl->MouseClick(button);
}

bool wxUIActionSimulator::MouseDblClick(int button)
{
    return m_impl->MouseDblClick(button);
}

bool wxUIActionSimulator::MouseDragDrop(long x1, long y1, long x2, long y2,
                                        int button)
{
    return m_impl->MouseDragDrop(x1, y1, x2, y2, button);
}

// ----------------------------------------------------------------------------
// Methods implemented in wxUIActionSimulatorImpl itself
// ----------------------------------------------------------------------------

bool wxUIActionSimulatorImpl::MouseClick(int button)
{
    MouseDown(button);
    MouseUp(button);

    return true;
}

bool wxUIActionSimulatorImpl::MouseDblClick(int button)
{
    MouseDown(button);
    MouseUp(button);
    MouseDown(button);
    MouseUp(button);

    return true;
}

bool wxUIActionSimulatorImpl::MouseDragDrop(long x1, long y1, long x2, long y2,
                                   int button)
{
    MouseMove(x1, y1);
    MouseDown(button);
    MouseMove(x2, y2);
    MouseUp(button);
    
    return true;
}

bool
wxUIActionSimulator::Key(int keycode, int modifiers, bool isDown)
{
    wxASSERT_MSG( (modifiers & wxMOD_ALTGR) != wxMOD_ALTGR,
        "wxMOD_ALTGR is not implemented" );
    wxASSERT_MSG( !(modifiers & wxMOD_META ),
        "wxMOD_META is not implemented" );
    wxASSERT_MSG( !(modifiers & wxMOD_WIN ),
        "wxMOD_WIN is not implemented" );

    if ( isDown )
        SimulateModifiers(modifiers, true);

    bool rc = m_impl->DoKey(keycode, modifiers, isDown);

    if ( !isDown )
        SimulateModifiers(modifiers, false);

    return rc;
}

void wxUIActionSimulator::SimulateModifiers(int modifiers, bool isDown)
{
    if ( modifiers & wxMOD_SHIFT )
        m_impl->DoKey(WXK_SHIFT, modifiers, isDown);
    if ( modifiers & wxMOD_ALT )
        m_impl->DoKey(WXK_ALT, modifiers, isDown);
    if ( modifiers & wxMOD_CONTROL )
        m_impl->DoKey(WXK_CONTROL, modifiers, isDown);
}

bool wxUIActionSimulator::Char(int keycode, int modifiers)
{
    switch(keycode)
    {
    case '0':
        keycode = '0';
        break;
    case '1':
        keycode = '1';
        break;
    case '2':
        keycode = '2';
        break;
    case '3':
        keycode = '3';
        break;
    case '4':
        keycode = '4';
        break;
    case '5':
        keycode = '5';
        break;
    case '6':
        keycode = '6';
        break;
    case '7':
        keycode = '7';
        break;
    case '8':
        keycode = '8';
        break;
    case '9':
        keycode = '9';
        break;
    case '+':
        keycode = '+';
        break;
    case '-':
        keycode = '-';
        break;
    case '.':
        keycode = '.';
        break;
    default:
        break;
    };

    Key(keycode, modifiers, true);
    Key(keycode, modifiers, false);

    return true;
}

bool wxUIActionSimulator::Text(const char *s)
{
    while ( *s != '\0' )
    {
        const char ch = *s++;
        if ( !Char(ch, isupper(ch) ? wxMOD_SHIFT : 0) )
            return false;
    }

    return true;
}

#endif // wxUSE_UIACTIONSIMULATOR

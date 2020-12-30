/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/image.cpp
// Purpose:     wxImage benchmarks
// Author:      Vadim Zeitlin
// Created:     2013-06-30
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/image.h"

#include "bench.h"

BENCHMARK_FUNC(LoadBMP)
{
    wxImage image;
    return image.LoadFile("horse.bmp");
}

BENCHMARK_FUNC(LoadJPEG)
{
    static bool s_handlerAdded = false;
    if ( !s_handlerAdded )
    {
        s_handlerAdded = true;
        wxImage::AddHandler(new wxJPEGHandler);
    }

    wxImage image;
    return image.LoadFile("horse.jpg");
}

BENCHMARK_FUNC(LoadPNG)
{
    static bool s_handlerAdded = false;
    if ( !s_handlerAdded )
    {
        s_handlerAdded = true;
        wxImage::AddHandler(new wxPNGHandler);
    }

    wxImage image;
    return image.LoadFile("horse.png");
}

#if wxUSE_LIBTIFF
BENCHMARK_FUNC(LoadTIFF)
{
    static bool s_handlerAdded = false;
    if ( !s_handlerAdded )
    {
        s_handlerAdded = true;
        wxImage::AddHandler(new wxTIFFHandler);
    }

    wxImage image;
    return image.LoadFile("horse.tif");
}
#endif // wxUSE_LIBTIFF

static const wxImage& GetTestImage()
{
    static wxImage s_image;
    static bool s_triedToLoad = false;
    if ( !s_triedToLoad )
    {
        s_triedToLoad = true;
        s_image.LoadFile("horse.bmp");
    }

    return s_image;
}

BENCHMARK_FUNC(EnlargeNormal)
{
    return GetTestImage().Scale(300, 300, wxIMAGE_QUALITY_NORMAL).IsOk();
}

BENCHMARK_FUNC(EnlargeHighQuality)
{
    return GetTestImage().Scale(300, 300, wxIMAGE_QUALITY_HIGH).IsOk();
}

BENCHMARK_FUNC(ShrinkNormal)
{
    return GetTestImage().Scale(50, 50, wxIMAGE_QUALITY_NORMAL).IsOk();
}

BENCHMARK_FUNC(ShrinkHighQuality)
{
    return GetTestImage().Scale(50, 50, wxIMAGE_QUALITY_HIGH).IsOk();
}

#include "wx/frame.h"
#include "wx/grid.h"
#ifdef _DEBUG
#pragma comment (lib, "wxmsw31ud_adv.lib")
#else
#pragma comment (lib, "wxmsw31u_adv.lib")
#endif

namespace
{

wxGrid *gs_grid = NULL;
int gs_numRows = 100;
int gs_numCols = 100;

bool InitBareGrid()
{
    wxFrame *frame = new wxFrame(NULL, wxID_ANY, "");

    gs_grid = new wxGrid(frame, wxID_ANY);

    gs_grid->CreateGrid(gs_numRows, gs_numCols);

    return true;
}

bool InitCheckeredGrid()
{
    InitBareGrid();

    for (int row=0; row<gs_numRows; ++row)
    {
        for (int col=0; col<gs_numCols; ++col)
        {
            if ((row ^ col) & 1)
                gs_grid->SetCellBackgroundColour(row, col, *wxLIGHT_GREY);
        }
    }

    return true;
}

} // anonymous namespace

bool RunGridMoveBenchmark()
{
    static bool s_MoveToEnd = false;
    s_MoveToEnd = !s_MoveToEnd;

    gs_grid->GoToCell(s_MoveToEnd * (gs_numRows - 1), s_MoveToEnd * (gs_numCols - 1));

    return true;
}

BENCHMARK_FUNC_WITH_INIT(BareGridMove, InitBareGrid, NULL)
{
    return RunGridMoveBenchmark();
}

BENCHMARK_FUNC_WITH_INIT(CheckeredGridMove, InitCheckeredGrid, NULL)
{
    return RunGridMoveBenchmark();
}

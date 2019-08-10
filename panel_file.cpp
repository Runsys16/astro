#include "panel_file.h"


//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
PanelFile::PanelFile(FileBrowser* p)
{
    pFB = p;
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFile::wheelUp( int xm, int ym)
{
    logf( (char*)"PanelFile::wheelUp(%d, %d)", xm, ym );
    pFB->scrollFile( 1 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFile::wheelDown( int xm, int ym)
{
    logf( (char*)"PanelFile::wheelDown(%d, %d)", xm, ym );
    pFB->scrollFile( -1 );
}
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
void PanelFile::releaseLeft( int x, int y )
{
    logf( (char*)"PanelFile::releaseLeft(%d,%d)", x, y );
    pFB->isInsideFile(x, y);
}



//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------


/**********************************************************************
** Copyright (C) 2000-2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
*KOMMANDER* GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "mainwindow.h"

#include <stdlib.h>
#include <qaction.h>
#include <qwhatsthis.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qlineedit.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qsignalmapper.h>
#include <qstylefactory.h>
#include <qworkspace.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qlistbox.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qmap.h>

#include "defs.h"
#ifndef KOMMANDER
#include "project.h"
#endif
#include "widgetdatabase.h"
#include "widgetfactory.h"
#include "preferences.h"
#include "formwindow.h"
#include "newformimpl.h"
#include "resource.h"
#ifndef KOMMANDER
#include "projectsettingsimpl.h"
#endif
#include "workspace.h"
#include "createtemplate.h"
#include "hierarchyview.h"
#include "editslotsimpl.h"
#ifndef KOMMANDER
#include "finddialog.h"
#endif
#ifndef KOMMANDER
#include "replacedialog.h"
#endif
#ifndef KOMMANDER
#include "gotolinedialog.h"
#endif
#include "connectionviewerimpl.h"
#include "formsettingsimpl.h"
#ifndef KOMMANDER
#include "pixmapcollectioneditor.h"
#endif
#include "styledbutton.h"
#include "customwidgeteditorimpl.h"
#include "actioneditorimpl.h"
#include "formfile.h"
#ifndef KOMMANDER
#include "sourcefile.h"
#endif
#ifndef QT_NO_SQL
#include "dbconnectionsimpl.h"
#include "dbconnectionimpl.h"
#endif
#include "command.h"

#include "scriptobjecteditorimpl.h"

#ifdef HAVE_KDE
#include <kstatusbar.h>
#include <kmenubar.h>
#include <kfiledialog.h>
#endif

static const char * whatsthis_image[] = {
    "16 16 3 1",
    "  c None",
    "o  c #000000",
    "a  c #000080",
    "o        aaaaa  ",
    "oo      aaa aaa ",
    "ooo    aaa   aaa",
    "oooo   aa     aa",
    "ooooo  aa     aa",
    "oooooo  a    aaa",
    "ooooooo     aaa ",
    "oooooooo   aaa  ",
    "ooooooooo aaa   ",
    "ooooo     aaa   ",
    "oo ooo          ",
    "o  ooo    aaa   ",
    "    ooo   aaa   ",
    "    ooo         ",
    "     ooo        ",
    "     ooo        "};

const QString toolbarHelp = "<p>Toolbars contain a number of buttons to "
"provide quick access to often used functions.%1"
"<br>Click on the toolbar handle to hide the toolbar, "
"or drag and place the toolbar to a different location.</p>";

static QIconSet createIconSet( const QString &name )
{
    QIconSet ic( PixmapChooser::loadPixmap( name, PixmapChooser::Small ) );
    ic.setPixmap( PixmapChooser::loadPixmap( name, PixmapChooser::Disabled ), QIconSet::Small, QIconSet::Disabled );
    return ic;
}

int forms = 0;

void MainWindow::setupEditActions()
{
    actionEditUndo = new QAction( tr("Undo"), createIconSet( "undo.xpm" ),tr("&Undo: Not Available"), CTRL + Key_Z, this, 0 );
    actionEditUndo->setStatusTip( tr( "Undoes the last action" ) );
    actionEditUndo->setWhatsThis( whatsThisFrom( "Edit|Undo" ) );
    connect( actionEditUndo, SIGNAL( activated() ), this, SLOT( editUndo() ) );
    actionEditUndo->setEnabled( FALSE );

    actionEditRedo = new QAction( tr( "Redo" ), createIconSet("redo.xpm"), tr( "&Redo: Not Available" ), CTRL + Key_Y, this, 0 );
    actionEditRedo->setStatusTip( tr( "Redoes the last undone operation") );
    actionEditRedo->setWhatsThis( whatsThisFrom( "Edit|Redo" ) );
    connect( actionEditRedo, SIGNAL( activated() ), this, SLOT( editRedo() ) );
    actionEditRedo->setEnabled( FALSE );

    actionEditCut = new QAction( tr( "Cut" ), createIconSet("editcut.xpm"), tr( "Cu&t" ), CTRL + Key_X, this, 0 );
    actionEditCut->setStatusTip( tr( "Cuts the selected widgets and puts them on the clipboard" ) );
    actionEditCut->setWhatsThis(  whatsThisFrom( "Edit|Cut" ) );
    connect( actionEditCut, SIGNAL( activated() ), this, SLOT( editCut() ) );
    actionEditCut->setEnabled( FALSE );

    actionEditCopy = new QAction( tr( "Copy" ), createIconSet("editcopy.xpm"), tr( "&Copy" ), CTRL + Key_C, this, 0 );
    actionEditCopy->setStatusTip( tr( "Copies the selected widgets to the clipboard" ) );
    actionEditCopy->setWhatsThis(  whatsThisFrom( "Edit|Copy" ) );
    connect( actionEditCopy, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    actionEditCopy->setEnabled( FALSE );

    actionEditPaste = new QAction( tr( "Paste" ), createIconSet("editpaste.xpm"), tr( "&Paste" ), CTRL + Key_V, this, 0 );
    actionEditPaste->setStatusTip( tr( "Pastes the clipboard's contents" ) );
    actionEditPaste->setWhatsThis( whatsThisFrom( "Edit|Paste" ) );
    connect( actionEditPaste, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    actionEditPaste->setEnabled( FALSE );

    actionEditDelete = new QAction( tr( "Delete" ), QPixmap(), tr( "&Delete" ), Key_Delete, this, 0 );
    actionEditDelete->setStatusTip( tr( "Deletes the selected widgets" ) );
    actionEditDelete->setWhatsThis( whatsThisFrom( "Edit|Delete" ) );
    connect( actionEditDelete, SIGNAL( activated() ), this, SLOT( editDelete() ) );
    actionEditDelete->setEnabled( FALSE );

    actionEditSelectAll = new QAction( tr( "Select All" ), QPixmap(), tr( "Select &All" ), CTRL + Key_A, this, 0 );
    actionEditSelectAll->setStatusTip( tr( "Selects all widgets" ) );
    actionEditSelectAll->setWhatsThis( whatsThisFrom( "Edit|Select All" ) );
    connect( actionEditSelectAll, SIGNAL( activated() ), this, SLOT( editSelectAll() ) );
    actionEditSelectAll->setEnabled( TRUE );

    actionEditRaise = new QAction( tr( "Bring to Front" ), createIconSet("editraise.xpm"), tr( "Bring to &Front" ), 0, this, 0 );
    actionEditRaise->setStatusTip( tr( "Raises the selected widgets" ) );
    actionEditRaise->setWhatsThis( tr( "Raises the selected widgets" ) );
    connect( actionEditRaise, SIGNAL( activated() ), this, SLOT( editRaise() ) );
    actionEditRaise->setEnabled( FALSE );

    actionEditLower = new QAction( tr( "Send to Back" ), createIconSet("editlower.xpm"), tr( "Send to &Back" ), 0, this, 0 );
    actionEditLower->setStatusTip( tr( "Lowers the selected widgets" ) );
    actionEditLower->setWhatsThis( tr( "Lowers the selected widgets" ) );
    connect( actionEditLower, SIGNAL( activated() ), this, SLOT( editLower() ) );
    actionEditLower->setEnabled( FALSE );

    actionEditAccels = new QAction( tr( "Check Accelerators" ), QPixmap(),
            tr( "Chec&k Accelerators" ), ALT + Key_R, this, 0 );
    actionEditAccels->setStatusTip( tr("Checks if the accelerators used in the form are unique") );
    actionEditAccels->setWhatsThis( whatsThisFrom( "Edit|Check Accelerator" ) );
    connect( actionEditAccels, SIGNAL( activated() ), this, SLOT( editAccels() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), actionEditAccels, SLOT( setEnabled(bool) ) );

    //Script Objects Action
    actionEditScriptObjects = new QAction(tr("Script Objects"), QPixmap(), tr("&Script Objects"), ALT + Key_S, this);
    actionEditScriptObjects->setStatusTip(tr("Opens the script object editor."));
    actionEditScriptObjects->setWhatsThis("Script Objects Editor.");
    connect(actionEditScriptObjects, SIGNAL(activated()), this, SLOT(editScriptObjects()));
    connect(this, SIGNAL(hasActiveForm(bool)), actionEditScriptObjects, SLOT(setEnabled(bool)));


    actionEditSlots = new QAction( tr( "Slots" ), createIconSet("editslots.xpm"),
           tr( "S&lots..." ), 0, this, 0 );
    actionEditSlots->setStatusTip( tr("Opens a dialog for editing slots") );
    actionEditSlots->setWhatsThis( whatsThisFrom( "Edit|Slots" ) );
    connect( actionEditSlots, SIGNAL( activated() ), this, SLOT( editSlots() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), actionEditSlots, SLOT( setEnabled(bool) ) );

    actionEditConnections = new QAction( tr( "Connections" ), createIconSet("connecttool.xpm"),
           tr( "Co&nnections..." ), 0, this, 0 );
    actionEditConnections->setStatusTip( tr("Opens a dialog for editing connections") );
    actionEditConnections->setWhatsThis( whatsThisFrom( "Edit|Connections" ) );
    connect( actionEditConnections, SIGNAL( activated() ), this, SLOT( editConnections() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), actionEditConnections, SLOT( setEnabled(bool) ) );

#ifndef KOMMANDER
    actionEditSource = new QAction( tr( "Source" ), QIconSet(),
           tr( "&Source..." ), CTRL + Key_E, this, 0 );
    actionEditSource->setStatusTip( tr("Opens an editor to edit the form's source code") );
    actionEditSource->setWhatsThis( whatsThisFrom( "Edit|Source" ) );
    connect( actionEditSource, SIGNAL( activated() ), this, SLOT( editSource() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), actionEditSource, SLOT( setEnabled(bool) ) );
#endif

    actionEditFormSettings = new QAction( tr( "Form Settings" ), QPixmap(),
            tr( "&Form Settings..." ), 0, this, 0 );
    actionEditFormSettings->setStatusTip( tr("Opens a dialog to change the form's settings") );
    actionEditFormSettings->setWhatsThis( whatsThisFrom( "Edit|Form Settings" ) );
    connect( actionEditFormSettings, SIGNAL( activated() ), this, SLOT( editFormSettings() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), actionEditFormSettings, SLOT( setEnabled(bool) ) );

    actionEditPreferences = new QAction( tr( "Preferences" ), QPixmap(),
           tr( "Preferences..." ), 0, this, 0 );
    actionEditPreferences->setStatusTip( tr("Opens a dialog to change preferences") );
    actionEditPreferences->setWhatsThis( whatsThisFrom( "Edit|Preferences" ) );
    connect( actionEditPreferences, SIGNAL( activated() ), this, SLOT( editPreferences() ) );

#if defined(HAVE_KDE)
    KToolBar *tb = new KToolBar( this, "Edit" );
    tb->setFullSize( FALSE );
#else
    QToolBar *tb = new QToolBar( this, "Edit" );
    tb->setCloseMode( QDockWindow::Undocked );
#endif
    QWhatsThis::add( tb, tr( "<b>The Edit toolbar</b>%1").arg(tr(toolbarHelp).arg("")) );
    addToolBar( tb, tr( "Edit" ) );
    actionEditUndo->addTo( tb );
    actionEditRedo->addTo( tb );
    tb->addSeparator();
    actionEditCut->addTo( tb );
    actionEditCopy->addTo( tb );
    actionEditPaste->addTo( tb );
#if 0
    tb->addSeparator();
    actionEditLower->addTo( tb );
    actionEditRaise->addTo( tb );
#endif

    QPopupMenu *menu = new QPopupMenu( this, "Edit" );
    menubar->insertItem( tr( "&Edit" ), menu );
    actionEditUndo->addTo( menu );
    actionEditRedo->addTo( menu );
    menu->insertSeparator();
    actionEditCut->addTo( menu );
    actionEditCopy->addTo( menu );
    actionEditPaste->addTo( menu );
    actionEditDelete->addTo( menu );
    actionEditSelectAll->addTo( menu );
    actionEditAccels->addTo( menu );
#if 0
    menu->insertSeparator();
    actionEditLower->addTo( menu );
    actionEditRaise->addTo( menu );
#endif
    menu->insertSeparator();
    actionEditScriptObjects->addTo(menu);
    actionEditSlots->addTo( menu );
    actionEditConnections->addTo( menu );
    actionEditFormSettings->addTo( menu );
    menu->insertSeparator();
    actionEditPreferences->addTo( menu );
}

#ifndef KOMMANDER
void MainWindow::setupSearchActions()
{
    actionSearchFind = new QAction( tr( "Find" ), createIconSet( "searchfind.xpm" ),
            tr( "&Find..." ), CTRL + Key_F, this, 0 );
    connect( actionSearchFind, SIGNAL( activated() ), this, SLOT( searchFind() ) );
    actionSearchFind->setEnabled( FALSE );
    actionSearchFind->setWhatsThis( whatsThisFrom( "Search|Find" ) );

    actionSearchIncremetal = new QAction( tr( "Find Incremental" ), QIconSet(),
            tr( "Find &Incremental" ), ALT + Key_I, this, 0 );
    connect( actionSearchIncremetal, SIGNAL( activated() ), this, SLOT( searchIncremetalFindMenu() ) );
    actionSearchIncremetal->setEnabled( FALSE );
    actionSearchIncremetal->setWhatsThis( whatsThisFrom( "Search|Find Incremental" ) );

    actionSearchReplace = new QAction( tr( "Replace" ), QIconSet(),
            tr( "&Replace..." ), CTRL + Key_R, this, 0 );
    connect( actionSearchReplace, SIGNAL( activated() ), this, SLOT( searchReplace() ) );
    actionSearchReplace->setEnabled( FALSE );
    actionSearchReplace->setWhatsThis( whatsThisFrom( "Search|Replace" ) );

    actionSearchGotoLine = new QAction( tr( "Goto Line" ), QIconSet(),
            tr( "&Goto Line..." ), ALT + Key_G, this, 0 );
    connect( actionSearchGotoLine, SIGNAL( activated() ), this, SLOT( searchGotoLine() ) );
    actionSearchGotoLine->setEnabled( FALSE );
    actionSearchGotoLine->setWhatsThis( whatsThisFrom( "Search|Goto line" ) );

#if defined(HAVE_KDE)
    KToolBar *tb = new KToolBar( this, "Search" );
    tb->setFullSize( FALSE );
#else
    QToolBar *tb = new QToolBar( this, "Search" );
    tb->setCloseMode( QDockWindow::Undocked );
#endif
    addToolBar( tb, tr( "Search" ) );

    actionSearchFind->addTo( tb );
    incrementalSearch = new QLineEdit( tb );
    QToolTip::add( incrementalSearch, tr( "Incremetal Search (Alt+I)" ) );
    connect( incrementalSearch, SIGNAL( textChanged( const QString & ) ),
       this, SLOT( searchIncremetalFind() ) );
    connect( incrementalSearch, SIGNAL( returnPressed() ),
       this, SLOT( searchIncremetalFindNext() ) );
    incrementalSearch->setEnabled( FALSE );

    QPopupMenu *menu = new QPopupMenu( this, "Search" );
    menubar->insertItem( tr( "&Search" ), menu );
    actionSearchFind->addTo( menu );
    actionSearchIncremetal->addTo( menu );
    actionSearchReplace->addTo( menu );
    menu->insertSeparator();
    actionSearchGotoLine->addTo( menu );
}
#endif

void MainWindow::setupLayoutActions()
{
    if ( !actionGroupTools ) {
  actionGroupTools = new QActionGroup( this );
  actionGroupTools->setExclusive( TRUE );
  connect( actionGroupTools, SIGNAL( selected(QAction*) ), this, SLOT( toolSelected(QAction*) ) );
    }

    actionEditAdjustSize = new QAction( tr( "Adjust Size" ), createIconSet("adjustsize.xpm"),
          tr( "Adjust &Size" ), CTRL + Key_J, this, 0 );
    actionEditAdjustSize->setStatusTip(tr("Adjusts the size of the selected widget") );
    actionEditAdjustSize->setWhatsThis( whatsThisFrom( "Layout|Adjust Size" ) );
    connect( actionEditAdjustSize, SIGNAL( activated() ), this, SLOT( editAdjustSize() ) );
    actionEditAdjustSize->setEnabled( FALSE );

    actionEditHLayout = new QAction( tr( "Lay Out Horizontally" ), createIconSet("edithlayout.xpm"),
             tr( "Lay Out &Horizontally" ), CTRL + Key_H, this, 0 );
    actionEditHLayout->setStatusTip(tr("Lays out the selected widgets horizontally") );
    actionEditHLayout->setWhatsThis( whatsThisFrom( "Layout|Lay Out Horizontally" ) );
    connect( actionEditHLayout, SIGNAL( activated() ), this, SLOT( editLayoutHorizontal() ) );
    actionEditHLayout->setEnabled( FALSE );

    actionEditVLayout = new QAction( tr( "Lay Out Vertically" ), createIconSet("editvlayout.xpm"),
             tr( "Lay Out &Vertically" ), CTRL + Key_L, this, 0 );
    actionEditVLayout->setStatusTip(tr("Lays out the selected widgets vertically") );
    actionEditVLayout->setWhatsThis(  whatsThisFrom( "Layout|Lay Out Vertically" ) );
    connect( actionEditVLayout, SIGNAL( activated() ), this, SLOT( editLayoutVertical() ) );
    actionEditVLayout->setEnabled( FALSE );

    actionEditGridLayout = new QAction( tr( "Lay Out in a Grid" ), createIconSet("editgrid.xpm"),
          tr( "Lay Out in a &Grid" ), CTRL + Key_G, this, 0 );
    actionEditGridLayout->setStatusTip(tr("Lays out the selected widgets in a grid") );
    actionEditGridLayout->setWhatsThis( whatsThisFrom( "Layout|Lay Out in a Grid" ) );
    connect( actionEditGridLayout, SIGNAL( activated() ), this, SLOT( editLayoutGrid() ) );
    actionEditGridLayout->setEnabled( FALSE );

    actionEditSplitHorizontal = new QAction( tr( "Lay Out Horizontally (in Splitter)" ), createIconSet("editvlayoutsplit.xpm"),
               tr( "Lay Out Horizontally (in S&plitter)" ), 0, this, 0 );
    actionEditSplitHorizontal->setStatusTip(tr("Lays out the selected widgets horizontally in a splitter") );
    actionEditSplitHorizontal->setWhatsThis( whatsThisFrom( "Layout|Lay Out Horizontally (in Splitter)" ) );
    connect( actionEditSplitHorizontal, SIGNAL( activated() ), this, SLOT( editLayoutHorizontalSplit() ) );
    actionEditSplitHorizontal->setEnabled( FALSE );

    actionEditSplitVertical = new QAction( tr( "Lay Out Vertically (in Splitter)" ), createIconSet("edithlayoutsplit.xpm"),
               tr( "Lay Out Vertically (in Sp&litter)" ), 0, this, 0 );
    actionEditSplitVertical->setStatusTip(tr("Lays out the selected widgets vertically in a splitter") );
    actionEditSplitVertical->setWhatsThis( whatsThisFrom( "Layout|Lay Out Vertically (in Splitter)" ) );
    connect( actionEditSplitVertical, SIGNAL( activated() ), this, SLOT( editLayoutVerticalSplit() ) );
    actionEditSplitVertical->setEnabled( FALSE );

    actionEditBreakLayout = new QAction( tr( "Break Layout" ), createIconSet("editbreaklayout.xpm"),
           tr( "&Break Layout" ), CTRL + Key_B, this, 0 );
    actionEditBreakLayout->setStatusTip(tr("Breaks the selected layout") );
    actionEditBreakLayout->setWhatsThis( whatsThisFrom( "Layout|Break Layout" ) );
    connect( actionEditBreakLayout, SIGNAL( activated() ), this, SLOT( editBreakLayout() ) );

    int id = WidgetDatabase::idFromClassName( "Spacer" );
    QAction* a = new QAction( actionGroupTools, QString::number( id ).latin1() );
    a->setToggleAction( TRUE );
    a->setText( WidgetDatabase::className( id ) );
    a->setMenuText( tr( "Add ") + WidgetDatabase::className( id ) );
    a->setIconSet( WidgetDatabase::iconSet( id ) );
    a->setToolTip( WidgetDatabase::toolTip( id ) );
    a->setStatusTip( tr( "Insert a %1").arg(WidgetDatabase::toolTip( id )) );
    a->setWhatsThis( QString("<b>A %1</b><p>%2</p>"
           "<p>Click to insert a single %3,"
           "or double click to keep the tool selected.")
  .arg(WidgetDatabase::toolTip( id ))
  .arg(WidgetDatabase::whatsThis( id ))
  .arg(WidgetDatabase::toolTip( id ) ));

    QWhatsThis::add( layoutToolBar, tr( "<b>The Layout toolbar</b>%1" ).arg(tr(toolbarHelp).arg("")) );
    actionEditAdjustSize->addTo( layoutToolBar );
    layoutToolBar->addSeparator();
    actionEditHLayout->addTo( layoutToolBar );
    actionEditVLayout->addTo( layoutToolBar );
    actionEditGridLayout->addTo( layoutToolBar );
    actionEditSplitHorizontal->addTo( layoutToolBar );
    actionEditSplitVertical->addTo( layoutToolBar );
    actionEditBreakLayout->addTo( layoutToolBar );
    layoutToolBar->addSeparator();
    a->addTo( layoutToolBar );

    QPopupMenu *menu = new QPopupMenu( this, "Layout" );
    menubar->insertItem( tr( "&Layout" ), menu );
    actionEditAdjustSize->addTo( menu );
    menu->insertSeparator();
    actionEditHLayout->addTo( menu );
    actionEditVLayout->addTo( menu );
    actionEditGridLayout->addTo( menu );
    actionEditSplitHorizontal->addTo( menu );
    actionEditSplitVertical->addTo( menu );
    actionEditBreakLayout->addTo( menu );
    menu->insertSeparator();
    a->addTo( menu );
}

void MainWindow::setupToolActions()
{
    if ( !actionGroupTools ) {
  actionGroupTools = new QActionGroup( this );
  actionGroupTools->setExclusive( TRUE );
  connect( actionGroupTools, SIGNAL( selected(QAction*) ), this, SLOT( toolSelected(QAction*) ) );
    }

    actionPointerTool = new QAction( tr("Pointer"), createIconSet("pointer.xpm"), tr("&Pointer"),  Key_F2,
             actionGroupTools, QString::number(POINTER_TOOL).latin1(), TRUE );
    actionPointerTool->setStatusTip( tr("Selects the pointer tool") );
    actionPointerTool->setWhatsThis( whatsThisFrom( "Tools|Pointer" ) );

    actionConnectTool = new QAction( tr("Connect Signal/Slots"), createIconSet("connecttool.xpm"),
             tr("&Connect Signal/Slots"),  Key_F3,
             actionGroupTools, QString::number(CONNECT_TOOL).latin1(), TRUE );
    actionConnectTool->setStatusTip( tr("Selects the connection tool") );
    actionConnectTool->setWhatsThis( whatsThisFrom( "Tools|Connect Signals and Slots" ) );

    actionOrderTool = new QAction( tr("Tab Order"), createIconSet("ordertool.xpm"),
           tr("Tab &Order"),  Key_F4,
           actionGroupTools, QString::number(ORDER_TOOL).latin1(), TRUE );
    actionOrderTool->setStatusTip( tr("Selects the tab order tool") );
    actionOrderTool->setWhatsThis( whatsThisFrom( "Tools|Tab Order" ) );

#if defined(HAVE_KDE)
    KToolBar *tb = new KToolBar( this, "Tools" );
    tb->setFullSize( FALSE );
#else
    QToolBar *tb = new QToolBar( this, "Tools" );
    tb->setCloseMode( QDockWindow::Undocked );
#endif
    QWhatsThis::add( tb, tr( "<b>The Tools toolbar</b>%1" ).arg(tr(toolbarHelp).arg("")) );

    addToolBar( tb, tr( "Tools" ), QMainWindow::DockTop, TRUE );
    actionPointerTool->addTo( tb );
    actionConnectTool->addTo( tb );
    actionOrderTool->addTo( tb );

    QPopupMenu *mmenu = new QPopupMenu( this, "Tools" );
    menubar->insertItem( tr( "&Tools" ), mmenu );
    actionPointerTool->addTo( mmenu );
    actionConnectTool->addTo( mmenu );
    actionOrderTool->addTo( mmenu );
    mmenu->insertSeparator();

    customWidgetToolBar = 0;
    customWidgetMenu = 0;

    actionToolsCustomWidget = new QAction( tr("Custom Widgets"),
             createIconSet( "customwidget.xpm" ), tr("Edit &Custom Widgets..."), 0, this, 0 );
    actionToolsCustomWidget->setStatusTip( tr("Opens a dialog to add and change custom widgets") );
    actionToolsCustomWidget->setWhatsThis( whatsThisFrom( "Tools|Custom|Edit Custom Widgets" ) );

    connect( actionToolsCustomWidget, SIGNAL( activated() ), this, SLOT( toolsCustomWidget() ) );

    for ( int j = 0; j < WidgetDatabase::numWidgetGroups(); ++j ) {
  QString grp = WidgetDatabase::widgetGroup( j );
  if ( !WidgetDatabase::isGroupVisible( grp ) ||
       WidgetDatabase::isGroupEmpty( grp ) )
      continue;
#if defined(HAVE_KDE)
  KToolBar *tb = new KToolBar( this, grp.latin1() );
  tb->setFullSize( FALSE );
#else
  QToolBar *tb = new QToolBar( this, grp.latin1() );
  tb->setCloseMode( QDockWindow::Undocked );
#endif
  bool plural = grp[(int)grp.length()-1] == 's';
  if ( plural ) {
      QWhatsThis::add( tb, tr( "<b>The %1</b>%2" ).arg(grp).arg(tr(toolbarHelp).
            arg( tr(" Click on a button to insert a single widget, "
            "or double click to insert multiple %1.") ).arg(grp)) );
  } else {
      QWhatsThis::add( tb, tr( "<b>The %1 Widgets</b>%2" ).arg(grp).arg(tr(toolbarHelp).
            arg( tr(" Click on a button to insert a single %1 widget, "
            "or double click to insert multiple widgets.") ).arg(grp)) );
  }
  addToolBar( tb, grp );
  QPopupMenu *menu = new QPopupMenu( this, grp.latin1() );
  mmenu->insertItem( grp, menu );

  if ( grp == "Custom" ) {
      if ( !customWidgetMenu )
    actionToolsCustomWidget->addTo( menu );
      else
    menu->insertSeparator();
      customWidgetMenu = menu;
      customWidgetToolBar = tb;
  }

  for ( int i = 0; i < WidgetDatabase::count(); ++i ) {
      if ( WidgetDatabase::group( i ) != grp )
    continue; // only widgets, i.e. not forms and temp stuff
      QAction* a = new QAction( actionGroupTools, QString::number( i ).latin1() );
      a->setToggleAction( TRUE );
      QString atext = WidgetDatabase::className( i );
      if(atext == "ScriptObject") continue; // don't put a script object in the tool bar
      if ( atext[0] == 'Q' )
    atext = atext.mid(1);
      while ( atext.length() && atext[0] >= 'a' && atext[0] <= 'z' )
    atext = atext.mid(1);
      if ( atext.isEmpty() )
    atext = WidgetDatabase::className( i );
      a->setText( atext );
      QString ttip = WidgetDatabase::toolTip( i );
      a->setIconSet( WidgetDatabase::iconSet( i ) );
      a->setToolTip( ttip );
      if ( !WidgetDatabase::isWhatsThisLoaded() )
    WidgetDatabase::loadWhatsThis( documentationPath() );
      a->setStatusTip( tr( "Insert a %1").arg(WidgetDatabase::className( i )) );

      QString whats = QString("<b>A %1</b>").arg( WidgetDatabase::className( i ) );
      if ( !WidgetDatabase::whatsThis( i ).isEmpty() )
      whats += QString("<p>%1</p>").arg(WidgetDatabase::whatsThis( i ));
      a->setWhatsThis( whats + tr("<p>Double click on this tool to keep it selected.</p>") );

      if ( grp != "KDE" )
    a->addTo( tb );
      a->addTo( menu );
  }
    }

    if ( !customWidgetToolBar ) {
#if defined(HAVE_KDE)
  KToolBar *tb = new KToolBar( this, "Custom Widgets" );
  tb->setFullSize( FALSE );
#else
  QToolBar *tb = new QToolBar( this, "Custom Widgets" );
  tb->setCloseMode( QDockWindow::Undocked );
#endif
  QWhatsThis::add( tb, tr( "<b>The Custom Widgets toolbar</b>%1"
         "<p>Click <b>Edit Custom Widgets...</b> in the <b>Tools|Custom</b> menu to "
         "add and change custom widgets</p>" ).arg(tr(toolbarHelp).
         arg( tr(" Click on the buttons to insert a single widget, "
         "or double click to insert multiple widgets.") )) );
  addToolBar( tb, "Custom" );
  customWidgetToolBar = tb;
  QPopupMenu *menu = new QPopupMenu( this, "Custom Widgets" );
  mmenu->insertItem( "Custom", menu );
  customWidgetMenu = menu;
  customWidgetToolBar->hide();
  actionToolsCustomWidget->addTo( customWidgetMenu );
  customWidgetMenu->insertSeparator();
    }

    resetTool();
}

void MainWindow::setupFileActions()
{
#if defined(HAVE_KDE)
    KToolBar *tb = new KToolBar( this, "File" );
    tb->setFullSize( FALSE );
#else
    QToolBar* tb  = new QToolBar( this, "File" );
    tb->setCloseMode( QDockWindow::Undocked );
#endif
#ifndef KOMMANDER
    projectToolBar = tb;
#endif

    QWhatsThis::add( tb, tr( "<b>The File toolbar</b>%1" ).arg(tr(toolbarHelp).arg("")) );
    addToolBar( tb, tr( "File" ) );
    fileMenu = new QPopupMenu( this, "File" );
    menubar->insertItem( tr( "&File" ), fileMenu );

    QAction *a = 0;

    a = new QAction( this, 0 );
    a->setText( tr( "New" ) );
    a->setMenuText( tr( "&New..." ) );
    a->setIconSet( createIconSet("filenew.xpm") );
    a->setAccel( CTRL + Key_N );
    a->setStatusTip( tr( "Creates a new dialog" ) );
    a->setWhatsThis( whatsThisFrom( "File|New" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
#if 0
  connect( a, SIGNAL( activated() ), this, SLOT( newActivated() ) );
#endif
    a->addTo( tb );
    a->addTo( fileMenu );

    a = new QAction( this, 0 );
    a->setText( tr( "Open" ) );
    a->setMenuText( tr( "&Open..." ) );
    a->setIconSet( createIconSet("fileopen.xpm") );
    a->setAccel( CTRL + Key_O );
    a->setStatusTip( tr( "Opens an existing dialog ") );
    a->setWhatsThis( whatsThisFrom( "File|Open" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    a->addTo( tb );
    a->addTo( fileMenu );

    fileMenu->insertSeparator();

    a = new QAction( this, 0 );
    a->setText( tr( "Close" ) );
    a->setMenuText( tr( "&Close" ) );
    a->setStatusTip( tr( "Closes the current dialog" ) );
    a->setWhatsThis(whatsThisFrom( "File|Close" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileClose() ) );
#ifndef KOMMANDER
    connect( this, SIGNAL( hasActiveWindowOrProject(bool) ), a, SLOT( setEnabled(bool) ) );
#else
    connect( this, SIGNAL( hasActiveWindow(bool) ), a, SLOT( setEnabled(bool) ) );
#endif
    a->addTo( fileMenu );

    fileMenu->insertSeparator();

    a = new QAction( this, 0 );
    a->setText( tr( "Save" ) );
    a->setMenuText( tr( "&Save" ) );
    a->setIconSet( createIconSet("filesave.xpm") );
    a->setAccel( CTRL + Key_S );
    a->setStatusTip( tr( "Saves the current dialog" ) );
    a->setWhatsThis(whatsThisFrom( "File|Save" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileSave() ) );
#ifndef KOMMANDER
    connect( this, SIGNAL( hasActiveWindowOrProject(bool) ), a, SLOT( setEnabled(bool) ) );
#else
    connect( this, SIGNAL( hasActiveWindow(bool) ), a, SLOT( setEnabled(bool) ) );
#endif
    a->addTo( tb );
    a->addTo( fileMenu );

    a = new QAction( this, 0 );
    a->setText( tr( "Save As" ) );
    a->setMenuText( tr( "Save &As..." ) );
    a->setStatusTip( tr( "Saves the current dialog with a new filename" ) );
    a->setWhatsThis( whatsThisFrom( "File|Save As" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
    connect( this, SIGNAL( hasActiveWindow(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( fileMenu );

    a = new QAction( this, 0 );
    a->setText( tr( "Save All" ) );
    a->setMenuText( tr( "Sa&ve All" ) );
    a->setStatusTip( tr( "Saves all open dialogs" ) );
    a->setWhatsThis( whatsThisFrom( "File|Save All" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileSaveAll() ) );
#ifndef KOMMANDER
    connect( this, SIGNAL( hasActiveWindowOrProject(bool) ), a, SLOT( setEnabled(bool) ) );
#else
    connect( this, SIGNAL( hasActiveWindow(bool) ), a, SLOT( setEnabled(bool) ) );
#endif
    a->addTo( fileMenu );

    fileMenu->insertSeparator();
#ifndef KOMMANDER
    a = new QAction( this, 0 );
    a->setText( tr( "Create Template" ) );
    a->setMenuText( tr( "Create &Template..." ) );
    a->setStatusTip( tr( "Creates a new template" ) );
    a->setWhatsThis( whatsThisFrom( "File|Create Template" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileCreateTemplate() ) );
    a->addTo( fileMenu );

    fileMenu->insertSeparator();
#endif

    recentlyFilesMenu = new QPopupMenu( this );
#ifndef KOMMANDER
    recentlyProjectsMenu = new QPopupMenu( this );
#endif

    fileMenu->insertItem( tr( "Recently Opened Files " ), recentlyFilesMenu );
#ifndef KOMMANDER
    fileMenu->insertItem( tr( "Recently opened projects" ), recentlyProjectsMenu );
#endif

    connect( recentlyFilesMenu, SIGNAL( aboutToShow() ),
       this, SLOT( setupRecentlyFilesMenu() ) );
    connect( recentlyFilesMenu, SIGNAL( activated( int ) ),
       this, SLOT( recentlyFilesMenuActivated( int ) ) );
#ifndef KOMMANDER
    connect( recentlyProjectsMenu, SIGNAL( activated( int ) ),
       this, SLOT( recentlyProjectsMenuActivated( int ) ) );
    connect( recentlyProjectsMenu, SIGNAL( aboutToShow() ),
       this, SLOT( setupRecentlyProjectsMenu() ) );
#endif

    fileMenu->insertSeparator();

    a = new QAction( this, 0 );
    a->setText( tr( "Exit" ) );
    a->setMenuText( tr( "E&xit" ) );
    a->setStatusTip( tr( "Quits the application and prompts to save any changed dialogs" ) );
    a->setWhatsThis( whatsThisFrom( "File|Exit" ) );
    connect( a, SIGNAL( activated() ), qApp, SLOT( closeAllWindows() ) );
    a->addTo( fileMenu );
}

#if 0
void MainWindow::newActivated()
{
  fprintf(stderr, "NEW ACTIVIATED!\n");
}
#endif

#ifndef KOMMANDER
void MainWindow::setupProjectActions() // TODO : KOMMANDER doesn't need this
{
#ifndef KOMMANDER
    projectMenu = new QPopupMenu( this, "Project" );
    menubar->insertItem( tr( "Pr&oject" ), projectMenu );

    QActionGroup *ag = new QActionGroup( this, 0 );
    ag->setText( tr( "Active Project" ) );
    ag->setMenuText( tr( "Active Project" ) );
    ag->setExclusive( TRUE );
    ag->setUsesDropDown( TRUE );
    connect( ag, SIGNAL( selected( QAction * ) ), this, SLOT( projectSelected( QAction * ) ) );
    connect( ag, SIGNAL( selected( QAction * ) ), this, SIGNAL( projectChanged() ) );
    QAction *a = new QAction( tr( "<No Project>" ), tr( "<No Project>" ), 0, ag, 0, TRUE );

    eProject = new Project( "", tr( "<No Project>" ), projectSettingsPluginManager, TRUE );
    projects.insert( a, eProject );

    a->setOn( TRUE );
    ag->addTo( projectMenu );
    ag->addTo( projectToolBar );
    actionGroupProjects = ag;

    projectMenu->insertSeparator();

    a = new QAction( tr( "Add File" ), QPixmap(), tr( "&Add File..." ), 0, this, 0 );
    a->setStatusTip( tr("Adds a file to the current project") );
    a->setWhatsThis( whatsThisFrom( "Project|Add File" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( projectInsertFile() ) );
    a->setEnabled( FALSE );
    connect( this, SIGNAL( hasNonDummyProject(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( projectMenu );

    QAction* actionEditPixmapCollection = new QAction( tr( "Image Collection..." ), QPixmap(),
            tr( "&Image Collection..." ), 0, this, 0 );
    actionEditPixmapCollection->setStatusTip( tr("Opens a dialog for editing the current project's image collection") );
    actionEditPixmapCollection->setWhatsThis( whatsThisFrom( "Project|Image Collection" ) );
    connect( actionEditPixmapCollection, SIGNAL( activated() ), this, SLOT( editPixmapCollection() ) );
    actionEditPixmapCollection->setEnabled( FALSE );
    connect( this, SIGNAL( hasNonDummyProject(bool) ), actionEditPixmapCollection, SLOT( setEnabled(bool) ) );
    actionEditPixmapCollection->addTo( projectMenu );

#ifndef QT_NO_SQL
    QAction* actionEditDatabaseConnections = new QAction( tr( "Database Connections..." ), QPixmap(),
             tr( "&Database Connections..." ), 0, this, 0 );
    actionEditDatabaseConnections->setStatusTip( tr("Opens a dialog for editing the current project's database connections") );
    actionEditDatabaseConnections->setWhatsThis( whatsThisFrom( "Project|Database Connections" ) );
    connect( actionEditDatabaseConnections, SIGNAL( activated() ), this, SLOT( editDatabaseConnections() ) );
    //actionEditDatabaseConnections->setEnabled( FALSE );
    //connect( this, SIGNAL( hasNonDummyProject(bool) ), actionEditDatabaseConnections, SLOT( setEnabled(bool) ) );
    actionEditDatabaseConnections->addTo( projectMenu );
#endif

    QAction* actionEditProjectSettings = new QAction( tr( "Project Settings..." ), QPixmap(),
            tr( "&Project Settings..." ), 0, this, 0 );
    actionEditProjectSettings->setStatusTip( tr("Opens a dialog to change the project's settings") );
    actionEditProjectSettings->setWhatsThis( whatsThisFrom( "Project|Project Settings" ) );
    connect( actionEditProjectSettings, SIGNAL( activated() ), this, SLOT( editProjectSettings() ) );
    actionEditProjectSettings->setEnabled( FALSE );
    connect( this, SIGNAL( hasNonDummyProject(bool) ), actionEditProjectSettings, SLOT( setEnabled(bool) ) );
    actionEditProjectSettings->addTo( projectMenu );
#endif
    eProject = new Project( "", tr( "<No Project>" ), projectSettingsPluginManager, TRUE );
    projects.insert(0, eProject); // dummy project
    projectSelected(0);
}
#endif

void MainWindow::setupPreviewActions()
{
    QAction* a = 0;
    QPopupMenu *menu = new QPopupMenu( this, "Preview" );
    menubar->insertItem( tr( "&Preview" ), menu );

    a = new QAction( tr( "Preview Form" ), createIconSet("previewform.xpm"),
             tr( "Preview &Form" ), 0, this, 0 );
    a->setAccel( CTRL + Key_T );
    a->setStatusTip( tr("Opens a preview") );
    a->setWhatsThis( whatsThisFrom( "Preview|Preview Form" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( previewForm() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    menu->insertSeparator();

    QSignalMapper *mapper = new QSignalMapper( this );
    connect( mapper, SIGNAL(mapped(const QString&)), this, SLOT(previewForm(const QString&)) );
    QStringList styles = QStyleFactory::keys();
    for ( QStringList::Iterator it = styles.begin(); it != styles.end(); ++it ) {
  QString info;
  if ( *it == "Motif" )
      info = tr( "The preview will use the Motif look and feel which is used as the default style on most UNIX systems." );
  else if ( *it == "Windows" )
      info = tr( "The preview will use the Windows look and feel." );
  else if ( *it == "Platinum" )
      info = tr( "The preview will use the Platinum look and feel which is similar to the Macinosh GUI style." );
  else if ( *it == "CDE" )
      info = tr( "The preview will use the CDE look and feel which is similar to some versions of the Common Desktop Environment." );
  else if ( *it == "SGI" )
      info = tr( "The preview will use the Motif look and feel which is used as the default style on SGI systems." );
  else if ( *it == "MotifPlus" )
      info = tr( "The preview will use the advanced Motif look and feel used by the GIMP toolkit (GTK) on Linux." );

  a = new QAction( tr( "Preview Form in %1 Style" ).arg( *it ), createIconSet("previewform.xpm"),
           tr( "... in %1 Style" ).arg( *it ), 0, this, 0 );
  a->setStatusTip( tr("Opens a preview in %1 style").arg( *it ) );
  a->setWhatsThis( tr("<b>Open a preview in %1 style.</b>"
      "<p>Use the preview to test the design and "
      "signal-slot connections of the current form. %2</p>").arg( *it ).arg( info ) );
  mapper->setMapping( a, *it );
  connect( a, SIGNAL(activated()), mapper, SLOT(map()) );
  connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
  a->addTo( menu );
    }
}

void MainWindow::setupWindowActions()
{
    static bool windowActionsSetup = FALSE;
    if ( !windowActionsSetup ) {
  windowActionsSetup = TRUE;

  actionWindowTile = new QAction( tr( "Tile" ), tr( "&Tile" ), 0, this );
  actionWindowTile->setStatusTip( tr("Tiles the windows so that they are all visible") );
  actionWindowTile->setWhatsThis( whatsThisFrom( "Window|Tile" ) );
  connect( actionWindowTile, SIGNAL( activated() ), qworkspace, SLOT( tile() ) );
  actionWindowCascade = new QAction( tr( "Cascade" ), tr( "&Cascade" ), 0, this );
  actionWindowCascade->setStatusTip( tr("Cascades the windows so that all their title bars are visible") );
  actionWindowCascade->setWhatsThis( whatsThisFrom( "Window|Cascade" ) );
  connect( actionWindowCascade, SIGNAL( activated() ), qworkspace, SLOT( cascade() ) );

  actionWindowClose = new QAction( tr( "Close" ), tr( "Cl&ose" ), CTRL + Key_F4, this );
  actionWindowClose->setStatusTip( tr( "Closes the active window") );
  actionWindowClose->setWhatsThis( whatsThisFrom( "Window|Close" ) );
  connect( actionWindowClose, SIGNAL( activated() ), qworkspace, SLOT( closeActiveWindow() ) );

  actionWindowCloseAll = new QAction( tr( "Close All" ), tr( "Close Al&l" ), 0, this );
  actionWindowCloseAll->setStatusTip( tr( "Closes all form windows") );
  actionWindowCloseAll->setWhatsThis( whatsThisFrom( "Window|Close All" ) );
  connect( actionWindowCloseAll, SIGNAL( activated() ), qworkspace, SLOT( closeAllWindows() ) );

  actionWindowNext = new QAction( tr( "Next" ), tr( "Ne&xt" ), CTRL + Key_F6, this );
  actionWindowNext->setStatusTip( tr( "Activates the next window" ) );
  actionWindowNext->setWhatsThis( whatsThisFrom( "Window|Next" ) );
  connect( actionWindowNext, SIGNAL( activated() ), qworkspace, SLOT( activateNextWindow() ) );

  actionWindowPrevious = new QAction( tr( "Previous" ), tr( "Pre&vious" ), CTRL + SHIFT + Key_F6, this );
  actionWindowPrevious->setStatusTip( tr( "Activates the previous window" ) );
  actionWindowPrevious->setWhatsThis( whatsThisFrom( "Window|Previous" ) );
  connect( actionWindowPrevious, SIGNAL( activated() ), qworkspace, SLOT( activatePreviousWindow() ) );
    }

    if ( !windowMenu ) {
  windowMenu = new QPopupMenu( this, "Window" );
  menubar->insertItem( tr( "&Window" ), windowMenu );
  connect( windowMenu, SIGNAL( aboutToShow() ),
     this, SLOT( setupWindowActions() ) );
    } else {
  windowMenu->clear();
    }

    actionWindowClose->addTo( windowMenu );
    actionWindowCloseAll->addTo( windowMenu );
    windowMenu->insertSeparator();
    actionWindowNext->addTo( windowMenu );
    actionWindowPrevious->addTo( windowMenu );
    windowMenu->insertSeparator();
    actionWindowTile->addTo( windowMenu );
    actionWindowCascade->addTo( windowMenu );
    windowMenu->insertSeparator();
    windowMenu->insertItem( tr( "Vie&ws" ), createDockWindowMenu( NoToolBars ) );
    windowMenu->insertItem( tr( "Tool&bars" ), createDockWindowMenu( OnlyToolBars ) );
    QWidgetList windows = qworkspace->windowList();
    if ( windows.count() && formWindow() )
  windowMenu->insertSeparator();
    int j = 0;
    for ( int i = 0; i < int( windows.count() ); ++i ) {
  QWidget *w = windows.at( i );
  if ( !w->inherits( "FormWindow" ) && !w->inherits( "SourceEditor" ) )
      continue;
  j++;
  QString itemText;
  if ( j < 10 )
      itemText = QString("&%1 ").arg( j );
  if ( w->inherits( "FormWindow" ) )
      itemText += w->name();
  else
      itemText += w->caption();

  int id = windowMenu->insertItem( itemText, this, SLOT( windowsMenuActivated( int ) ) );
  windowMenu->setItemParameter( id, i );
  windowMenu->setItemChecked( id, qworkspace->activeWindow() == windows.at( i ) );
    }
}

void MainWindow::setupHelpActions()
{
    actionHelpContents = new QAction( tr( "Contents" ), tr( "&Contents" ), Key_F1, this, 0 );
    actionHelpContents->setStatusTip( tr("Opens the online help") );
    actionHelpContents->setWhatsThis( whatsThisFrom( "Help|Contents" ) );
    connect( actionHelpContents, SIGNAL( activated() ), this, SLOT( helpContents() ) );

    actionHelpManual = new QAction( tr( "Manual" ), tr( "&Manual" ), CTRL + Key_M, this, 0 );
    actionHelpManual->setStatusTip( tr("Opens the Qt Designer manual") );
    actionHelpManual->setWhatsThis( whatsThisFrom( "Help|Manual" ) );
    connect( actionHelpManual, SIGNAL( activated() ), this, SLOT( helpManual() ) );

    actionHelpAbout = new QAction( tr("About"), QPixmap(), tr("&About"), 0, this, 0 );
    actionHelpAbout->setStatusTip( tr("Displays information about Qt Designer") );
    actionHelpAbout->setWhatsThis( whatsThisFrom( "Help|About" ) );
    connect( actionHelpAbout, SIGNAL( activated() ), this, SLOT( helpAbout() ) );

    actionHelpAboutQt = new QAction( tr("About Qt"), QPixmap(), tr("About &Qt"), 0, this, 0 );
    actionHelpAboutQt->setStatusTip( tr("Displays information about the Qt Toolkit") );
    actionHelpAboutQt->setWhatsThis( whatsThisFrom( "Help|About Qt" ) );
    connect( actionHelpAboutQt, SIGNAL( activated() ), this, SLOT( helpAboutQt() ) );

#if defined(QT_NON_COMMERCIAL)
    actionHelpRegister = new QAction( tr("Register Qt"), QPixmap(), tr("&Register Qt..."), 0, this, 0 );
    actionHelpRegister->setStatusTip( tr("Opens a web browser at the evaluation form on www.trolltech.com") );
    actionHelpRegister->setWhatsThis( tr("Register with Trolltech") );
    connect( actionHelpRegister, SIGNAL( activated() ), this, SLOT( helpRegister() ) );
#endif

    actionHelpWhatsThis = new QAction( tr("What's This?"), QIconSet( whatsthis_image, whatsthis_image ),
               tr("What's This?"), SHIFT + Key_F1, this, 0 );
    actionHelpWhatsThis->setStatusTip( tr("\"What's This?\" context sensitive help") );
    actionHelpWhatsThis->setWhatsThis( whatsThisFrom( "Help|What's This?" ) );
    connect( actionHelpWhatsThis, SIGNAL( activated() ), this, SLOT( whatsThis() ) );

#if defined(HAVE_KDE)
    KToolBar *tb = new KToolBar( this, "Help" );
    tb->setFullSize( FALSE );
#else
    QToolBar *tb = new QToolBar( this, "Help" );
    tb->setCloseMode( QDockWindow::Undocked );
#endif
    QWhatsThis::add( tb, tr( "<b>The Help toolbar</b>%1" ).arg(tr(toolbarHelp).arg("") ));
    addToolBar( tb, tr( "Help" ) );
    actionHelpWhatsThis->addTo( tb );

    QPopupMenu *menu = new QPopupMenu( this, "Help" );
    menubar->insertSeparator();
    menubar->insertItem( tr( "&Help" ), menu );
    actionHelpContents->addTo( menu );
    actionHelpManual->addTo( menu );
    menu->insertSeparator();
    actionHelpAbout->addTo( menu );
    actionHelpAboutQt->addTo( menu );
#if defined(QT_NON_COMMERCIAL)
    actionHelpRegister->addTo( menu );
#endif

    menu->insertSeparator();
    actionHelpWhatsThis->addTo( menu );
}

void MainWindow::fileNew()
{
    statusBar()->message( tr( "Create a new dialog...") );
#if 1
    NewForm dlg( this, QString::null);
    dlg.exec();
#else
    QString n = "Form" + QString::number( ++forms );
    FormWindow *fw = 0;

#ifndef KOMMANDER
    FormFile *ff = new FormFile( FormFile::createUnnamedFileName(), TRUE, currentProject);
#else
    FormFile *ff = new FormFile( FormFile::createUnnamedFileName(), TRUE );
#endif
    fw = new FormWindow( ff, MainWindow::self, MainWindow::self->qWorkspace(), n );
#ifndef KOMMANDER
    fw->setProject(currentProject);
#endif
    MetaDataBase::addEntry( fw );
    int id = WidgetDatabase::idFromClassName("Dialog");
    QWidget *w = WidgetFactory::create( id, fw, n.latin1() );
    fw->setMainContainer( w );

    fw->setCaption( n );
    fw->resize( 600, 480 );
    MainWindow::self->insertFormWindow( fw );

    TemplateWizardInterface *iface =
  MainWindow::self->templateWizardInterface( fw->mainContainer()->className() );
    if ( iface ) {
  iface->setup( fw->mainContainer()->className(), fw->mainContainer(),
          fw->iFace(), MainWindow::self->designerInterface() );
  iface->release();
    }

    // the wizard might have changed a lot, lets update everything
    MainWindow::self->actioneditor()->setFormWindow( fw );
    MainWindow::self->objectHierarchy()->setFormWindow( fw, fw );
#ifndef KOMMANDER
  MainWindow::self->objectHierarchy()->formDefinitionView()->refresh();
#endif
    fw->killAccels( fw );
#ifndef KOMMANDER
    fw->project()->setModified( TRUE );
#endif
    fw->setFocus();
#ifndef KOMMANDER
    if ( !currentProject->isDummy() ) {
  fw->setSavePixmapInProject( TRUE );
  fw->setSavePixmapInline( FALSE );
    }
#endif
#endif
    statusBar()->clear();
}

void MainWindow::fileClose()
{
#ifndef KOMMANDER
    if ( !currentProject->isDummy() ) {
  fileCloseProject();
    } else {
  QWidget *w = qworkspace->activeWindow();
  if ( w->inherits( "FormWindow" ) )
  {
      ( (FormWindow*)w )->formFile()->close();
  }
  else if ( w->inherits( "SourceEditor" ) )
      ( (SourceEditor*)w )->close();
    }
#else
  QWidget *w = qworkspace->activeWindow();
  if ( w->inherits( "FormWindow" ) )
  {
      emit removedFormFile(((FormWindow *)w)->formFile());
      ( (FormWindow*)w )->formFile()->close();
  }
  #ifndef KOMMANDER
  else if ( w->inherits( "SourceEditor" ) )
      ( (SourceEditor*)w )->close();
      #endif
#endif
}


#ifndef KOMMANDER
void MainWindow::fileCloseProject()
{
    if ( currentProject->isDummy() )
  return;
    Project *pro = currentProject;
    QAction* a = 0;
    QAction* lastValid = 0;
    for ( QMap<QAction*, Project* >::Iterator it = projects.begin(); it != projects.end(); ++it ) {
  if ( it.data() == pro ) {
      a = it.key();
      if ( lastValid )
    break;
  }
  lastValid = it.key();
    }
    if ( a ) {
  if ( pro->isModified() ) {
      switch ( QMessageBox::warning( this, tr( "Save Project Settings" ),
             tr( "Save changes to '%1'?" ).arg( pro->fileName() ),
             tr( "&Yes" ), tr( "&No" ), tr( "&Cancel" ), 0, 2 ) ) {
      case 0: // save
    pro->save();
    break;
      case 1: // don't save
    break;
      case 2: // cancel
    return;
      default:
    break;
      }
  }

  QWidgetList windows = qWorkspace()->windowList();
  qWorkspace()->blockSignals( TRUE );
  QWidgetListIt wit( windows );
  while ( wit.current() ) {
      QWidget *w = wit.current();
      ++wit;
      if ( w->inherits( "FormWindow" ) ) {
    if ( ( (FormWindow*)w )->project() == pro ) {
        if ( ( (FormWindow*)w )->formFile()->editor() )
      windows.removeRef( ( (FormWindow*)w )->formFile()->editor() );
        if ( !( (FormWindow*)w )->formFile()->close() )
      return;
    }
      } else if ( w->inherits( "SourceEditor" ) ) {
    if ( !( (SourceEditor*)w )->close() )
        return;
      }
  }
  hierarchyView->clear();
  windows = qWorkspace()->windowList();
  qWorkspace()->blockSignals( FALSE );
  actionGroupProjects->removeChild( a );
  projects.remove( a );
  delete a;
  currentProject = 0;
  if ( lastValid ) {
      projectSelected( lastValid );
      statusBar()->message( "Selected project '" + tr( currentProject->projectName() + "'") );
  }
  if ( !windows.isEmpty() ) {
      for ( QWidget *w = windows.first(); w; w = windows.next() ) {
    if ( !w->inherits( "FormWindow" ) )
        continue;
    w->setFocus();
    activeWindowChanged( w );
    break;
      }
  } else {
      emit hasActiveWindow( FALSE );
      emit hasActiveForm( FALSE );
      updateUndoRedo( FALSE, FALSE, QString::null, QString::null );
  }
    }
}
#endif

void MainWindow::fileOpen() // as called by the menu
{
#ifndef KOMMANDER
  fileOpen( "", "", "", FALSE );
#else
  fileOpen("", "");
#endif
}

#ifndef KOMMANDER
void MainWindow::projectInsertFile()
{
    fileOpen( "", "" );
}
#endif

#ifndef KOMMANDER
void MainWindow::fileOpen( const QString &filter, const QString &extension, const QString &fn, bool inProject  )
#else
void MainWindow::fileOpen( const QString &filter, const QString &fn )
#endif

{
    statusBar()->message( tr( "Open a file...") );

#ifndef KOMMANDER
    QPluginManager<ImportFilterInterface> manager( IID_ImportFilter, QApplication::libraryPaths(), "/designer" );
#endif

#ifndef KOMMANDER
    Project* project = inProject ? currentProject : eProject;
    Project *project = project = currentProject;
#endif

    QStringList additionalSources;

    {
  QStringList filterlist;
  if ( filter.isEmpty() ) {
#ifndef KOMMANDER
      if ( !inProject )
    filterlist << tr( "Designer Files (*.ui *.pro)" );
      filterlist << tr( "DQt User-Interface Files (*.ui)" );
#else
      filterlist << tr("*.kmdr|Kommander Files");
#endif
#ifndef KOMMANDER
      if ( !inProject )
    filterlist << tr( "QMAKE Project Files (*.pro)" );
      QStringList list = manager.featureList();
      for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    filterlist << *it;
      LanguageInterface *iface = MetaDataBase::languageInterface( project->language() );
      if ( iface && iface->supports( LanguageInterface::AdditionalFiles ) ) {
    filterlist +=  iface->fileFilterList();
    additionalSources += iface->fileExtensionList();
      }
      filterlist << tr( "All Files (*)" );
#endif
  } else {
      filterlist << filter;
  }

  QString filters = filterlist.join( " " );

  QStringList filenames;
  if ( fn.isEmpty() ) {
#ifndef KOMMANDER
      if ( !inProject ) {
    QString f = QFileDialog::getOpenFileName( QString::null, filters, this, 0,
                (inProject ? tr("Add") : tr("Open" )),
                &lastOpenFilter );
    filenames << f;
      } else {
    filenames = QFileDialog::getOpenFileNames( filters, QString::null, this, 0,
                (inProject ? tr("Add") : tr("Open" )),
                &lastOpenFilter );
#else
    filenames = KFileDialog::getOpenFileNames(QString::null, filters, this, tr("Open Files"));
#endif
#ifndef KOMMANDER
      }
#endif
  } else {
      filenames << fn;
  }
  for ( QStringList::Iterator fit = filenames.begin(); fit != filenames.end(); ++fit ) {
      QString filename = *fit;
      if ( !filename.isEmpty() ) {
    QFileInfo fi( filename );

#ifndef KOMMANDER
    if ( fi.extension( FALSE ) == "pro" && ( extension.isEmpty() || extension.find( ";pro" ) != -1 ) ) {
        addRecentlyOpened( filename, recentlyProjects );
        openProject( filename );
    } else if ( fi.extension( FALSE ) == "ui" && ( extension.isEmpty() || extension.find( ";ui" ) != -1 ) ) {
        if ( !inProject )
      setCurrentProject( eProject );
        openFormWindow( filename );
        addRecentlyOpened( filename, recentlyFiles );
    } else if ( !extension.isEmpty() && extension.find( ";" + fi.extension( FALSE ) ) != -1 ||
          additionalSources.find( fi.extension( FALSE ) ) != additionalSources.end() ) {
        LanguageInterface *iface = MetaDataBase::languageInterface( project->language() );
        if ( iface && iface->supports( LanguageInterface::AdditionalFiles ) ) {
      SourceFile *sf = project->findSourceFile( project->makeRelative( filename ) );
      if ( !sf )
          sf = new SourceFile( project->makeRelative( filename ), FALSE, project );
      editSource( sf );
        }
    } else if ( extension.isEmpty() ) {
        QString filter;
        for ( QStringList::Iterator it2 = filterlist.begin(); it2 != filterlist.end(); ++it2 ) {
      if ( (*it2).contains( fi.extension( FALSE ), FALSE ) ) {
          filter = *it2;
          break;
      }
        }

        ImportFilterInterface* iface = 0;
        manager.queryInterface( filter, &iface );
        if ( !iface ) {
      statusBar()->message( tr( "No import filter is available to import '%1'").
                arg( filename ), 3000 );
      return;
        }
        statusBar()->message( tr( "Importing '%1' using import filter ...").arg( filename ) );
        QStringList list = iface->import( filter, filename );
        iface->release();
        if ( list.isEmpty() ) {
      statusBar()->message( tr( "Nothing to load in '%1'").arg( filename ), 3000 );
      return;
        }
        if ( !inProject )
      setCurrentProject( eProject );
        addRecentlyOpened( filename, recentlyFiles );
        for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
      openFormWindow( *it, FALSE );
      QFile::remove( *it );
        }
        statusBar()->clear();
    }
#else
        openFormWindow( filename );

        addRecentlyOpened( filename, recentlyFiles );
#endif

      }
  }
    }
}

FormWindow *MainWindow::openFormWindow( const QString &filename, bool validFileName, FormFile *ff )
{
    if ( filename.isEmpty() )
  return 0;

    bool makeNew = FALSE;

    if ( !QFile::exists( filename ) ) {
  makeNew = TRUE;
    } else {
  QFile f( filename );
  f.open( IO_ReadOnly );
  QTextStream ts( &f );
  makeNew = ts.read().length() < 2;
    }
    if ( !makeNew ) {
  statusBar()->message( tr( "Reading file '%1'...").arg( filename ) );
  if ( QFile::exists( filename ) ) {
#ifndef KOMMANDER
      FormFile *ff2 = currentProject->findFormFile( currentProject->makeRelative( filename ) );
      if ( ff2 && ff2->formWindow() ) {
    ff2->formWindow()->setFocus();
    return ff2->formWindow();
      }
      if ( ff2 )
    ff = ff2;
#endif
      QApplication::setOverrideCursor( WaitCursor );
      Resource resource( this );
      if ( !ff )
#ifndef KOMMANDER
    ff = new FormFile( currentProject->makeRelative( filename ), FALSE, currentProject );
#else
    ff = new FormFile( filename , FALSE );

#endif
      bool b = resource.load( ff ) && (FormWindow*)resource.widget();
      if ( !validFileName && resource.widget() )
    ( (FormWindow*)resource.widget() )->setFileName( QString::null );
      QApplication::restoreOverrideCursor();
      if ( b ) {
    rebuildCustomWidgetGUI();
    statusBar()->message( tr( "Loaded file '%1'").arg( filename ), 3000 );
      } else {
    statusBar()->message( tr( "Failed to load file '%1'").arg( filename ), 5000 );
    QMessageBox::information( this, tr("Load File"), tr("Couldn't load file '%1'").arg( filename ) );
    delete ff;
      }
      return (FormWindow*)resource.widget();
  } else {
      statusBar()->clear();
  }
    } else {
  fileNew();
  if ( formWindow() )
      formWindow()->setFileName( filename );
  return formWindow();
    }
    return 0;
}


bool MainWindow::fileSave()
{

#ifndef KOMMANDER
    if ( !currentProject->isDummy() )
  return fileSaveProject();
#endif
    return fileSaveForm();
}

bool MainWindow::fileSaveForm()
{
    FormWindow *fw = 0;
#ifndef KOMMANDER
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
  if ( e->object() == formWindow() || e == qWorkspace()->activeWindow() ) {
      e->save();
  }
    }
#endif


    #ifndef KOMMANDER
    QWidget *w = qWorkspace()->activeWindow();
    if ( w ) {
  if ( w->inherits( "SourceEditor" ) ) {
      SourceEditor *se = (SourceEditor*)w;
      if ( se->formWindow() )
    fw = se->formWindow();
      else if ( se->sourceFile() ) {
    se->sourceFile()->save();
    return TRUE;
      }
  }
    }
    #endif

    if ( !fw )
  fw = formWindow();
    if ( !fw || !fw->formFile()->save() )
  return FALSE;
    QApplication::restoreOverrideCursor();
    return TRUE;
}

#ifndef KOMMANDER
bool MainWindow::fileSaveProject()
{
    currentProject->save();
    statusBar()->message( tr( "Project '%1' saved.").arg( currentProject->projectName() ), 3000 );
    return TRUE;
}
#endif

bool MainWindow::fileSaveAs()
{
    statusBar()->message( tr( "Enter a filename..." ) );

    QWidget *w = qworkspace->activeWindow();
    if ( !w )
  return TRUE;
    if ( w->inherits( "FormWindow" ) )
  return ( (FormWindow*)w )->formFile()->saveAs();
  #ifndef KOMMANDER
    else if ( w->inherits( "SourceEditor" ) )
  return ( (SourceEditor*)w )->saveAs();
  #endif
    return FALSE;
}

void MainWindow::fileSaveAll()
{
#ifndef KOMMANDER
    for ( QMap<QAction*, Project* >::Iterator it = projects.begin(); it != projects.end(); ++it )
  (*it)->save();
    projects.getFirst()->save();
#else
    fprintf(stderr, "MainWindow::fileSaveAll");
#endif // FIXME
}

static bool inSaveAllTemp = FALSE;

void MainWindow::saveAllTemp()
{
    if ( inSaveAllTemp )
  return;
    inSaveAllTemp = TRUE;
    statusBar()->message( tr( "Qt Designer is crashing. Attempting to save files..." ) );
    QWidgetList windows = qWorkspace()->windowList();
    QString baseName = QDir::homeDirPath() + "/.designer/saved-form-";
    int i = 1;
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
  if ( !w->inherits( "FormWindow" ) )
      continue;

#ifndef KOMMANDER
  QString fn = baseName + QString::number( i++ ) + ".ui";
#else
  QString fn = baseName + QString::number( i++ ) + ".kmdr";
#endif
  ( (FormWindow*)w )->setFileName( fn );
  ( (FormWindow*)w )->formFile()->save();
    }
    inSaveAllTemp = FALSE;
}

void MainWindow::fileCreateTemplate()
{
    CreateTemplate dia( this, 0, TRUE );

    int i = 0;
    for ( i = 0; i < WidgetDatabase::count(); ++i ) {
  if ( WidgetDatabase::isForm( i ) && WidgetDatabase::widgetGroup( i ) != "Temp") {
      dia.listClass->insertItem( WidgetDatabase::className( i ) );
  }
    }
    for ( i = 0; i < WidgetDatabase::count(); ++i ) {
  if ( WidgetDatabase::isContainer( i ) && !WidgetDatabase::isForm(i) &&
       WidgetDatabase::className( i ) != "QTabWidget" && WidgetDatabase::widgetGroup( i ) != "Temp" ) {
      dia.listClass->insertItem( WidgetDatabase::className( i ) );
  }
    }

    QPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
  if ( w->isContainer )
      dia.listClass->insertItem( w->className );
    }

    dia.editName->setText( tr( "NewTemplate" ) );
    connect( dia.buttonCreate, SIGNAL( clicked() ),
       this, SLOT( createNewTemplate() ) );
    dia.exec();
}

void MainWindow::createNewTemplate()
{
    CreateTemplate *dia = (CreateTemplate*)sender()->parent();
    QString fn = dia->editName->text();
    QString cn = dia->listClass->currentText();
    if ( fn.isEmpty() || cn.isEmpty() ) {
  QMessageBox::information( this, tr( "Create Template" ), tr( "Couldn't create the template" ) );
  return;
    }

    QStringList templRoots;
    const char *qtdir = getenv( "QTDIR" );
    if(qtdir)
  templRoots << qtdir;
#ifdef QT_INSTALL_PREFIX
    templRoots << QT_INSTALL_PREFIX;
#endif
#ifdef QT_INSTALL_DATA
    templRoots << QT_INSTALL_DATA;
#endif
    if(qtdir) //try the tools/designer directory last!
  templRoots << QString(qtdir) + "/tools/designer";
    QFile f;
    for ( QStringList::Iterator it = templRoots.begin(); it != templRoots.end(); ++it ) {
  if ( QFile::exists( (*it) + "/templates/" )) {
#ifndef KOMMANDER
    QString tmpfn = (*it) + "/templates/" + fn + ".ui";
#else
    QString tmpfn = (*it) + "/templates/" + fn + ".kmdr";
#endif
      f.setName(tmpfn);
      if(f.open(IO_WriteOnly))
    break;
  }
    }
    if ( !f.isOpen() ) {
  QMessageBox::information( this, tr( "Create Template" ), tr( "Couldn't create the template" ) );
  return;
    }
    QTextStream ts( &f );

    ts << "<!DOCTYPE UI><UI>" << endl;
    ts << "<widget>" << endl;
    ts << "<class>" << cn << "</class>" << endl;
    ts << "<property stdset=\"1\">" << endl;
    ts << "    <name>name</name>" << endl;
    ts << "    <cstring>" << cn << "Form</cstring>" << endl;
    ts << "</property>" << endl;
    ts << "<property stdset=\"1\">" << endl;
    ts << "    <name>geometry</name>" << endl;
    ts << "    <rect>" << endl;
    ts << "        <width>300</width>" << endl;
    ts << "        <height>400</height>" << endl;
    ts << "    </rect>" << endl;
    ts << "</property>" << endl;
    ts << "</widget>" << endl;
    ts << "</UI>" << endl;

    dia->editName->setText( tr( "NewTemplate" ) );

    f.close();
}

void MainWindow::editUndo()
{
#ifndef KOMMANDER
    if ( qWorkspace()->activeWindow() &&
   qWorkspace()->activeWindow()->inherits( "SourceEditor" ) ) {
  ( (SourceEditor*)qWorkspace()->activeWindow() )->editUndo();
  return;
    }
#endif
    if ( formWindow() )
  formWindow()->undo();
}

void MainWindow::editRedo()
{
#ifndef KOMMANDER
    if ( qWorkspace()->activeWindow() &&
   qWorkspace()->activeWindow()->inherits( "SourceEditor" ) ) {
  ( (SourceEditor*)qWorkspace()->activeWindow() )->editRedo();
  return;
    }
#endif
    if ( formWindow() )
  formWindow()->redo();
}

void MainWindow::editCut()
{
#ifndef KOMMANDER
    if ( qWorkspace()->activeWindow() &&
   qWorkspace()->activeWindow()->inherits( "SourceEditor" ) ) {
  ( (SourceEditor*)qWorkspace()->activeWindow() )->editCut();
  return;
    }
#endif
    editCopy();
    editDelete();
}

void MainWindow::editCopy()
{
#ifndef KOMMANDER
    if ( qWorkspace()->activeWindow() &&
   qWorkspace()->activeWindow()->inherits( "SourceEditor" ) ) {
  ( (SourceEditor*)qWorkspace()->activeWindow() )->editCopy();
  return;
    }
#endif
    if ( formWindow() )
  qApp->clipboard()->setText( formWindow()->copy() );
}

void MainWindow::editPaste()
{
#ifndef KOMMANDER
    if ( qWorkspace()->activeWindow() &&
   qWorkspace()->activeWindow()->inherits( "SourceEditor" ) ) {
  ( (SourceEditor*)qWorkspace()->activeWindow() )->editPaste();
  return;
    }
#endif
    if ( !formWindow() )
  return;

    QWidget *w = formWindow()->mainContainer();
    QWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 ) {
  w = l.first();
  if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
       ( !WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) &&
         w != formWindow()->mainContainer() ) )
      w = formWindow()->mainContainer();
    }

    if ( w && WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout ) {
  formWindow()->paste( qApp->clipboard()->text(), WidgetFactory::containerOfWidget( w ) );
  hierarchyView->widgetInserted( 0 );
  formWindow()->commandHistory()->setModified( TRUE );
    } else {
  QMessageBox::information( this, tr( "Paste error" ),
          tr( "Can't paste widgets. Designer couldn't find a container\n"
              "to paste into which does not contain a layout. Break the layout\n"
              "of the container you want to paste into and select this container\n"
              "and then paste again." ) );
    }
}

void MainWindow::editDelete()
{
    if ( formWindow() )
  formWindow()->deleteWidgets();
}

void MainWindow::editSelectAll()
{
#ifndef KOMMANDER
    if ( qWorkspace()->activeWindow() &&
   qWorkspace()->activeWindow()->inherits( "SourceEditor" ) ) {
  ( (SourceEditor*)qWorkspace()->activeWindow() )->editSelectAll();
  return;
    }
    #endif
    if ( formWindow() )
  formWindow()->selectAll();
}


void MainWindow::editLower()
{
    if ( formWindow() )
  formWindow()->lowerWidgets();
}

void MainWindow::editRaise()
{
    if ( formWindow() )
  formWindow()->raiseWidgets();
}

void MainWindow::editAdjustSize()
{
    if ( formWindow() )
  formWindow()->editAdjustSize();
}

void MainWindow::editLayoutHorizontal()
{
    if ( layoutChilds )
  editLayoutContainerHorizontal();
    else if ( layoutSelected && formWindow() )
  formWindow()->layoutHorizontal();
}

void MainWindow::editLayoutVertical()
{
    if ( layoutChilds )
  editLayoutContainerVertical();
    else if ( layoutSelected && formWindow() )
  formWindow()->layoutVertical();
}

void MainWindow::editLayoutHorizontalSplit()
{
    if ( layoutChilds )
  ; // no way to do that
    else if ( layoutSelected && formWindow() )
  formWindow()->layoutHorizontalSplit();
}

void MainWindow::editLayoutVerticalSplit()
{
    if ( layoutChilds )
  ; // no way to do that
    else if ( layoutSelected && formWindow() )
  formWindow()->layoutVerticalSplit();
}

void MainWindow::editLayoutGrid()
{
    if ( layoutChilds )
  editLayoutContainerGrid();
    else if ( layoutSelected && formWindow() )
  formWindow()->layoutGrid();
}

void MainWindow::editLayoutContainerVertical()
{
    if ( !formWindow() )
  return;
    QWidget *w = formWindow()->mainContainer();
    QWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 )
  w = l.first();
    if ( w )
  formWindow()->layoutVerticalContainer( w  );
}

void MainWindow::editLayoutContainerHorizontal()
{
    if ( !formWindow() )
  return;
    QWidget *w = formWindow()->mainContainer();
    QWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 )
  w = l.first();
    if ( w )
  formWindow()->layoutHorizontalContainer( w );
}

void MainWindow::editLayoutContainerGrid()
{
    if ( !formWindow() )
  return;
    QWidget *w = formWindow()->mainContainer();
    QWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 )
  w = l.first();
    if ( w )
  formWindow()->layoutGridContainer( w  );
}

void MainWindow::editBreakLayout()
{
    if ( !formWindow() || !breakLayout )
    {
  return;
    }
    QWidget *w = formWindow()->mainContainer();
    if ( formWindow()->currentWidget() )
  w = formWindow()->currentWidget();
    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
   w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) {
  formWindow()->breakLayout( w );
  return;
    } else {
  QWidgetList widgets = formWindow()->selectedWidgets();
  for ( w = widgets.first(); w; w = widgets.next() ) {
      if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
     w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout )
    break;
  }
  if ( w ) {
      formWindow()->breakLayout( w );
      return;
  }
    }

    w = formWindow()->mainContainer();
    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
   w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout )
    {
  formWindow()->breakLayout( w );
    }
}

void MainWindow::editAccels()
{
    if ( !formWindow() )
  return;
    formWindow()->checkAccels();
}

void MainWindow::editSlots()
{
    if ( !formWindow() )
  return;

    statusBar()->message( tr( "Edit the current form's slots..." ) );
    EditSlots dlg( this, formWindow() );
    dlg.exec();
    statusBar()->clear();
}

void MainWindow::editConnections()
{
    if ( !formWindow() )
  return;

    statusBar()->message( tr( "Edit the current form's connections..." ) );
    ConnectionViewer dlg( this, formWindow() );
    dlg.exec();
    statusBar()->clear();
}

void MainWindow::editScriptObjects()
{
	if(!formWindow())
		return;

	QMap<QString, QString> oldObjects;
	oldObjects = formWindow()->scriptObjects();

	ScriptObjectEditor *ed = new ScriptObjectEditor(oldObjects, this);
	if(ed->exec() == QDialog::Accepted)
	{
		Command *cmd = new ScriptObjectCommand("Modify Script Objects", formWindow(), oldObjects, ed->scriptObjects());
		cmd->execute();
		formWindow()->commandHistory()->addCommand(cmd);
	}


	delete ed;
}

#ifndef KOMMANDER
SourceEditor *MainWindow::editSource()
{
    if ( !formWindow() )
  return 0;
    return formWindow()->formFile()->showEditor();
}
#endif

#ifndef KOMMANDER
SourceEditor *MainWindow::openSourceEdior()
{
    if ( !formWindow() )
  return 0;
    SourceEditor *editor = 0;

    QString lang = currentProject->language();
    if ( !MetaDataBase::hasEditor( lang ) ) {
  QMessageBox::information( this, tr( "Edit Source" ),
          tr( "There is no plugin for editing " + lang + " code installed" ) );
  return 0;
    }
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
  if ( e->language() == lang && e->object() == formWindow() ) {
      editor = e;
      break;
  }
    }
    if ( !editor ) {
  EditorInterface *eIface = 0;
  editorPluginManager->queryInterface( lang, &eIface );
  if ( !eIface )
      return 0;
  LanguageInterface *lIface = MetaDataBase::languageInterface( lang );
  if ( !lIface )
      return 0;
  QApplication::setOverrideCursor( WaitCursor );
  editor = new SourceEditor( qWorkspace(), eIface, lIface );
  eIface->release();
  lIface->release();

  editor->setLanguage( lang );
  sourceEditors.append( editor );
  QApplication::restoreOverrideCursor();
    }
    if ( editor->object() != formWindow() )
  editor->setObject( formWindow(), formWindow()->project() );
    editor->show();
    editor->setFocus();
    emit editorChanged();
    return editor;
}
#endif

#ifndef KOMMANDER
SourceEditor *MainWindow::editSource( SourceFile *f )
{
    SourceEditor *editor = 0;
    QString lang = currentProject->language();
    if ( !MetaDataBase::hasEditor( lang ) ) {
  QMessageBox::information( this, tr( "Edit Source" ),
          tr( "There is no plugin for edit " + lang + " code installed" ) );
  return 0;
    }
    if ( f )
  editor = f->editor();

    if ( !editor ) {
  EditorInterface *eIface = 0;
  editorPluginManager->queryInterface( lang, &eIface );
  if ( !eIface )
      return 0;
  LanguageInterface *lIface = MetaDataBase::languageInterface( lang );
  if ( !lIface )
      return 0;
  QApplication::setOverrideCursor( WaitCursor );
  editor = new SourceEditor( qWorkspace(), eIface, lIface );
  eIface->release();
  lIface->release();

  editor->setLanguage( lang );
  sourceEditors.append( editor );
  QApplication::restoreOverrideCursor();
    }
    if ( editor->object() != f )
  editor->setObject( f, currentProject );
    editor->show();
    editor->setFocus();
    emit editorChanged();
    Q_UNUSED(f);
    return editor;
}
#endif

void MainWindow::editFormSettings()
{
    if ( !formWindow() )
  return;

    statusBar()->message( tr( "Edit the current form's settings..." ) );
    FormSettings dlg( this, formWindow() );
    dlg.exec();
    statusBar()->clear();
}

#ifndef KOMMANDER
void MainWindow::editProjectSettings()
{
    openProjectSettings( currentProject );
    wspace->setCurrentProject( currentProject );
}
#endif

#ifndef KOMMANDER
void MainWindow::editPixmapCollection()
{
    PixmapCollectionEditor dia( this, 0, TRUE );
    dia.setProject( currentProject );
    dia.exec();
}
#endif

#ifndef KOMMANDER
void MainWindow::editDatabaseConnections()
{
#ifndef QT_NO_SQL
    DatabaseConnectionsEditor dia( currentProject, this, 0, TRUE );
    dia.exec();
#endif
}
#endif

void MainWindow::editPreferences()
{
    statusBar()->message( tr( "Edit preferences..." ) );
    Preferences *dia = new Preferences( this, 0, TRUE );
    prefDia = dia;
    connect( dia->helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    dia->buttonColor->setEditor( StyledButton::ColorEditor );
    dia->buttonPixmap->setEditor( StyledButton::PixmapEditor );
    dia->checkBoxShowGrid->setChecked( sGrid );
    dia->checkBoxGrid->setChecked( snGrid );
    dia->spinGridX->setValue( grid().x() );
    dia->spinGridY->setValue( grid().y() );
    dia->checkBoxWorkspace->setChecked( restoreConfig );
    dia->checkBoxBigIcons->setChecked( usesBigPixmaps() );
    dia->checkBoxBigIcons->hide(); // ##### disabled for now
    dia->checkBoxTextLabels->setChecked( usesTextLabel() );
    dia->buttonColor->setColor( qworkspace->backgroundColor() );
    if ( qworkspace->backgroundPixmap() )
  dia->buttonPixmap->setPixmap( *qworkspace->backgroundPixmap() );
    if ( backPix )
  dia->radioPixmap->setChecked( TRUE );
    else
  dia->radioColor->setChecked( TRUE );
    dia->checkBoxSplash->setChecked( splashScreen );
    dia->editDocPath->setText( docPath );
    dia->checkAutoEdit->setChecked( !databaseAutoEdit );
    connect( dia->buttonDocPath, SIGNAL( clicked() ),
       this, SLOT( chooseDocPath() ) );
#ifndef KOMMANDER
/* DITCH EVERYTHING TO DO WITH THE PREFERENCE INTERFACE - WE DON'T NEED IT FOR KOMMANDER */
    SenderObject *senderObject = new SenderObject( designerInterface() );
    QValueList<Tab>::Iterator it;
    for ( it = preferenceTabs.begin(); it != preferenceTabs.end(); ++it ) {
  Tab t = *it;
  dia->tabWidget->addTab( t.w, t.title );
  if ( t.receiver ) {
      connect( dia->buttonOk, SIGNAL( clicked() ), senderObject, SLOT( emitAcceptSignal() ) );
      connect( senderObject, SIGNAL( acceptSignal( QUnknownInterface * ) ), t.receiver, t.accept_slot );
      connect( senderObject, SIGNAL( initSignal( QUnknownInterface * ) ), t.receiver, t.init_slot );
      senderObject->emitInitSignal();
      disconnect( senderObject, SIGNAL( initSignal( QUnknownInterface * ) ), t.receiver, t.init_slot );
  }
    }
#endif

    if ( dia->exec() == QDialog::Accepted ) {
  setSnapGrid( dia->checkBoxGrid->isChecked() );
  setShowGrid( dia->checkBoxShowGrid->isChecked() );
  setGrid( QPoint( dia->spinGridX->value(),
       dia->spinGridY->value() ) );
  restoreConfig = dia->checkBoxWorkspace->isChecked();
  setUsesBigPixmaps( FALSE /*dia->checkBoxBigIcons->isChecked()*/ ); // ### disable for now
  setUsesTextLabel( dia->checkBoxTextLabels->isChecked() );
  if ( dia->radioPixmap->isChecked() && dia->buttonPixmap->pixmap() ) {
      qworkspace->setBackgroundPixmap( *dia->buttonPixmap->pixmap() );
      backPix = TRUE;
  } else {
      qworkspace->setBackgroundColor( dia->buttonColor->color() );
      backPix = FALSE;
  }
  splashScreen = dia->checkBoxSplash->isChecked();
  docPath = dia->editDocPath->text();
  databaseAutoEdit = !dia->checkAutoEdit->isChecked();
    }
#ifndef KOMMANDER
/* BYE BYE INTERFACES */
    delete senderObject;
    for ( it = preferenceTabs.begin(); it != preferenceTabs.end(); ++it ) {
  Tab t = *it;
  dia->tabWidget->removePage( t.w );
  t.w->reparent( 0, QPoint(0,0), FALSE );
    }

    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
  e->configChanged();
#endif
    delete dia;
    prefDia = 0;
    statusBar()->clear();
}

void MainWindow::chooseDocPath()
{
    if ( !prefDia )
  return;
    QString fn = QFileDialog::getExistingDirectory( QString::null, this );
    if ( !fn.isEmpty() )
  prefDia->editDocPath->setText( fn );
}

#ifndef KOMMANDER
void MainWindow::searchFind()
{
    if ( !qWorkspace()->activeWindow() ||
   !qWorkspace()->activeWindow()->inherits( "SourceEditor" ) )
   return;

    if ( !findDialog )
  findDialog = new FindDialog( this, 0, FALSE );
    findDialog->show();
    findDialog->raise();
    findDialog->setEditor( ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface(),
         ( (SourceEditor*)qWorkspace()->activeWindow() )->object() );
    findDialog->comboFind->setFocus();
    findDialog->comboFind->lineEdit()->selectAll();
}
#endif

#ifndef KOMMANDER
void MainWindow::searchIncremetalFindMenu()
{
    incrementalSearch->selectAll();
    incrementalSearch->setFocus();
}
#endif

#ifndef KOMMANDER
void MainWindow::searchIncremetalFind()
{
    if ( !qWorkspace()->activeWindow() ||
   !qWorkspace()->activeWindow()->inherits( "SourceEditor" ) )
   return;

    ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface()->find( incrementalSearch->text(),
                       FALSE, FALSE, TRUE, FALSE );
}
#endif

#ifndef KOMMANDER
void MainWindow::searchIncremetalFindNext()
{
    if ( !qWorkspace()->activeWindow() ||
   !qWorkspace()->activeWindow()->inherits( "SourceEditor" ) )
   return;

    ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface()->find( incrementalSearch->text(),
                       FALSE, FALSE, TRUE, TRUE );
}
#endif

#ifndef KOMMANDER
void MainWindow::searchReplace()
{
    if ( !qWorkspace()->activeWindow() ||
   !qWorkspace()->activeWindow()->inherits( "SourceEditor" ) )
   return;

    if ( !replaceDialog )
  replaceDialog = new ReplaceDialog( this, 0, FALSE );
    replaceDialog->show();
    replaceDialog->raise();
    replaceDialog->setEditor( ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface(),
         ( (SourceEditor*)qWorkspace()->activeWindow() )->object() );
    replaceDialog->comboFind->setFocus();
    replaceDialog->comboFind->lineEdit()->selectAll();
}
#endif

#ifndef KOMMANDER
void MainWindow::searchGotoLine()
{
    if ( !qWorkspace()->activeWindow() ||
   !qWorkspace()->activeWindow()->inherits( "SourceEditor" ) )
   return;

    if ( !gotoLineDialog )
  gotoLineDialog = new GotoLineDialog( this, 0, FALSE );
    gotoLineDialog->show();
    gotoLineDialog->raise();
    gotoLineDialog->setEditor( ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface() );
    gotoLineDialog->spinLine->setFocus();
    gotoLineDialog->spinLine->setMinValue( 1 );
    gotoLineDialog->spinLine->setMaxValue( ( (SourceEditor*)qWorkspace()->activeWindow() )->numLines() );
    gotoLineDialog->spinLine->selectAll();
}
#endif

void MainWindow::toolsCustomWidget()
{
    statusBar()->message( tr( "Edit custom widgets..." ) );
    CustomWidgetEditor edit( this, this );
    edit.exec();
    rebuildCustomWidgetGUI();
    statusBar()->clear();
}

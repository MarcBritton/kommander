/***************************************************************************
                          tabwidget.cpp - Widget with tabs 
                             -------------------
    copyright            : (C) 2002 by Marc Britton
    email                : consume@optusnet.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* KDE INCLUDES */

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qtabwidget.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *a_parent, const char *a_name, int a_flags)
	: QTabWidget(a_parent, a_name, a_flags), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

TabWidget::~TabWidget()
{
}

QString TabWidget::currentState() const
{
  return QString("default");
}

bool TabWidget::isKommanderWidget() const
{
  return TRUE;
}

QStringList TabWidget::associatedText() const
{
  return KommanderWidget::associatedText();
}

void TabWidget::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void TabWidget::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText( a_text );
}

QString TabWidget::populationText() const
{
  return KommanderWidget::populationText();
}

void TabWidget::populate()
{
}

void TabWidget::showEvent(QShowEvent* e)
{
  QTabWidget::showEvent(e);
  emit widgetOpened();
}

bool TabWidget::isFunctionSupported(int f)
{
  return f == DCOP::currentItem || f == DCOP::setCurrentItem;
}

QString TabWidget::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::currentItem:
      return QString::number(currentPageIndex());
    case DCOP::setCurrentItem:
      setCurrentPage(args[0].toUInt());
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }
  return QString::null;
}

#include "tabwidget.moc"

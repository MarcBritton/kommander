/***************************************************************************
                         progressbar.cpp - ProgressBar widget 
                             -------------------
    copyright            : (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <specials.h>
#include "progressbar.h"

ProgressBar::ProgressBar(QWidget *a_parent, const char *a_name)
  : KProgress(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
}

ProgressBar::~ProgressBar()
{
}

QString ProgressBar::currentState() const
{
  return QString("default");
}

bool ProgressBar::isKommanderWidget() const
{
  return TRUE;
}

QStringList ProgressBar::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ProgressBar::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ProgressBar::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ProgressBar::populationText() const
{
  return KommanderWidget::populationText();
}

void ProgressBar::populate()
{
  setProgress(KommanderWidget::evalAssociatedText(populationText()).toUInt());
}

void ProgressBar::showEvent(QShowEvent *e)
{
  QProgressBar::showEvent(e);
  emit widgetOpened();
}

QString ProgressBar::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::setText:
      setProgress(args[0].toUInt());
      break;
    case DCOP::text:
      return QString::number(value());
    case DCOP::clear:
      setProgress(0);
      break;
    default:
      break;
  }
  return QString::null;
}

#include "progressbar.moc"

/***************************************************************************
                          checkbox.cpp - Checkbox widget 
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
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qcheckbox.h>
#include <qbutton.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "checkbox.h"

CheckBox::CheckBox(QWidget *a_parent, const char *a_name)
	: QCheckBox(a_parent, a_name), KommanderWidget(this)
{
	QStringList states;
	states << "unchecked";
	states << "semichecked";
	states << "checked";
	setStates(states);
	QStringList displayStates;
	displayStates << "checked";
	displayStates << "semichecked";
	displayStates << "unchecked";
	setDisplayStates(displayStates);
}

CheckBox::~CheckBox()
{
}

QString CheckBox::currentState() const
{
	if(state() == QButton::Off)
		return "unchecked";
	else if(state() == QButton::NoChange)
		return "semichecked";
	else if(state() == QButton::On)
		return "checked";
	return QString::null;
}

bool CheckBox::isKommanderWidget() const
{
	return TRUE;
}

QStringList CheckBox::associatedText() const
{
	return KommanderWidget::associatedText();
}

void CheckBox::setAssociatedText(QStringList a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void CheckBox::setPopulationText( QString a_text )
{
    KommanderWidget::setPopulationText( a_text );
}

QString CheckBox::populationText() const
{
    return KommanderWidget::populationText();
}

void CheckBox::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    setWidgetText( txt );
}

void CheckBox::setWidgetText(const QString &a_text)
{
    setText(a_text);
    emit widgetTextChanged(a_text);
}

QString CheckBox::widgetText() const
{
    return text();
}

void CheckBox::setSelectedWidgetText( const QString &)
{
}

QString CheckBox::selectedWidgetText() const
{
    return QString::null;
}

void CheckBox::showEvent( QShowEvent *e )
{
    QCheckBox::showEvent( e );
    emit widgetOpened();
}
#include "checkbox.moc"

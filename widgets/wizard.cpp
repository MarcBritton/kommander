/***************************************************************************
                          wizard.cpp - Widget providing a wizard 
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
#include <k3process.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qwizard.h>
#include <qdialog.h>

/* OTHER INCLUDES */
#include <kommanderfactory.h>
#include <kommanderwidget.h>
#include "wizard.h"

Wizard::Wizard(QWidget *a_parent, const char *a_name, bool a_modal, int a_flags)
  : QWizard(a_parent, a_name, a_modal, a_flags), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  connect(this, SIGNAL(helpClicked()), SLOT(runHelp()));
}

Wizard::~Wizard()
{
}

QString Wizard::currentState() const
{
  return QString("default");
}

bool Wizard::isKommanderWidget() const
{
  return true;
}

QStringList Wizard::associatedText() const
{
  return KommanderWidget::associatedText();
}

void Wizard::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void Wizard::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString Wizard::populationText() const
{
  return KommanderWidget::populationText();
}

void Wizard::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
//FIXME: implement me
}

void Wizard::setWidgetText(const QString &a_text)
{
  setCaption(a_text);
  emit widgetTextChanged(a_text);
}

void Wizard::exec()
{
  QWizard::exec();
  emit finished();
}

void Wizard::runHelp()
{
  if (helpAction() == Command)
  {
    K3Process proc;
    proc << helpActionText();
    proc.start(K3Process::DontCare, K3Process::NoCommunication);
  }
  else if (helpAction() == Dialog)
  {
    KommanderFactory::loadPlugins();
    QDialog *dialog = (QDialog *)KommanderFactory::create(helpActionText());
    dialog->exec();
    delete dialog;
  }
}

Wizard::HelpAction Wizard::helpAction() const
{
  return m_helpAction;
}

void Wizard::setHelpAction(HelpAction a_helpAction)
{
  m_helpAction = a_helpAction;
}

QString Wizard::helpActionText() const
{
  return m_helpActionText;
}

void Wizard::setHelpActionText(const QString& a_helpActionText)
{
  m_helpActionText = a_helpActionText;
}


void Wizard::showEvent(QShowEvent *e)
{
  QWizard::showEvent( e );
  emit widgetOpened();
}

QString Wizard::handleDCOP(int function, const QStringList& args)
{
  return KommanderWidget::handleDCOP(function, args);
}


#include "wizard.moc"

/***************************************************************************
                          execbutton.cpp - Button that runs its text association 
                             -------------------
    copyright            : (C) 2002-2003 Marc Britton <consume@optusnet.com.au>
                           (C) 2004      Michal Rudolf <mrudolf@kdewebdev.org>
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
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include <specials.h>
#include "execbutton.h"
#include <cstdio>

ExecButton::ExecButton(QWidget* a_parent, const char* a_name)
  : KPushButton(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);
  setWriteStdout(true);
  bufferStdin = 0;
  connect(this, SIGNAL(clicked()), this, SLOT(startProcess()));
}

ExecButton::~ExecButton()
{
}

QString ExecButton::currentState() const
{
  return QString("default");
}

bool ExecButton::isKommanderWidget() const
{
  return TRUE;
}

QStringList ExecButton::associatedText() const
{
  return KommanderWidget::associatedText();
}

void ExecButton::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void ExecButton::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString ExecButton::populationText() const
{
  return KommanderWidget::populationText();
}

void ExecButton::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void ExecButton::setWidgetText(const QString& a_text)
{
  setText(a_text);
  emit widgetTextChanged(a_text);
}

void ExecButton::startProcess()
{
//FIXME Move this functionality to MyProcess (adding a flag to mark non-blocking mode)  

  QString at = evalAssociatedText().stripWhiteSpace();
  QString shellName = "/bin/sh";

  if (at.isEmpty())
    return;

  // Look for shell
  if (at.startsWith("#!"))
  {
    int eol = at.find("\n");
    if (eol == -1)
      eol = at.length();
    shellName = at.mid(2, eol - 1).stripWhiteSpace();
    at = at.mid(eol + 1);
  }

  KProcess *process = new KProcess();
  *process << shellName;
  connect(process, SIGNAL(processExited(KProcess*)), SLOT(endProcess(KProcess*)));
  connect(process, SIGNAL(receivedStdout(KProcess*, char*, int)), 
    SLOT(appendOutput(KProcess*, char*, int)));
  connect(process, SIGNAL(receivedStderr(KProcess*, char*, int)), 
    SLOT(appendOutput(KProcess*, char*, int)));

  if (!process->start(KProcess::NotifyOnExit, KProcess::All))
  {
    KMessageBox::error(this,
        i18n("<qt>Failed to start shell process<br><b>%1</b></qt>").arg(shellName));
    delete process;
    return;
  } else
  {
    int len = at.local8Bit().length();
    bufferStdin = new char[len + 1];
    strcpy(bufferStdin, at.local8Bit());
    process->writeStdin(bufferStdin, len);
    process->closeStdin();
  }
  setEnabled(false);            // disabled until process ends
}

void ExecButton::appendOutput(KProcess *, char *a_buffer, int a_len)
{
  char *buffer = new char[a_len + 1];
  buffer[a_len] = 0;
  for (int i = 0; i < a_len; ++i)
    buffer[i] = a_buffer[i];

  QString bufferString(buffer);
  m_output += bufferString;
  if (writeStdout())
  {
    fputs(buffer, stdout);
    fflush(stdout);
  }
  delete buffer;
}

void ExecButton::endProcess(KProcess * a_process)
{
  emit widgetTextChanged(m_output);
  m_output = "";

  setEnabled(true);

  delete[]bufferStdin;
  bufferStdin = 0;

  delete a_process;
}

bool ExecButton::writeStdout() const
{
  return m_writeStdout;
}

void ExecButton::setWriteStdout(bool a_enable)
{
  m_writeStdout = a_enable;
}

void ExecButton::showEvent(QShowEvent* e)
{
  KPushButton::showEvent(e);
  emit widgetOpened();
}

QString ExecButton::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DCOP::text:
      return m_output;
    case DCOP::setText:
      setWidgetText(args[0]);
      break;
    default:
      break;
  }
  return QString::null;
}


#include "execbutton.moc"

/***************************************************************************
                         fileselector.cpp - File selection widget 
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
#include <kfiledialog.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kurlcompletion.h>

/* QT INCLUDES */
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qlayout.h>
#include <qsizepolicy.h>

/* OTHER INCLUDES */
#include <kommanderwidget.h>
#include "fileselector.h"

FileSelector::FileSelector(QWidget * a_parent, const char *a_name)
   : QWidget(a_parent, a_name), KommanderWidget(this)
{
  QStringList states;
  states << "default";
  setStates(states);
  setDisplayStates(states);

  m_lineEdit = new KLineEdit(this);
  
  m_selectButton = new KPushButton("...", this);

  m_boxLayout = new QHBoxLayout(this, 0, 11);
  m_boxLayout->addWidget(m_lineEdit);
  m_boxLayout->addWidget(m_selectButton);

  setSizePolicy(m_lineEdit->sizePolicy());
  m_lineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  m_selectButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

  connect(m_selectButton, SIGNAL(clicked()), this, SLOT(makeSelection()));

  setSelectionType(Open);
  setSelectionOpenMultiple(FALSE);
}

FileSelector::~FileSelector()
{
}

QString FileSelector::currentState() const
{
  return QString("default");
}

bool FileSelector::isKommanderWidget() const
{
  return TRUE;
}

QStringList FileSelector::associatedText() const
{
  return KommanderWidget::associatedText();
}

void FileSelector::setAssociatedText(const QStringList& a_at)
{
  KommanderWidget::setAssociatedText(a_at);
}

void FileSelector::setPopulationText(const QString& a_text)
{
  KommanderWidget::setPopulationText(a_text);
}

QString FileSelector::populationText() const
{
  return KommanderWidget::populationText();
}

void FileSelector::populate()
{
  QString txt = KommanderWidget::evalAssociatedText(populationText());
  setWidgetText(txt);
}

void FileSelector::setWidgetText(const QString& a_text)
{
  m_lineEdit->setText(a_text);
  emit widgetTextChanged(a_text);
}

QString FileSelector::widgetText() const
{
  return m_lineEdit->text();
}

void FileSelector::setSelectedWidgetText(const QString& a_text)
{
  QString curText = m_lineEdit->text();
  int f = curText.find(a_text);
  if (f != -1)
    m_lineEdit->setSelection(f, a_text.length());
}

QString FileSelector::selectedWidgetText() const
{
  return m_lineEdit->selectedText();
}

FileSelector::SelectionType FileSelector::selectionType() const
{
  return m_selectionType;
}

void FileSelector::setSelectionType(SelectionType a_selectionType)
{
  m_selectionType = a_selectionType;
  delete m_lineEdit->completionObject();
  if (m_selectionType == Directory) 
     m_lineEdit->setCompletionObject(new KURLCompletion(KURLCompletion::DirCompletion));
  else
     m_lineEdit->setCompletionObject(new KURLCompletion(KURLCompletion::FileCompletion));
  
}

QString FileSelector::selectionFilter() const
{
  return m_filter;
}

void FileSelector::setSelectionFilter(const QString& a_filter)
{
  m_filter = a_filter;
}

QString FileSelector::selectionCaption() const
{
  return m_caption;
}

void FileSelector::setSelectionCaption(const QString& a_caption)
{
  m_caption = a_caption;
}

bool FileSelector::selectionOpenMultiple() const
{
  return m_openMultiple;
}

void FileSelector::setSelectionOpenMultiple(bool a_openMultiple)
{
  m_openMultiple = a_openMultiple;
}

void FileSelector::makeSelection()
{
  // Depending on the SelectionType property we need to show either a save, open or directory dialog.
  QString text;
  if (m_selectionType == Open)
    if (m_openMultiple)
      text = KFileDialog::getOpenFileNames(m_lineEdit->text(), m_filter, this, m_caption).join(" ");
    else
      text = KFileDialog::getOpenFileName(m_lineEdit->text(), m_filter, this, m_caption);
  else if (m_selectionType == Save)
    text = KFileDialog::getSaveFileName(m_lineEdit->text(), m_filter, this, m_caption);
  else if (m_selectionType == Directory)
    text = KFileDialog::getExistingDirectory(m_lineEdit->text(), this, m_caption);
  
  if (!text.isEmpty())
    setWidgetText(text);
}

void FileSelector::showEvent(QShowEvent * e)
{
  QWidget::showEvent(e);
  emit widgetOpened();
}

#include "fileselector.moc"

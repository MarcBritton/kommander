/* KDE INCLUDES */

/* QT INCLUDES */
#include <qobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qgroupbox.h>
#include <qobjectlist.h>

/* OTHER INCLUDES */
#include "assoctextwidget.h"
#include "fileselector.h"
#include "groupbox.h"

GroupBox::GroupBox(QWidget *a_parent, const char *a_name)
	: QGroupBox(a_parent, a_name), AssocTextWidget(this)
{
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	emit widgetOpened();
}

GroupBox::~GroupBox()
{
}

QString GroupBox::currentState() const
{
	return QString("default");
}

bool GroupBox::isAssociatedTextWidget() const
{
	return TRUE;
}

QStringList GroupBox::associatedText() const
{
	return AssocTextWidget::associatedText();
}

void GroupBox::setAssociatedText(QStringList a_at)
{
	AssocTextWidget::setAssociatedText(a_at);
}

void GroupBox::setWidgetText(const QString &)
{
}

QString GroupBox::widgetText() const
{
#if 1
	QString text;
	QObjectList childList = m_childList;
	QObjectListIt it(childList);
	while(it.current() != 0)
	{
		AssocTextWidget *atw = dynamic_cast<AssocTextWidget *>(it.current());
		if(atw)
		{
			text += atw->evalAssociatedText();
		}
		++it;
	}
	return text;
#else
	QString text;
	const QObjectList *list = children();
	if(!list)
		return QString::null;

	QObjectListIt it(*list);
	while(it.current() != 0)
	{
		// FIXME : will these be in the right order?
		AssocTextWidget *atw = dynamic_cast<AssocTextWidget *>(*it);
		if(atw)
		{
			text += atw->evalAssociatedText();
		}
		++it;
	}
	delete list; // qt allocates memory for the list

	return text;
#endif
}

/* We need to keep track of all widgets created as a child of a groupbox, in order of creation. */
void GroupBox::insertChild(QObject *a_child)
{
	m_childList.append(a_child);
	QObject::insertChild(a_child);
}
 
void GroupBox::removeChild(QObject *a_child)
{
	m_childList.remove(a_child);
	QObject::removeChild(a_child);
}
#include "groupbox.moc"

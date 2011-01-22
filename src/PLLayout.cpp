#include "PLLayout.h"

#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>

PLLayout::PLLayout()
{
    m_numOfClients=0;
    m_currentLayoutIdx=-1;
    m_portraitLayoutIdx=-1;
    m_landscapeLayoutIdx=-1;
    m_hasLLayout=false;
    m_hasPLayout=false;
    m_isPLayout=false;
    m_isLLayout=false;
}

PLLayout::~PLLayout()
{
    //take ownership and responsibility to clean up the child layouts
    while(m_numOfClients>0) {
        delete(m_layoutList[--m_numOfClients]);
    }
}

/*
  FIRST, some basic methods that implement the quite
  universal engine of this portrait landscape manager.
*/
int PLLayout::newLayout(QLayout *clientlayout)
{
    m_layoutList.append(clientlayout);
    if(m_currentLayoutIdx==-1)
        m_currentLayoutIdx=0;
    return m_numOfClients++;
}

void PLLayout::activateLayout(int idx) {
    if((idx>-1)&&(idx<m_numOfClients)) {
        if(m_currentLayoutIdx != idx) {
            m_currentLayoutIdx = idx;
            redrawParent();
        }
    } else
        qDebug() << "ERROR: selecting unassigned MultLayoutClient. IDX=" << idx
                 << " numOfClients=" << m_numOfClients;
}

int PLLayout::getLayoutIdx() const
{
    return m_currentLayoutIdx;
}

void PLLayout::redrawParent()
{
    activate(); //inherited from QLayout
    update();   //inherited from QLayout	
    QWidget *w = parentWidget();
    if(!(w == 0))
        w->updateGeometry();
}

/*
  NOW, implement all required methods of a QLayout Subclass, and point
  them to the actual activated layout
*/
void PLLayout::addItem(QLayoutItem *item)
{
    if (m_numOfClients<0) {
        qDebug() << "ERROR: using MultLayout before assigning client.";
        return;
    }
    m_layoutList[getLayoutIdx()]->addItem(item);
}

QSize PLLayout::sizeHint() const {
    if (m_numOfClients<0) {
        qDebug() << "ERROR: using MultLayout before assigning client.";
        return QSize(0,0);
    }
    return (m_layoutList[getLayoutIdx()]->sizeHint());
}

QSize PLLayout::minimumSize() const
{
    if (m_numOfClients<0) {
        qDebug() << "ERROR: using MultLayout before assigning client.";
        return QSize(0,0);
    }
    return( m_layoutList[getLayoutIdx()]->minimumSize());
}

void PLLayout::setGeometry(const QRect &rect)
{
    if (m_numOfClients<0) {
        qDebug() << "ERROR: using MultLayout before assigning client.";
        return ;
    }
    m_layoutList[getLayoutIdx()]->setGeometry(rect);
}

bool PLLayout::hasHeightForWidth() const
{
    if (m_numOfClients<0) {
        qDebug() << "ERROR: using MultLayout before assigning client.";
        return false;
    }
    return m_layoutList[getLayoutIdx()]->hasHeightForWidth();
}

int PLLayout::heightForWidth(int n) const
{
    if (m_numOfClients<0) {
        qDebug() << "ERROR: using MultLayout before assigning client.";
        return 0;
    }
    return m_layoutList[getLayoutIdx()]->heightForWidth(n);
}


QLayoutItem * PLLayout::itemAt(int index) const
{
    if (m_numOfClients<0) {
        qDebug() << "ERROR: using MultLayout before assigning client.";
        return 0;
    }
    return m_layoutList[getLayoutIdx()]->itemAt(index);
}


QLayoutItem * PLLayout::takeAt(int index)
{
    if (m_numOfClients<0) {
        qDebug() << "ERROR: using MultLayout before assigning client.";
        return 0;
    }
    return m_layoutList[getLayoutIdx()]->takeAt(index);
}

int PLLayout::count() const
{
    if (m_numOfClients<0) {
        qDebug() << "ERROR: using MultLayout before assigning client.";
        return 0;
    }
    return m_layoutList[getLayoutIdx()]->count();
}

/*
  Now we add some convenience API methods to handle portrait/landscape comfortable
  While the above mechanism is quite universal and may be re-used beyond portrait
  landscape use case, these implement the dedicated API for portrait / landscape use.
*/
int PLLayout::setLLayout(QLayout *clientlayout)
{
    if(m_hasLLayout) {
        qDebug() << "ERROR: re-assigning Layout to FthLPMultiLayout is not allowed";
        return -1;
    }
    int i = newLayout(clientlayout);
    m_hasLLayout = true;
    m_landscapeLayoutIdx = i;
    activateLayout(m_landscapeLayoutIdx);
    m_isLLayout = true;
    return i;
}

int PLLayout::setPLayout(QLayout *clientlayout)
{
    if(m_hasPLayout) {
        qDebug() << "ERROR: re-assigning Layout to FthLPMultiLayout is not allowed";
        return -1;
    }
    int i = newLayout(clientlayout);
    m_hasPLayout = true;
    m_portraitLayoutIdx = i;
    activateLayout(m_portraitLayoutIdx);
    m_isPLayout = true;
    return i;
}

void PLLayout::activatePLayout()
{
    if((m_isLLayout)&&(m_hasPLayout)) {
        activateLayout(m_portraitLayoutIdx);
        m_isPLayout = true;
    } else {
        qDebug() << "ERROR: first assign portrait and landscape layouts to FthLPMultiLayout";
    }
}

void PLLayout::activateLLayout()
{
    if((m_isPLayout)&&(m_hasLLayout)) {
        activateLayout(m_landscapeLayoutIdx);
        m_isLLayout = true;
    } else {
        qDebug() << "ERROR: first assign portrait and landscape layouts to FthLPMultiLayout";
    }
}

/*
    and finally a convenience function for the parent widgets event handler
*/

void PLLayout::resizeEvent(QResizeEvent* event)
{
    if(event->size().height() > event->size().width())
        activatePLayout();
    else
        activateLLayout();
}

/*
  REFERENCES
        http://lists.trolltech.com/qt-interest/2004-01/thread00746-0.html
        http://wiki.forum.nokia.com/index.php/CS001437_-_Listening_for_screen_orientation_changes_in_Qt
        http://wiki.forum.nokia.com/index.php/Dynamic_Layout_handling_with_QWidget
*/

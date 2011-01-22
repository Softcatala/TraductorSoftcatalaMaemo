#ifndef PLLAYOUT_H
#define PLLAYOUT_H

#include <QLayout>
#include <QResizeEvent>

/*!
  PLLayout (Portrait-Landscape Layout) is a pseudo layout class,
  that looks like a layout class, but effectively just
  manages the swiching between two assigned layouts, from
  which one is intended for portrait usage, and the other for landscape
  usage.
  Usage:
    create any two layouts for portrait and landscape usage
    create an instance of this PLLayout.
    assign the pre-created portrait and landscape layouts
        to the PLLayout instance by using the setPLayout and setLLayout functions.
    Implement an event handler in the widget, that owns the PLLayout and
    the associated landscape and portrait layouts, which activates
    either of the two layouts by using the activatePLayout or activateLLayout method.
    This event handler shall re-implement the
    resizeEvent (QResizeEvent* event)  event filter of that widget, and
    check for the global applications orientation through the here implemented
    convenience function.

    example:

    void MainWindow::resizeEvent (QResizeEvent* event)
    {
        // Call the autoLayout convenience function for the event handling
        autoLayout->resizeEvent(event);
        // Call base class resize event handler
        QMainWindow::resizeEvent(event);
    }

  */


class PLLayout : public QLayout
{
public:
    PLLayout();
    ~PLLayout();
    //the main API methods
    int setLLayout(QLayout *clientlayout);
    int setPLayout(QLayout *clientlayout);
    void activatePLayout();
    void activateLLayout();
    //re-implementation of QLayout Methods, as needed
    void addItem(QLayoutItem *item);
    QSize sizeHint() const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    bool hasHeightForWidth() const;
    int heightForWidth( int ) const;
    QLayoutItem * itemAt(int index ) const;
    QLayoutItem * takeAt ( int index );
    int count() const;

    //a convenience method for the parent widgets event handler
    void resizeEvent(QResizeEvent* event);

protected:
    //these two may be promoted to public, when extending to more generic use cases
    void activateLayout(int idx);
    int newLayout(QLayout *clientlayout);

private:
    //internal management variables and methods
    QList<QLayout*> m_layoutList;
    int m_numOfClients;
    int m_currentLayoutIdx;

    int m_portraitLayoutIdx;
    int m_landscapeLayoutIdx;
    int getLayoutIdx()const ;

    bool m_hasPLayout;
    bool m_hasLLayout;
    bool m_isPLayout;
    bool m_isLLayout;

    void redrawParent();
};

#endif // PLLAYOUT_H

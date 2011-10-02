/*
qgvdial is a cross platform Google Voice Dialer
Copyright (C) 2010  Yuvraaj Kelkar

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Contact: yuvraaj@gmail.com
*/

#include "WebWidget.h"
#include "Singletons.h"

WebWidget::WebWidget(QDeclarativeItem *parent)
: QDeclarativeItem(parent)
, wv (new QWebView)
, proxy (new QGraphicsProxyWidget(this))
, selectStopper (NULL)
{
    proxy->setWidget(wv);

    GVAccess &webPage = Singletons::getRef().getGVAccess ();
    webPage.setView (wv);

#ifdef Q_WS_MAEMO_5
    selectStopper = new WebViewSelectSuppressor(wv);
    selectStopper->enable();
    wv->property("kineticScroller").value<QAbstractKineticScroller *>()->setEnabled(true);
#endif
}//WebWidget::WebWidget

WebWidget::~WebWidget()
{
    delete wv;
    wv = NULL;
}//WebWidget::~WebWidget

void
WebWidget::keyPressEvent (QKeyEvent *event)
{
    bool bIgnore = true;
    do // Begin cleanup block (not a loop)
    {
        if (Qt::Key_N != event->key ())
        {
            break;
        }

        if (Qt::ControlModifier != event->modifiers ())
        {
            break;
        }
        bIgnore = false;

        // Ask for a new page input
        bool ok = false;
        QString strUrl = QInputDialog::getText(
                            NULL,
                            tr("Enter new URL"),
                            tr("URL:"),
                            QLineEdit::Normal,
                            tr ("http://"),
                            &ok);
        if (!ok)
        {
            qDebug ("User canceled URL input");
            break;
        }

        QUrl url = QUrl::fromUserInput (strUrl);
        wv->load (url);
    } while (0); // End cleanup block (not a loop)

    if (bIgnore) {
        QDeclarativeItem::keyPressEvent (event);
    }
}//WebWidget::keyPressEvent

void
WebWidget::geometryChanged (const QRectF &newG, const QRectF &oldG)
{
    wv->resize (newG.width(), newG.height());
    QDeclarativeItem::geometryChanged (newG, oldG);
}//WebWidget::geometryChanged

WebViewSelectSuppressor::WebViewSelectSuppressor(QWebView *wv)
: QObject(wv)
, view(wv)
, enabled(false)
, mousePressed(false)
{
}//WebViewSelectSuppressor::WebViewSelectSuppressor

bool
WebViewSelectSuppressor::eventFilter(QObject *, QEvent *e)
{
    switch (e->type()) {
    case QEvent::MouseButtonPress:
        if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
            mousePressed = true;
        break;
    case QEvent::MouseButtonRelease:
        if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
            mousePressed = false;
        break;
    case QEvent::MouseMove:
        if (mousePressed)
            return true;
        break;
    default:
        break;
    }
    return false;
}//WebViewSelectSuppressor::eventFilter
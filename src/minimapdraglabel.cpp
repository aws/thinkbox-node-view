// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/minimapdraglabel.h"
#include "NodeView/nodeview.h"
#include "NodeView/nodeviewminimap.h"

#include <qglobal.h>
#if QT_VERSION >= 0x050000
#ifndef Qt5
#define Qt5
#endif
#else
#ifndef Qt4
#define Qt4
#endif
#endif

#ifdef Qt5
#include <QApplication>
#endif

#include <QMouseEvent>
#include <QtGui>

/***********************Public Members***********************/

MinimapDragLabel::MinimapDragLabel( NodeViewMiniMap* miniMap, QWidget* parent )
    : QWidget( parent )
    , m_miniMap( miniMap )
    , m_drag( false ) {
    this->setFixedSize( 8, 8 );
    this->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

/***********************Protected Members********************/
void MinimapDragLabel::enterEvent( QEvent* event ) {
    QCursor cursor = QCursor( Qt::SizeFDiagCursor );
    NodeView::MinimapPosition pos = this->m_miniMap->magmaView()->minimapPosition();
    if( pos == NodeView::TopRight || pos == NodeView::BottomLeft )
        cursor = QCursor( Qt::SizeBDiagCursor );
    QApplication::setOverrideCursor( cursor );

    QWidget::enterEvent( event );
}

void MinimapDragLabel::leaveEvent( QEvent* event ) {
    QApplication::restoreOverrideCursor();

    QWidget::leaveEvent( event );
}

void MinimapDragLabel::mouseMoveEvent( QMouseEvent* event ) {
    if( this->m_miniMap != NULL && this->m_drag ) {

        QLine* line = new QLine( this->m_currentPos, event->globalPos() );
        int deltaX = line->dx();
        int deltaY = line->dy();
        NodeView::MinimapPosition pos = this->m_miniMap->magmaView()->minimapPosition();

        if( pos == NodeView::TopRight || pos == NodeView::BottomRight )
            deltaX = 0 - deltaX;
        if( pos == NodeView::BottomLeft || pos == NodeView::BottomRight )
            deltaY = 0 - deltaY;
        this->m_miniMap->setFixedSize( qMax( this->m_miniMap->width() + deltaX, this->width() ),
                                       qMax( this->m_miniMap->height() + deltaY, this->height() ) );
        this->m_currentPos = event->globalPos();
    }
}

void MinimapDragLabel::mousePressEvent( QMouseEvent* event ) {
    if( this->m_miniMap != NULL ) {
        this->m_drag = true;
        this->m_currentPos = event->globalPos();
    }

    QWidget::mousePressEvent( event );
}

void MinimapDragLabel::mouseReleaseEvent( QMouseEvent* event ) {
    this->m_drag = false;

    QWidget::mouseReleaseEvent( event );
}

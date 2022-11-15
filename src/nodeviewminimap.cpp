// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/nodeviewminimap.h"
#include "NodeView/minimapdraglabel.h"
#include "NodeView/node.h"
#include "NodeView/nodeview.h"

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
#include <QGridLayout>
#endif

#include <QMouseEvent>
#include <QtGui>
/***********************Public Members***********************/

NodeViewMiniMap::NodeViewMiniMap( NodeView* magmaView, QWidget* parent )
    : QGraphicsView( parent )
    , m_mouseDrag( false )
    , m_magmaView( magmaView ) {
    setInteractive( false );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setStyleSheet( "background: rgba(150,150,150,128)" );
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    setFixedSize( 200, 200 );
    setRenderHint( QPainter::Antialiasing );
    setWindowFlags( Qt::SubWindow );
    // setWindowOpacity(0.2);
    if( m_magmaView != NULL ) {
        setScene( m_magmaView->scene() );
        connect( (QObject*)m_magmaView->verticalScrollBar(), SIGNAL( valueChanged( int ) ), this->scene(),
                 SLOT( invalidate() ) );
        connect( (QObject*)m_magmaView->horizontalScrollBar(), SIGNAL( valueChanged( int ) ), this->scene(),
                 SLOT( invalidate() ) );
        connect( (QObject*)m_magmaView->verticalScrollBar(), SIGNAL( sliderMoved( int ) ), this->scene(),
                 SLOT( invalidate() ) );
        connect( (QObject*)m_magmaView->horizontalScrollBar(), SIGNAL( sliderMoved( int ) ), this->scene(),
                 SLOT( invalidate() ) );
    }
    QGridLayout* layout = new QGridLayout( this );
    this->setLayout( layout );
    layout->setContentsMargins( 0, 0, 0, 0 );
    m_dragArea = new MinimapDragLabel( this, this );
    moveDragArea();
    connect( this->scene(), SIGNAL( changed( QList<QRectF> ) ), this, SLOT( adjust() ) );
}

/***********************Protected Members********************/

void NodeViewMiniMap::drawForeground( QPainter* painter, const QRectF& rect ) {
    QGraphicsView::drawForeground( painter, rect );

    if( this->m_magmaView != NULL ) {
        QRectF viewRect = this->m_magmaView->mapToScene( this->m_magmaView->viewport()->rect() ).boundingRect();

        painter->setPen( Qt::black );
        painter->drawRect( viewRect );
    }
}

NodeView* NodeViewMiniMap::magmaView() { return this->m_magmaView; }

void NodeViewMiniMap::mouseMoveEvent( QMouseEvent* event ) {
    if( this->m_magmaView != NULL && this->m_mouseDrag )
        this->m_magmaView->centerOn( this->mapToScene( event->pos() ) );

    QGraphicsView::mouseMoveEvent( event );
}

void NodeViewMiniMap::mousePressEvent( QMouseEvent* event ) {
    if( this->m_magmaView != NULL ) {
        this->m_mouseDrag = true;
        this->m_magmaView->centerOn( this->mapToScene( event->pos() ) );
    }

    QGraphicsView::mousePressEvent( event );
}

void NodeViewMiniMap::mouseReleaseEvent( QMouseEvent* event ) {
    this->m_mouseDrag = false;

    QGraphicsView::mouseReleaseEvent( event );
}

void NodeViewMiniMap::moveDragArea() {
    QGridLayout* layout = qobject_cast<QGridLayout*>( this->layout() );
    QLayoutItem* item;

    // clear the old layout removing the spacer and minimap
    while( ( item = layout->takeAt( 0 ) ) != 0 ) {
        layout->removeItem( item );
    }

    switch( this->m_magmaView->minimapPosition() ) {
    case NodeView::TopLeft:
        layout->addWidget( this->m_dragArea, 1, 1 );
        layout->setRowStretch( 0, 1 );
        layout->setColumnStretch( 0, 1 );
        layout->setRowStretch( 1, 0 );
        layout->setColumnStretch( 1, 0 );
        break;
    case NodeView::TopRight:
        layout->addWidget( this->m_dragArea, 1, 0 );
        layout->setRowStretch( 0, 1 );
        layout->setColumnStretch( 0, 0 );
        layout->setRowStretch( 1, 0 );
        layout->setColumnStretch( 1, 1 );
        break;
    case NodeView::BottomLeft:
        layout->addWidget( this->m_dragArea, 0, 1 );
        layout->setRowStretch( 0, 0 );
        layout->setColumnStretch( 0, 1 );
        layout->setRowStretch( 1, 1 );
        layout->setColumnStretch( 1, 0 );
        break;
    case NodeView::BottomRight:
        layout->addWidget( this->m_dragArea, 0, 0 );
        layout->setRowStretch( 0, 0 );
        layout->setColumnStretch( 0, 0 );
        layout->setRowStretch( 1, 1 );
        layout->setColumnStretch( 1, 1 );
        break;
    }
}

void NodeViewMiniMap::resizeEvent( QResizeEvent* event ) {
    QRectF sceneRect;
    foreach( Node* node, this->m_magmaView->nodes() ) {
        if( sceneRect == QRectF() )
            sceneRect = node->sceneBoundingRect();
        else
            sceneRect |= node->sceneBoundingRect();
    }
    this->fitInView( sceneRect, Qt::KeepAspectRatio );

    QGraphicsView::resizeEvent( event );
}

/******Private Slots*****/
void NodeViewMiniMap::adjust() {
    QRectF sceneRect;
    foreach( Node* node, this->m_magmaView->nodes() ) {
        if( sceneRect == QRectF() )
            sceneRect = node->sceneBoundingRect();
        else
            sceneRect |= node->sceneBoundingRect();
    }
    this->fitInView( sceneRect, Qt::KeepAspectRatio );
}

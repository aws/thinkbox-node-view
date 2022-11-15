// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/textlesssocket.h"
#include "NodeView/connection.h"
#include "NodeView/nodeview.h"
#include "NodeView/trianglesocketshape.h"

#include <QDebug>
#include <QtGui>

/***********************Public Members***********************/

TextlessSocket::TextlessSocket( NodeView* graphWidget, const SocketType type, QGraphicsItem* parent )
    : Socket( graphWidget, type, "Textless Socket", parent ) {
    this->m_label->hide();
}

QBrush TextlessSocket::labelBrush() const { return QBrush(); }

QPen TextlessSocket::labelPen() const { return QPen(); }

void TextlessSocket::resizeGroup( qreal width ) {
    Q_UNUSED( width );
    QRectF shapeRect;
    QGraphicsRectItem* squareShape = 0;
    QGraphicsEllipseItem* circleShape = 0;
    TriangleSocketShape* triangleShape = 0;

    // QRectF boundingRect = this->m_shape->boundingRect();

    this->removeFromGroup( this->m_shape );

    if( this->m_socketShape == Square )
        squareShape = qgraphicsitem_cast<QGraphicsRectItem*>( this->m_shape );
    else if( this->m_socketShape == Circle )
        circleShape = qgraphicsitem_cast<QGraphicsEllipseItem*>( this->m_shape );
    else if( this->m_socketShape == Triangle )
        triangleShape = qgraphicsitem_cast<TriangleSocketShape*>( this->m_shape );

    shapeRect.setRect( 0, 0, this->m_socketShapeSize, this->m_socketShapeSize );

    if( squareShape ) {
        squareShape->setRect( shapeRect );
        this->m_shape = squareShape;
    } else if( circleShape ) {
        circleShape->setRect( shapeRect );
        this->m_shape = circleShape;
    } else if( triangleShape ) {
        triangleShape->setRect( shapeRect );
    }
    // this->m_shape->setPos(0,0);

    this->addToGroup( this->m_shape );
}

void TextlessSocket::setFont( const QFont font ) { Q_UNUSED( font ); }

void TextlessSocket::setLabelBrush( const QBrush brush ) { Q_UNUSED( brush ); }

void TextlessSocket::setLabelPen( const QPen pen ) { Q_UNUSED( pen ); }

void TextlessSocket::constructGroup() {
    QRectF shapeRect;
    QGraphicsRectItem* squareShape = 0;
    QGraphicsEllipseItem* circleShape = 0;
    TriangleSocketShape* triangleShape = 0;

    // QRectF boundingRect = this->m_shape->boundingRect();

    this->removeFromGroup( this->m_shape );

    if( this->m_socketShape == Square )
        squareShape = qgraphicsitem_cast<QGraphicsRectItem*>( this->m_shape );
    else if( this->m_socketShape == Circle )
        circleShape = qgraphicsitem_cast<QGraphicsEllipseItem*>( this->m_shape );
    else if( this->m_socketShape == Triangle )
        triangleShape = qgraphicsitem_cast<TriangleSocketShape*>( this->m_shape );

    shapeRect.setRect( 0, 0, this->m_socketShapeSize, this->m_socketShapeSize );

    if( squareShape ) {
        squareShape->setRect( shapeRect );
        squareShape->setPos( this->pos() );
    } else if( circleShape ) {
        circleShape->setRect( shapeRect );
        circleShape->setPos( this->pos() );
    } else if( triangleShape ) {
        triangleShape->setRect( shapeRect );
        triangleShape->setPos( this->pos() );
    }

    this->addToGroup( this->m_shape );
}

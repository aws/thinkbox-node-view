// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/trianglesocketshape.h"

TriangleSocketShape::TriangleSocketShape( QGraphicsItem* parent )
    : QGraphicsPolygonItem( parent ) {
    m_orientation = North;
}

TriangleSocketShape::Orientation TriangleSocketShape::orientation() const { return this->m_orientation; }

QRectF TriangleSocketShape::rect() const { return this->m_rect; }

void TriangleSocketShape::setOrientation( Orientation orientation ) {
    this->m_orientation = orientation;
    this->generateTriangle();
}

void TriangleSocketShape::setRect( const QRectF& rect ) {
    this->m_rect = rect;
    this->generateTriangle();
}

void TriangleSocketShape::setRect( qreal x, qreal y, qreal width, qreal height ) {
    this->setRect( QRect( x, y, width, height ) );
}

int TriangleSocketShape::type() const { return Type; }

void TriangleSocketShape::generateTriangle() {
    QPointF p1;
    QPointF p2;
    QPointF p3;
    QPolygonF polygon;

    switch( this->m_orientation ) {
    case North:
        p1 = this->m_rect.bottomLeft();
        p2 = this->m_rect.center();
        p2.setY( this->m_rect.top() );
        p3 = this->m_rect.bottomRight();
        break;

    case East:
        p1 = this->m_rect.topLeft();
        p2 = this->m_rect.center();
        p2.setX( this->m_rect.right() );
        p3 = this->m_rect.bottomLeft();
        break;

    case South:
        p1 = this->m_rect.topLeft();
        p2 = this->m_rect.center();
        p2.setY( this->m_rect.bottom() );
        p3 = this->m_rect.topRight();
        break;

    case West:
        p1 = this->m_rect.topRight();
        p2 = this->m_rect.center();
        p2.setX( this->m_rect.left() );
        p3 = this->m_rect.bottomRight();
        break;
    case None:
        p1 = QPointF( 0, 0 );
        p2 = QPointF( 0, 0 );
        p2 = QPointF( 0, 0 );
        break;
    }

    polygon << p1 << p2 << p3 << p1;
    this->setPolygon( polygon );
}

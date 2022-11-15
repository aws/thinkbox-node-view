// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/fulllinesocket.h"
#include "NodeView/connection.h"
#include "NodeView/nodeview.h"
#include "NodeView/trianglesocketshape.h"

#include <QDebug>
#include <QtGui>

/***********************Public Members***********************/

FullLineSocket::FullLineSocket( NodeView* graphWidget, const SocketType type, const QString label,
                                QGraphicsItem* parent )
    : Socket( graphWidget, type, label, parent ) {
    setSocketShape( Socket::Square );
    this->m_label->setZValue( this->m_shape->zValue() + 1 );

    if( this->m_socketType == Socket::Input ) {
        this->m_labelPos = Left;
    } else {
        this->m_labelPos = Right;
    }
    constructGroup();
}

FullLineSocket::LabelPos FullLineSocket::labelPos() { return this->m_labelPos; }

void FullLineSocket::setLabelPos( LabelPos pos ) {
    this->m_labelPos = pos;
    this->constructGroup();
}

int FullLineSocket::type() const { return Type; }

void FullLineSocket::mouseMoveEvent( QGraphicsSceneMouseEvent* event ) { QGraphicsItemGroup::mouseMoveEvent( event ); }

void FullLineSocket::mousePressEvent( QGraphicsSceneMouseEvent* event ) {
    QGraphicsItemGroup::mousePressEvent( event );
}

void FullLineSocket::mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
    QGraphicsItemGroup::mouseReleaseEvent( event );
}

void FullLineSocket::resizeGroup( qreal width ) {
    this->m_label->setPlainText( this->m_labelText );
    QRectF shapeRect;
    QRectF boundingRect = this->m_label->boundingRect();
    QGraphicsRectItem* squareShape = 0;
    QGraphicsEllipseItem* circleShape = 0;
    TriangleSocketShape* triangleShape = 0;
    QFontMetricsF fm( this->m_label->font() );

    this->removeFromGroup( this->m_shape );
    this->removeFromGroup( this->m_label );

    boundingRect.setWidth( width );
    shapeRect.setWidth( boundingRect.width() );
    this->m_label->setPlainText( fm.elidedText( this->m_labelText, Qt::ElideRight, boundingRect.width() ) );

    if( this->m_socketShape == Square )
        squareShape = qgraphicsitem_cast<QGraphicsRectItem*>( this->m_shape );
    else if( this->m_socketShape == Circle )
        circleShape = qgraphicsitem_cast<QGraphicsEllipseItem*>( this->m_shape );
    else if( this->m_socketShape == Triangle )
        triangleShape = qgraphicsitem_cast<TriangleSocketShape*>( this->m_shape );

    if( this->m_socketType == Output ) {
        this->m_label->setPos( this->pos() );

        shapeRect.setRect( boundingRect.topLeft().x() - boundingRect.width(), boundingRect.topRight().y(),
                           boundingRect.width(), this->m_socketShapeSize );

        if( squareShape )
            squareShape->setRect( shapeRect );
        else if( circleShape )
            circleShape->setRect( shapeRect );
        else if( triangleShape )
            triangleShape->setRect( shapeRect );
    } else {
        shapeRect.setRect( 0, 0, boundingRect.width(), this->m_socketShapeSize );

        if( squareShape )
            squareShape->setRect( shapeRect );
        else if( circleShape )
            circleShape->setRect( shapeRect );
        else if( triangleShape )
            triangleShape->setRect( shapeRect );
    }

    if( this->m_labelPos == Left ) {
        this->m_label->setPos(
            this->m_shape->mapToParent( this->m_shape->boundingRect().topLeft().x() + fm.width( ' ' ),
                                        this->m_shape->boundingRect().center().y() - ( fm.height() / 2 ) ) );
    } else if( this->m_labelPos == Right ) {
        this->m_label->setPos( this->m_shape->mapToParent(
            this->m_shape->boundingRect().topRight().x() - fm.width( ' ' ) - fm.width( this->m_labelText ),
            this->m_shape->boundingRect().center().y() - ( fm.height() / 2 ) ) );
    } else {
        this->m_label->setPos( this->m_shape->mapToParent(
            this->m_shape->boundingRect().topRight().x() - fm.width( ' ' ) -
                ( ( qMin( fm.width( this->m_labelText ), boundingRect.width() - 3 * fm.width( ' ' ) ) +
                    this->m_shape->boundingRect().width() ) /
                  2 ),
            this->m_shape->boundingRect().center().y() - ( fm.height() / 2 ) ) );
    }

    this->addToGroup( this->m_shape );
    this->addToGroup( this->m_label );
}

void FullLineSocket::constructGroup() {
    this->m_label->setPlainText( this->m_labelText );
    QRectF shapeRect;
    QRectF boundingRect = this->m_label->boundingRect();
    QGraphicsRectItem* squareShape = 0;
    QGraphicsEllipseItem* circleShape = 0;
    TriangleSocketShape* triangleShape = 0;
    QFontMetricsF fm( this->m_label->font() );

    this->removeFromGroup( this->m_shape );
    this->removeFromGroup( this->m_label );

    boundingRect.setWidth( this->parentItem()->shape().boundingRect().width() );
    shapeRect.setWidth( boundingRect.width() );
    this->m_label->setPlainText( fm.elidedText( this->m_labelText, Qt::ElideRight, boundingRect.width() ) );

    if( this->m_socketShape == Square )
        squareShape = qgraphicsitem_cast<QGraphicsRectItem*>( this->m_shape );
    else if( this->m_socketShape == Circle )
        circleShape = qgraphicsitem_cast<QGraphicsEllipseItem*>( this->m_shape );
    else if( this->m_socketShape == Triangle )
        triangleShape = qgraphicsitem_cast<TriangleSocketShape*>( this->m_shape );

    if( this->m_socketType == Output ) {
        this->m_label->setPos( this->pos() );

        shapeRect.setRect( boundingRect.topLeft().x() - boundingRect.width(), boundingRect.topRight().y(),
                           boundingRect.width(), this->m_socketShapeSize );

        if( squareShape )
            squareShape->setRect( shapeRect );
        else if( circleShape )
            circleShape->setRect( shapeRect );
        else if( triangleShape )
            triangleShape->setRect( shapeRect );
    } else {
        shapeRect.setRect( 0, 0, boundingRect.width(), this->m_socketShapeSize );

        if( squareShape )
            squareShape->setRect( shapeRect );
        else if( circleShape )
            circleShape->setRect( shapeRect );
        else if( triangleShape )
            triangleShape->setRect( shapeRect );
    }

    if( this->m_labelPos == Left ) {
        this->m_label->setPos(
            this->m_shape->mapToParent( this->m_shape->boundingRect().topLeft().x() + fm.width( ' ' ),
                                        this->m_shape->boundingRect().center().y() - ( fm.height() / 2 ) ) );
    } else if( this->m_labelPos == Right ) {
        this->m_label->setPos( this->m_shape->mapToParent(
            this->m_shape->boundingRect().topRight().x() - fm.width( ' ' ) - fm.width( this->m_labelText ),
            this->m_shape->boundingRect().center().y() - ( fm.height() / 2 ) ) );
    } else {
        this->m_label->setPos( this->m_shape->mapToParent(
            this->m_shape->boundingRect().topRight().x() - fm.width( ' ' ) -
                ( ( qMin( fm.width( this->m_labelText ), boundingRect.width() - 3 * fm.width( ' ' ) ) +
                    this->m_shape->boundingRect().width() ) /
                  2 ),
            this->m_shape->boundingRect().center().y() - ( fm.height() / 2 ) ) );
    }

    this->addToGroup( this->m_shape );
    this->addToGroup( this->m_label );
}

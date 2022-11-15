// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/widgetsocket.h"
#include "NodeView/node.h"

#include <QDebug>
#include <QWidget>

WidgetSocket::WidgetSocket( NodeView* graphWidget, QWidget* widget, const SocketType type, const QString label,
                            QGraphicsItem* parent )
    : Socket( graphWidget, type, label, parent ) {
    // m_proxyWidget.setParentItem(this);
    setWidget( widget );
    setSocketShape( Socket::Square );
    constructGroup();
}

void WidgetSocket::setWidget( QWidget* widget ) {
    this->m_proxyWidget.setWidget( widget );
    this->m_proxyWidget.show();
}

void WidgetSocket::update( const QRectF& rect ) {
    if( this->isValid() ) {
        Node* parentNode = dynamic_cast<Node*>( this->parentItem() );
        this->constructGroup();
        if( parentNode )
            parentNode->redrawNode();
    }
    QGraphicsItemGroup::update( rect );
}

QWidget* WidgetSocket::widget() const { return this->m_proxyWidget.widget(); }

void WidgetSocket::constructGroup() {
    this->m_label->setPlainText( this->m_labelText );
    QRectF shapeRect;
    QRectF boundingRect = this->m_label->boundingRect();
    QGraphicsRectItem* squareShape = 0;
    QGraphicsEllipseItem* circleShape = 0;
    QFontMetricsF fm( this->m_label->font() );

    this->removeFromGroup( this->m_shape );
    this->removeFromGroup( this->m_label );
    this->removeFromGroup( &( this->m_proxyWidget ) );

    if( ( boundingRect.width() + this->m_socketShapeSize + this->m_proxyWidget.widget()->sizeHint().width() + 3 ) >
        this->parentItem()->shape().boundingRect().width() ) {
        boundingRect.setWidth( this->parentItem()->shape().boundingRect().width() - this->m_socketShapeSize -
                               this->m_proxyWidget.widget()->sizeHint().width() - 3 );
        this->m_label->setPlainText( fm.elidedText( this->m_labelText, Qt::ElideRight, boundingRect.width() ) );
    }

    if( this->m_socketShape == Square )
        squareShape = qgraphicsitem_cast<QGraphicsRectItem*>( this->m_shape );
    else
        circleShape = qgraphicsitem_cast<QGraphicsEllipseItem*>( this->m_shape );

    if( this->m_socketType == Output ) {
        this->m_label->setPos( this->m_proxyWidget.pos() + QPointF( this->m_proxyWidget.widget()->width() + 3, 0 ) );

        shapeRect.setRect( 0, 0, this->m_socketShapeSize, this->m_socketShapeSize );

        if( squareShape ) {
            squareShape->setRect( shapeRect );
            squareShape->setPos( this->m_label->pos().x() +
                                     this->m_label->boundingRect().width() /*fm.width(this->m_label->text())*/ +
                                     fm.width( ' ' ),
                                 this->m_label->pos().y() );
        } else {
            circleShape->setRect( shapeRect );
            circleShape->setPos( this->m_label->pos().x() +
                                     this->m_label->boundingRect().width() /*fm.width(this->m_label->text())*/ +
                                     fm.width( ' ' ),
                                 this->m_label->pos().y() );
        }

    } else {
        shapeRect.setRect( 0, 0, this->m_socketShapeSize, this->m_socketShapeSize );

        if( squareShape )
            squareShape->setRect( shapeRect );
        else
            circleShape->setRect( shapeRect );

        this->m_label->setPos(
            this->m_shape->mapToParent( this->m_shape->boundingRect().topRight().x() + fm.width( ' ' ),
                                        this->m_shape->boundingRect().center().y() - ( fm.height() / 2 ) ) );
        this->m_proxyWidget.setPos( this->m_label->pos() + QPointF( this->m_label->boundingRect().width() + 3, 0 ) );
    }

    //	if (this->m_socketType == Input) {
    //		QPointF pos = this->m_label->pos() + QPointF(this->m_label->boundingRect().width(), 0);
    //		this->m_proxyWidget.setPos(pos);
    //	} else if (this->m_socketType == Output){
    //		QPointF pos = this->m_label->pos() - QPointF(this->m_proxyWidget.boundingRect().width(), 0);
    //		this->m_proxyWidget.setPos(pos);
    //	}

    this->addToGroup( this->m_shape );
    this->addToGroup( this->m_label );
    this->addToGroup( &( this->m_proxyWidget ) );
}

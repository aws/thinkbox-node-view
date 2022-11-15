// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/expandablecontainer.h"
#include "NodeView/connection.h"
#include "NodeView/node.h"
#include "NodeView/nodeview.h"
#include "NodeView/sidesocketnode.h"

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
#include <QGraphicsSceneMouseEvent>
#endif

#include <QDebug>
#include <QPointF>
#include <QtGui>

/***********************Public Members***********************/

ExpandableContainer::ExpandableContainer( QGraphicsItem* containedItem, const QString label, QGraphicsItem* parent,
                                          ExpansionButtonPosition expPos, TitleLocation titleLoc )
    : QGraphicsItemGroup( parent ) {
    m_labelText = label;
    m_label.setParentItem( this );
    m_label.setText( m_labelText );
    m_titleLoc = titleLoc;
    m_expansionPos = expPos;
    setToolTip( label );
    m_containedItem = containedItem;
    m_contentsVisible = true;
    m_width = 5000;
    m_picture.setPixmap( QPixmap( ":Images/Minus_9.png" ) );
    m_picture.setParentItem( this );
    m_picture.setShapeMode( QGraphicsPixmapItem::BoundingRectShape );
    m_leftAdjust = m_rightAdjust = 0;
}

ExpandableContainer::ExpansionButtonPosition ExpandableContainer::buttonPosition() { return this->m_expansionPos; }

void ExpandableContainer::expandArea( bool expanded ) {
    this->m_contentsVisible = expanded;
    if( this->m_contentsVisible )
        this->m_picture.setPixmap( QPixmap( ":Images/Minus_9.png" ) );
    else
        this->m_picture.setPixmap( QPixmap( ":Images/Plus_9.png" ) );
}

QFont ExpandableContainer::font() const { return this->m_label.font(); }

QGraphicsItem* ExpandableContainer::getContainedItem() { return this->m_containedItem; }

QString ExpandableContainer::label() const { return this->m_labelText; }

void ExpandableContainer::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event ) {
    if( this->isPixmapUnderMouse() || this->m_label.isUnderMouse() )
        event->accept();
    else
        QGraphicsItemGroup::mouseDoubleClickEvent( event );
}

void ExpandableContainer::mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
    Q_UNUSED( event );
    if( this->m_contentsVisible ) {
        this->expandArea( false );
    } else {
        this->expandArea( true );
    }
    dynamic_cast<SideSocketNode*>( this->parentItem() )->setAreaExpanded();
    dynamic_cast<Node*>( this->parentItem() )->redrawNode();
}

void ExpandableContainer::setButtonLocation( ExpansionButtonPosition expansionPos ) {
    this->m_expansionPos = expansionPos;
    this->update();
}

void ExpandableContainer::setFont( const QFont font ) {
    this->m_label.setFont( font );
    this->update();
}

void ExpandableContainer::setLabel( const QString label ) {
    this->m_labelText = label;
    this->update();
}

void ExpandableContainer::setLeftAdjust( int adjust ) { this->m_leftAdjust = adjust; }

void ExpandableContainer::setTitleBrush( QBrush brush ) { this->m_label.setBrush( brush ); }

void ExpandableContainer::setTitleLocation( TitleLocation titleLoc ) {
    this->m_titleLoc = titleLoc;
    this->update();
}

void ExpandableContainer::setWidth( int width ) { this->m_width = width; }

QBrush ExpandableContainer::titleBrush() const { return this->m_label.brush(); }

qreal ExpandableContainer::titleHeight() {
    return qMax( this->m_label.boundingRect().height(), m_picture.boundingRect().height() );
}

ExpandableContainer::TitleLocation ExpandableContainer::titleLocation() { return this->m_titleLoc; }

int ExpandableContainer::type() const { return Type; }

void ExpandableContainer::update() { constructGroup(); }

/***********************Protected Members********************/

bool ExpandableContainer::isPixmapUnderMouse() const {
    return this->m_picture.isUnderMouse();

    return false;
}

void ExpandableContainer::mousePressEvent( QGraphicsSceneMouseEvent* event ) {
    if( this->isPixmapUnderMouse() || this->m_label.isUnderMouse() )
        event->accept();
    else
        QGraphicsItemGroup::mousePressEvent( event );
}

/***********************Private Members**********************/

void ExpandableContainer::constructGroup() {
    this->m_label.setText( this->m_labelText );
    qreal height = titleHeight();
    QRectF boundingRect = this->m_label.boundingRect();
    boundingRect.setWidth( this->m_width );
    boundingRect.setHeight(
        qMax( height, height + this->m_contentsVisible ? this->m_containedItem->boundingRect().height() : 0 ) );

    QFontMetricsF fm( this->m_label.font() );

    this->removeFromGroup( this->m_containedItem );
    this->removeFromGroup( &( this->m_label ) );

    this->addToGroup( &( this->m_label ) );
    this->addToGroup( &( this->m_picture ) );
    QString elidedText = fm.elidedText( this->m_labelText, Qt::ElideRight, boundingRect.width() - 20 );

    this->m_label.setText( elidedText );
    if( this->m_expansionPos == ButtonRight ) {

        if( this->m_titleLoc == TitleLeft ) {
            this->m_label.setPos( this->m_label.mapToParent( 0, 0 ) );
        } else if( this->m_titleLoc == TitleRight ) {
            qreal pos = boundingRect.right() - m_picture.boundingRect().width() - 3 - fm.width( elidedText );
            this->m_label.setPos( pos, 0 );
        } else {
            qreal pos = boundingRect.right() - m_picture.boundingRect().width() - 3 - fm.width( elidedText );
            pos = pos / 2;
            this->m_label.setPos( pos, 0 );
        }

        this->m_picture.setPos( boundingRect.right() - m_picture.boundingRect().width(), m_label.boundingRect().top() );

    } else {
        this->m_picture.setPos( 0, 0 );

        if( this->m_titleLoc == TitleLeft ) {
            this->m_label.setPos( this->m_picture.mapToParent( m_picture.boundingRect().width() + 3, 0 ) );
        } else if( this->m_titleLoc == TitleRight ) {
            qreal pos = boundingRect.right() - fm.width( elidedText );
            this->m_label.setPos( this->m_picture.mapToParent( pos, boundingRect.top() ) );
        } else {
            qreal pos = boundingRect.right() - fm.width( elidedText ) - m_picture.boundingRect().width() - 3;
            pos = pos / 2 + m_picture.boundingRect().width() - 3;
            this->m_label.setPos( pos, 0 );
        }

        // this->m_label.setPos(m_picture.boundingRect().right(),m_picture.boundingRect().top());
    }

    if( this->m_contentsVisible ) {
        this->m_containedItem->setVisible( true );
        this->m_containedItem->setPos(
            this->mapToParent( this->m_leftAdjust, qMax( this->m_picture.boundingRect().bottom(),
                                                         this->m_label.boundingRect().bottom() ) ) );
        this->addToGroup( this->m_containedItem );
    } else {
        this->m_containedItem->setVisible( false );
    }
}

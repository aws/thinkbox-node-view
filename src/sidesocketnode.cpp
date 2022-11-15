// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/sidesocketnode.h"
#include "NodeView/expandablecontainer.h"
#include "NodeView/nodeview.h"
#include "NodeView/socket.h"
#include "NodeView/textlesssocket.h"

#include <QDebug>
#include <QStyleOption>
/***********************Public Members***********************/

SideSocketNode::SideSocketNode( NodeView* graphWidget, QGraphicsItem* parent, QGraphicsScene* scene )
    : Node( graphWidget, parent, scene ) {
    this->m_container = new ExpandableContainer( new QGraphicsTextItem(), "", this );
    // m_container.setContainedItem(new QGraphicsTextItem() );

    QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>( this->m_container->getContainedItem() );
    textItem->setTextWidth( this->polygon().boundingRect().width() );
    textItem->setDefaultTextColor( QColor( Qt::black ) );
    textItem->setVisible( true );

    this->updateBoundingRect();
    this->resizeNode();
}

int SideSocketNode::addInputSocket() {
    Socket* sock = new TextlessSocket( this->m_graphWidget, Socket::Input, this );
    m_inSockets.append( sock );

    QObject::connect( sock, SIGNAL( connectionStarted() ), this->m_graphWidget, SLOT( onConnectionStarted() ) );
    QObject::connect( sock, SIGNAL( connectionCreated() ), this->m_graphWidget, SLOT( onConnectionCreated() ) );
    QObject::connect( sock, SIGNAL( connectionDeleted() ), this->m_graphWidget, SLOT( onConnectionDeleted() ) );
    QObject::connect( sock, SIGNAL( connectToEmpty( Socket*, QPointF ) ), this->m_graphWidget,
                      SLOT( onConnectToEmpty( Socket*, QPointF ) ) );

    this->updateBoundingRect();
    this->resizeNode();

    return m_inSockets.count() - 1;
}

int SideSocketNode::addOutputSocket() {
    Socket* sock = new TextlessSocket( this->m_graphWidget, Socket::Output, this );
    m_outSockets.append( sock );

    QObject::connect( sock, SIGNAL( connectionStarted() ), this->m_graphWidget, SLOT( onConnectionStarted() ) );
    QObject::connect( sock, SIGNAL( connectionCreated() ), this->m_graphWidget, SLOT( onConnectionCreated() ) );
    QObject::connect( sock, SIGNAL( connectionDeleted() ), this->m_graphWidget, SLOT( onConnectionDeleted() ) );
    QObject::connect( sock, SIGNAL( connectToEmpty( Socket*, QPointF ) ), this->m_graphWidget,
                      SLOT( onConnectToEmpty( Socket*, QPointF ) ) );

    this->updateBoundingRect();
    this->resizeNode();

    return m_outSockets.count() - 1;
}

QString SideSocketNode::detailText() const {
    return qgraphicsitem_cast<QGraphicsTextItem*>( this->m_container->getContainedItem() )->toPlainText();
    // this->update();
}

QColor SideSocketNode::detailTextColor() const {
    return qgraphicsitem_cast<QGraphicsTextItem*>( this->m_container->getContainedItem() )->defaultTextColor();
    // this->update();
}

QFont SideSocketNode::detailTextFont() const {
    return qgraphicsitem_cast<QGraphicsTextItem*>( this->m_container->getContainedItem() )->font();
    // this->update();
}

void SideSocketNode::expandArea( bool expanded ) {
    this->m_container->expandArea( expanded );
    this->resizeNode();
}

QString SideSocketNode::expandableTitle() const { return this->m_container->label(); }

QFont SideSocketNode::expandableTitleFont() const { return this->m_container->font(); }

void SideSocketNode::resizeNode() {

    if( this->m_disableRedraw )
        return;

    this->resizeTitleBar();
    int adjust = this->m_titleBarItem.pen().width() + 3;

    QRectF boundingRect; // = this->m_polygon.boundingRect();
    QPointF inStartPoint;
    QPointF outStartPoint;
    QFontMetricsF fm( this->m_titleTextItem.font() );
    int containerTitleHeight = this->m_container->titleHeight() + 3;

    boundingRect.setHeight( this->m_titleBarItem.boundingRect().height() /*fm.height() + 10*/ );
    boundingRect.setWidth( this->m_titleBarItem.rect().width() );

    qreal width;
    width = fm.width( this->m_titleTextItem.text() );

    qreal titleHeight = boundingRect.height();

    qreal outWidthMod = adjust;
    qreal inWidthMod = adjust;
    qreal outHeight = containerTitleHeight;

    foreach( Socket* outSocket, this->m_outSockets ) {
        outSocket->rebuildSocket( boundingRect.width() );
    }
    foreach( Socket* inSocket, this->m_inSockets ) {
        inSocket->rebuildSocket( boundingRect.width() );
    }
    this->m_container->update();

    foreach( Socket* outSocket, this->m_outSockets ) {

        TextlessSocket* sock = qobject_cast<TextlessSocket*>( outSocket );
        outHeight += sock->boundingRect().height() + this->m_socketBuffer;
        outWidthMod = qMax( outWidthMod, adjust + sock->socketShapeSize() / 2 + 3 );
    }

    qreal inHeight = containerTitleHeight;
    int leftIndent = 0;
    foreach( Socket* inSocket, this->m_inSockets ) {
        TextlessSocket* sock = qobject_cast<TextlessSocket*>( inSocket );
        inHeight += sock->boundingRect().height() + m_socketBuffer;
        inWidthMod = qMax( inWidthMod, adjust + sock->socketShapeSize() / 2 + 3 );
        leftIndent = qMax( (int)( sock->socketShapeSize() / 2 ), 0 );
        if( sock->socketShapeSize() / 2 > leftIndent )
            leftIndent = sock->socketShapeSize() / 2;
    }
    this->m_container->setLeftAdjust( leftIndent );
    this->m_container->setWidth( this->m_titleBarItem.rect().width() - 2 * adjust );
    this->m_container->update();

    qreal detailHeight = this->m_container->boundingRect().height();
    detailHeight = qMax( detailHeight, qMax( outHeight, inHeight ) );

    qreal height = detailHeight + titleHeight;

    height += ( this->outlinePen().width() + 3 ) * 2;

    qreal leftAdjust;

    if( this->m_inSockets.count() > 0 ) {
        qreal num = this->m_inSockets[0]->socketShapeSize() / 2 + 3;
        leftAdjust = qMax( adjust + 0.0, num );
        inStartPoint =
            QPointF( boundingRect.left(), titleHeight + detailHeight / 2 + ( this->outlinePen().width() + 3 ) );
        if( this->m_inSockets.count() % 2 == 1 ) {
            inStartPoint -= QPointF( 0, ( m_inSockets[m_inSockets.count() / 2]->boundingRect().height() + 2 ) / 2 );
            for( int i = m_inSockets.count() / 2; i > 0; i-- ) {
                inStartPoint -= QPointF( 0, qobject_cast<TextlessSocket*>( m_inSockets[i] )->boundingRect().height() +
                                                m_socketBuffer );
            }
        } else {

            for( int i = m_inSockets.count() / 2; i > 0; i-- ) {
                inStartPoint -= QPointF( 0, qobject_cast<TextlessSocket*>( m_inSockets[i] )->boundingRect().height() +
                                                m_socketBuffer );
            }
        }
    }

    qreal rightAdjust = leftAdjust + adjust + 3;
    if( this->m_outSockets.count() > 0 ) {
        rightAdjust = qMin( rightAdjust, leftAdjust + this->m_outSockets[0]->socketShapeSize() / 2 + 3 );
        outStartPoint =
            QPointF( boundingRect.width(), titleHeight + detailHeight / 2 + ( this->outlinePen().width() + 3 ) );
        if( this->m_outSockets.count() % 2 == 1 ) {
            outStartPoint -= QPointF( 0, ( m_outSockets[m_outSockets.count() / 2]->boundingRect().height() + 2 ) / 2 );
            for( int i = m_outSockets.count() / 2; i > 0; i-- ) {
                outStartPoint -= QPointF( 0, qobject_cast<TextlessSocket*>( m_outSockets[i] )->boundingRect().height() +
                                                 m_socketBuffer );
            }
        } else {
            // outStartPoint += QPointF(0,(m_outSockets[m_outSockets.count()/2]->boundingRect().height() + 2)/2);
            for( int i = m_outSockets.count() / 2; i > 0; i-- ) {
                outStartPoint -= QPointF( 0, qobject_cast<TextlessSocket*>( m_outSockets[i] )->boundingRect().height() +
                                                 m_socketBuffer );
            }
        }
    }

    QPointF topLeftAdjust( leftAdjust, 0 );
    this->m_container->setPos( boundingRect.bottomLeft() + QPointF( adjust, 3 ) );

    width = qMax( width - 2 * adjust, this->m_minWidth - rightAdjust );
    qgraphicsitem_cast<QGraphicsTextItem*>( this->m_container->getContainedItem() )
        ->setTextWidth( this->m_titleBarItem.rect().width() - inWidthMod - outWidthMod );
    boundingRect.setHeight( height );
    // Create a new polygon based on the new size of the node
    QPolygonF newPolygon( boundingRect );
    this->prepareGeometryChange();
    this->m_polygon = newPolygon;

    // Set the starting points of the output sockets
    foreach( Socket* outSocket, this->m_outSockets ) {
        outStartPoint += QPointF( 0, outSocket->boundingRect().height() + m_socketBuffer );
        outSocket->setPos( outStartPoint -
                           QPoint( ( outSocket->socketShapeSize() / 2 ), outSocket->boundingRect().height() ) );
    }

    // Set the starting points of the input sockets
    foreach( Socket* inSocket, this->m_inSockets ) {
        inStartPoint += QPointF( 0, inSocket->boundingRect().height() + m_socketBuffer );
        inSocket->setPos( inStartPoint +
                          QPoint( -( inSocket->socketShapeSize() / 2 ), -inSocket->boundingRect().height() ) );
    }

    /*	foreach (Socket *outSocket, this->m_outSockets) {

         qobject_cast<TextlessSocket*>(outSocket)->update();
      }
      foreach (Socket *inSocket, this->m_inSockets) {
        qobject_cast<TextlessSocket*>(inSocket)->update();
      }
    */
    if( this->m_areaExpanded ) {
        this->updateBoundingRect();
        this->m_areaExpanded = false;
    }
    this->updateBoundingRect();
    this->updateConnections();
}

void SideSocketNode::setAreaExpanded() { this->m_areaExpanded = true; }

ExpandableContainer* SideSocketNode::expandableArea() { return this->m_container; }

void SideSocketNode::resizeTitleBar() {
    int adjust = this->m_titleBarItem.pen().width() + 3;
    QRectF titleBarRect;
    QPointF topLeftAdjust( adjust, adjust );
    QFontMetricsF fm( this->m_titleTextItem.font() );
    qreal width = 0;
    qreal height = 0;
    qreal textWidth;

    switch( this->m_pixmapPosition ) {
    case None:

        width = 0;
        if( this->m_elidedTitle == Qt::ElideNone ) {
            width = fm.width( this->m_titleText ); // qMax(this->m_detailTextItem.textWidth(),
                                                   // fm.width(this->m_titleTextItem.text()));
            width = qMax( width + adjust * 2, this->m_minWidth );
        } else
            width = this->m_minWidth;
        this->m_titleTextItem.setText( fm.elidedText( this->m_titleText, this->m_elidedTitle, width ) );
        this->m_detailTextItem.setTextWidth( width );

        height = fm.height();
        break;

    case Left:
    case Right:

        width = 0;
        if( this->m_elidedTitle == Qt::ElideNone ) {
            textWidth = fm.width( this->m_titleText ); // qMax(fm.width(this->m_titleTextItem.text()),
                                                       // this->m_detailTextItem.boundingRect().width());
            width = this->m_pixmapItem.boundingRect().width() + textWidth;
            width = qMax( width, this->m_minWidth );
        } else {
            width = qMax( this->m_pixmapItem.boundingRect().width(), this->m_minWidth );
        }
        this->m_titleTextItem.setText( fm.elidedText( this->m_titleText, this->m_elidedTitle,
                                                      width - this->m_pixmapItem.boundingRect().width() - adjust ) );
        this->m_detailTextItem.setTextWidth( width - this->m_pixmapItem.boundingRect().width() - adjust /*textWidth*/ );

        height = qMax( this->m_pixmapItem.boundingRect().height(), ( fm.height() ) );
        break;

    case Top:
    case Center:
    case Bottom:

        width = this->m_pixmapItem.boundingRect()
                    .width(); // qMax(this->m_pixmapItem.boundingRect().width(), this->m_detailTextItem.textWidth());
        if( this->m_elidedTitle == Qt::ElideNone ) {
            width = qMax( width, fm.width( this->m_titleText ) );
        }
        width = qMax( width, this->m_minWidth );

        this->m_titleTextItem.setText( fm.elidedText( this->m_titleText, this->m_elidedTitle, width ) );
        this->m_detailTextItem.setTextWidth( width );

        height = fm.height() + ( this->m_pixmapItem.isVisible() ? this->m_pixmapItem.boundingRect().height() : 0 );
        break;
    }

    titleBarRect.setHeight( height + ( adjust * 2 ) );
    titleBarRect.setWidth( width + ( adjust * 2 ) ); // + (adjust*2));

    this->m_titleTextItem.setToolTip( this->m_titleText );

    this->prepareGeometryChange();
    this->m_titleBarItem.setRect( titleBarRect );
    int pixmapAdjust = 0;
    int titleAdjust = 0;
    switch( this->m_pixmapPosition ) {
    case None:
        if( this->m_titleJustify == TitleCenter )
            this->m_titleTextItem.setPos( titleBarRect.center().x() - ( fm.width( this->m_titleTextItem.text() ) / 2 ),
                                          adjust );
        else if( this->m_titleJustify == TitleLeft )
            this->m_titleTextItem.setPos( titleBarRect.left() + adjust, adjust );
        else
            this->m_titleTextItem.setPos( titleBarRect.right() - adjust - m_titleTextItem.boundingRect().width(),
                                          adjust );
        break;

    case Left:
        if( this->m_pixmapItem.boundingRect().height() > fm.height() )
            titleAdjust += ( this->m_pixmapItem.boundingRect().height() - fm.height() ) / 2;
        else
            pixmapAdjust += ( fm.height() - this->m_pixmapItem.boundingRect().height() ) / 2;
        this->m_pixmapItem.setPos( titleBarRect.topLeft() + topLeftAdjust + QPointF( 0, pixmapAdjust ) );

        if( this->m_titleJustify == TitleCenter )
            this->m_titleTextItem.setPos(
                ( titleBarRect.center().x() - ( fm.width( this->m_titleTextItem.text() ) / 2 ) ) +
                    ( this->m_pixmapItem.boundingRect().width() / 2 ),
                adjust + titleAdjust );
        else if( this->m_titleJustify == TitleLeft )
            this->m_titleTextItem.setPos( this->m_pixmapItem.boundingRect().right() + adjust * ( 3.0 / 2.0 ),
                                          adjust + titleAdjust );
        else
            this->m_titleTextItem.setPos( titleBarRect.right() - adjust - m_titleTextItem.boundingRect().width(),
                                          adjust + titleAdjust );

        break;

    case Right:
        if( this->m_pixmapItem.boundingRect().height() > fm.height() )
            titleAdjust += ( this->m_pixmapItem.boundingRect().height() - fm.height() ) / 2;
        else
            pixmapAdjust += ( fm.height() - this->m_pixmapItem.boundingRect().height() ) / 2;
        // this->m_titleTextItem.setPos((titleBarRect.center().x() - (fm.width(this->m_titleTextItem.text()) / 2)) -
        // (this->m_pixmapItem.boundingRect().width()/2)-adjust/2, adjust);
        this->m_pixmapItem.setPos(
            titleBarRect.topRight() -
            QPointF( m_pixmapItem.boundingRect().width() + topLeftAdjust.x(), 0 - topLeftAdjust.y() - pixmapAdjust ) );
        if( this->m_titleJustify == TitleCenter )
            this->m_titleTextItem.setPos(
                ( titleBarRect.center().x() - ( fm.width( this->m_titleTextItem.text() ) / 2 ) ) -
                    ( this->m_pixmapItem.boundingRect().width() / 2 ),
                adjust + titleAdjust );
        else if( this->m_titleJustify == TitleLeft )
            this->m_titleTextItem.setPos( titleBarRect.left() + adjust, adjust + titleAdjust );
        else
            this->m_titleTextItem.setPos( titleBarRect.topRight().x() - ( fm.width( this->m_titleTextItem.text() ) ) -
                                              ( this->m_pixmapItem.boundingRect().width() ) - ( adjust ),
                                          adjust + titleAdjust );
        break;

    case Top:
        this->m_pixmapItem.setPos( titleBarRect.center().x() - ( this->m_pixmapItem.boundingRect().width() / 2 ),
                                   adjust );
        if( this->m_titleJustify == TitleCenter )
            this->m_titleTextItem.setPos( titleBarRect.center().x() - ( fm.width( this->m_titleTextItem.text() ) / 2 ),
                                          this->m_pixmapItem.pos().y() + this->m_pixmapItem.boundingRect().height() );
        else if( this->m_titleJustify == TitleLeft )
            this->m_titleTextItem.setPos( titleBarRect.left() + adjust,
                                          this->m_pixmapItem.pos().y() + this->m_pixmapItem.boundingRect().height() );
        else
            this->m_titleTextItem.setPos( titleBarRect.right() - adjust - m_titleTextItem.boundingRect().width(),
                                          this->m_pixmapItem.pos().y() + this->m_pixmapItem.boundingRect().height() );
        break;

    case Center:
    case Bottom:
        if( this->m_titleJustify == TitleCenter )
            this->m_titleTextItem.setPos( titleBarRect.center().x() - ( fm.width( this->m_titleTextItem.text() ) / 2 ),
                                          adjust );
        else if( this->m_titleJustify == TitleLeft )
            this->m_titleTextItem.setPos( titleBarRect.left() + adjust, adjust );
        else
            this->m_titleTextItem.setPos( titleBarRect.right() - adjust - m_titleTextItem.boundingRect().width(),
                                          adjust );
        this->m_pixmapItem.setPos( titleBarRect.center().x() - ( this->m_pixmapItem.boundingRect().width() / 2 ),
                                   this->m_titleTextItem.pos().y() + fm.height() );
        break;
    }
}

void SideSocketNode::setDetailText( const QString& text ) {
    qgraphicsitem_cast<QGraphicsTextItem*>( this->m_container->getContainedItem() )->setHtml( text );
    this->updateBoundingRect();
    this->redrawNode();
}

void SideSocketNode::setDetailTextColor( const QColor& color ) {
    qgraphicsitem_cast<QGraphicsTextItem*>( this->m_container->getContainedItem() )->setDefaultTextColor( color );
    // this->update();
}

void SideSocketNode::setDetailTextEnabled( bool enabled ) {
    qgraphicsitem_cast<QGraphicsTextItem*>( this->m_container->getContainedItem() )->setVisible( enabled );
    this->updateBoundingRect();
    this->redrawNode();
}

void SideSocketNode::setDetailTextFont( const QFont& font ) {
    qgraphicsitem_cast<QGraphicsTextItem*>( this->m_container->getContainedItem() )->setFont( font );
    this->updateBoundingRect();
    this->resizeNode();
}

void SideSocketNode::setExpandableArea( ExpandableContainer* container ) {
    this->m_container = container;
    this->updateBoundingRect();
    this->resizeNode();
}

void SideSocketNode::setExpandableTitle( const QString& text ) {
    this->m_container->setLabel( text );
    this->updateBoundingRect();
    this->redrawNode();
}

void SideSocketNode::setExpandableTitleFont( const QFont& font ) {
    this->m_container->setFont( font );
    this->updateBoundingRect();
    this->redrawNode();
}

int SideSocketNode::type() const { return Type; }

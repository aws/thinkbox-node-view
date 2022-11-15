// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/simplenode.h"
#include "NodeView/nodeview.h"
#include "NodeView/socket.h"
#include "NodeView/textlesssocket.h"

#include <QDebug>
#include <QFontMetricsF>

SimpleNode::SimpleNode( NodeView* graphWidget, QGraphicsItem* parent, QGraphicsScene* scene )
    : Node( graphWidget, parent, scene )
    , m_minHeight( 0 ) {
    m_polygon = QPolygonF( QRectF( 0, 0, 10, 10 ) );
    m_titleBarItem.hide();
    m_titleTextItem.setParentItem( this );
    redrawNode();
}

int SimpleNode::addInputSocket() {
    TextlessSocket* sock = new TextlessSocket( this->m_graphWidget, Socket::Input, this );
    m_inSockets.append( sock );

    QObject::connect( sock, SIGNAL( connectionStarted() ), this->m_graphWidget, SLOT( onConnectionStarted() ) );
    QObject::connect( sock, SIGNAL( connectionCreated() ), this->m_graphWidget, SLOT( onConnectionCreated() ) );
    QObject::connect( sock, SIGNAL( connectionDeleted() ), this->m_graphWidget, SLOT( onConnectionDeleted() ) );
    QObject::connect( sock, SIGNAL( connectToEmpty( Socket*, QPointF ) ), this->m_graphWidget,
                      SLOT( onConnectToEmpty( Socket*, QPointF ) ) );

    this->resizeNode();

    return m_inSockets.count() - 1;
}

int SimpleNode::addOutputSocket() {
    TextlessSocket* sock = new TextlessSocket( this->m_graphWidget, Socket::Output, this );
    m_outSockets.append( sock );

    QObject::connect( sock, SIGNAL( connectionStarted() ), this->m_graphWidget, SLOT( onConnectionStarted() ) );
    QObject::connect( sock, SIGNAL( connectionCreated() ), this->m_graphWidget, SLOT( onConnectionCreated() ) );
    QObject::connect( sock, SIGNAL( connectionDeleted() ), this->m_graphWidget, SLOT( onConnectionDeleted() ) );
    QObject::connect( sock, SIGNAL( connectToEmpty( Socket*, QPointF ) ), this->m_graphWidget,
                      SLOT( onConnectToEmpty( Socket*, QPointF ) ) );

    this->resizeNode();

    return m_outSockets.count() - 1;
}

QRectF SimpleNode::boundingRect() const {
    QRectF boundingRect = this->m_polygon.boundingRect();

    qreal penWidth = this->m_outlinePen.width();

    qreal topAdjust = penWidth / 2;
    qreal bottomAdjust = penWidth / 2;

    foreach( Socket* socket, this->m_inSockets ) {
        bottomAdjust = qMax( bottomAdjust, socket->socketShapeSize() );
    }

    foreach( Socket* socket, this->m_outSockets ) {
        topAdjust = qMax( topAdjust, socket->socketShapeSize() );
    }

    boundingRect.adjust( -penWidth / 2, -bottomAdjust, penWidth / 2, topAdjust );

    if( this->m_dropShadow ) {
        QRectF dropShadowRect = this->m_polygon.boundingRect().translated( 8, 8 );
        return boundingRect.united( dropShadowRect );
    } else
        return boundingRect;
}

qreal SimpleNode::minimumHeight() const { return m_minHeight; }

void SimpleNode::setMinimumHeight( qreal height ) { m_minHeight = height; }

void SimpleNode::resizeNode() {
    QRectF boundingRect = this->shape().boundingRect();
    QPointF inStartPoint;
    QPointF outStartPoint;
    QFontMetricsF fm( this->m_titleTextItem.font() );

    qreal width = boundingRect.width();
    qreal height = fm.height();
    qreal inSocketWidth = 0;
    qreal outSocketWidth = 0;

    foreach( Socket* outSocket, this->m_outSockets ) {
        outSocket->rebuildSocket( outSocket->socketShapeSize() );
    }

    foreach( Socket* inSocket, this->m_inSockets ) {
        inSocket->rebuildSocket( inSocket->socketShapeSize() );
    }

    foreach( Socket* outSocket, this->m_outSockets ) {
        outSocketWidth += outSocket->socketShapeSize() + 4;
    }

    foreach( Socket* inSocket, this->m_inSockets ) {
        inSocketWidth += inSocket->socketShapeSize() + 4;
    }

    m_titleTextItem.setText( m_titleText );

    width = qMax( outSocketWidth, inSocketWidth );
    width = qMax( fm.width( this->m_titleTextItem.text() ) + 10, width );
    width = qMax( m_minWidth, width );

    height = qMax( m_minHeight, height );

    boundingRect.setWidth( width );
    boundingRect.setHeight( height );
    inStartPoint = boundingRect.center() - QPointF( inSocketWidth / 2, boundingRect.height() / 2 );
    outStartPoint = boundingRect.center() + QPointF( -( outSocketWidth / 2 ), boundingRect.height() / 2 );

    // Create a new polygon based on the new size of the node
    QPolygonF newPolygon( boundingRect );
    this->prepareGeometryChange();
    this->m_polygon = newPolygon;

    this->m_titleTextItem.setPos( boundingRect.topLeft() +
                                  QPointF( ( boundingRect.width() - fm.width( this->m_titleTextItem.text() ) ) / 2,
                                           ( boundingRect.height() - fm.height() ) / 2 ) );

    // Set the starting points of the input sockets
    foreach( Socket* inSocket, this->m_inSockets ) {
        inSocket->setPos(
		    inStartPoint -
		    QPointF(
		        0,
		        inSocket
		            ->socketShapeSize() ) /*inStartPoint + QPoint(-(inSocket->socketShapeSize()/2), -inSocket->boundingRect().height())*/ );
        inStartPoint += QPointF( inSocket->socketShapeSize() + 4, 0 );
    }

    // Set the starting points of the output sockets
    foreach( Socket* outSocket, this->m_outSockets ) {
        outSocket->setPos(
		    outStartPoint /*outStartPoint - QPoint(outSocket->boundingRect().width() - (outSocket->socketShapeSize()/2), outSocket->boundingRect().height())*/ );
        outStartPoint += QPointF( outSocket->socketShapeSize() + 4, 0 );
    }

    //	foreach (Socket *outSocket, this->m_outSockets) {
    //		outSocket->update();
    //	}
    //	foreach (Socket *inSocket, this->m_inSockets) {
    //		inSocket->update();
    //	}

    this->updateConnections();
}

QPainterPath SimpleNode::shape() const {
    QPainterPath path;
    path.addRoundedRect( this->m_polygon.boundingRect(), this->m_cornerXRadius, this->m_cornerYRadius );
    return path;
}

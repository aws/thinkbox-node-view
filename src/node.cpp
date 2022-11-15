// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/node.h"
#include "NodeView/connection.h"
#include "NodeView/fulllinesocket.h"
#include "NodeView/nodeview.h"
#include "NodeView/socket.h"
#include "NodeView/widgetsocket.h"

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

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

int Node::NextZNode = 0;

/***********************Public Members***********************/

Node::Node( NodeView* graphWidget, QGraphicsItem* parent, QGraphicsScene* scene )
#ifdef Qt5
    : QGraphicsPolygonItem( parent )
    , m_graphWidget( graphWidget ) {
    Q_UNUSED( scene );
#else
    : QGraphicsPolygonItem( parent, scene )
    , m_graphWidget( graphWidget ) {
#endif

    setFlag( ItemIsMovable );
    setFlag( ItemSendsGeometryChanges );
    setFlag( ItemIsSelectable );
    setFlag( ItemIsFocusable );
    setCacheMode( DeviceCoordinateCache );

    Node::NextZNode++;
    if( Node::NextZNode == INT_MAX )
        Node::NextZNode = 0;
    setZValue( Node::NextZNode );

    setAcceptHoverEvents( true );

    m_disableRedraw = false;
    m_polygon = QPolygonF( BASE_RECT );

    m_outlinePen = QPen( Qt::black );
    m_outlinePen.setWidth( 3 );
    m_selectedPen = QPen( Qt::darkGray );
    m_selectedPen.setWidth( 3 );
    m_backgroundBrush = QBrush( QColor( 200, 255, 200 ) );
    m_selectedBrush = m_backgroundBrush;
    m_titleBarBrush = m_backgroundBrush;
    m_titleBarBrush.setColor( m_titleBarBrush.color().darker( 125 ) );
    m_titleBarSelectedBrush = m_titleBarBrush;

    m_cornerXRadius = 5;
    m_cornerYRadius = 5;
    m_minWidth = 150;

    m_elidedTitle = Qt::ElideNone;

    // setName("Node");
    m_titleText = "Node";

    m_pixmapPosition = None;

    m_dropShadow = true;
    // setGraphicsEffect(new QGraphicsDropShadowEffect());
    // graphicsEffect()->setEnabled(m_dropShadow);

    m_titleBarItem.setParentItem( this );
    m_titleBarItem.setPos( 0, 0 );
    m_titleBarItem.setPen( m_outlinePen );
    m_titleBarItem.setBrush( m_titleBarBrush );

    // Set up title font
    QFont titleFont;
    titleFont.setPointSize( 12 );
    titleFont.setBold( true );
    // titleFont.setFamily("Arial");

    m_titleTextItem.setParentItem( &m_titleBarItem );
    m_titleTextItem.setFont( titleFont );

    m_detailTextItem.setParentItem( &m_titleBarItem );
    m_detailTextItem.setTextWidth( this->polygon().boundingRect().width() );
    m_detailTextItem.setDefaultTextColor( QColor( Qt::black ) );
    m_detailTextItem.setVisible( false );

    m_bodyDetailTextItem.setParentItem( this );
    m_bodyDetailTextItem.setTextWidth( this->polygon().boundingRect().width() );
    m_bodyDetailTextItem.setDefaultTextColor( QColor( Qt::black ) );
    m_bodyDetailTextItem.setVisible( false );
    m_bodyDetailDivider.setParentItem( this );
    m_bodyDetailDivider.setVisible( false );
    m_bodyDetailDivider.setPen( m_outlinePen );

    m_pixmapItem.setParentItem( &m_titleBarItem );
    m_pixmapItem.setVisible( false );

    m_socketBuffer = 2;

    m_inlineSockets = true;
    m_inlineInputSocketWidth = 0.475;
    m_inlineSocketGap = 0.05;

    this->redrawNode();
    m_graphWidget->registerStyleableItem( this );
    m_titleJustify = TitleCenter;
}

Node::~Node() {
    foreach( Socket* socket, this->m_inSockets ) {
        this->m_inSockets.removeAll( socket );
        socket->deleteLater();
    }
    foreach( Socket* socket, this->m_outSockets ) {
        this->m_outSockets.removeAll( socket );
        socket->deleteLater();
    }
}

int Node::addInputFullLineSocket() {
    FullLineSocket* sock = new FullLineSocket( this->m_graphWidget, Socket::Input, "Input", this );
    m_inSockets.append( sock );
    this->updateBoundingRect();
    this->resizeNode();

    return m_inSockets.count() - 1;
}

int Node::addInputSocket() {
    Socket* sock = new Socket( this->m_graphWidget, Socket::Input, "Input", this );
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

int Node::addInputWidgetSocket() {
    WidgetSocket* sock =
        new WidgetSocket( this->m_graphWidget, new QPushButton( "test" ), Socket::Input, "Input", this );
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

int Node::addOutputFullLineSocket() {
    FullLineSocket* sock = new FullLineSocket( this->m_graphWidget, Socket::Output, "Output", this );
    m_outSockets.append( sock );

    this->updateBoundingRect();
    this->resizeNode();

    return m_outSockets.count() - 1;
}

int Node::addOutputSocket() {
    Socket* sock = new Socket( this->m_graphWidget, Socket::Output, "Output", this );
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

int Node::addOutputWidgetSocket() {
    WidgetSocket* sock =
        new WidgetSocket( this->m_graphWidget, new QPushButton( "test" ), Socket::Output, "Output", this );
    m_outSockets.append( sock );

    QObject::connect( sock, SIGNAL( connectionStarted() ), this->m_graphWidget, SLOT( onConnectionStarted() ) );
    QObject::connect( sock, SIGNAL( connectionCreated() ), this->m_graphWidget, SLOT( onConnectionCreated() ) );
    QObject::connect( sock, SIGNAL( connectionDeleted() ), this->m_graphWidget, SLOT( onConnectionDeleted() ) );
    QObject::connect( sock, SIGNAL( connectToEmpty( Socket*, QPointF ) ), this->m_graphWidget,
                      SLOT( onConnectToEmpty( Socket*, QPointF ) ) );

    this->updateBoundingRect();
    this->resizeNode();

    return m_inSockets.count() - 1;
}

QBrush Node::backgroundBrush() const { return this->m_backgroundBrush; }

void Node::beginModifyNode() { this->m_disableRedraw = true; }

void Node::updateBoundingRect() {
    QRectF boundingRect = this->m_polygon.boundingRect();

    qreal penWidth = this->m_outlinePen.width();

    qreal leftAdjust = penWidth / 2;
    qreal rightAdjust = penWidth / 2;

    foreach( Socket* socket, this->m_inSockets ) {
        if( socket->type() != FullLineSocket::Type )
            leftAdjust = qMax( leftAdjust, socket->socketShapeSize() / 2 );
    }

    foreach( Socket* socket, this->m_outSockets ) {
        if( socket->type() != FullLineSocket::Type )
            rightAdjust = qMax( rightAdjust, socket->socketShapeSize() / 2 );
    }

    boundingRect.adjust( -leftAdjust, -penWidth / 2, rightAdjust, penWidth / 2 );
    if( this->m_dropShadow ) {
        QRectF dropShadowRect = this->m_polygon.boundingRect().translated( 8, 8 );
        boundingRect = boundingRect.united( this->m_titleBarItem.boundingRect() ).united( dropShadowRect );
    } else
        boundingRect = boundingRect.united( this->m_titleBarItem.boundingRect() );
    m_boundingRect = boundingRect;
}

QTextCursor Node::bodyDetailTextCursor() { return this->m_bodyDetailTextItem.textCursor(); }

QString Node::bodyDetailText() const { return this->m_bodyDetailTextItem.toPlainText(); }

QColor Node::bodyDetailTextColor() const { return this->m_bodyDetailTextItem.defaultTextColor(); }

bool Node::bodyDetailTextEnabled() const { return this->m_bodyDetailTextItem.isVisible(); }

QFont Node::bodyDetailTextFont() const { return this->m_bodyDetailTextItem.font(); }

QRectF Node::boundingRect() const { return m_boundingRect; }

qreal Node::cornerXRadius() const { return this->m_cornerXRadius; }

qreal Node::cornerYRadius() const { return this->m_cornerYRadius; }

void Node::deleteInputSocket( int index ) {
    if( index >= 0 && index < this->m_inSockets.count() ) {
        delete this->m_inSockets[index];
        this->m_inSockets.removeAt( index );
        this->updateBoundingRect();
        this->resizeNode();
    }
}

void Node::deleteOutputSocket( int index ) {
    if( index >= 0 && index < this->m_outSockets.count() ) {
        delete this->m_outSockets[index];
        this->m_outSockets.removeAt( index );
        this->updateBoundingRect();
        this->resizeNode();
    }
}

void Node::deletingSelf() { emit deletingNode( this ); }

QString Node::detailText() const { return this->m_detailTextItem.toPlainText(); }

QColor Node::detailTextColor() const { return this->m_detailTextItem.defaultTextColor(); }

bool Node::detailTextEnabled() const { return this->m_detailTextItem.isVisible(); }

QFont Node::detailTextFont() const { return this->m_detailTextItem.font(); }

Qt::TextElideMode Node::elidedTitle() { return this->m_elidedTitle; }

void Node::endModifyNode() {
    this->m_disableRedraw = false;
    this->redrawNode();
}

bool Node::inlineSockets() const { return this->m_inlineSockets; }

qreal Node::inlineTextWidthPercentage() const { return this->m_inlineInputSocketWidth; }

qreal Node::inlineTextGapPercentage() const { return this->m_inlineSocketGap; }

bool Node::dropShadow() const { return this->m_dropShadow; }

int Node::socketBuffer() const { return this->m_socketBuffer; }

void Node::setSocketBuffer( int buffer ) {
    this->m_socketBuffer = buffer;
    this->updateBoundingRect();
    redrawNode();
}

QMap<Node*, int> Node::getConnectedNodesToInputSocket( int inputSocketIndex ) const {
    Socket* inputSocket = this->m_inSockets[inputSocketIndex];
    QMap<Node*, int> connectedNodes;

    foreach( Connection* conn, inputSocket->connections() ) {
        Node* connectedNode = qgraphicsitem_cast<Node*>( conn->sourceSocket()->parentItem() );

        if( connectedNode )
            connectedNodes.insert( connectedNode, connectedNode->getOutputSockets().indexOf( conn->sourceSocket() ) );
    }

    return connectedNodes;
}

QMap<Node*, int> Node::getConnectedNodesToOutputSocket( int outputSocketIndex ) const {
    QMap<Node*, int> connectedNodes;
    Socket* outputSocket = this->m_outSockets[outputSocketIndex];

    foreach( Connection* conn, outputSocket->connections() ) {
        Node* connectedNode = qgraphicsitem_cast<Node*>( conn->destinationSocket()->parentItem() );

        if( connectedNode )
            connectedNodes.insert( connectedNode,
                                   connectedNode->getInputSockets().indexOf( conn->destinationSocket() ) );
    }

    return connectedNodes;
}

Socket* Node::getInputSocket( int index ) const { return this->m_inSockets[index]; }

QList<Socket*> Node::getInputSockets() const { return QList<Socket*>( this->m_inSockets ); }

int Node::getInputSocketCount() const { return this->m_inSockets.count(); }

Socket* Node::getOutputSocket( int index ) const { return this->m_outSockets[index]; }

QList<Socket*> Node::getOutputSockets() const { return QList<Socket*>( this->m_outSockets ); }

int Node::getOutputSocketCount() const { return this->m_outSockets.count(); }

bool Node::isNodeConnected( Node* check ) {
    if( isNodeConnectedToInput( check ) )
        return true;
    if( isNodeConnectedToOutput( check ) )
        return true;
    return false;
}

bool Node::isNodeConnectedToInput( Node* check ) {
    for( int i = 0; i < this->m_inSockets.length(); i++ ) {
        if( isNodeConnectedToInputSocket( i, check ) )
            return true;
    }
    return false;
}

bool Node::isNodeConnectedToInputSocket( int inputSocketIndex, Node* check ) {
    Socket* inputSocket = this->m_inSockets[inputSocketIndex];

    foreach( Connection* conn, inputSocket->connections() ) {
        if( conn->sourceSocket() ) {
            Node* connectedNode = qgraphicsitem_cast<Node*>( conn->sourceSocket()->parentItem() );

            if( connectedNode )
                if( connectedNode == check )
                    return true;
        }
    }

    return false;
}

bool Node::isNodeConnectedToOutput( Node* check ) {
    for( int i = 0; i < this->m_outSockets.length(); i++ ) {
        if( isNodeConnectedToOutputSocket( i, check ) )
            return true;
    }
    return false;
}

bool Node::isNodeConnectedToOutputSocket( int outputSocketIndex, Node* check ) {
    Socket* outputSocket = this->m_outSockets[outputSocketIndex];

    foreach( Connection* conn, outputSocket->connections() ) {
        if( conn->destinationSocket() ) {
            Node* connectedNode = qgraphicsitem_cast<Node*>( conn->destinationSocket()->parentItem() );

            if( connectedNode ) {
                if( connectedNode == check )
                    return true;
            }
        }
    }

    return false;
}

qreal Node::minimumWidth() const { return this->m_minWidth; }

QPen Node::outlinePen() const { return this->m_outlinePen; }

void Node::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) {
    Q_UNUSED( widget );
    Q_UNUSED( option );

    // Draws a drop shadow
    if( this->m_dropShadow ) {
        int shadowAlpha = ( 75 * this->m_backgroundBrush.color().alpha() ) / 255;
        painter->setPen( Qt::NoPen );
        painter->setBrush( QBrush( QColor( 0, 0, 0, shadowAlpha ) ) );
        // painter->drawRect(this->boundingRect());
        // QPainterPath path;
        // path.addRegion(this->boundingRegion(QTransform()));
        painter->drawRoundedRect( this->m_polygon.boundingRect().translated( 8, 8 ), this->m_cornerXRadius,
                                  this->cornerYRadius() );
        // painter->drawPath(path);
    }

    if( isSelected() ) {
        painter->setPen( this->m_selectedPen );
        painter->setBrush( this->m_selectedBrush );
    } else {
        painter->setPen( this->m_outlinePen );
        painter->setBrush( this->m_backgroundBrush );
    }

    // Draws the node on the screen.
    painter->drawPath( this->shape() );
}

QPixmap Node::pixmap() const { return this->m_pixmapItem.pixmap(); }

bool Node::pixmapEnabled() const {
    if( this->m_pixmapPosition != None )
        return this->m_pixmapItem.isVisible();
    else
        return false;
}

Node::PixmapPosition Node::pixmapPosition() const { return this->m_pixmapPosition; }

QSize Node::pixmapSize() const { return this->m_pixmapItem.pixmap().size(); }

QPolygonF Node::polygon() const { return m_polygon; }

void Node::redrawNode() { this->resizeNode(); }

QBrush Node::selectedBrush() const { return this->m_selectedBrush; }

QPen Node::selectedPen() const { return this->m_selectedPen; }

void Node::setBodyDetailCursor( const QTextCursor cursor ) {
    this->m_bodyDetailTextItem.setTextCursor( cursor );
    this->updateBoundingRect();
    this->redrawNode();
}

void Node::setBackgroundBrush( const QBrush& brush ) {
    this->m_backgroundBrush = brush;
    this->update();
}

void Node::setBodyDetailText( const QString& text ) {
    this->m_bodyDetailTextItem.setHtml( text );
    this->updateBoundingRect();
    this->redrawNode();
}

void Node::setBodyDetailTextColor( const QColor& color ) {
    this->m_bodyDetailTextItem.setDefaultTextColor( color );
    // this->update();
}

void Node::setBodyDetailTextEnabled( bool enabled ) {
    this->m_bodyDetailTextItem.setVisible( enabled );
    this->m_bodyDetailDivider.setVisible( enabled );
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setBodyDetailTextFont( const QFont& font ) {
    this->m_bodyDetailTextItem.setFont( font );
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setCornerXRadius( qreal radius ) {
    this->m_cornerXRadius = radius;
    this->updateBoundingRect();
    this->update();
}

void Node::setCornerYRadius( qreal radius ) {
    this->m_cornerYRadius = radius;
    this->updateBoundingRect();
    this->update();
}

void Node::setDetailText( const QString& text ) {
    this->m_detailTextItem.setHtml( text );
    this->updateBoundingRect();
    this->redrawNode();
}

void Node::setDetailTextColor( const QColor& color ) {
    this->m_detailTextItem.setDefaultTextColor( color );
    // this->update();
}

void Node::setDetailTextEnabled( bool enabled ) {
    this->m_detailTextItem.setVisible( enabled );
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setDetailTextFont( const QFont& font ) {
    this->m_detailTextItem.setFont( font );
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setDropShadow( bool enabled ) {
    this->m_dropShadow = enabled;
    // this->graphicsEffect()->setEnabled(enabled);
    prepareGeometryChange();
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setElidedTitle( Qt::TextElideMode mode ) {
    this->m_elidedTitle = mode;
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setInlineSockets( bool enabled ) { this->m_inlineSockets = enabled; }

void Node::setInlineTextWidthPercentage( qreal percentage ) { this->m_inlineInputSocketWidth = percentage; }

void Node::setInlineTextGapPercentage( qreal percentage ) { this->m_inlineSocketGap = percentage; }

void Node::setInputSocketName( int index, const QString& name ) {
    if( index >= 0 && index < this->m_inSockets.count() )
        this->m_inSockets[index]->setSocketName( name );
}

void Node::setMinimumWidth( qreal width ) {
    this->m_minWidth = width;
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setObjectName( const QString& name ) {
    QObject::setObjectName( name );
    emit requiresStyling( this );
}

void Node::setOutlinePen( const QPen& pen ) {
    this->m_outlinePen = pen;
    if( !this->isSelected() )
        this->m_titleBarItem.setPen( pen );
    this->m_bodyDetailDivider.setPen( pen );
    this->updateBoundingRect();
    this->update();
}

void Node::setOutputSocketName( int index, const QString& name ) {
    if( index >= 0 && index < this->m_outSockets.count() )
        this->m_outSockets[index]->setSocketName( name );
}

void Node::setPixmap( const QPixmap& pixmap ) {
    this->m_pixmapItem.setPixmap( pixmap );
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setPixmapEnabled( bool enabled ) {
    // if (this->m_pixmapPosition != None)
    this->m_pixmapItem.setVisible( enabled );

    if( enabled )
        this->m_pixmapPosition = Center;

    this->resizeNode();
}

void Node::setTitleJustify( TitleJustify justify ) {
    this->m_titleJustify = justify;
    this->resizeNode();
}

void Node::setPixmapPosition( PixmapPosition pos ) {
    this->m_pixmapPosition = pos;
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setPixmapSize( const QSize& size ) {
    this->m_pixmapItem.setPixmap( this->m_pixmapItem.pixmap().scaled( size ) );
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setSelectedBrush( const QBrush& brush ) {
    this->m_selectedBrush = brush;
    this->update();
}

void Node::setSelectedPen( const QPen& pen ) {
    this->m_selectedPen = pen;
    if( this->isSelected() )
        this->m_titleBarItem.setPen( pen );
    this->m_bodyDetailDivider.setPen( pen );
    this->updateBoundingRect();
    this->update();
}

void Node::setTitleBarBrush( const QBrush& brush ) {
    this->m_titleBarBrush = brush;
    if( !this->isSelected() )
        this->m_titleBarItem.setBrush( brush );
    this->update();
}

void Node::setTitleBarSelectedBrush( const QBrush& brush ) {
    this->m_titleBarSelectedBrush = brush;
    if( this->isSelected() )
        this->m_titleBarItem.setBrush( brush );
}

void Node::setTitleText( const QString& text ) {
    this->m_titleText = text;
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setTitleTextBrush( const QBrush& brush ) { this->m_titleTextItem.setBrush( brush ); }

void Node::setTitleTextFont( const QFont& font ) {
    this->m_titleTextItem.setFont( font );
    this->updateBoundingRect();
    this->resizeNode();
}

void Node::setTitleTextPen( const QPen& pen ) {
    this->m_titleTextItem.setPen( pen );
    this->updateBoundingRect();
    this->resizeNode();
}

QPainterPath Node::shape() const {
    QPainterPath path;
    path.addRoundedRect( this->m_polygon.boundingRect(), this->m_cornerXRadius, this->m_cornerYRadius );
    return path;
}

QRectF Node::titleBarBoundingRect() const { return this->m_titleBarItem.boundingRect(); }

QBrush Node::titleBarBrush() const { return this->m_titleBarBrush; }

QBrush Node::titleBarSelectedBrush() const { return this->m_titleBarSelectedBrush; }

Node::TitleJustify Node::titleJustify() const { return this->m_titleJustify; }

QString Node::titleText() const { return this->m_titleTextItem.text(); }

QBrush Node::titleTextBrush() const { return this->m_titleTextItem.brush(); }

QFont Node::titleTextFont() const { return this->m_titleTextItem.font(); }

QPen Node::titleTextPen() const { return this->m_titleTextItem.pen(); }

int Node::type() const { return Type; }

/***********************Protected Members********************/

void Node::focusInEvent( QFocusEvent* event ) {
    // setZValue(2);
    update();
    QGraphicsItem::focusInEvent( event );
}

void Node::focusOutEvent( QFocusEvent* event ) {
    // setZValue(-2);
    update();
    QGraphicsItem::focusOutEvent( event );
}

QVariant Node::itemChange( GraphicsItemChange change, const QVariant& value ) {
    switch( change ) {
    case ItemPositionHasChanged:
        this->updateConnections();
        emit itemUpdated();
        break;

    case ItemSelectedChange:
        if( value == true ) {
            Node::NextZNode++;
            if( Node::NextZNode == INT_MAX )
                Node::NextZNode = 0;
            setZValue( Node::NextZNode );
            this->m_titleBarItem.setPen( this->m_selectedPen );
            this->m_titleBarItem.setBrush( this->m_titleBarSelectedBrush );
            // this->m_bodyDetailDivider.setPen(QPen(QColor(255,0,0)));
            this->m_bodyDetailDivider.setPen( this->m_selectedPen );
        } else {
            this->m_titleBarItem.setPen( this->m_outlinePen );
            this->m_titleBarItem.setBrush( this->m_titleBarBrush );
            this->m_bodyDetailDivider.setPen( this->m_outlinePen );
            // this->m_bodyDetailDivider.setPen(QPen(QColor(255,0,0)));
        }
        break;

    default:
        break;
    }

    return QGraphicsItem::itemChange( change, value );
}

void Node::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event ) {
    emit doubleClicked( event );
    QGraphicsItem::mouseDoubleClickEvent( event );
}

void Node::mousePressEvent( QGraphicsSceneMouseEvent* event ) {
    update();
    QGraphicsItem::mousePressEvent( event );
}

void Node::mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
    update();

    foreach( Node* node, this->m_graphWidget->selectedNodes() ) {
        node->snapToGrid();
    }

    // snapToGrid();

    QGraphicsItem::mouseReleaseEvent( event );
}

void Node::snapToGrid() {
    // Causes the snapping in effect after the node is dropped.
    QList<QGraphicsView*> views = this->scene()->views();
    NodeView* magma = qobject_cast<NodeView*>( views.at( 0 ) );

    if( magma != 0 && magma->snapToGrid() ) {
        QPointF point = this->pos();
        point.setX( qRound( point.x() / (qreal)magma->gridSize() ) * (qreal)magma->gridSize() );
        point.setY( qRound( point.y() / (qreal)magma->gridSize() ) * (qreal)magma->gridSize() );
        this->setPos( point );
    }
}

/***********************Private Members**********************/

void Node::resizeNode() {
    if( this->m_disableRedraw )
        return;

    this->resizeTitleBar();

    QRectF boundingRect; // = this->m_polygon.boundingRect();
    QPointF inStartPoint;
    QPointF outStartPoint;

    boundingRect.setHeight( this->m_titleBarItem.rect().height() /*fm.height() + 10*/ );
    boundingRect.setWidth( this->m_titleBarItem.rect().width() );

    qreal height = boundingRect.height();
    if( this->isSelected() )
        this->m_bodyDetailDivider.setPen( this->m_selectedPen );
    else
        this->m_bodyDetailDivider.setPen( this->m_outlinePen );

    foreach( Socket* inSocket, this->m_inSockets ) {
        int inWidth = 0;
        QFontMetrics fm = QFontMetrics( inSocket->font() );
        int maxLength = this->m_titleBarItem.boundingRect().width() - inSocket->socketShapeSize() - 2 * fm.width( ' ' );
        if( this->m_inlineSockets ) {
            inWidth = int( ( maxLength * qMin( qMax( m_inlineInputSocketWidth, 0.0 ), 1.0 ) ) + 0.5 );
        } else
            inWidth = maxLength;
        inSocket->rebuildSocket( inWidth );
    }

    foreach( Socket* outSocket, this->m_outSockets ) {
        int outWidth = 0;
        QFontMetrics fm = QFontMetrics( outSocket->font() );
        int maxLength =
            this->m_titleBarItem.boundingRect().width() - outSocket->socketShapeSize() - 2 * fm.width( ' ' );
        if( this->m_inlineSockets ) {
            outWidth = maxLength - int( ( maxLength * m_inlineInputSocketWidth ) + 0.5 ) -
                       int( ( maxLength * m_inlineSocketGap ) + 0.5 );
            if( outWidth < 0 )
                outWidth = 0;
        } else
            outWidth = maxLength;

        outSocket->rebuildSocket( outWidth );
    }

    if( m_inlineSockets ) {
        qreal inHeight = 0;
        qreal outHeight = 0;
        foreach( Socket* outSocket, this->m_outSockets ) {
            outHeight += outSocket->boundingRect().height() + this->m_socketBuffer;
        }
        foreach( Socket* inSocket, this->m_inSockets ) {
            inHeight += inSocket->boundingRect().height() + this->m_socketBuffer;
        }
        height += qMax( inHeight, outHeight );
    } else {
        foreach( Socket* outSocket, this->m_outSockets ) {
            height += outSocket->boundingRect().height() + this->m_socketBuffer;
        }
        foreach( Socket* inSocket, this->m_inSockets ) {
            height += inSocket->boundingRect().height() + this->m_socketBuffer;
        }
    }

    if( this->m_bodyDetailTextItem.isVisible() ) {
        this->m_bodyDetailTextItem.setTextWidth( boundingRect.width() );
        this->m_bodyDetailTextItem.setPos( boundingRect.bottomLeft() );
        height += this->m_bodyDetailTextItem.boundingRect().height();
        boundingRect.setHeight( boundingRect.height() + this->m_bodyDetailTextItem.boundingRect().height() );
        // m_bodyDetailDivider.setPos(m_bodyDetailDivider.boundingRect().bottomLeft());
        this->m_bodyDetailDivider.setLine( QLineF( boundingRect.bottomLeft(), boundingRect.bottomRight() ) );
        boundingRect.setHeight( boundingRect.height() + this->m_bodyDetailDivider.pen().width() );
        height += this->m_bodyDetailDivider.pen().width();
    }

    inStartPoint = boundingRect.bottomLeft();
    outStartPoint = boundingRect.bottomRight();
    boundingRect.setHeight( height + 5 );

    // Create a new polygon based on the new size of the node
    QPolygonF newPolygon( boundingRect );
    this->prepareGeometryChange();
    this->m_polygon = newPolygon;

    // Set the starting points of the output sockets
    foreach( Socket* outSocket, this->m_outSockets ) {
        if( !m_inlineSockets )
            inStartPoint += QPointF( 0, outSocket->boundingRect().height() + this->m_socketBuffer );
        outStartPoint += QPointF( 0, outSocket->boundingRect().height() + this->m_socketBuffer );

        if( qgraphicsitem_cast<FullLineSocket*>( outSocket ) ) {
            outSocket->setPos( outStartPoint - QPoint( 0, outSocket->boundingRect().height() ) );
        } else {
            outSocket->setPos( outStartPoint -
                               QPoint( ( outSocket->socketShapeSize() / 2 ), outSocket->boundingRect().height() ) );
        }
    }
    // Set the starting points of the input sockets
    foreach( Socket* inSocket, this->m_inSockets ) {
        inStartPoint += QPointF( 0, inSocket->boundingRect().height() + this->m_socketBuffer );
        if( qgraphicsitem_cast<FullLineSocket*>( inSocket ) ) {
            inSocket->setPos( inStartPoint + QPoint( 0, -inSocket->boundingRect().height() ) );
        } else {
            inSocket->setPos( inStartPoint +
                              QPoint( -( inSocket->socketShapeSize() / 2 ), -inSocket->boundingRect().height() ) );
        }
    }
    this->updateBoundingRect();
    this->updateConnections();
    emit itemUpdated();
}

void Node::resizeTitleBar() {
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
            width = qMax( width, this->m_minWidth );
        } else
            width = this->m_minWidth;
        this->m_titleTextItem.setText( fm.elidedText( this->m_titleText, this->m_elidedTitle, width ) );
        this->m_detailTextItem.setTextWidth( width );
        height =
            fm.height() + ( this->m_detailTextItem.isVisible() ? this->m_detailTextItem.boundingRect().height() : 0 );
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
                                                      width - this->m_pixmapItem.boundingRect().width() ) );
        this->m_detailTextItem.setTextWidth( width - this->m_pixmapItem.boundingRect().width() /*textWidth*/ );

        height = qMax(
            this->m_pixmapItem.boundingRect().height(),
            fm.height() + ( this->m_detailTextItem.isVisible() ? this->m_detailTextItem.boundingRect().height() : 0 ) );
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

        height = fm.height() + ( this->m_pixmapItem.isVisible() ? this->m_pixmapItem.boundingRect().height() : 0 ) +
                 ( this->m_detailTextItem.isVisible() ? this->m_detailTextItem.boundingRect().height() : 0 );
        break;
    }

    titleBarRect.setHeight( height + ( adjust * 2 ) );
    titleBarRect.setWidth( width + ( adjust * 2 ) );

    this->m_titleTextItem.setToolTip( this->m_titleText );

    this->prepareGeometryChange();
    this->m_titleBarItem.setRect( titleBarRect );

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
        this->m_detailTextItem.setPos( titleBarRect.topLeft() + topLeftAdjust + QPointF( 0, fm.height() ) );
        break;

    case Left:
        this->m_pixmapItem.setPos( titleBarRect.topLeft() + topLeftAdjust );
        if( this->m_titleJustify == TitleCenter )
            this->m_titleTextItem.setPos(
                ( titleBarRect.center().x() - ( fm.width( this->m_titleTextItem.text() ) / 2 ) ) +
                    ( this->m_pixmapItem.boundingRect().width() / 2 ),
                adjust );
        else if( this->m_titleJustify == TitleLeft )
            this->m_titleTextItem.setPos( this->m_pixmapItem.boundingRect().right() + adjust * ( 3.0 / 2.0 ), adjust );
        else
            this->m_titleTextItem.setPos( titleBarRect.right() - adjust - m_titleTextItem.boundingRect().width(),
                                          adjust );
        this->m_detailTextItem.setPos( titleBarRect.topLeft().x() + adjust + this->m_pixmapItem.boundingRect().width(),
                                       fm.height() );
        break;

    case Right:

        this->m_detailTextItem.setPos( titleBarRect.topLeft() + topLeftAdjust + QPointF( 0, fm.height() ) );
        this->m_pixmapItem.setPos( titleBarRect.topRight() -
                                   QPointF( m_pixmapItem.boundingRect().width() + adjust / 2, 0 - adjust ) );
        if( this->m_titleJustify == TitleCenter )
            this->m_titleTextItem.setPos(
                ( titleBarRect.center().x() - ( fm.width( this->m_titleTextItem.text() ) / 2 ) ) -
                    ( this->m_pixmapItem.boundingRect().width() / 2 ),
                adjust );
        else if( this->m_titleJustify == TitleLeft )
            this->m_titleTextItem.setPos( titleBarRect.left() + adjust, adjust );
        else
            this->m_titleTextItem.setPos( titleBarRect.topRight().x() - ( fm.width( this->m_titleTextItem.text() ) ) -
                                              ( this->m_pixmapItem.boundingRect().width() ) - ( adjust ),
                                          adjust );
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
        this->m_detailTextItem.setPos( adjust, this->m_titleTextItem.pos().y() + fm.height() );
        break;

    case Center:
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
        this->m_detailTextItem.setPos( adjust,
                                       this->m_pixmapItem.pos().y() + this->m_pixmapItem.boundingRect().height() );
        break;

    case Bottom:
        if( this->m_titleJustify == TitleCenter )
            this->m_titleTextItem.setPos( titleBarRect.center().x() - ( fm.width( this->m_titleTextItem.text() ) / 2 ),
                                          adjust );
        else if( this->m_titleJustify == TitleLeft )
            this->m_titleTextItem.setPos( titleBarRect.left() + adjust, adjust );
        else
            this->m_titleTextItem.setPos( titleBarRect.right() - adjust - m_titleTextItem.boundingRect().width(),
                                          adjust );
        this->m_detailTextItem.setPos( titleBarRect.topLeft() + topLeftAdjust + QPointF( 0, fm.height() ) );
        this->m_pixmapItem.setPos(
            titleBarRect.center().x() - ( this->m_pixmapItem.boundingRect().width() / 2 ),
            this->m_detailTextItem.pos().y() +
                ( this->m_detailTextItem.isVisible() ? this->m_detailTextItem.boundingRect().height() : 0 ) );
        break;
    }
}

void Node::updateConnections() {
    foreach( Socket* out, this->m_outSockets ) {
        out->updateConnections();
    }

    foreach( Socket* in, this->m_inSockets ) {
        in->updateConnections();
    }
}

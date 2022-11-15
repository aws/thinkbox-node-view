// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/nodeview.h"
#include "NodeView/connection.h"
#include "NodeView/node.h"
#include "NodeView/nodegroup.h"
#include "NodeView/nodeviewminimap.h"
#include "NodeView/socket.h"

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

#include <math.h>

//#include <QtGui>
#include <QDebug>
#include <QGridLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QWidget>

/***********************Public Members***********************/

NodeView::NodeView( QWidget* parent )
    : QGraphicsView( parent )
    , m_backgroundMap( 10, 10 )
    , m_miniMap( NULL ) {
    // Setup memeber variables
    m_snapToGrid = true;
    m_gridLines = true;
    m_gridSize = 10;

    // Set up the basic properties for the QGraphicsView
    setCacheMode( CacheBackground );
    setViewportUpdateMode( SmartViewportUpdate );
    setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
    setTransformationAnchor( AnchorUnderMouse );
    setBackgroundBrush( QBrush( Qt::lightGray ) );
    setRubberBandSelectionMode( Qt::IntersectsItemShape );
    setDragMode( RubberBandDrag );
    setGeometry( QRect( 0, 0, 300, 300 ) );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    setSceneRect( -50000, -50000, 100000, 100000 );

    // Setup the QGrpahicsScene
    QGraphicsScene* scene = new QGraphicsScene( this );
    scene->setItemIndexMethod( QGraphicsScene::NoIndex );
    connect( scene, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
    setScene( scene );

    // Set the window title
    setWindowTitle( tr( "NodeView" ) );

    // Setup background
    setGridColor( QColor( Qt::gray ) );

    // Setup Minimap layout
    QGridLayout* layout = new QGridLayout( this );
    this->setLayout( layout );
    layout->setContentsMargins( 0, 0, 0, 0 );
    this->m_miniMapPosition = TopLeft;

    // Setup the creation point marker
    m_creationPoint = QPoint( 0, 0 );
    m_creationPointMarker.setZValue( INT_MIN );
    m_creationPointMarker.setRect( 0, 0, 5, 5 );
    m_creationPointMarker.setBrush( QBrush( Qt::black ) );
    this->scene()->addItem( &m_creationPointMarker );

    setCreationPointMarkerVisible( false );
    this->m_midClickMoved = false;
}

NodeView::~NodeView() {}

QColor NodeView::backgroundColor() const { return this->m_backgroundColor; }

QColor NodeView::creationPointColor() const { return this->m_creationPointMarker.brush().color(); }

void NodeView::deleteAllNodes() {
    foreach( Node* node, this->m_nodes ) {
        this->scene()->removeItem( node );
        this->m_nodes.removeAll( node );
        node->deletingSelf();
        node->deleteLater();
    }

    Connection::resetZDepth();
    Node::resetZDepth();
    // qDeleteAll(this->m_nodes);
    emit nodeDeleted();
}

void NodeView::deleteNode( Node* node ) {
    this->scene()->removeItem( node );
    this->m_nodes.removeAll( node );
    node->deletingSelf();
    node->deleteLater();
    emit nodeDeleted();
}

Node* NodeView::nodeByPosition( const QPointF& pos ) const {
    QGraphicsScene* scene = this->scene();
#ifdef Qt5
    QGraphicsItem* item = scene->itemAt( pos, transform() );
#else
    QGraphicsItem* item = scene->itemAt( pos );
#endif
    Node* node = qgraphicsitem_cast<Node*>( item );

    return node;
}

int NodeView::nodeCount() const { return this->m_nodes.count(); }

int NodeView::selectionCount() const { return this->scene()->selectedItems().count(); }

QColor NodeView::gridColor() const { return this->m_gridColor; }

bool NodeView::gridLines() const { return this->m_gridLines; }

int NodeView::gridSize() const { return this->m_gridSize; }

void NodeView::hideMiniMap() {
    if( this->m_miniMap != NULL )
        this->m_miniMap->hide();
}

bool NodeView::isCreationPointMarkerVisible() const { return this->m_creationPointMarker.isVisible(); }

QString NodeView::nodeViewStyleSheet() const { return this->m_styleManager.styleSheet(); }

NodeView::MinimapPosition NodeView::minimapPosition() const { return this->m_miniMapPosition; }

bool NodeView::minimapVisible() {
    if( this->m_miniMap == NULL )
        return false;
    return this->m_miniMap->isVisible();
}

void NodeView::moveMiniMap() {
    if( this->m_miniMap == NULL )
        return;

    QGridLayout* layout = qobject_cast<QGridLayout*>( this->layout() );
    QLayoutItem* item;

    // clear the old layout removing the spacer and minimap
    while( ( item = layout->takeAt( 0 ) ) != 0 ) {
        layout->removeItem( item );
    }
    // read the minimap and a new spacer and set the layout direction to be the correct direction
    switch( this->m_miniMapPosition ) {
    case TopLeft:
        layout->addWidget( this->m_miniMap, 0, 0 );
        layout->setRowStretch( 0, 0 );
        layout->setColumnStretch( 0, 0 );
        layout->setRowStretch( 1, 1 );
        layout->setColumnStretch( 1, 1 );
        break;
    case TopRight:
        layout->addWidget( this->m_miniMap, 0, 1 );
        layout->setRowStretch( 0, 0 );
        layout->setColumnStretch( 0, 1 );
        layout->setRowStretch( 1, 1 );
        layout->setColumnStretch( 1, 0 );
        break;
    case BottomLeft:
        layout->addWidget( this->m_miniMap, 1, 0 );
        layout->setRowStretch( 0, 1 );
        layout->setColumnStretch( 0, 0 );
        layout->setRowStretch( 1, 0 );
        layout->setColumnStretch( 1, 1 );
        break;
    case BottomRight:
        layout->addWidget( this->m_miniMap, 1, 1 );
        layout->setRowStretch( 0, 1 );
        layout->setColumnStretch( 0, 1 );
        layout->setRowStretch( 1, 0 );
        layout->setColumnStretch( 1, 0 );
        break;
    }

    this->m_miniMap->moveDragArea();
}

QList<Node*> NodeView::nodes() { return this->m_nodes; }

/*
QList<NodeGroup *> nodeGroups()
{
    return this->m_nodeGroups;
}
*/

void NodeView::pan( qreal dx, qreal dy ) { this->translate( dx, dy ); }

void NodeView::redrawView() { this->update(); }

QList<Connection*> NodeView::selectedConnections() const {
    QList<QGraphicsItem*> selectedItems = this->scene()->selectedItems();
    QList<Connection*> selectedConnections;
    Connection* conn;

    foreach( QGraphicsItem* item, selectedItems ) {
        conn = qgraphicsitem_cast<Connection*>( item );
        if( conn )
            selectedConnections.append( conn );
    }

    return selectedConnections;
}

QList<Node*> NodeView::selectedNodes() const {
    QList<QGraphicsItem*> selectedItems = this->scene()->selectedItems();
    QList<Node*> selectedNodes;
    Node* node;

    foreach( QGraphicsItem* item, selectedItems ) {
        node = qgraphicsitem_cast<Node*>( item );
        if( node )
            selectedNodes.append( node );
    }

    return selectedNodes;
}

QList<NodeGroup*> NodeView::selectedNodeGroups() const {
    QList<QGraphicsItem*> selectedItems = this->scene()->selectedItems();
    QList<NodeGroup*> selectedNodeGroups;
    NodeGroup* nodegroup;

    foreach( QGraphicsItem* item, selectedItems ) {
        nodegroup = qgraphicsitem_cast<NodeGroup*>( item );
        if( nodegroup )
            selectedNodeGroups.append( nodegroup );
    }

    return selectedNodeGroups;
}

void NodeView::setBackgroundColor( const QColor& color ) {
    this->m_backgroundColor = color;

    this->setBackgroundBrush( QBrush( color ) );
}

void NodeView::setCreationPointMarkerColor( const QColor& color ) {
    this->m_creationPointMarker.setBrush( QBrush( color ) );

    // this->m_creationPointMarker.setVisible(visible);
}

void NodeView::setCreationPointMarkerVisible( bool visible ) { this->m_creationPointMarker.setVisible( visible ); }

void NodeView::setGridColor( const QColor& color ) {
    this->m_gridColor = color;

    this->setCacheMode( CacheNone );
    this->invalidateScene();
    this->viewport()->update();
    this->setCacheMode( CacheBackground );
}

void NodeView::setGridLines( bool enabled ) {
    this->m_gridLines = enabled;

    this->setCacheMode( CacheNone );
    this->invalidateScene();
    this->viewport()->update();
    this->setCacheMode( CacheBackground );
}

void NodeView::setGridSize( int size ) {
    this->m_gridSize = size;

    if( this->m_snapToGrid ) {
        foreach( Node* node, this->m_nodes ) {
            node->snapToGrid();
        }
    }
    this->setCacheMode( CacheNone );
    this->invalidateScene();
    this->viewport()->update();
    this->setCacheMode( CacheBackground );
}

void NodeView::setNodeViewStyleSheet( const QString& styleSheet ) { this->m_styleManager.setStyleSheet( styleSheet ); }

void NodeView::setNodeViewStyleMap( const QVariantMap& styleMap ) { this->m_styleManager.setStyleMap( styleMap ); }

void NodeView::setMiniMapPosition( const MinimapPosition pos ) {
    this->m_miniMapPosition = pos;

    this->moveMiniMap();
}

void NodeView::setSnapToGrid( bool enable ) {
    this->m_snapToGrid = enable;
    if( enable ) {
        foreach( Node* node, this->m_nodes ) {
            node->snapToGrid();
        }
    }
}

void NodeView::setZoom( qreal zoomPercent ) {
    qreal scaleFactor = zoomPercent / this->transform().m11();
    this->scaleView( scaleFactor );
    emit zoomChanged();
}

void NodeView::showMiniMap() {
    if( this->m_miniMap == NULL )
        this->m_miniMap = new NodeViewMiniMap( this, this );

    this->m_miniMap->show();
    moveMiniMap();
}

bool NodeView::snapToGrid() const { return m_snapToGrid; }

void NodeView::zoom( qreal zoomFactor ) {
    this->scaleView( zoomFactor );
    emit zoomChanged();
}

void NodeView::zoomToSceneRect() {
    this->fitInView( this->sceneRect(), Qt::KeepAspectRatio );
    emit zoomChanged();
}

void NodeView::zoomToItemBoundingRect() {
    QRectF boundingRect;
    foreach( Node* node, this->m_nodes ) {
        boundingRect |= node->sceneBoundingRect();
    }

    this->fitInView( boundingRect, Qt::KeepAspectRatio );
    emit zoomChanged();
}

/***********************Public Slots*************************/

void NodeView::addNode( Node* node ) {
    node->setParent( this );

    this->scene()->addItem( node );
    this->m_nodes.append( node );
    node->setPos( this->m_creationPoint );

    emit nodeAdded( node );
}

Node* NodeView::createNode( int inSockets, int outSockets, const QString& nodeType ) {
    Node* newNode = new Node( this );

    newNode->beginModifyNode();

    for( int i = 0; i < inSockets; i++ )
        newNode->addInputSocket();

    for( int i = 0; i < outSockets; i++ )
        newNode->addOutputSocket();

    if( nodeType != "" )
        newNode->setObjectName( nodeType );

    newNode->endModifyNode();

    this->addNode( newNode );
    newNode->setPos( this->m_creationPoint );

    return newNode;
}

void NodeView::deleteSelectedNodes() {
    QList<QGraphicsItem*> selectedItems = this->scene()->selectedItems();
    this->scene()->clearSelection();

    foreach( QGraphicsItem* item, selectedItems ) {
        Node* node = qgraphicsitem_cast<Node*>( item );
        if( node != 0 ) {
            //			this->scene()->removeItem(node);
            //			this->m_nodes.removeAt(this->m_nodes.indexOf(node));
            //            node->deletingSelf();
            //            node->deleteLater();
            deleteNode( node );
        }
    }

    emit nodeDeleted();
}

/***********************Protected Slots**********************/

void NodeView::onConnectionMouseOver( Connection* conn ) { emit connectionMouseOver( conn ); }

/***********************Protected Members********************/

void NodeView::registerStyleableItem( Styleable* styleItem ) {
    this->m_styleManager.registerStyleableItem( styleItem );
}

/***********************Protected Members********************/

void NodeView::drawBackground( QPainter* painter, const QRectF& rect ) {
    QVector<QLineF> lines;
    QRectF sceneRect = this->mapToScene( this->rect() ).boundingRect();
    qreal adjustFactor = qMin( -3 / ( this->transform().m11() ), -0.5 );
    // qDebug()<<this->transform().m11()<<adjustFactor;
    sceneRect.adjust( adjustFactor, adjustFactor, -adjustFactor, -adjustFactor );

    painter->save();

    painter->setBrush( this->backgroundBrush() );
    painter->drawRect( sceneRect );
    if( this->m_gridLines && ( this->transform().m11() * this->m_gridSize ) >= 5 ) {
        int left = rect.x() - int( rect.x() ) % this->m_gridSize;
        int top = rect.y() - int( rect.y() ) % this->m_gridSize;

        for( int x = left; x < rect.right(); x += this->m_gridSize ) {
            lines << QLineF( x, rect.y(), x, rect.bottom() );
        }

        for( int y = top; y < rect.bottom(); y += this->m_gridSize ) {
            lines << QLineF( rect.x(), y, rect.right(), y );
        }

        painter->setPen( this->m_gridColor );
        painter->drawLines( lines );
    }

    painter->restore();
}

void NodeView::focusOutEvent( QFocusEvent* event ) {
    emit focusOut( event );
    QGraphicsView::focusOutEvent( event );
}

void NodeView::keyPressEvent( QKeyEvent* event ) {
    Connection* conn;

    switch( event->key() ) {
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        foreach( QGraphicsItem* item, this->scene()->selectedItems() ) {
            conn = qgraphicsitem_cast<Connection*>( item );
            if( conn != 0 )
                conn->deleteLater();
            // conn->sourceSocket()->removeConnection(conn);
        }

    default:
        QGraphicsView::keyPressEvent( event );
    }
}

void NodeView::mouseDoubleClickEvent( QMouseEvent* event ) {
    QGraphicsView::mouseDoubleClickEvent( event );
    emit doubleClick( event );
}

void NodeView::mouseMoveEvent( QMouseEvent* event ) {
    if( this->m_panStartPos != QPoint() ) {
        QPoint delta = event->pos() - this->m_panStartPos;
        this->horizontalScrollBar()->setValue( this->horizontalScrollBar()->value() - delta.x() );
        this->verticalScrollBar()->setValue( this->verticalScrollBar()->value() - delta.y() );
        this->m_panStartPos = event->pos();
        this->m_midClickMoved = true;
    } else {
        emit mouseMove( event );
        QGraphicsView::mouseMoveEvent( event );
    }
}

void NodeView::mousePressEvent( QMouseEvent* event ) {

    if( event->button() == Qt::MiddleButton ) {
        event->accept();
        this->m_panStartPos = event->pos();
        this->m_midClickMoved = false;
    } else if( event->button() == Qt::RightButton ) {
        event->accept();
#ifdef Qt5
    } else if( event->button() == Qt::LeftButton &&
               this->scene()->itemAt( this->mapToScene( event->pos() ), transform() ) == 0 ) {
#else
    } else if( event->button() == Qt::LeftButton && this->scene()->itemAt( this->mapToScene( event->pos() ) ) == 0 ) {
#endif

        // event->accept();
        this->m_creationPoint = this->mapToScene( event->pos() ).toPoint();
        this->m_creationPointMarker.setPos( this->m_creationPoint );
        QGraphicsView::mousePressEvent( event );
    } else
        QGraphicsView::mousePressEvent( event );

    emit mousePress( event );
}

void NodeView::mouseReleaseEvent( QMouseEvent* event ) {
    if( event->button() == Qt::MiddleButton ) {
        this->m_panStartPos = QPoint();
        if( !this->m_midClickMoved ) {
            this->zoomToItemBoundingRect();
        }
    }

    emit mouseRelease( event );
    QGraphicsView::mouseReleaseEvent( event );
}

void NodeView::scaleView( qreal scaleFactor ) {
    qreal factor = transform().scale( scaleFactor, scaleFactor ).mapRect( QRectF( 0, 0, 1, 1 ) ).width();

    // factor < 0.07 ||
    if( factor > 100 || factor < 0.001 )
        return;

    scale( scaleFactor, scaleFactor );

    m_creationPointMarker.setRect( 0, 0, ( 5.0 / transform().m11() ), ( 5.0 / transform().m11() ) );
    // m_creationPoint.transform().m11()

    emit zoomChanged();
}

void NodeView::wheelEvent( QWheelEvent* event ) {
    qreal scaleFactor = pow( 2.0, event->delta() / 480.0 );

    scaleView( scaleFactor );

    emit scrollWheel( event );
}

/***********************Private Slots************************/

void NodeView::onConnectionCreated() {
    Socket* sock = qobject_cast<Socket*>( this->sender() );
    emit connectionCreated( sock );
}

void NodeView::onConnectionDeleted() { emit connectionDeleted(); }

void NodeView::onConnectionStarted() { emit connectionStarted(); }

void NodeView::onConnectToEmpty( Socket* socket, QPointF pos ) { emit connectToEmpty( socket, pos ); }

void NodeView::onDisconnectToEmpty( Socket* socket, QPointF pos ) { emit disconnectToEmpty( socket, pos ); }

void NodeView::onSelectionChanged() {
    QList<QGraphicsItem*> selectedItems = this->scene()->selectedItems();

    if( selectedItems.count() == 1 ) {
        Connection* conn = qgraphicsitem_cast<Connection*>( selectedItems[0] );

        if( conn != 0 )
            emit connectionSelected( conn );
    }

    emit selectionChanged();
}

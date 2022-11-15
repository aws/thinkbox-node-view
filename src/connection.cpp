// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/connection.h"
#include "NodeView/node.h"
#include "NodeView/nodeview.h"
#include "NodeView/socket.h"

#include <limits.h>

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
#include <QtGui>
#include <math.h>

#ifdef Qt5
#include <QGraphicsSceneMouseEvent>
#endif
static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;
int Connection::NextZConnection = INT_MIN + 1;
/***********************Public Members***********************/

Connection::Connection( NodeView* graphWidget, QGraphicsItem* parent, QGraphicsScene* scene )
#ifdef Qt5
    : QGraphicsPathItem( parent )
    , m_source( 0 )
    , m_dest( 0 )
    , m_graphWidget( graphWidget ) {
    scene->addItem( this );
#else // Qt4
    : QGraphicsPathItem( parent, scene )
    , m_source( 0 )
    , m_dest( 0 )
    , m_graphWidget( graphWidget ) {
#endif

    setPen( QPen( Qt::darkGray, 3, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin ) );
    setBrush( QBrush( Qt::darkGray ) );

    Connection::NextZConnection++;
    if( Connection::NextZConnection == 0 )
        Connection::NextZConnection = INT_MIN + 1;
    setZValue( Connection::NextZConnection );

    setFlag( ItemIsSelectable );
    setAcceptHoverEvents( true );
    m_arrowSize = 15;
    m_arrowPosition = 0.5;
    m_curvature = 0.5;
    m_dragDistance = 20.0;

    setSelectedBrush( QBrush( Qt::white ) );
    setSelectedPen( QPen( Qt::white, 3, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin ) );

    m_animation = new QPropertyAnimation( this, "color", this );
    m_animationDuration = 250;
    m_animationLightness = 175;
    m_animationEnabled = true;

    m_graphWidget->registerStyleableItem( this );
}

Connection::~Connection() {
    // if (this->m_source != 0)
    // this->m_source->removeConnection(QSharedPointer<Connection>(this));
    // if (this->m_dest != 0)
    // this->m_dest->removeConnection(QSharedPointer<Connection>(this));
    // m_source->removeConnection(this);
    // m_dest->removeConnection(this);
    // qDebug() << "Connection Destroyed";
}

int Connection::animationDuration() const { return this->m_animationDuration; }

bool Connection::animationEnabled() const { return this->m_animationEnabled; }

int Connection::animationLightness() const { return this->m_animationLightness; }

qreal Connection::arrowPositionPercent() const { return this->m_arrowPosition; }

qreal Connection::arrowSize() const { return this->m_arrowSize; }

QColor Connection::color() const { return this->pen().color(); }

qreal Connection::curvature() const { return this->m_curvature; }

Socket* Connection::destinationSocket() const { return this->m_dest; }

qreal Connection::dragDistance() const { return this->m_dragDistance; }

void Connection::removeDestinationSocket( const QPointF& dragPoint ) {
    disconnect( this, SIGNAL( destroyed( QObject* ) ), this->m_dest, SLOT( removeConnection( QObject* ) ) );
    this->m_dest->removeConnection( this );
    this->m_dest = 0;
    this->setDragPoint( dragPoint );
    this->updatePosition();
}

void Connection::removeSourceSocket( const QPointF& dragPoint ) {
    disconnect( this, SIGNAL( destroyed( QObject* ) ), this->m_source, SLOT( removeConnection( QObject* ) ) );
    this->m_source->removeConnection( this );
    this->m_source = 0;
    this->setDragPoint( dragPoint );
    this->updatePosition();
}

QBrush Connection::selectedBrush() const { return this->m_selectedBrush; }

QColor Connection::selectedColor() const { return this->selectedPen().color(); }

QPen Connection::selectedPen() const { return this->m_selectedPen; }

void Connection::setAnimationDuration( int duration ) { this->m_animationDuration = duration; }

void Connection::setAnimationEnabled( bool enabled ) { this->m_animationEnabled = enabled; }

void Connection::setAnimationLightness( int lightness ) { this->m_animationLightness = lightness; }

void Connection::setArrowPositionPercent( const qreal position ) {
    if( position > 1 )
        this->m_arrowPosition = 1.0;
    else if( position < 0 )
        this->m_arrowPosition = 0.0;
    else
        this->m_arrowPosition = position;
    this->updatePosition();
}

void Connection::setArrowSize( const qreal size ) {
    this->m_arrowSize = size;
    this->updatePosition();
}

void Connection::setColor( const QColor& color ) {
    QBrush brush = this->brush();
    QPen pen = this->pen();

    brush.setColor( color );
    pen.setColor( color );

    this->setBrush( brush );
    this->setPen( pen );
}

void Connection::setCurvature( qreal curvature ) {
    this->m_curvature = curvature;
    this->updatePosition();
}

void Connection::setDestinationSocket( Socket* destination ) {
    this->m_dest = destination;
    connect( this, SIGNAL( destroyed( QObject* ) ), destination, SLOT( removeConnection( QObject* ) ) );
    destination->addConnection( this );
    this->updatePosition();
}

void Connection::setDragDistance( qreal dragDistance ) { this->m_dragDistance = dragDistance; }

void Connection::setDragPoint( const QPointF& dragPoint ) {
    this->m_dragPoint = dragPoint;
    this->updatePosition();
}

void Connection::setObjectName( const QString& name ) {
    QObject::setObjectName( name );
    emit requiresStyling( this );
}

void Connection::setSelectedBrush( const QBrush& brush ) {
    this->m_selectedBrush = brush;
    if( this->isSelected() )
        this->update();
}

void Connection::setSelectedColor( const QColor& color ) {
    QBrush brush = this->selectedBrush();
    QPen pen = this->selectedPen();

    brush.setColor( color );
    pen.setColor( color );

    this->setSelectedBrush( brush );
    this->setSelectedPen( pen );
}

void Connection::setSelectedPen( const QPen& pen ) {
    this->m_selectedPen = pen;
    if( this->isSelected() )
        this->update();
}

void Connection::setSourceSocket( Socket* source ) {
    this->m_source = source;
    connect( this, SIGNAL( destroyed( QObject* ) ), source, SLOT( removeConnection( QObject* ) ) );
    source->addConnection( this );
    this->updatePosition();
}

QPainterPath Connection::shape() const {
    QPainterPathStroker stroker;

    stroker.setWidth( this->pen().width() * 3 );
    return stroker.createStroke( this->path() );
}

Socket* Connection::sourceSocket() const { return m_source; }

int Connection::type() const { return Type; }

void Connection::updatePosition() {
    this->calculatePath();
    this->setPath( this->m_path );
    this->update( this->boundingRect() );
}

/***********************Protected Members********************/

void Connection::hoverEnterEvent( QGraphicsSceneHoverEvent* event ) {
    Q_UNUSED( event );

    if( this->m_animationEnabled ) {
        this->m_graphWidget->onConnectionMouseOver( this );

        if( this->m_animation->state() == QAbstractAnimation::Stopped ) {
            this->m_animation->setStartValue( this->color() );
            this->m_animation->setEndValue( this->color().lighter( this->m_animationLightness ) );
            this->m_animation->setDuration( this->m_animationDuration );
        } else if( this->m_animation->state() == QAbstractAnimation::Running )
            this->m_animation->pause();

        this->m_animation->setDirection( QPropertyAnimation::Forward );

        if( this->m_animation->state() == QAbstractAnimation::Paused )
            this->m_animation->resume();
        else
            this->m_animation->start();
    }
}

void Connection::hoverLeaveEvent( QGraphicsSceneHoverEvent* event ) {
    Q_UNUSED( event );

    if( this->m_animationEnabled ) {
        if( this->m_animation->state() == QAbstractAnimation::Running )
            this->m_animation->pause();

        this->m_animation->setDirection( QPropertyAnimation::Backward );

        if( this->m_animation->state() == QAbstractAnimation::Paused )
            this->m_animation->resume();
        else
            this->m_animation->start();
    }
}

QVariant Connection::itemChange( GraphicsItemChange change, const QVariant& value ) {
    switch( change ) {
    case ItemSelectedChange:
        if( value == true ) {
            Connection::NextZConnection++;
            if( Connection::NextZConnection == 0 )
                Connection::NextZConnection = INT_MIN + 1;
            setZValue( Connection::NextZConnection );
        }
        break;
    default:
        break;
    }

    return QGraphicsItem::itemChange( change, value );
}

void Connection::mouseMoveEvent( QGraphicsSceneMouseEvent* event ) {
    if( QLineF( event->screenPos(), event->buttonDownScreenPos( Qt::LeftButton ) ).length() < this->m_dragDistance &&
        ( this->m_source != 0 && this->m_dest != 0 ) )
        return;

    if( this->m_source != 0 && this->m_dest != 0 ) {
        qreal distanceToStart = QLineF( event->scenePos(), this->m_source->socketLocation() ).length();
        qreal distanceToEnd = QLineF( event->scenePos(), this->m_dest->socketLocation() ).length();

        foreach( Connection* con, m_graphWidget->selectedConnections() ) {
            if( con != this ) {
                con->setSelected( false );
            }
        }

        if( distanceToStart > distanceToEnd ) {
            this->removeDestinationSocket( event->scenePos() );
        } else {
            this->removeSourceSocket( event->scenePos() );
        }
    } else {
        this->setDragPoint( event->scenePos() );
    }

    QGraphicsPathItem::mouseMoveEvent( event );
}

void Connection::mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
    if( this->m_source == 0 || this->m_dest == 0 ) {
        Socket* connectedSocket = 0;

        if( this->m_source != 0 )
            connectedSocket = this->m_source;
        else if( this->m_dest != 0 )
            connectedSocket = this->m_dest;

        QList<QGraphicsItem*> items = this->scene()->collidingItems( this );
        bool found = false;
        foreach( QGraphicsItem* item, items ) {
            Socket* socket = qgraphicsitem_cast<Socket*>( item );

            if( connectedSocket && socket != 0 && socket != connectedSocket &&
                socket->isConnectionPointUnderMouse( event ) ) {
                connectedSocket->createConnection( socket );
                found = true;
                break;
            }
        }
        if( !found ) {

            this->m_graphWidget->onDisconnectToEmpty( connectedSocket, event->scenePos() );
        }
        this->deleteLater();
    }

    QGraphicsPathItem::mouseReleaseEvent( event );
}

void Connection::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) {
    Q_UNUSED( option );
    Q_UNUSED( widget );

    // Draw the outline of the path and the arrow head
    if( this->isSelected() ) {
        painter->setPen( this->m_selectedPen );
    } else {
        painter->setPen( this->pen() );
    }

    painter->drawPath( this->path() );

    // Fill in the arrow head
    if( this->path().toSubpathPolygons().count() > 1 ) {
        QPolygonF arrowHead = this->path().toSubpathPolygons()[1];

        if( this->isSelected() ) {
            painter->setBrush( this->m_selectedBrush );
        } else {
            painter->setBrush( this->brush() );
        }
        painter->drawPolygon( arrowHead );
    }
}

/***********************Private Members**********************/

void Connection::calculatePath() {
    QPointF endPoint;
    QPointF startPoint; /* = m_source->mapToScene(m_source->socketLocation());*/

    if( this->m_source != 0 )
        startPoint = this->m_source->socketLocation();
    /*this->m_source->mapToScene(this->m_source->socketLocation());*/ // this->mapFromItem(this->m_source->parentItem(),
                                                                      // this->m_source->socketLocation());
    // //this->m_source->mapToScene(this->m_source->socketLocation());
    else
        startPoint = this->m_dragPoint;

    if( this->m_dest != 0 )
        endPoint = this->m_dest->socketLocation(); // this->mapFromItem(this->m_dest, this->m_dest->socketLocation());
    else
        endPoint = this->m_dragPoint;

    if( endPoint == m_prevEnd && startPoint == m_prevStart && m_forceRecalc == false )
        return;

    m_prevEnd = endPoint;
    m_prevStart = startPoint;

    // QPointF startPoint = m_source->mapToScene(m_source->boundingRect().center());
    // QPainterPath *path = new QPainterPath(startPoint);
    this->m_path = QPainterPath( startPoint );
    this->m_path.setFillRule( Qt::WindingFill );

    // Draws the curved path between two nodes.
    QPointF sourceAdjust = startPoint;
    QPointF destAdjust = endPoint;

    if( m_curvature > 0 ) {
        qreal adjust = QLineF( startPoint, endPoint ).dx() * this->m_curvature;

        sourceAdjust.setX( startPoint.x() + adjust );
        destAdjust.setX( endPoint.x() - adjust );
        this->m_path.cubicTo( sourceAdjust, destAdjust, endPoint );
    } else {
        this->m_path.lineTo( endPoint );
    }

    // Draws the arrow in the center of the path.
    qreal pathLength = this->m_path.length();
    if( pathLength > this->m_arrowSize * 2 && this->m_arrowSize > 0 ) {
        qreal arrowLengthPercent = ( this->m_arrowSize / 2 ) / pathLength;
        QPolygonF arrowHead;
        QPointF arrowStart = this->m_path.pointAtPercent(
            this->m_arrowPosition + arrowLengthPercent > 1 ? 1 : this->m_arrowPosition + arrowLengthPercent );
        QPointF arrowEnd = this->m_path.pointAtPercent(
            this->m_arrowPosition - arrowLengthPercent < 0 ? 0 : this->m_arrowPosition - arrowLengthPercent );
        QLineF line( arrowStart, arrowEnd );

        double angle = ::acos( line.dx() / line.length() );
        if( line.dy() >= 0 )
            angle = TwoPi - angle;

        QPointF arrowP1 =
            line.p1() + QPointF( sin( angle + Pi / 3 ) * this->m_arrowSize, cos( angle + Pi / 3 ) * this->m_arrowSize );
        QPointF arrowP2 = line.p1() + QPointF( sin( angle + Pi - Pi / 3 ) * this->m_arrowSize,
                                               cos( angle + Pi - Pi / 3 ) * this->m_arrowSize );

        arrowHead << line.p1() << arrowP1 << arrowP2;
        this->m_path.moveTo( line.p1() );
        this->m_path.addPolygon( arrowHead );
        this->m_path.closeSubpath();
    }
}

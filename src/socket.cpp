// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/socket.h"
#include "NodeView/connection.h"
#include "NodeView/node.h"
#include "NodeView/nodeview.h"
#include "NodeView/trianglesocketshape.h"

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
#include <QtGui>

/***********************Public Members***********************/

Socket::Socket( NodeView* graphWidget, const SocketType type, const QString label, QGraphicsItem* parent )
    : QGraphicsItemGroup( parent )
    , m_graphWidget( graphWidget )
    , m_tempConnection( 0 ) {
    m_socketType = type;

    if( m_socketType != Invalid ) {

        // m_connIn = 0;
        m_socketShape = ( m_socketType == Input ) ? Square : Circle;
        m_multiConnections = ( m_socketType == Input ) ? false : true;
        m_label = new QGraphicsTextItem( label, this );
        // m_label->setFlag(ItemIsSelectable);
        // m_label->setFlag(ItemIsSelectable);
        QTextDocument* document = new QTextDocument();
        document->setDocumentMargin( 1 );

        m_label->setDocument( document );
        // m_label = new QGraphicsSimpleTextItem(label, this);
        m_labelText = label;
        setToolTip( label );
        setFlag( ItemIsMovable );
        m_socketShapeSize = 20;
        m_labelStyle = Elide;

        if( m_socketShape == Square )
            m_shape = new QGraphicsRectItem( this );
        else if( m_socketShape == Circle )
            m_shape = new QGraphicsEllipseItem( this );
        else if( m_socketShape == Triangle )
            m_shape = new TriangleSocketShape( this );

        m_shape->setZValue( 10 );

        m_animation = new QPropertyAnimation( this, "fillColor", this );
        m_animationDuration = 250;
        m_animationLightness = 175;
        m_animationEnabled = true;
        m_dragEnabled = true;

        setHandlesChildEvents( false );
        setAcceptedMouseButtons( Qt::LeftButton );
        setAcceptHoverEvents( true );
        setFillColor( QColor( Qt::cyan ) );
        setLabelColor( Qt::black );
        setOutlineColor( QColor( Qt::black ) );

        constructGroup();
        m_graphWidget->registerStyleableItem( this );
        m_verticalAlignment = Center;
    } else {
        m_label = 0;
        m_shape = 0;
        m_socketShape = None;
        m_socketShapeSize = 0;
        m_verticalAlignment = Center;
    }
}

Socket::~Socket() {
    if( this->m_shape != 0 )
        delete this->m_shape;
    if( this->m_label != 0 )
        delete this->m_label;

    this->clearConnections();
}

void Socket::addConnection( Connection* connection ) {
    if( this->isValid() ) {
        //		if (this->m_connections.count() != 0 && !this->m_multiConnections)
        //			this->clearConnections();

        //		if (this->m_socketType == Input)
        //			connection->setDestinationSocket(this);
        //		else if (this->m_socketType == Output)
        //			connection->setSourceSocket(this);

        this->m_connections.append( connection );
    }
}

int Socket::animationDuration() const { return this->m_animationDuration; }

bool Socket::animationEnabled() const { return this->m_animationEnabled; }

int Socket::animationLightness() const { return this->m_animationLightness; }

void Socket::clearConnections() {
    foreach( Connection* conn, this->m_connections ) {
        conn->deleteLater();
    }
    this->m_connections.clear();
}

QList<Connection*> Socket::connections() const {
    //	QList<Connection *> conns;

    //	foreach(QSharedPointer<Connection> conn, this->m_connections) {
    //		conns.append(conn.data());
    //	}

    return this->m_connections;
}

bool Socket::createConnection( Socket* socket ) {
    if( socket->isValid() && this->socketType() != socket->socketType() &&
        this->parentItem() != socket->parentItem() ) {
        if( this->m_connections.count() != 0 && !this->m_multiConnections )
            this->clearConnections();

        if( !socket->multipleConnections() )
            socket->clearConnections();

        Connection* connection = new Connection( this->m_graphWidget, 0, this->scene() );

        if( this->m_socketType == Input ) {
            connection->setDestinationSocket( this );
            connection->setSourceSocket( socket );
        } else if( this->m_socketType == Output ) {
            connection->setSourceSocket( this );
            connection->setDestinationSocket( socket );
        }

        this->updateConnections();
        emit connectionCreated();

        return true;
    }

    return false;
}

bool Socket::dragEnabled() { return m_dragEnabled; }

QColor Socket::fillColor() const { return this->m_fillColor; }

QFont Socket::font() const { return this->m_label->font(); }

bool Socket::isConnectionPointUnderMouse( QGraphicsSceneMouseEvent* event ) const {
    if( this->isValid() ) {
        if( this->m_shape != 0 ) {
            return this->m_shape->contains( this->m_shape->mapFromScene( event->scenePos() ) );
        }
    }

    return false;
}

bool Socket::isValid() const {
    if( this->m_socketType == Input || this->m_socketType == Output )
        return true;
    else
        return false;
}

QColor Socket::labelColor() const { return this->m_label->defaultTextColor(); }

Socket::LabelStyle Socket::labelStyle() const { return this->m_labelStyle; }

int Socket::maxTextLength() {
    QFontMetricsF fm( this->m_label->font() );
    return ( this->parentItem()->shape().boundingRect().width() - this->m_socketShapeSize - 2 * fm.width( ' ' ) );
}

bool Socket::multipleConnections() const { return this->m_multiConnections; }

QColor Socket::outlineColor() const { return this->m_outlineColor; }

QPointF Socket::position() const { return this->pos(); }

void Socket::removeConnection( QObject* conn ) {
    Connection* connection = (Connection*)conn;

    if( this->m_connections.contains( connection ) ) {
        this->m_connections.removeAll( connection );
        emit connectionDeleted();
    }
}

void Socket::setAnimationDuration( int duration ) { this->m_animationDuration = duration; }

void Socket::setAnimationEnabled( bool enabled ) { this->m_animationEnabled = enabled; }

void Socket::setAnimationLightness( int lightness ) { this->m_animationLightness = lightness; }

void Socket::setDragEnabled( bool enabled ) { m_dragEnabled = enabled; }

void Socket::setFillColor( const QColor& fillColor ) {
    this->m_fillColor = fillColor;

    if( this->m_animation->state() == QAbstractAnimation::Stopped ||
        this->m_animation->state() == QAbstractAnimation::Paused )
        this->m_animation->setStartValue( fillColor );

    this->m_shape->setBrush( QBrush( fillColor ) );
    this->update();
}

void Socket::setFont( const QFont& font ) {
    this->m_label->setFont( font );
    this->update();
}

void Socket::setLabelColor( const QColor& color ) {
    this->m_label->setDefaultTextColor( color );
    this->update();
}

void Socket::setLabelStyle( const LabelStyle style ) {
    this->m_labelStyle = style;
    this->update();
}

void Socket::setMultipleConnections( bool enabled ) {
    this->m_multiConnections = enabled;

    if( !enabled && this->m_connections.count() > 1 )
        this->clearConnections();
}

void Socket::setObjectName( const QString& name ) {
    QObject::setObjectName( name );
    emit requiresStyling( this );
}

void Socket::setOutlineColor( const QColor& outlineColor ) {
    this->m_outlineColor = outlineColor;
    this->m_shape->setPen( QPen( outlineColor ) );

    this->update();
}

void Socket::setSocketBrush( const QBrush& brush ) {
    this->m_shape->setBrush( brush );
    this->m_animation->setStartValue( brush.color() );
    this->update();
}

void Socket::setSocketName( const QString& name ) {
    this->m_label->setPlainText( name );
    this->m_labelText = name;
    this->setToolTip( name );
    this->update();
}

void Socket::setSocketPen( const QPen& pen ) {
    this->m_shape->setPen( pen );
    this->update();
}

void Socket::setSocketShape( const SocketShape shape ) {
    if( this->m_socketShape != shape ) {
        QAbstractGraphicsShapeItem* tempShape = 0;
        if( shape == Square )
            tempShape = new QGraphicsRectItem( this );
        else if( shape == Circle )
            tempShape = new QGraphicsEllipseItem( this );
        else if( shape == Triangle )
            tempShape = new TriangleSocketShape( this );

        tempShape->setBrush( this->m_shape->brush() );
        tempShape->setPen( this->m_shape->pen() );
        tempShape->setPos( this->m_shape->pos() );
        tempShape->setZValue( 10 );

        this->removeFromGroup( this->m_shape );
        delete this->m_shape;
        this->m_shape = tempShape;
        this->addToGroup( this->m_shape );

        this->m_socketShape = shape;
        this->update();
    }
}

void Socket::setSocketShapeSize( const qreal size ) {
    this->m_socketShapeSize = size;
    this->update();
}

void Socket::setTriangleOrientation( TriangleSocketShape::Orientation dir ) {
    TriangleSocketShape* triangleShape = 0;
    if( this->m_socketShape == Triangle )
        triangleShape = qgraphicsitem_cast<TriangleSocketShape*>( this->m_shape );
    triangleShape->setOrientation( dir );
}

void Socket::setVerticalAlignment( VerticalAlignment align ) {
    this->m_verticalAlignment = align;
    this->update();
}

QPainterPath Socket::shape() const {
    QPainterPath path;
    path += this->m_shape->shape();

    return path;
}

QBrush Socket::socketBrush() const { return this->m_shape->brush(); }

QPointF Socket::socketLocation() const {
    return this->mapToScene( this->m_shape->boundingRect().center() + this->mapFromItem( this->m_shape, 0, 0 ) );
}

QString Socket::socketName() const { return this->m_labelText; }

QPen Socket::socketPen() const { return this->m_shape->pen(); }

Socket::SocketShape Socket::socketShape() const { return this->m_socketShape; }

qreal Socket::socketShapeSize() const { return this->m_socketShapeSize; }

Socket::SocketType Socket::socketType() const { return m_socketType; }

TriangleSocketShape::Orientation Socket::triangleOrientation() {
    TriangleSocketShape* triangleShape = 0;
    if( this->m_socketShape == Triangle )
        triangleShape = qgraphicsitem_cast<TriangleSocketShape*>( this->m_shape );
    return triangleShape->orientation();
    return TriangleSocketShape::None;
}

int Socket::type() const { return Type; }

void Socket::update( const QRectF& rect ) {
    if( this->isValid() ) {
        Node* parentNode = qgraphicsitem_cast<Node*>( this->parentItem() );
        if( parentNode ) {
            parentNode->updateBoundingRect();
            parentNode->redrawNode();
        }
    }
    QGraphicsItemGroup::update( rect );
}

void Socket::updateConnections() {
    if( this->m_tempConnection != 0 )
        this->m_tempConnection->updatePosition();

    foreach( Connection* connection, this->m_connections ) {
        connection->updatePosition();
    }
}

Socket::VerticalAlignment Socket::verticalAlignment() { return this->m_verticalAlignment; }

/***********************Protected Members********************/

void Socket::hoverEnterEvent( QGraphicsSceneHoverEvent* event ) {
    Q_UNUSED( event );

    if( this->animationEnabled() ) {
        if( this->m_animation->state() == QAbstractAnimation::Stopped ) {
            this->m_animation->setStartValue( this->m_fillColor );
            this->m_animation->setEndValue( this->m_fillColor.lighter( this->m_animationLightness ) );
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

void Socket::hoverLeaveEvent( QGraphicsSceneHoverEvent* event ) {
    Q_UNUSED( event );

    if( this->animationEnabled() ) {
        if( this->m_animation->state() == QAbstractAnimation::Running )
            this->m_animation->pause();

        this->m_animation->setDirection( QPropertyAnimation::Backward );

        if( this->m_animation->state() == QAbstractAnimation::Paused )
            this->m_animation->resume();
        else
            this->m_animation->start();
    }
}

void Socket::mouseMoveEvent( QGraphicsSceneMouseEvent* event ) {

    if( this->isValid() && this->m_dragEnabled ) {
        event->accept();

        if( this->isConnectionPointUnderMouse( event ) && this->m_tempConnection == 0 ) {
            this->m_tempConnection = new Connection( this->m_graphWidget, 0, this->scene() );

            if( this->m_socketType == Output )
                this->m_tempConnection->setSourceSocket( this );
            else if( this->m_socketType == Input )
                this->m_tempConnection->setDestinationSocket( this );

            this->m_tempConnection->setDragPoint( event->scenePos() );

            emit connectionStarted();
        } else if( this->m_tempConnection != 0 ) {
            this->m_tempConnection->setDragPoint( event->scenePos() );
        }
    }
}

void Socket::mousePressEvent( QGraphicsSceneMouseEvent* event ) {
    if( this->isValid() && this->isConnectionPointUnderMouse( event ) )
        event->accept();
    else
        event->ignore();
    QGraphicsItemGroup::mousePressEvent( event );
    // this->setSelected(true);
}

void Socket::mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
    if( this->isValid() && this->m_dragEnabled ) {
        if( this->m_tempConnection != 0 ) {
            QList<QGraphicsItem*> items = this->scene()->collidingItems( this->m_tempConnection );

            foreach( QGraphicsItem* item, items ) {
                Socket* socket = qgraphicsitem_cast<Socket*>( item );

                if( socket != 0 ) {
                    if( socket->socketType() != this->m_socketType && socket->isConnectionPointUnderMouse( event ) ) {
                        this->m_tempConnection->deleteLater();
                        this->m_tempConnection = 0;
                        this->createConnection( socket );
                        return;
                    }
                }
            }

            this->m_tempConnection->deleteLater();
            this->m_tempConnection = 0;
            emit connectToEmpty( this, event->scenePos() );
        }
    }
}

void Socket::rebuildSocket( qreal width ) { this->resizeGroup( width ); }
/***********************Private Members**********************/

void Socket::resizeGroup( qreal width ) {
    this->m_label->setPlainText( this->m_labelText );
    this->m_label->setTextWidth( -1 );
    QRectF shapeRect;
    QRectF boundingRect = this->m_label->boundingRect();
    QGraphicsRectItem* squareShape = 0;
    QGraphicsEllipseItem* circleShape = 0;
    TriangleSocketShape* triangleShape = 0;
    QFontMetricsF fm( this->m_label->font() );

    this->removeFromGroup( this->m_shape );
    this->removeFromGroup( this->m_label );
    if( ( boundingRect.width() ) > width ) {
        boundingRect.setWidth( width );
        if( this->m_labelStyle == Elide ) {
            this->m_label->setPlainText( fm.elidedText( this->m_labelText, Qt::ElideRight, boundingRect.width() ) );
        } else {
            this->m_label->setTextWidth( boundingRect.width() );
        }
    }

    if( this->m_socketShape == Square )
        squareShape = qgraphicsitem_cast<QGraphicsRectItem*>( this->m_shape );
    else if( this->m_socketShape == Circle )
        circleShape = qgraphicsitem_cast<QGraphicsEllipseItem*>( this->m_shape );
    else if( this->m_socketShape == Triangle )
        triangleShape = qgraphicsitem_cast<TriangleSocketShape*>( this->m_shape );

    shapeRect.setRect( 0, 0, this->m_socketShapeSize, this->m_socketShapeSize );

    if( squareShape )
        squareShape->setRect( shapeRect );
    else if( circleShape )
        circleShape->setRect( shapeRect );
    else if( triangleShape )
        triangleShape->setRect( shapeRect );
    if( this->m_socketShapeSize > this->m_label->boundingRect().height() ) {
        this->m_shape->setPos( this->mapToParent( 0, 0 ) );
        if( this->m_socketType == Output ) {
            switch( this->m_verticalAlignment ) {
            case Top:
                this->m_label->setPos( this->m_shape->mapToParent( this->m_shape->boundingRect().topLeft().x() -
                                                                       fm.width( ' ' ) -
                                                                       this->m_label->boundingRect().width(),
                                                                   this->m_shape->boundingRect().top() ) );
                break;
            case Center:
                this->m_label->setPos( this->m_shape->mapToParent(
                    this->m_shape->boundingRect().topLeft().x() - fm.width( ' ' ) -
                        this->m_label->boundingRect().width(),
                    this->m_shape->boundingRect().center().y() - ( this->m_label->boundingRect().height() / 2 ) ) );
                break;
            case Bottom:
                this->m_label->setPos( this->m_shape->mapToParent(
                    this->m_shape->boundingRect().topLeft().x() - fm.width( ' ' ) -
                        this->m_label->boundingRect().width(),
                    this->m_shape->boundingRect().top() + this->m_shape->boundingRect().height() -
                        ( this->m_label->boundingRect().height() ) ) );
                break;
            }
        } else {
            switch( this->m_verticalAlignment ) {
            case Top:
                this->m_label->setPos(
                    this->m_shape->mapToParent( this->m_shape->boundingRect().topRight().x() + fm.width( ' ' ),
                                                this->m_shape->boundingRect().top() ) );
                break;
            case( Center ):
                this->m_label->setPos( this->m_shape->mapToParent(
                    this->m_shape->boundingRect().topRight().x() + fm.width( ' ' ),
                    this->m_shape->boundingRect().center().y() - ( this->m_label->boundingRect().height() / 2 ) ) );
                break;
            case Bottom:
                this->m_label->setPos( this->m_shape->mapToParent(
                    this->m_shape->boundingRect().topRight().x() + fm.width( ' ' ),
                    this->m_shape->boundingRect().top() + this->m_shape->boundingRect().height() -
                        ( this->m_label->boundingRect().height() ) ) );
                break;
            }
        }
    } else {
        switch( this->m_verticalAlignment ) {
        case Top:
            this->m_shape->setPos( this->mapToParent( 0, 0 ) );
            break;
        case Center:
            this->m_shape->setPos(
                this->mapToParent( 0, this->m_label->boundingRect().height() / 2 - this->m_socketShapeSize / 2 ) );
            break;
        case Bottom:
            this->m_shape->setPos(
                this->mapToParent( 0, this->m_label->boundingRect().height() - this->m_socketShapeSize ) );
            break;
        }

        if( this->m_socketType == Output ) {
            this->m_label->setPos( this->mapToParent( this->m_shape->boundingRect().topLeft().x() - fm.width( ' ' ) -
                                                          this->m_label->boundingRect().width(),
                                                      0 ) );
        } else {
            this->m_label->setPos(
                this->mapToParent( this->m_shape->boundingRect().topRight().x() + fm.width( ' ' ), 0 ) );
        }
    }

    this->addToGroup( this->m_shape );
    this->addToGroup( this->m_label );
}

void Socket::constructGroup() {
    this->m_label->setPlainText( this->m_labelText );
    QRectF shapeRect;
    QRectF boundingRect = this->m_label->boundingRect();
    QGraphicsRectItem* squareShape = 0;
    QGraphicsEllipseItem* circleShape = 0;
    TriangleSocketShape* triangleShape = 0;
    QFontMetricsF fm( this->m_label->font() );

    this->removeFromGroup( this->m_shape );
    this->removeFromGroup( this->m_label );

    if( ( boundingRect.width() + this->m_socketShapeSize ) > this->parentItem()->shape().boundingRect().width() ) {
        boundingRect.setWidth( this->parentItem()->shape().boundingRect().width() - this->m_socketShapeSize );
        if( this->m_labelStyle == Elide ) {
            this->m_label->setTextWidth( -1 );
            this->m_label->setPlainText( fm.elidedText( this->m_labelText, Qt::ElideRight, boundingRect.width() ) );
        } else {
            this->m_label->setTextWidth( boundingRect.width() );
        }
    }

    shapeRect.setRect( 0, 0, this->m_socketShapeSize, this->m_socketShapeSize );

    if( squareShape )
        squareShape->setRect( shapeRect );
    else if( circleShape )
        circleShape->setRect( shapeRect );
    else if( triangleShape )
        triangleShape->setRect( shapeRect );

    if( this->m_socketType == Output ) {

        if( this->m_socketShapeSize > this->m_label->boundingRect().height() ) {
            this->m_shape->setPos( this->mapToParent( 0, 0 ) );
            this->m_label->setPos( this->m_shape->mapToParent(
                this->m_shape->boundingRect().topLeft().x() - fm.width( ' ' ) - this->m_label->boundingRect().width(),
                this->m_shape->boundingRect().center().y() - ( this->m_label->boundingRect().height() / 2 ) ) );
        } else {
            this->m_shape->setPos(
                this->mapToParent( 0, this->m_label->boundingRect().height() / 2 - this->m_socketShapeSize / 2 ) );
            this->m_label->setPos( this->mapToParent( this->m_shape->boundingRect().topLeft().x() - fm.width( ' ' ) -
                                                          this->m_label->boundingRect().width(),
                                                      0 ) );
        }
    } else {

        if( this->m_socketShapeSize > this->m_label->boundingRect().height() ) {
            this->m_shape->setPos( this->mapToParent( 0, 0 ) );
            this->m_label->setPos( this->m_shape->mapToParent(
                this->m_shape->boundingRect().topRight().x() + fm.width( ' ' ),
                this->m_shape->boundingRect().center().y() - ( this->m_label->boundingRect().height() / 2 ) ) );
        } else {
            this->m_shape->setPos(
                this->mapToParent( 0, this->m_label->boundingRect().height() / 2 - this->m_socketShapeSize / 2 ) );
            this->m_label->setPos(
                this->mapToParent( this->m_shape->boundingRect().topRight().x() + fm.width( ' ' ), 0 ) );
        }
    }

    this->addToGroup( this->m_shape );
    this->addToGroup( this->m_label );
}

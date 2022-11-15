// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "NodeView/nodegroup.h"
#include "NodeView/connection.h"
#include "NodeView/fulllinesocket.h"
#include "NodeView/node.h"
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

/***********************Public Members***********************/

NodeGroup::NodeGroup( NodeView* graphWidget, QGraphicsItem* parent, QGraphicsScene* scene )
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
    // setCacheMode(DeviceCoordinateCache);

    setZValue( INT_MIN );
    m_outlinePen = QPen( Qt::black );
    m_outlinePen.setWidth( 3 );
    m_selectedPen = QPen( Qt::darkGray );
    m_selectedPen.setWidth( 3 );
    m_backgroundBrush = QBrush( QColor( 200, 255, 200 ) );
    m_selectedBrush = m_backgroundBrush;
    m_titleBarBrush = m_backgroundBrush;
    m_titleBarBrush.setColor( m_titleBarBrush.color().darker( 125 ) );
    m_titleBarSelectedBrush = m_titleBarBrush;

    m_topCorner = QPointF( 0, 0 );
    m_mainRect = QRectF( 0, 0, 100, 100 );
    m_curPos = this->pos();

    QFont titleFont;
    // titleFont.setFamily("Arial");
    m_titleTextItem.setParentItem( this );
    m_titleTextItem.setFont( titleFont );
    m_titleText = "Title";

    m_titleJustify = TitleCenter;
    m_elidedTitle = Qt::ElideNone;

    this->updateBoundingRect();
}

NodeGroup::~NodeGroup() {}

void NodeGroup::addNode( Node* node ) {
    this->m_containedNodes.append( node );
    this->updateGroup();
    QObject::connect( node, SIGNAL( deletingNode( Node* ) ), this, SLOT( removeNode( Node* ) ) );
    QObject::connect( node, SIGNAL( itemUpdated() ), this, SLOT( updateGroup() ) );
}

QBrush NodeGroup::backgroundBrush() const { return this->m_backgroundBrush; }

QList<Node*> NodeGroup::containedNodes() { return this->m_containedNodes; }

Qt::TextElideMode NodeGroup::elidedTitle() { return this->m_elidedTitle; }

void NodeGroup::updateBoundingRect() {
    QFontMetrics* fm = new QFontMetrics( m_titleTextItem.font() );
    int height = fm->height();
    if( this->m_mainRect.width() > 0 ) {
        QRectF boundingRect = this->m_mainRect;
        boundingRect.setY( boundingRect.y() - height - 6 );
        m_boundingRect = boundingRect;
    } else {
        QRectF boundingRect = QRectF( this->m_topCorner.x(), this->m_topCorner.y() - 25, 50, 50 + height + 6 );
        m_boundingRect = boundingRect;
    }
}

QRectF NodeGroup::boundingRect() const { return m_boundingRect; }

QPen NodeGroup::outlinePen() const { return this->m_outlinePen; }

void NodeGroup::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) {
    Q_UNUSED( widget );
    Q_UNUSED( option );

    QFontMetrics* fm = new QFontMetrics( m_titleTextItem.font() );

    int width = 0;
    QString elidedTitle;
    if( this->m_mainRect.width() > 0 ) {
        elidedTitle = fm->elidedText( m_titleText, m_elidedTitle, m_mainRect.width() - 6 );
    } else {
        elidedTitle = fm->elidedText( m_titleText, m_elidedTitle, 50 );
    }
    m_titleTextItem.setText( elidedTitle );
    width = fm->width( elidedTitle );
    int left = 3;
    if( m_titleJustify == TitleCenter )
        left = this->shape().boundingRect().width() / 2 + 3 - width / 2;
    else if( m_titleJustify == TitleRight )
        left = this->shape().boundingRect().width() - width - 3;
    m_titleTextItem.setPos( this->shape().boundingRect().left() + left, this->shape().boundingRect().top() + 3 );

    if( isSelected() ) {
        painter->setPen( this->m_selectedPen );
        painter->setBrush( this->m_titleBarSelectedBrush );
    } else {
        painter->setPen( this->m_outlinePen );
        painter->setBrush( this->m_titleBarBrush );
    }
    // Draws the node on the screen.
    painter->drawPath( this->shape() );

    if( isSelected() ) {
        painter->setPen( this->m_selectedPen );
        painter->setBrush( this->m_selectedBrush );
    } else {
        painter->setPen( this->m_outlinePen );
        painter->setBrush( this->m_backgroundBrush );
    }

    QPainterPath path = QPainterPath();
    if( this->m_mainRect.width() > 0 )
        path.addRect( this->m_mainRect.x(), this->shape().boundingRect().bottom(), this->m_mainRect.width(),
                      this->m_mainRect.height() );
    else
        path.addRect( this->m_topCorner.x(), this->m_topCorner.y(), 50, 50 );
    // Draws the node on the screen.
    painter->drawPath( path );
}

QBrush NodeGroup::selectedBrush() const { return this->m_selectedBrush; }

QPen NodeGroup::selectedPen() const { return this->m_selectedPen; }

void NodeGroup::setBackgroundBrush( const QBrush& brush ) {
    this->m_backgroundBrush = brush;
    this->update();
}

void NodeGroup::setElidedTitle( Qt::TextElideMode mode ) { this->m_elidedTitle = mode; }

void NodeGroup::setOutlinePen( const QPen& pen ) {
    this->m_outlinePen = pen;
    this->update();
}

void NodeGroup::setSelectedBrush( const QBrush& brush ) {
    this->m_selectedBrush = brush;
    this->update();
}

void NodeGroup::setSelectedPen( const QPen& pen ) {
    this->m_selectedPen = pen;
    this->update();
}

void NodeGroup::setTitleBarBrush( const QBrush& brush ) {
    this->m_titleBarBrush = brush;
    this->update();
}

void NodeGroup::setTitleBarSelectedBrush( const QBrush& brush ) { this->m_titleBarSelectedBrush = brush; }

void NodeGroup::setTitleJustify( TitleJustify justify ) { this->m_titleJustify = justify; }

void NodeGroup::setTitleText( const QString& text ) {
    this->m_titleText = text;
    this->updateGroup();
}

void NodeGroup::setTitleTextBrush( const QBrush& brush ) {
    this->m_titleTextItem.setBrush( brush );
    this->update();
}

void NodeGroup::setTitleTextFont( const QFont& font ) {
    this->m_titleTextItem.setFont( font );
    this->updateGroup();
}

void NodeGroup::setTitleTextPen( const QPen& pen ) {
    this->m_titleTextItem.setPen( pen );
    this->updateGroup();
}

QPainterPath NodeGroup::shape() const {
    QPainterPath path;
    QFontMetrics fm = QFontMetrics( this->m_titleTextItem.font() );

    if( this->m_mainRect.width() > 0 )
        path.addRect( this->m_mainRect.x(), this->m_mainRect.y() - ( fm.height() + 6 ), this->m_mainRect.width(),
                      fm.height() + 6 );
    else
        path.addRect( this->m_topCorner.x(), this->m_topCorner.y() - ( fm.height() + 6 ), 50, fm.height() + 6 );
    return path;
}

QBrush NodeGroup::titleBarBrush() const { return this->m_titleBarBrush; }

QBrush NodeGroup::titleBarSelectedBrush() const { return this->m_titleBarSelectedBrush; }

NodeGroup::TitleJustify NodeGroup::titleJustify() const { return this->m_titleJustify; }

QString NodeGroup::titleText() const { return this->m_titleText; }

QBrush NodeGroup::titleTextBrush() const { return this->m_titleTextItem.brush(); }

QFont NodeGroup::titleTextFont() const { return this->m_titleTextItem.font(); }

int NodeGroup::type() const { return Type; }

void NodeGroup::updateArea() {
    this->prepareGeometryChange();
    QRectF workingRect;
    foreach( Node* contained, this->m_containedNodes ) {
        if( workingRect.isEmpty() ) {
            workingRect = this->mapFromItem( contained, contained->boundingRect() ).boundingRect();
        } else {
            workingRect =
                workingRect.united( this->mapFromItem( contained, contained->boundingRect() ).boundingRect() );
        }
    }
    if( workingRect.width() > 0 )
        this->m_topCorner = workingRect.topLeft();

    if( m_elidedTitle == Qt::ElideNone ) {
        QFontMetrics* fm = new QFontMetrics( m_titleTextItem.font() );
        int textWidth = fm->width( m_titleText );
        int rectWidth = workingRect.width();
        if( textWidth > rectWidth ) {
            int widthDiff = textWidth - rectWidth;
            int mod = int( ( widthDiff / 2.0 ) + 0.5 );
            workingRect.adjust( -mod, 0, mod, 0 );
        }
    }
    workingRect.adjust( -3, -3, 3, 3 );
    this->m_mainRect = workingRect;
}

void NodeGroup::mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
    update();

    foreach( Node* node, this->m_containedNodes ) {
        node->snapToGrid();
    }

    QGraphicsItem::mouseReleaseEvent( event );
}

QVariant NodeGroup::itemChange( GraphicsItemChange change, const QVariant& value ) {
    QPointF movement;
    switch( change ) {
    case ItemPositionHasChanged:
        movement = value.toPointF() - this->m_curPos;
        foreach( Node* inner, this->m_containedNodes ) {
            inner->setPos( inner->pos() + movement );
        }
        this->m_curPos = value.toPointF();
        break;
    default:
        break;
    }

    return QGraphicsItem::itemChange( change, value );
}

void NodeGroup::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event ) {
    emit doubleClicked( event );
    QGraphicsItem::mouseDoubleClickEvent( event );
}

void NodeGroup::updateGroup() {
    this->updateArea();
    this->updateBoundingRect();
    this->update();
}

void NodeGroup::removeNode( Node* node ) {
    this->m_containedNodes.removeAll( node );
    this->updateGroup();
}

// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/NodeView_global.h"
#include "NodeView/stylemanager.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsPolygonItem>
#include <QList>
#include <QPen>
#include <QRectF>

#define BASE_RECT QRectF( -10, -10, 153, 33 )

class Node;
class Connection;
class Socket;
class NodeView;

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

/*!
 * \brief A subclass of QGraphicsPolygonItem that is used
 * to represent a node within a NodeView view.
 */
class NodeView_API NodeGroup : public QObject, public QGraphicsPolygonItem {
    Q_OBJECT
  public:
    enum { Type = UserType + 32 };

    enum TitleJustify { TitleLeft, TitleRight, TitleCenter };

  protected:
    NodeView* m_graphWidget;

    QPen m_outlinePen;
    QPen m_selectedPen;
    QBrush m_backgroundBrush;
    QBrush m_selectedBrush;
    QBrush m_titleBarBrush;
    QBrush m_titleBarSelectedBrush;
    QRectF m_mainRect;

    QPointF m_curPos;
    QPointF m_topCorner;
    QGraphicsSimpleTextItem m_titleTextItem;
    QString m_titleText;
    Qt::TextElideMode m_elidedTitle;
    TitleJustify m_titleJustify;

  protected:
    QList<Node*> m_containedNodes;

    QRectF m_boundingRect;

    static int NextZNode;

  public:
    /*!
     * \brief Constructor
     * \param graphWidget The NodeView class that the Node will exist whthin.
     * \param parent The QGraphicsItem parent item of the Node.
     * \param scene The QGraphicsScene that the Node will be
     * displayed within.
     */
    NodeGroup( NodeView* graphWidget, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0 );

    void addNode( Node* node );

    /*!
     * \brief Deconstructor
     */
    virtual ~NodeGroup();

    QBrush backgroundBrush() const;

    /*!
     * \brief Gets the bounding rectangle of the Node
     * \return The bounding rectangle of the Node as a QRectF.
     */
    virtual QRectF boundingRect() const;

    QList<Node*> containedNodes();

    Qt::TextElideMode elidedTitle();

    QPen outlinePen() const;

    void updateBoundingRect();

    QBrush selectedBrush() const;

    QPen selectedPen() const;

    void setBackgroundBrush( const QBrush& brush );

    void setElidedTitle( Qt::TextElideMode mode );

    void setOutlinePen( const QPen& pen );

    void setSelectedBrush( const QBrush& brush );

    void setSelectedPen( const QPen& pen );

    void setTitleBarBrush( const QBrush& brush );

    void setTitleBarSelectedBrush( const QBrush& brush );

    void setTitleJustify( TitleJustify justify );

    void setTitleTextBrush( const QBrush& brush );

    void setTitleTextFont( const QFont& font );

    void setTitleTextPen( const QPen& pen );

    void setTitleText( const QString& text );

    /*!
     * \brief This is the shape of the Node as described by a [QPainterPath](http:
     * //qt-project.org/doc/qt-4.8/qpainterpath.html) object.
     * \return The QPainterPath that contains the shape of the
     * Node.
     */
    QPainterPath shape() const;

    QBrush titleBarBrush() const;

    QBrush titleBarSelectedBrush() const;

    TitleJustify titleJustify() const;

    QString titleText() const;

    QBrush titleTextBrush() const;

    QFont titleTextFont() const;

    /*!
     * \brief This returns the user defined type of a Node
     * QGraphicsItem. All
     * QGrpahicsItems need to have a type in order to properly
     * function within a QGraphicsScene.
     * \return The Node object type identifier.
     */
    virtual int type() const;

    void updateArea();

    /*!
     * \brief This function performs all the drawing aspects of the Node. Reimplement this function to change the
     * appearence of the Node.
     * \param painter The QPainter object that will perform the
     * drawing. \param option \param widget
     */
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

  protected:
    QVariant itemChange( GraphicsItemChange change, const QVariant& value );

    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );

    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event );
  signals:
    void doubleClicked( QGraphicsSceneMouseEvent* event );
  public slots:

    void updateGroup();
    void removeNode( Node* node );
};

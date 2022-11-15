// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/NodeView_global.h"
#include "NodeView/stylemanager.h"

#include <QBrush>
#include <QGraphicsPathItem>
#include <QPen>

class Socket;
class NodeView;

class QPainterPath;
class QPropertyAnimation;

/*!
 * \brief The Connection class is used to join two [Nodes](\ref Node) together within the NodeView view. To change the
 * color and thickness of the Connection, as well as other properties you need to use the function setPen(QPen).
 */
class NodeView_API Connection : public QObject, public QGraphicsPathItem, public Styleable {
    Q_OBJECT

    Q_PROPERTY( int animationDuration READ animationDuration WRITE setAnimationDuration DESIGNABLE true )
    Q_PROPERTY( int animationLightness READ animationLightness WRITE setAnimationLightness DESIGNABLE true )
    Q_PROPERTY( QBrush brush READ brush WRITE setBrush DESIGNABLE true )
    Q_PROPERTY( QColor color READ color WRITE setColor DESIGNABLE true )
    Q_PROPERTY( QPen pen READ pen WRITE setPen DESIGNABLE true )
    Q_PROPERTY( QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush DESIGNABLE true )
    Q_PROPERTY( QPen selectedPen READ selectedPen WRITE setSelectedPen DESIGNABLE true )
    Q_PROPERTY( qreal arrowPositionPercent READ arrowPositionPercent WRITE setArrowPositionPercent DESIGNABLE true )
    Q_PROPERTY( qreal arrowSize READ arrowSize WRITE setArrowSize DESIGNABLE true )
    Q_PROPERTY( qreal curvature READ curvature WRITE setCurvature DESIGNABLE true )

  private:
    /*!
     * \brief Calculates the path that the Connection will take. This is used by the drawing functions.
     * \return
     */
    void calculatePath();

    Socket* m_source; /*!< The source Socket. */
    Socket* m_dest;   /*!< The destination Socket. */
    NodeView* m_graphWidget;

    QPropertyAnimation* m_animation;

    QPointF m_dragPoint; /*!< The dragging point of the Connection. */
    QPainterPath m_path; /*!< The path object that describes the Connection shape. */
    QPen m_selectedPen;
    QBrush m_selectedBrush;

    qreal m_arrowPosition;
    qreal m_arrowSize;
    qreal m_curvature;
    qreal m_dragDistance;

    bool m_animationEnabled;
    int m_animationDuration;
    int m_animationLightness;

    QPointF m_prevStart;
    QPointF m_prevEnd;
    bool m_forceRecalc;

    static int NextZConnection;

  public:
    /*!
     * \brief Definition of type ID for Conneciton objects. All QGrpahicsItems need a type ID.
     */
    enum { Type = UserType + 3 };

    /*!
     * \brief Constructor
     * \param parent The parent QGraphicsItem.
     * \param scene The QGraphicsScene that the Connection resides in.
     */
    Connection( NodeView* graphWidget, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0 );

    /*!
     * \brief Deconstructor
     */
    virtual ~Connection();

    /*!
     * \brief Returns the amount of time in miliseconds that the animation will run for.
     * \return Int that is the number of miliseconds the animation will run for.
     */
    int animationDuration() const;

    /*!
     * \brief Returns true if animation is currently enabled.
     * \return Bool which states if animation is enabled for
     */
    bool animationEnabled() const;

    /*!
     * \brief Returns the level that the animation will lighten the color of the Connection.
     * \return Int the is the level of lightness the color will change too. Default is 175.
     */
    int animationLightness() const;

    /*!
     * \brief Returns the point in the Connection line that the arrow should appear. This will be a value between 0
     * and 1. \return The qreal that contains the location of the arrow.
     */
    qreal arrowPositionPercent() const;

    /*!
     * \brief Returns the size of the arrow.
     * \return The qreal that represents the current size of the arrow.
     */
    qreal arrowSize() const;

    /*!
     * \brief Returns the color of the Connection.
     * \return QColor that the Connection is set too.
     */
    QColor color() const;

    /*!
     * \brief Returns the currently set cruvature for the Connection. A value of 0 means that the line has no curvature
     * (a straight line); Anything else and there will be a curve in the line. \return the qreal that represents the
     * current level of curvature in the Connection.
     */
    qreal curvature() const;

    /*!
     * \brief Returns the currently set drag distance. The drag distance is how far away the mouse has to move from the
     * Connection before a drag event is fired.
     * \return The currently set drag distance as a qreal.
     */
    qreal dragDistance() const;

    /*!
     * \brief The destination Socket of the Connection.
     * \return The Socket object that is the destination of the Connection.
     */
    Socket* destinationSocket() const;

    /*!
     * \brief Removes the destination Socket from this Connection and sets its drag point for drawing. This is used when
     * pulling a currently connected Connection off of a Socket.
     * \param dragPoint QPointF that you want the drag point to be for drawing.
     */
    void removeDestinationSocket( const QPointF& dragPoint );

    /*!
     * \brief Removes the source Socket from this Connection and sets its drag point for drawing. This is used when
     * pulling a currently connected Connection off of a Socket.
     * \param dragPoint QPointF that you want the drag point to be for drawing.
     */
    void removeSourceSocket( const QPointF& dragPoint );

    static void resetZDepth() { NextZConnection = INT_MIN; }

    /*!
     * \brief Returns the brush that will be used to draw the arrow when the Connection is selected.
     * \return The QBrush that will be returned.
     */
    QBrush selectedBrush() const;

    /*!
     * \brief Returns the color of the Connection when selected.
     * \return QColor that the Connection is set too.
     */
    QColor selectedColor() const;

    /*!
     * \brief Returns the pen that will be used to draw the Connection when it is selected.
     * \return The QPen that will be returned.
     */
    QPen selectedPen() const;

    /*!
     * \brief Sets the animation duration.
     * \param duration int in miliseconds that you want the animation duration to be.
     */
    void setAnimationDuration( int duration );

    /*!
     * \brief Sets if the animation is to be run
     * \param bool reperesenting if you want the animation to run
     * \note may give incorrect results if changed while animation is running
     */
    void setAnimationEnabled( bool enabled );

    /*!
     * \brief Sets the animtion lightness. A value of 100 will keep it the same. A valye greater than 100 will cause the
     * color to become lighter. A value less than 100 but greater thn 0 will make the color darker. Values less than 0
     * will cause undefined results.
     * \param lightness int that will change the color of the animation.
     */
    void setAnimationLightness( int lightness );

    /*!
     * \brief Sets the arrow position to be the position passed in. This should be a value between 0 and 1.
     * \param position The qreal percentage that you want the arrow to be positioned at.
     */
    void setArrowPositionPercent( const qreal position );

    /*!
     * \brief Sets the size of the arrow that will appear on the Connection line.
     * \param size The qreal that you want each side of the arrow to be.
     */
    void setArrowSize( const qreal size );

    /*!
     * \brief Sets the color for the Connection.`
     * \param color QColor that you want the connection to be.
     */
    void setColor( const QColor& color );

    /*!
     * \brief Sets the curvature of the Connection line.
     * \param curvature The qreal that you want the curvature to be. 0 will produce a straight line. Values between 0
     * and 1 will produce nice soft curves. Values greater than 1 will cause the line to double back and form a "z" like
     * shape.
     */
    void setCurvature( qreal curvature );

    /*!
     * \brief Sets the destination Socket of the Connection.
     * \param destination The Socket that will be the desination of the Connection.
     */
    void setDestinationSocket( Socket* destination );

    /*!
     * \brief Sets the drag distance for this Connection.
     * \param dragDistance int distacne that you want to be exceeded before the Connection will be detached from a
     * Socket.
     */
    void setDragDistance( qreal dragDistance );

    /*!
     * \brief Sets the point outside of a Socket that will draw the Connection too while it is being dragged by the
     * mouse. \param dragPoint The QPointF that contains the coordinate to draw the Connection to.
     */
    void setDragPoint( const QPointF& dragPoint );

    /*!
     * \brief Overloaded from QObject. Sets the object name for this Connection object and infroms the style manager
     * that it requires styling. \param name The name that you want the object to have.
     */
    void setObjectName( const QString& name );

    /*!
     * \brief Sets the passed brush to be the one used when filling in the arrow of the Connection when it is selected.
     * \param brush The QBrush that you want the selected Connection to use.
     */
    void setSelectedBrush( const QBrush& brush );

    /*!
     * \brief Sets the color for the Connection when selected.`
     * \param color QColor that you want the connection to be.
     */
    void setSelectedColor( const QColor& color );

    /*!
     * \brief Sets the passed pen to be the one used to draw the Connection when it is selected.
     * \param pen The QPen that you want the selected Connection to use
     */
    void setSelectedPen( const QPen& pen );

    /*!
     * \brief Sets the starting Socket for this Connection
     * \param source The Socket that the Connection will start from.
     */
    void setSourceSocket( Socket* source );

    /*!
     * \brief Overloaded from QGraphicsPathItem. Returns the shape of the Connection.
     * \return QPainterPath that represents the Connections shape.
     */
    virtual QPainterPath shape() const;

    /*!
     * \brief This is the source Socket of the Connection.
     * \return The Socket that is the source of the Connection.
     */
    Socket* sourceSocket() const;

    /*!
     * \brief type
     * \return
     */
    virtual int type() const;

    /*!
     * \brief Updates the Connection and performs a redraw.
     */
    void updatePosition();

  signals:
    /*!
     * \brief This signal is emitted when the Connection requires to be styled by the style manager.
     */
    void requiresStyling( QObject* item );

  protected:
    /*!
     * \brief hoverEnterEvent
     * \param event
     */
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event );

    /*!
     * \brief hoverLeaveEvent
     * \param event
     */
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event );

    virtual QVariant itemChange( GraphicsItemChange change, const QVariant& value );

    /*!
     * \brief mouseMoveEvent
     * \param event
     */
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief mouseReleaseEvent
     * \param event
     */
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief paint
     * \param painter
     * \param option
     * \param widget
     */
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );
};

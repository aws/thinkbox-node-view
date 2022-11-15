// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/NodeView_global.h"
#include "NodeView/stylemanager.h"
#include "NodeView/trianglesocketshape.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsPathItem>
#include <QPen>

class Connection;
class NodeView;

class QPropertyAnimation;

/*!
 * \brief [Sockets](\ref Socket) are used to make connections between [Nodes](\ref Node) using Connection objects.
 */
class NodeView_API Socket : public QObject, public QGraphicsItemGroup, public Styleable {
    Q_OBJECT

    Q_PROPERTY( int animationDuration READ animationDuration WRITE setAnimationDuration DESIGNABLE true )
    Q_PROPERTY( int animationLightness READ animationLightness WRITE setAnimationLightness DESIGNABLE true )
    Q_PROPERTY( QColor fillColor READ fillColor WRITE setFillColor DESIGNABLE true )
    Q_PROPERTY( QFont font READ font WRITE setFont DESIGNABLE true )
    Q_PROPERTY( QColor labelColor READ labelColor WRITE setLabelColor DESIGNABLE true )
    Q_PROPERTY( bool multipleConnections READ multipleConnections WRITE setMultipleConnections DESIGNABLE true )
    Q_PROPERTY( QColor outlineColor READ outlineColor WRITE setOutlineColor DESIGNABLE true )
    Q_PROPERTY( QBrush socketBrush READ socketBrush WRITE setSocketBrush DESIGNABLE true )
    Q_PROPERTY( QPen socketPen READ socketPen WRITE setSocketPen DESIGNABLE true )
    Q_PROPERTY( SocketShape socketShape READ socketShape WRITE setSocketShape DESIGNABLE true )
    Q_PROPERTY( qreal socketShapeSize READ socketShapeSize WRITE setSocketShapeSize DESIGNABLE true )
    Q_PROPERTY( bool animationEnabled READ animationEnabled WRITE setAnimationEnabled DESIGNABLE true )
    Q_ENUMS( SocketShape )

  public:
    /*!
     * \brief Definition of type ID for Socket objects. All QGrpahicsItems need a type ID.
     */
    enum { Type = UserType + 2 };

    /*!
     * \brief This describes the shape of the Socket connection point.
     */
    enum SocketShape { Circle, Square, Triangle, None };

    /*!
     * \brief This describes the relative alignment of the text and socket shape
     */
    enum VerticalAlignment { Top, Center, Bottom };

    /*!
     * \brief This describes what will happen if the text on the label is too large to fully display on a single line
     */
    enum LabelStyle { Elide, Wrap };

    /*!
     * \brief This describes whether the object will be an input or output socket. This will determine the orientation
     * and behavior of the Socket.
     */
    enum SocketType { Input, Output, Invalid };

  protected:
    SocketShape m_socketShape; /*!< The shape of the Socket connection point. */
    qreal m_socketShapeSize;
    QAbstractGraphicsShapeItem* m_shape;
    SocketType m_socketType; /*!< Whether this is an input or output Socket. */
    QGraphicsTextItem* m_label;
    VerticalAlignment m_verticalAlignment;
    // private:

    QPropertyAnimation* m_animation;
    int m_animationDuration;
    int m_animationLightness;
    bool m_animationEnabled;
    bool m_dragEnabled;
    LabelStyle m_labelStyle;

    QColor m_fillColor;    /*!< The fill color of the Socket. */
    QColor m_outlineColor; /*!< The outline color of the Socket */

    // QColor m_textColor;				 /*!< The color of the text of the Socket. */
    bool m_multiConnections;

    QString m_labelText;

    NodeView* m_graphWidget;

    QList<Connection*> m_connections;
    Connection* m_tempConnection;

  public:
    /*!
     * \brief Constructor.
     * \param type Whether this is an input or output Socket.
     * \param label The label that will appear on the Socket.
     * \param parent The parent item of the Socket.
     */
    Socket( NodeView* graphWidget, const SocketType type = Invalid, const QString label = "",
            QGraphicsItem* parent = 0 );

    /*!
     * \brief
     */
    virtual ~Socket();

    /*!
     * \brief Adds the Connection object to the list of this [Sockets](\ref Socket) connections
     * \param connection The QSharedPointer that referenecs the Connection object that you want to add.
     */
    void addConnection( Connection* connection );

    /*!
     * \brief Returns the amount of time in miliseconds that the animation will run for.
     * \return Int that is the number of miliseconds the animation will run for.
     */
    int animationDuration() const;

    /*!
     * \brief Returns true if animation is currently enabled.  If the animation is enabled the sockets color will change
     * when hovered over
     * \return Bool which states if animation is enabled for
     */
    bool animationEnabled() const;

    /*!
     * \brief Returns the level that the animation will lighten the color of the Socket.
     * \return Int the is the level of lightness the color will change too. Default is 175.
     */
    int animationLightness() const;

    /*!
     * \brief Removes all connections from this Socket.
     */
    void clearConnections();

    /*!
     * \brief Returns the list of [Connections](\ref Connection) that this Socket has.
     * \return The QList of connections that this Socket has.
     */
    QList<Connection*> connections() const;

    /*!
     * \brief Creates a Connection between this Socket and the one passed. This is the perfered way to create
     * connections between two [Sockets](\ref Socket). \param socket The pointer to the Socket that you want to connect
     * to. \return True, if the connection was successful, false if not.
     */
    bool createConnection( Socket* socket );

    /*!
     * \brief Returns whether or not a connection can be started from this socket
     * \return whether or not a connection can be started from this socket
     */
    bool dragEnabled();

    /*!
     * \brief This returns the QColor object that describes the fill color of the Socket shape connector.
     * \return The color of the Socket shape.
     */
    QColor fillColor() const;

    /*!
     * \brief Returns the currently set font.
     * \return The QFont obeject that describes the currently set font being used for the text.
     */
    QFont font() const;

    /*!
     * \brief Returns true if the Socket connection point is under the mouse cursor, false if it is not.
     * \return
     */
    bool isConnectionPointUnderMouse( QGraphicsSceneMouseEvent* event ) const;

    /*!
     * \brief Returns a boolean value stating whether the Socket is valid or not.
     * \return Boolean value describing the [Sockets](\ref Socket) validity.
     */
    bool isValid() const;

    /*!
     * \brief Returns the QPen that is currently set to the label of the Socket. This will affect the outline color and
     * style of the Socket label.
     * \return The QPen that is assigned to the Socket label.
     */
    virtual QColor labelColor() const;

    /*!
     * \brief Returns the LabelStyle that is currently being used by the label when the text is too long to fit on a
     * single line
     * \return The LabelStyle that is being used
     */
    LabelStyle labelStyle() const;

    int maxTextLength();

    /*!
     * \brief Returns a boolean value stating whether multiple connections are enabled or not. For Output [Sockets](\ref
     * Socket) this is enabled by default. For Input [Sockets](\ref Socket) this is disabled by default.
     * \return Boolean value describing if multi ple connections are enabled or not.
     */
    bool multipleConnections() const;

    /*!
     * \brief This function returns the outline color of the text and connection point shape.
     * \return The QColor object that describes the outline color.
     */
    QColor outlineColor() const;

    /*!
     * \brief The starting position of the Socket on the Node.
     * \return The position as a QPointF.
     */
    QPointF position() const;

    /*!
     * \brief Rebuilds the socket giving it a maximum size that it is allowed to take up
     * \return The position as a QPointF.
     */
    void rebuildSocket( qreal width );

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
     * \brief Sets if connections can be started from this node
     * \param bool reperesenting if you want connections to be allowed to be started
     */
    void setDragEnabled( bool enabled );

    /*!
     * \brief Sets the fill color for this Socket.
     * \param fillColor The QColor object that describes the desired fill color.
     */
    void setFillColor( const QColor& fillColor );

    /*!
     * \brief Sets the font used by the text label.
     * \param font The QFont object that desbribes the font desired to be used for the text in the Socket.
     */
    virtual void setFont( const QFont& font );

    /*!
     * \brief Sets the Socket label pen to be the QPen passed. This will change the outline color and style of the text
     * in the label. \param pen The QPen with the settings you want the label text to have.
     */
    void setLabelColor( const QColor& color );

    /*!
     * \brief Sets the Socket label Style to be the LabelStyle passed. This will change the behaviour when the text of
     * the label will not fit on a single line \param style the LabelStyle you want the label to have
     */
    void setLabelStyle( const LabelStyle style );

    /*!
     * \brief Sets whether this Socket allows multiple connections or not.
     * \param enabled Pass 'true' if you want to enable multiple connections, 'false' if you want to disable them.
     */
    void setMultipleConnections( bool enabled );

    /*!
     * \brief Overloaded from QObject. Sets the object name for this Socket object and infroms the style manager that it
     * requires styling.
     * \param name The name that you want the object to have.
     */
    virtual void setObjectName( const QString& name );

    /*!
     * \brief Sets the outline color for this Socket.
     * \param outlineColor The QColor object that describes the desired outline color
     */
    void setOutlineColor( const QColor& outlineColor );

    /*!
     * \brief Sets the brush to be used to fill in the Socket shape.
     * \param brush The QBrush that you want the Socket shape to use.
     */
    void setSocketBrush( const QBrush& brush );

    /*!
     * \brief Sets the name of the Socket object. The name is displayed beside the shape connection point.
     * \param name The QString object that contains the name.
     */
    virtual void setSocketName( const QString& name );

    /*!
     * \brief Sets the pen to be used to outline the Socket shape.
     * \param pen The QPen that you want to Socket shape to use.
     */
    void setSocketPen( const QPen& pen );

    /*!
     * \brief Member function that allows you to change the connection point shape of the Socket.
     * \param shape The SocketShape value that describes the shape you want for the connection point.
     */
    void setSocketShape( const SocketShape shape );

    /*!
     * \brief Sets the socket shape size to the passed value.
     * \param size The size in a qreal that you want the socket shape to be.
     */
    void setSocketShapeSize( const qreal size );

    /*!
     * \brief Sets the orientation of the Socket if it has a triangle shape
     * \param dir the direction that the point of the triangle should be facing
     */
    void setTriangleOrientation( TriangleSocketShape::Orientation dir );

    /*!
     * \brief Sets the vertical alignment of the sockets text in comparison with the socket shape
     * \param Align where they are aligned to
     */
    void setVerticalAlignment( VerticalAlignment align );

    /*!
     * \brief shape
     * \return
     */
    virtual QPainterPath shape() const;

    /*!
     * \brief Returns the brush being used to fill in the Socket shape.
     * \return The QBrush being returned.
     */
    QBrush socketBrush() const;

    /*!
     * \brief The location point of the Socket connection point.
     * \return The QPointF that contains the location of the Socket connection point.
     */
    QPointF socketLocation() const;

    /*!
     * \brief Gets the name of the Socket.
     * \return The QString that contains the name of the Socket.
     */
    virtual QString socketName() const;

    /*!
     * \brief Returns the pen used to outline the Socket shape.
     * \return The QPen being returned.
     */
    QPen socketPen() const;

    /*!
     * \brief Gets the current connection point shape that the Socket has.
     * \return The connection point shap type of the Socket.
     */
    SocketShape socketShape() const;

    /*!
     * \brief Returns the currently set socket shape size.
     * \return The qreal that the socket shape size is currently set to.
     */
    qreal socketShapeSize() const;

    /*!
     * \brief Gets whether a Socket is an Output or Input Socket.
     * \return The SocketType of the Socket.
     */
    SocketType socketType() const;

    /*!
     * \brief Returns the direction in which the socket is facing if it is a triangular socket.
     * \return The direction in which the socket is facing if it is a triangular socket otherwise Orientation::None
     */
    TriangleSocketShape::Orientation triangleOrientation();

    /*!
     * \brief type
     * \return
     */
    virtual int type() const;

    /*!
     * \brief Schedules a redraw of the Socekt.
     * \param rect The are to be redrawn.
     */
    virtual void update( const QRectF& rect = QRectF() );

    /*!
     * \brief This updates the connection drawing points for redrawing the connection lines when a Node is being moved
     * around.
     */
    void updateConnections();

    /*!
     * \brief Returns the alignment of the socket shape and text
     * \return The alignment of the socket shape and text
     */
    VerticalAlignment verticalAlignment();

  public slots:
    /*!
     * \brief removeConnection
     * \param conn
     */
    void removeConnection( QObject* conn );

  signals:
    /*!
     * \brief This signal is emitted when a Connection has been made between two [Sockets](\ref Socket).
     */
    void connectionCreated();

    /*!
     * \brief This signal is emitted when a Connection is deleted or disconnected from the Socket.
     */
    void connectionDeleted();

    /*!
     * \brief This signal is emitted when a Connection had been started to be dragged from a Socket but not yet
     * completed by Connecting to another Socket.
     */
    void connectionStarted();

    /*!
     * \brief This signal is emitted when a Connection has been dropped on empty space in the view.
     * \param pos The QPointF that represents the position in the scene where the Connection was dropped.
     */
    void connectToEmpty( Socket* socket, QPointF pos );

    /*!
     * \brief This signal is emitted when the Socket requires to be styled by the style manager.
     */
    void requiresStyling( QObject* item );

  protected:
    /*!
     * \brief constructGroup
     */
    virtual void constructGroup();
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

    /*!
     * \brief mouseMoveEvent
     * \param event
     */
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief mousePressEvent
     * \param event
     */
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief mouseReleaseEvent
     * \param event
     */
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief resizeGroup
     * \param width
     */
    virtual void resizeGroup( qreal width );
};

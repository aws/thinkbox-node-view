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
#include <QTextCursor>

#define BASE_RECT QRectF( -10, -10, 153, 33 )

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
class NodeView_API Node : public QObject, public QGraphicsPolygonItem, public Styleable {
    Q_OBJECT

    Q_PROPERTY( QBrush backgroundBrush READ backgroundBrush WRITE setBackgroundBrush DESIGNABLE true )
    Q_PROPERTY( qreal cornerXRadius READ cornerXRadius WRITE setCornerXRadius DESIGNABLE true )
    Q_PROPERTY( qreal cornerYRadius READ cornerYRadius WRITE setCornerYRadius DESIGNABLE true )
    Q_PROPERTY( QColor detailTextColor READ detailTextColor WRITE setDetailTextColor DESIGNABLE true )
    Q_PROPERTY( QColor bodyDetailTextColor READ bodyDetailTextColor WRITE setBodyDetailTextColor DESIGNABLE true )
    Q_PROPERTY( bool bodyDetailTextEnabled READ bodyDetailTextEnabled WRITE setBodyDetailTextEnabled DESIGNABLE true )
    Q_PROPERTY( QFont bodyDetailTextFont READ bodyDetailTextFont WRITE setBodyDetailTextFont DESIGNABLE true )
    Q_PROPERTY( bool detailTextEnabled READ detailTextEnabled WRITE setDetailTextEnabled DESIGNABLE true )
    Q_PROPERTY( QFont detailTextFont READ detailTextFont WRITE setDetailTextFont DESIGNABLE true )
    Q_PROPERTY( bool dropShadow READ dropShadow WRITE setDropShadow DESIGNABLE true )
    Q_PROPERTY( qreal minimumWidth READ minimumWidth WRITE setMinimumWidth DESIGNABLE true )
    Q_PROPERTY( QPen outlinePen READ outlinePen WRITE setOutlinePen DESIGNABLE true )
    Q_PROPERTY( QPixmap pixmap READ pixmap WRITE setPixmap DESIGNABLE true )
    Q_PROPERTY( bool pixmapEnabled READ pixmapEnabled WRITE setPixmapEnabled DESIGNABLE true )
    Q_PROPERTY( PixmapPosition pixmapPosition READ pixmapPosition WRITE setPixmapPosition DESIGNABLE true )
    Q_PROPERTY( TitleJustify titleJustify READ titleJustify WRITE setTitleJustify DESIGNABLE true )
    Q_PROPERTY( QSize pixmapSize READ pixmapSize WRITE setPixmapSize DESIGNABLE true )
    Q_PROPERTY( QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush DESIGNABLE true )
    Q_PROPERTY( QPen selectedPen READ selectedPen WRITE setSelectedPen DESIGNABLE true )
    Q_PROPERTY( QBrush titleBarBrush READ titleBarBrush WRITE setTitleBarBrush DESIGNABLE true )
    Q_PROPERTY( QBrush titleBarSelectedBrush READ titleBarSelectedBrush WRITE setTitleBarSelectedBrush DESIGNABLE true )
    Q_PROPERTY( QBrush titleTextBrush READ titleTextBrush WRITE setTitleTextBrush DESIGNABLE true )
    Q_PROPERTY( QFont titleTextFont READ titleTextFont WRITE setTitleTextFont DESIGNABLE true )
    Q_PROPERTY( QPen titleTextPen READ titleTextPen WRITE setTitleTextPen DESIGNABLE true )
    Q_PROPERTY( int socketBuffer READ socketBuffer WRITE setSocketBuffer DESIGNABLE true )
    Q_PROPERTY( bool inlineSockets READ inlineSockets WRITE setInlineSockets DESIGNABLE true )
    Q_PROPERTY( qreal inlineTextWidthPercentage READ inlineTextWidthPercentage WRITE setInlineTextWidthPercentage
                    DESIGNABLE true )
    Q_PROPERTY(
        qreal inlineTextGapPercentage READ inlineTextGapPercentage WRITE setInlineTextGapPercentage DESIGNABLE true )

    Q_ENUMS( PixmapPosition )
    Q_ENUMS( TitleJustify )

  public:
    enum { Type = UserType + 1 };

    /*!
     * \brief The PixmapPosition enum is used to define where you want the pixel map item to appear in the title bar
     * area of the Node.
     */
    enum PixmapPosition { None, Left, Right, Top, Center, Bottom };

    enum TitleJustify { TitleLeft, TitleRight, TitleCenter };

  protected:
    QList<Socket*> m_inSockets; /*< QList of input [Sockets](\ref Socket)
                                   that this Node has. */
    QList<Socket*> m_outSockets; /*< QList of output [Sockets](\ref
                                    Socket) that this Node has. */

    QGraphicsRectItem m_titleBarItem;
    QGraphicsSimpleTextItem m_titleTextItem;
    QGraphicsTextItem m_detailTextItem;
    QGraphicsTextItem m_bodyDetailTextItem;
    QGraphicsPixmapItem m_pixmapItem;
    QGraphicsLineItem m_bodyDetailDivider;

    PixmapPosition m_pixmapPosition;
    TitleJustify m_titleJustify;

    int m_socketBuffer;
    bool m_dropShadow;
    bool m_disableRedraw;

    bool m_inlineSockets;
    qreal m_inlineInputSocketWidth;
    qreal m_inlineSocketGap;

    qreal m_cornerXRadius;
    qreal m_cornerYRadius;
    qreal m_minWidth;
    QString m_titleText;

    QPointF m_newPos;        /*< This Node's position after it has moved within the
                                QGraphicsScene. */
    NodeView* m_graphWidget; /*< This the NodeView view that contains the Node. */
    QPolygonF m_polygon; /*< This is the QPolygonF representation of
                            the Node that is used for drawing the Node. */
    QPen m_outlinePen;
    QPen m_selectedPen;
    QBrush m_backgroundBrush;
    QBrush m_selectedBrush;
    QBrush m_titleBarBrush;
    QBrush m_titleBarSelectedBrush;
    Qt::TextElideMode m_elidedTitle;

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
    Node( NodeView* graphWidget, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0 );

    /*!
     * \brief Deconstructor
     */
    virtual ~Node();

    /*!
     * \brief Adds a new FullLineSocket into the input socket list of the node
     * \return The index of the new input Socket created.
     * \see getInputSocket(), getInputSocketCount(), deleteInputSocket()
     */
    virtual int addInputFullLineSocket();

    /*!
     * \brief Adds a new input Socket to the Node.
     * \return The index of the new input Socket created.
     * \see getInputSocket(), getInputSocketCount(), deleteInputSocket()
     */
    virtual int addInputSocket();

    /*!
     * \brief Adds a new input WidgetSocket to the Node.
     * \return The index of the new input Socket created.
     * \see getInputSocket(), getInputSocketCount(), deleteInputSocket()
     */
    virtual int addInputWidgetSocket();

    /*!
     * \brief Adds a new output FullLineSocket to the Node.
     * \return The index of the new output Socket created.
     * \see getOutputSocket(), getOutputSocketCount(), deleteOutputSocket()
     */
    virtual int addOutputFullLineSocket();

    /*!
     * \brief Adds a new output Socket to the Node.
     * \return The index of the new output Socket created.
     * \see getOutputSocket(), getOutputSocketCount(), deleteOutputSocket()
     */
    virtual int addOutputSocket();

    /*!
     * \brief Adds a new output WidgetSocket to the Node.
     * \return The index of the new output Socket created.
     * \see getOutputSocket(), getOutputSocketCount(), deleteOutputSocket()
     */
    virtual int addOutputWidgetSocket();

    /*!
     * \brief Returns the brush that us curently being used to fill in the background of the Node.
     * \return The QBrush currently being used for the background fill.
     */
    QBrush backgroundBrush() const;

    /*!
     * \brief Begins modification of the node, disabling (costly) redraws until the endUpdateNode is called
     */
    void beginModifyNode();

    QTextCursor bodyDetailTextCursor();

    /*!
     * \brief Returns the currently set detail text in HTML format.
     * \return The QString containing the detail text.
     */
    virtual QString bodyDetailText() const;

    /*!
     * \brief Returns the default color of the detail text.
     * \return The QColor that the is the default color for unformatted detail text.
     */
    virtual QColor bodyDetailTextColor() const;

    /*!
     * \brief Returns true if the detail text will be displayed, false if not.
     * \return Boolean value of whether or not the detail text should be displayed.
     */
    bool bodyDetailTextEnabled() const;

    /*!
     * \brief Returns the font currently being used for the detail text.
     * \return The QFont the the detail text is using.
     */
    virtual QFont bodyDetailTextFont() const;

    /*!
     * \brief Gets the bounding rectangle of the Node
     * \return The bounding rectangle of the Node as a QRectF.
     */
    virtual QRectF boundingRect() const;

    void updateBoundingRect();

    /*!
     * \brief Returns the X component of the Node corner radius. This will effect the curvature of the Node's bottom
     * corners.
     * \return qreal that is currently set to the X corner radius.
     */
    qreal cornerXRadius() const;

    /*!
     * \brief Returns the Y component of the Node corner radius. This will effect the curvature of the Node's bottom
     * corners.
     * \return qreal that is currently set to the Y corner radius.
     */
    qreal cornerYRadius() const;

    /*!
     * \brief Removes the input Socket at the index provided from this Node. The underlying Socket object is destroyed.
     * \param index The index of the input Socket to be deleted.
     * \see addInputSocket(), getInputSocketCount(), getInputSocket()
     */
    void deleteInputSocket( int index );
    /*!
     * \brief Removes the output Socket at the index provided from this Node. The underlying Socket object is destroyed.
     * \param index The index of the output Socket to be deleted.
     * \see addOutputSocket(), getOutputSocketCount(), getOutputSocket()
     */
    void deleteOutputSocket( int index );

    void deletingSelf();

    /*!
     * \brief Returns the currently set detail text in HTML format.
     * \return The QString containing the detail text.
     */
    virtual QString detailText() const;

    /*!
     * \brief Returns the default color of the detail text.
     * \return The QColor that the is the default color for unformatted detail text.
     */
    virtual QColor detailTextColor() const;

    /*!
     * \brief Returns true if the detail text will be displayed, false if not.
     * \return Boolean value of whether or not the detail text should be displayed.
     */
    bool detailTextEnabled() const;

    /*!
     * \brief Returns the font currently being used for the detail text.
     * \return The QFont the the detail text is using.
     */
    virtual QFont detailTextFont() const;

    /*!
     * \brief Returns 'true' if drawing a drop shadow is enabled, 'false' if not.
     * \return The boolean value for having a drop shadow.
     */
    bool dropShadow() const;

    /*!
     * \brief Gets the TextElideMOde of the title
     * \return The direction in which the text is elided towards (where elpsises appear)
     */
    Qt::TextElideMode elidedTitle();

    /*!
     * \brief Ends the modification mode started by beginModifyNode and forces a redraw immediately
     */
    void endModifyNode();

    /*!
     * \brief Returns a list of Nodes which are attached to the specified Input Socket
     * \param The index of the socket at which you are searching for connected nodes off of
     * \return A list of Nodes
     */
    QMap<Node*, int> getConnectedNodesToInputSocket( int inputSocketIndex ) const;

    /*!
     * \brief Returns a list of Nodes which are attached to the specified Input Socket
     * \param The index of the socket at which you are searching for connected nodes off of
     * \return A list of Nodes
     */
    QMap<Node*, int> getConnectedNodesToOutputSocket( int outputSocketIndex ) const;

    /*!
     * \brief Gets the total number of input [Sockets](\ref Socket) that the Node has.
     * \return The number of input [Sockets](\ref Socket) the Node has.
     * \see addInputSocket(), deleteInputSocket(), getInputSocket()
     */
    int getInputSocketCount() const;

    /*!
     * \brief Gets the total number of output [Sockets](\ref Socket) that the Node has.
     * \return The number of output [Sockets](\ref Socket) the Node has.
     * \see addOutputSocket(), deleteOutputSocket(), getOutputSocket()
     */
    int getOutputSocketCount() const;

    /*!
     * \brief Gets the specified input Socket from the [Nodes](\ref Node) input sockets.
     * \param index The index of the desired input Socket.
     * \return The Socket object that is at the provided index.
     * \see addInputSocket(), getInputSocketCount(), getInputSocket()
     */
    Socket* getInputSocket( int index ) const;

    /*!
     * \brief Gets all input [Sockets](\ref Socket) asociated with this Node.
     * \return A QList containing pointers to all input [Sockets](\ref Socket) for this Node.
     */
    QList<Socket*> getInputSockets() const;

    /*!
     * \brief Gets the specified output Socket from the [Nodes](\ref Node) output sockets.
     * \param index The index of the desired output Socket.
     * \return The Socket object that is at the provided index.
     * \see addOutputSocket(), getOutputSocketCount(), getOutputSocket()
     */
    Socket* getOutputSocket( int index ) const;

    /*!
     * \brief Gets all output [Sockets](\ref Socket) asociated with this Node.
     * \return A QList containing pointers to all output [Sockets](\ref Socket) for this Node.
     */
    QList<Socket*> getOutputSockets() const;

    bool inlineSockets() const;

    qreal inlineTextWidthPercentage() const;

    qreal inlineTextGapPercentage() const;

    /*!
     * \brief Finds out if the given node is connected to the current node through any of its sockets
     * \param check The node that you are checking for
     * \return Whether or not the node was found
     */
    bool isNodeConnected( Node* check );

    /*!
     * \brief Finds out if the given node is connected to the current node through any of its input sockets
     * \param check The node that you are checking for
     * \return Whether or not the node was found
     */
    bool isNodeConnectedToInput( Node* check );

    /*!
     * \brief Finds out if the given node is connected to the current node through the specified input socket
     * \param inputSocketIndex The index of the inputSocket you are searching using
     * \param check The node that you are checking for
     * \return Whether or not the node was found
     */
    bool isNodeConnectedToInputSocket( int inputSocketIndex, Node* check );

    /*!
     * \brief Finds out if the given node is connected to the current node through any of its output sockets
     * \param check The node that you are checking for
     * \return Whether or not the node was found
     */
    bool isNodeConnectedToOutput( Node* check );

    /*!
     * \brief Finds out if the given node is connected to the current node through the specified output socket
     * \param outputSocketIndex The index of the inputSocket you are searching using
     * \param check The node that you are checking for
     * \return Whether or not the node was found
     */
    bool isNodeConnectedToOutputSocket( int outputSocketIndex, Node* check );

    /*!
     * \brief Returns the minimum width of the Node. The Node is guarenteed to be at least this wide.
     * \return The qreal that represents the minimum width of the Node.
     */
    qreal minimumWidth() const;

    /*!
     * \brief Returns the QPen that is currently set to be used as the outline for this Node.
     * \return The QPen that describes how the outline will be drawn for this Node.
     */
    QPen outlinePen() const;

    /*!
     * \brief This function performs all the drawing aspects of the Node. Reimplement this function to change the
     * appearence of the Node.
     * \param painter The QPainter object that will perform the
     * drawing. \param option \param widget
     */
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

    /*!
     * \brief Returns the currently set pixelmap for this Node.
     * \return The QPixmap the contains the pixelmap for this Node.
     */
    QPixmap pixmap() const;

    /*!
     * \brief Returns true if the pixelmap should be displayed, false if not.
     * \return The boolean value for displaying the pixelmap.
     */
    bool pixmapEnabled() const;

    /*!
     * \brief Returns the currently set pixel map position for this Node.
     * \return The currently set position of the pixel map withing the title area of the Node.
     */
    PixmapPosition pixmapPosition() const;

    /*!
     * \brief Returns the currently set pixelmap size for this Nodes pixelmap.
     * \return The QSize the represents the size of the Nodes pixelmap.
     */
    QSize pixmapSize() const;

    /*!
     * \brief Gets the polygon representation of the Node.
     * \return The QPolygonF representation of the Node.
     */
    QPolygonF polygon() const;

    /*!
     * \brief Schedules a redraw of the Node area covered by the bounding rectangle.
     * \see boundingRect()
     */
    void redrawNode();

    static void resetZDepth() { NextZNode = 0; }

    /*!
     * \brief Returns the QBrush that is used to fill in the Node color when the Node is currently selected.
     * \return The QBrush that currently describes how the Node will be filled in when it is selected.
     */
    QBrush selectedBrush() const;

    /*!
     * \brief Returns the QPen that is used to draw the Node outline when the Node is currently selected.
     * \return The QPen that currently describes how the Node will be outlined in when it is selected.
     */
    QPen selectedPen() const;

    /*!
     * \brief Sets a buffer distance between Sockets this is the distance seperateing them
     * \param the distance between the Sockets
     */
    void setSocketBuffer( int buffer );

    /*!
     * \brief Sets the Node to use the QBrush passed to fill in the Node background.
     * \param brush The QBrush that describes how you want the Node to be background filled.
     */
    void setBackgroundBrush( const QBrush& brush );

    void setBodyDetailCursor( const QTextCursor cursor );

    /*!
     * \brief Sets the text to be displayed as the detail text. This can contain HTML formatting structures.
     * \param text The QString that contains the text to be displayed.
     */
    virtual void setBodyDetailText( const QString& text );

    /*!
     * \brief Sets the default text color for unformatted text in the detail text.
     * \param color The QColor that you want the unformatted text to be.
     */
    virtual void setBodyDetailTextColor( const QColor& color );

    /*!
     * \brief Sets if the detail text should be displayed or not.
     * \param enabled If true, the detail text will be shown, otherwise it will be hidden.
     */
    virtual void setBodyDetailTextEnabled( bool enabled );

    /*!
     * \brief Sets the text font for the detail text.
     * \param font The QFont that you want the font to be.
     */
    virtual void setBodyDetailTextFont( const QFont& font );

    /*!
     * \brief Sets the X corner radius to the passed value. This will effect the curvature of the Node's bottom corners.
     * \param radius qreal that will be used for the X radius of the corner.
     */
    void setCornerXRadius( qreal radius );

    /*!
     * \brief Sets the Y corner radius to the passed value. This will effect the curvature of the Node's bottom corners.
     * \param radius qreal that will be used for the Y radius of the corner.
     */
    void setCornerYRadius( qreal radius );

    /*!
     * \brief Sets the text to be displayed as the detail text. This can contain HTML formatting structures.
     * \param text The QString that contains the text to be displayed.
     */
    virtual void setDetailText( const QString& text );

    /*!
     * \brief Sets the default text color for unformatted text in the detail text.
     * \param color The QColor that you want the unformatted text to be.
     */
    virtual void setDetailTextColor( const QColor& color );

    /*!
     * \brief Sets if the detail text should be displayed or not.
     * \param enabled If true, the detail text will be shown, otherwise it will be hidden.
     */
    virtual void setDetailTextEnabled( bool enabled );

    /*!
     * \brief Sets the text font for the detail text.
     * \param font The QFont that you want the font to be.
     */
    virtual void setDetailTextFont( const QFont& font );

    /*!
     * \brief This sets whether the Node should have a drop shadow drawn with it or not.
     * \param enabled Pass 'true' if you want a drop shadow to appear, 'false' if not.
     */
    void setDropShadow( bool enabled );

    /*!
     * \brief Set which Direction the text in the Nodes title will be elided towards
     * \param TextElideMode The direction in which you wish to elided the title
     */
    void setElidedTitle( Qt::TextElideMode mode );

    void setInlineSockets( bool enabled );

    void setInlineTextWidthPercentage( qreal percentage );

    void setInlineTextGapPercentage( qreal percentage );

    /*!
     * \brief Sets the text that describes an input Socket to the
     * QString passed. \param index The index of the input Socket that
     * you want to change the name of. \param name The QString
     * containing the text that you want the input Socket to have.
     */
    void setInputSocketName( int index, const QString& name );

    /*!
     * \brief Sets the minimum width of the Node to be the value provided. The Node will always be at least as wide as
     * the minimum width. \param width The qreal that you want the minimum width to be.
     */
    void setMinimumWidth( qreal width );

    /*!
     * \brief Overloaded from QObject. Sets the object name for this Node object and infroms the style manager that it
     * requires styling.
     * \param name The name that you want the object to have.
     */
    virtual void setObjectName( const QString& name );

    /*!
     * \brief Sets the passed QPen to be used to outline the Node.
     * \param pen The QPen that you want the outline to use.
     */
    void setOutlinePen( const QPen& pen );

    /*!
     * \brief Sets the text that describes an output Socket to the
     * QString passed.
     * \param index The index of the output Socket that you want to change the name of.
     * \param name The QString containing the text that you want the
     * output Socket to have.
     */
    void setOutputSocketName( int index, const QString& name );

    /*!
     * \brief Sets the pixelmap to use.
     * \param pixmap The QPixmap that you want to use.
     */
    void setPixmap( const QPixmap& pixmap );

    /*!
     * \brief Sets the pixelmap set to be displayed if passed true, otherwise it will be hideen.
     * \param enabled
     */
    void setPixmapEnabled( bool enabled );

    /*!
     * \brief Sets the pixel map position for the Node.
     * \param pos The position that you want the pixel map to appear in.
     */
    void setPixmapPosition( PixmapPosition pos );

    /*!
     * \brief Sets the size of the pixelmap.
     * \param size The QSize that you want the pizelmap to be.
     */
    void setPixmapSize( const QSize& size );

    /*!
     * \brief Sets the brush you want to use when filling in a selected Node.
     * \param brush The QBrush you want the Node to use when it is selected.
     */
    void setSelectedBrush( const QBrush& brush );

    /*!
     * \brief Sets the pen you want to use when outlining in a selected Node.
     * \param pen The QPen you want the Node to use when it is selected.
     */
    void setSelectedPen( const QPen& pen );

    /*!
     * \brief Sets the brush you want to use to paint the title bar area.
     * \param brush The QBrush you want the title area of the Node to use.
     */
    void setTitleBarBrush( const QBrush& brush );

    /*!
     * \brief Sets the brush your want to use to paint the title bar area when the Node is selected.
     * \param brush The QBrush you want the title area of the Node to use when it is seleced.
     */
    void setTitleBarSelectedBrush( const QBrush& brush );

    /*!
     * \brief Set which direction you want to justify the title towards
     * \param TitleJustify the direction in which you wish to justify the title towards
     */
    void setTitleJustify( TitleJustify justify );

    /*!
     * \brief Sets the title brush to QBrush passed. The title brush will be used to fill in the title text.
     * \param brush The QBrush you want the title text to use.
     */
    void setTitleTextBrush( const QBrush& brush );

    /*!
     * \brief Sets the title font to the passed QFont.
     * \param font The QFont object that you want to use as the title font.
     */
    void setTitleTextFont( const QFont& font );

    /*!
     * \brief Sets the title pen to be the QPen passed. The title pen will be used to outline the title text.
     * \param pen The QPen you want the title text to use.
     */
    void setTitleTextPen( const QPen& pen );

    /*!
     * \brief Sets the title text of the Node to the QString passed.
     * \param text The QString containing the text that you want the
     * Node to have.
     */
    void setTitleText( const QString& text );

    /*!
     * \brief This is the shape of the Node as described by a QPainterPath object.
     * \return The QPainterPath that contains the shape of the
     * Node.
     */
    QPainterPath shape() const;

    /*!
     * \brief Snap this node to the grid lines
     */
    void snapToGrid();

    /*!
     * \brief Return the buffer distance between multiple sockets
     * \return the buffer distance between mulitple sockets
     */
    int socketBuffer() const;

    /*!
     * \brief Returns the title bar area brush currently set.
     * \return The QBrush that is currently being used to paint the title bar area of the Node.
     */
    QBrush titleBarBrush() const;

    /*!
     * \brief Returns the title bar area selected brush currently set.
     * \return The QBrush that is currently being used to pain the title bar area of the Node when selected.
     */
    QBrush titleBarSelectedBrush() const;

    /*!
     * \brief Returns the bounding rectangle for the title bar area of the Node.
     * \return The QRectF that represents the bounding area of the title bar.
     */
    QRectF titleBarBoundingRect() const;

    /*!
     * \brief Returns which direction the title is justified towards
     * \return The direction in which the title is justified toards
     */
    TitleJustify titleJustify() const;

    /*!
     * \brief The name that is displayed at the top of the Node.
     * \return The QString that contains the currently set name of the Node.
     */
    QString titleText() const;

    /*!
     * \brief Returns the currently used title brush. The title brush is used to fill in the text.
     * \return The QBrush that is curretly set to the title brush.
     */
    QBrush titleTextBrush() const;

    /*!
     * \brief Returns the currently set title font.
     * \return The QFont that is currently set to the title font.
     */
    QFont titleTextFont() const;

    /*!
     * \brief Returns the currently used title pen. The title pen is used to outline the text.
     * \return The QPen that is curretly set to the title pen.
     */
    QPen titleTextPen() const;

    /*!
     * \brief This returns the user defined type of a Node
     * QGraphicsItem. All
     * QGrpahicsItems need to have a type in order to properly
     * function within a QGraphicsScene.
     * \return The Node object type identifier.
     */
    virtual int type() const;

  signals:
    /*!
     * \brief This signal is emitted when the Node requires to be styled by the style manager.
     */
    void requiresStyling( QObject* item );

    /*!
     * \brief This signal is emitted when the Node is double clicked
     */
    void doubleClicked( QGraphicsSceneMouseEvent* event );

    void itemUpdated();

    void deletingNode( Node* );

  protected:
    /*!
     * \brief Overloaded function from
     * QGraphicsPolygonItem. Handles the event that occurs
     * when the Node gains focus. \param event The QFocusEvent that
     * has all the data from the event that occured.
     */
    virtual void focusInEvent( QFocusEvent* event );

    /*!
     * \brief Overloaded function from
     * QGraphicsPolygonItem. Handles the event that occurs
     * when the Node loses focus. \param event The QFocusEvent that
     * has all the data from the event that occured.
     */
    virtual void focusOutEvent( QFocusEvent* event );

    /*!
     * \brief Overloaded function from
     * QGraphicsPolygonItem. Handles the event that occurs
     * when there is a change made to the Node. \param change This describes what aspect of the Node that has changed.
     * \param value This QVariant contains the data from the changed
     * value.
     * \return This QVariant contains the data after the change as
     * been made to the Node.
     */
    virtual QVariant itemChange( GraphicsItemChange change, const QVariant& value );

    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief Overloaded function from
     * QGraphicsPolygonItem. Handles any mouse press
     * events that the Node recieves. \param event The
     * QGraphicsSceneMouseEvent that contains all the
     * data from the recieved event.
     */
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief Overloaded function from
     * QGraphicsPolygonItem. Handles any mouse release
     * events that the Node recieves. \param event The
     * QGraphicsSceneMouseEvent that contains all the
     * data from the recieved event.
     */
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief This function is called whenever a new input or output Socket is added/removed from the Node. It
     * calculates its new size and puts all the components in the proper position.
     */
    virtual void resizeNode();

    /*!
     * \brief resizeTitleBar
     */
    virtual void resizeTitleBar();

    /*!
     * \brief updateConnections
     */
    virtual void updateConnections();
};

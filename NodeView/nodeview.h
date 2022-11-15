// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/NodeView_global.h"
#include "NodeView/stylemanager.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsView>

class Node;
class Socket;
class Connection;
class NodeGroup;
class NodeViewMiniMap;

/*!
 * \brief The NodeView class is a subclass of QGraphicsView and is used to display and control a node based user
 * interface.
 */
class NodeView_API NodeView : public QGraphicsView {
    Q_OBJECT

    Q_PROPERTY( QColor backgroundColor READ backgroundColor WRITE setBackgroundColor DESIGNABLE true )
    Q_PROPERTY( QColor gridColor READ gridColor WRITE setGridColor DESIGNABLE true )
    Q_PROPERTY( bool gridLines READ gridLines WRITE setGridLines DESIGNABLE true )
    Q_PROPERTY( int gridSize READ gridSize WRITE setGridSize DESIGNABLE true )
    Q_PROPERTY( bool snapToGrid READ snapToGrid WRITE setSnapToGrid DESIGNABLE true )
    Q_ENUMS( MinimapPosition )

  public:
    /*!
     * \brief The PixmapPosition enum is used to define where you want the pixel map item to appear in the title bar
     * area of the Node.
     */
    enum MinimapPosition { TopLeft, TopRight, BottomLeft, BottomRight };

  private:
    int m_timerId;     /*!< The identification number of the refresh timer for the NodeView view. */
    bool m_snapToGrid; /*!< Private member variable that indicates whether Node objects in the NodeView view should snap
                          to a grid or not. */
    bool m_gridLines;
    int m_gridSize;
    QList<Node*> m_nodes;    /*!< The list of [Nodes](\ref Node) currently in the NodeView view. */
    QPixmap m_backgroundMap; /*!< A dynamically generated pixel map of the background grid. The grid is drawn this way
                                for performance reasons. */
    QPainter
        m_backgroundPainter; /*!< The QPainter object that dynamically generates the m_backgroundMap member variable. */
    QPoint m_panStartPos;
    QColor m_gridColor;
    QColor m_backgroundColor;

    QPoint m_creationPoint;
    QGraphicsEllipseItem m_creationPointMarker;

    MinimapPosition m_miniMapPosition;
    NodeViewMiniMap* m_miniMap;
    StyleManager m_styleManager;

    bool m_midClickMoved;

  public:
    /*!
     * \brief Constructor
     * \param parent The parent QWidget of this NodeView instance.
     */
    NodeView( QWidget* parent = 0 );

    ~NodeView();

    /*!
     * \brief Returns the color that the background is currently set to.
     * \return Returns the background color of the NodeView view as a QColor.
     * \see setBackgroundColor()
     */
    QColor backgroundColor() const;

    QColor creationPointColor() const;

    /*!
     * \brief Removes and deletes the Node passed from the NodeView view.
     * \param node The Node that you want to remove and delete.
     */
    void deleteNode( Node* node );

    /*!
     * \brief Removes and deletes all [Nodes](\ref Node) from the NodeView view.
     */
    void deleteAllNodes();

    /*!
     * \brief Gets the pointer to the Node at position pos.
     * \param pos The position of the Node wanted in QPointF.
     * \return The Node that is at position, pos.
     */
    Node* nodeByPosition( const QPointF& pos ) const;

    /*!
     * \brief Member function that returns the total number of Node instances in the NodeView view.
     * \return The total number of Node instances in the MagamaFLUX view.
     * \see Node
     */
    int nodeCount() const;

    /*!
     * \brief Gets the count of QGraphicsItems currently selected in the MagamaFLUX view.
     * \return The number of QGraphicsItems currently selected.
     */
    int selectionCount() const;

    /*!
     * \brief Returns the currently set grid color.
     * \return This is the QColor that the grid is currently set to.
     */
    QColor gridColor() const;

    /*!
     * \brief Returns true if grid lines are enabled, false otherwise.
     * \return Bool describing if the grid lines are turned on or not.
     */
    bool gridLines() const;

    /*!
     * \brief Returns the currently set width and height for the grid lines.
     * \return The int value the is the grids current size.
     */
    int gridSize() const;

    /*!
     * \brief Hides the mini map for the NodeView view.
     */
    void hideMiniMap();

    /*!
     * \brief Returns whether or not the creation point marker is visible, if it is all new nodes will be created at
     * that point \return Whether or not the creation point marker is visible.
     */
    bool isCreationPointMarkerVisible() const;

    /*!
     * \brief Returns the style sheet that is currently being used to style the NodeView view.
     * \return The QString containing the JSON style sheet.
     */
    QString nodeViewStyleSheet() const;

    /*!
     * \brief Returns the position of the Minimap
     * \return The position of the minimap
     */
    MinimapPosition minimapPosition() const;

    /*!
     * \brief Returns whether or not the minimap is visible
     * \return Whether or not the minimap is visible
     */
    bool minimapVisible();

    /*!
     * \brief Moves the minimap to the correct corner
     */
    void moveMiniMap();

    QList<Node*> nodes();

    // QList<NodeGroup *> nodeGroups();//will need to rework some stuff for this then add it

    /*!
     * \brief Move the currently visable section of the NodeView view.
     * \param dx The amount to move the view in the X dimension.
     * \param dy The amount to move the view in the Y dimension.
     * \see zoom()
     */
    void pan( qreal dx, qreal dy );

    /*!
     * \brief Called to redraw the view by an outside source.
     */
    void redrawView();

    /*!
     * \brief Gets and returns a QList of all the currently selected [connections](\ref Connection) in the NodeView
     * view.
     * \return The [Connections](\ref Connection) that are currently selected.
     */
    QList<Connection*> selectedConnections() const;

    /*!
     * \brief Gets and returns a QList of all the currently selected [Nodes](\ref Node) in the NodeView view.
     * \return The [Nodes](\ref Node) that are currently selected.
     */
    QList<Node*> selectedNodes() const;

    QList<NodeGroup*> selectedNodeGroups() const;

    /*!
     * \brief Sets the background color of the NodeView view.
     * \param color The desired color of the background for the NodeView view.
     * \see backgroundColor()
     */
    void setBackgroundColor( const QColor& color );

    void setCreationPointMarkerColor( const QColor& color );

    /*!
     * \brief setCreationPointMarkerVisible
     * \param visible
     */
    void setCreationPointMarkerVisible( bool visible );

    /*!
     * \brief Sets the color of the grid lines in the NodeView view.
     * \param color The desired color of the grid lines.
     */
    void setGridColor( const QColor& color );

    /*!
     * \brief Turns the grid lines on or off.
     * \param enabled Pass 'true' if you want to turn grid lines on, 'false' to turn them off.
     */
    void setGridLines( bool enabled );

    /*!
     * \brief Sets the grid size to be the passed value. This affects both the snap-to-grid size and grid lines.
     * \param size The int size that you want the grid to be.
     */
    void setGridSize( int size );

    /*!
     * \brief This feature will cause any Node in the scene to snap in place to the background grid if enabled.
     * \param enable Pass true if you want to enable snapToGrid, or false to disable snapToGrid.
     * \see snapToGrid()
     */
    void setSnapToGrid( bool enable );

    /*!
     * \brief Sets the style sheet to be used for styling Nodes, Sockets, and Connections.
     * \param styleSheet The style sheet you want to use in JSON format.
     */
    void setNodeViewStyleMap( const QVariantMap& styleMap );

    /*!
     * \brief Sets the style sheet to be used for styling Nodes, Sockets, and Connections.
     * \param styleSheet The style sheet you want to use in JSON format.
     */
    void setNodeViewStyleSheet( const QString& styleSheet );

    /*!
     * \brief Sets the MiniMaps Position then moves the minimap
     * \param pos The corner in which you want the minimap
     */
    void setMiniMapPosition( const MinimapPosition pos );

    /*!
     * \brief Set a specific zoom level for the NodeView
     * \param zoomPercent The value to set the zoom to default = 1.0
     */
    void setZoom( qreal zoomPercent );

    /*!
     * \brief Shows the mini map for the NodeView view.
     */
    void showMiniMap();

    /*!
     * \brief Returns a bool that describes the current state of snapToGrid feature.
     * \return The bool value of whether the snapToGrid feature is currently turned on.
     * \see setSnapToGrid()
     */
    bool snapToGrid() const;

    /*!
     * \brief Scales the currently visable section of the NodeView view in or out by factor given.
     * \param zoomFactor The amount to zoom the NodeView view in or out.
     * \see pan()
     */
    void zoom( qreal zoomFactor );

    /*!
     * \brief Sets the zoom level to be the minimal to include the bounding box of all items
     */
    void zoomToItemBoundingRect();

    /*!
     * \brief Sets the zoom level to be the minimal to include the entire scene rect
     */
    void zoomToSceneRect();

  public slots:
    /*!
     * \brief Adds an allready created Node to the NodeView view.
     * \param node The Node you want added.
     */
    void addNode( Node* node );

    /*!
     * \brief Creates a Node with the passed perameters and adds it to the NodeView view.
     * \param inSockets The number of input sockets you want the node to have.
     * \param outSockets The number of output sockets you want the node to have.
     * \param nodeType The Node type for style sheet purposes.
     * \return The pointer to the node that was created.
     */
    Node* createNode( int inSockets = 0, int outSockets = 0, const QString& nodeType = "" );

    /*!
     * \brief Deletes the currently selected [Nodes](\ref Node) from the NodeView view.
     */
    void deleteSelectedNodes();

  signals:

    /*!
     * \brief This signal is emitted when a Connection has been made between two [Nodes](\ref Node).
     * \param sock The Input Socket of the new Connection.
     */
    void connectionCreated( Socket* sock );

    /*!
     * \brief This signal is emitted when a Connection is deleted from the NodeView view.
     */
    void connectionDeleted();

    /*!
     * \brief This signal is emitted when a Connection is currently under the mouse cursor.
     * \param conn The Connection that is under the mouse cursor
     */
    void connectionMouseOver( Connection* conn );

    /*!
     * \brief This signal is emitted when a Connection is selected within the NodeView view.
     * \param conn The Connection that is selected.
     */
    void connectionSelected( Connection* conn );

    /*!
     * \brief This signal is emitted when a Connection has been started from a Socket, but not yet completed.
     */
    void connectionStarted();

    /*!
     * \brief This signal is emitted when a Connection has been dropped on empty space in the view.
     * \param pos The QPointF that represents the position in the scene where the Connection was dropped.
     */
    void connectToEmpty( Socket* socket, QPointF pos );

    /*!
     * \brief disconnectToEmpty
     * \param socket
     * \param pos
     */
    void disconnectToEmpty( Socket* socket, QPointF pos );

    /*!
     * \brief This signal is emitted when the users performs a 'double click' of a mouse button within the NodeView
     * view.
     * \param event The data that was generated by the doubleClick event.
     */
    void doubleClick( QMouseEvent* event );

    /*!
     * \brief This signal is emitted when the NodeView view loses focus to another window or widget.
     * \param event The data that was generated by the focusOut event.
     */
    void focusOut( QFocusEvent* event );

    /*!
     * \brief This signal is emitted when the user moves the mouse around after a mousePress event has ocurred within
     * the NodeView view.
     * \param event The data that was generated by the mouseMove event.
     */
    void mouseMove( QMouseEvent* event );

    /*!
     * \brief This signal is emitted when the a mouse button is pressed down within the NodeView view.
     * \param event The data that was generated by the mousePress event.
     */
    void mousePress( QMouseEvent* event );

    /*!
     * \brief This signal is emitted when the user releases a mouse button after already pressing a mouse button within
     * the NodeView view.
     * \param event The data that was generated by the mouseRelease event.
     */
    void mouseRelease( QMouseEvent* event );

    /*!
     * \brief This signal is emitted whenever a Node is added to the NodeView view.
     * \param node The Node that is being added to the NodeView view.
     */
    void nodeAdded( Node* node );

    /*!
     * \brief This signal is emitted whenever a Node or Nodes are deleted from the NodeView view.
     */
    void nodeDeleted();

    /*!
     * \brief This signal is emitted when the user uses the mouse scroll wheel within the NodeView view.
     * \param event The data that was generated by the scrollWheel event.
     */
    void scrollWheel( QWheelEvent* event );

    /*!
     * \brief This signal is emitted whenever the selection within the NodeView view changes.
     */
    void selectionChanged();

    /*!
     * \brief This signal is emitted when a Connection is deleted from the NodeView view.
     */
    void zoomChanged();

  protected slots:
    /*!
     * \brief This function is called by a Connection object in the MagamFLUX when the Connection object handles a mouse
     * hover event.
     * \param conn This is the Connection that is currently being hovered over by the mouse.
     * \see connectionMouseOver
     */
    void onConnectionMouseOver( Connection* conn );

  protected:
    void registerStyleableItem( Styleable* styleItem );

    friend class Connection;
    friend class Socket;
    friend class Node;

  protected:
    virtual void drawBackground( QPainter* painter, const QRectF& rect );

    /*!
     * \brief focusOutEvent
     * \param event
     */
    virtual void focusOutEvent( QFocusEvent* event );

    /*!
     * \brief keyPressEvent
     * \param event
     */
    virtual void keyPressEvent( QKeyEvent* event );

    /*!
     * \brief mouseDoubleClickEvent
     * \param event
     */
    virtual void mouseDoubleClickEvent( QMouseEvent* event );

    /*!
     * \brief mouseMoveEvent
     * \param event
     */
    virtual void mouseMoveEvent( QMouseEvent* event );

    /*!
     * \brief mousePressEvent
     * \param event
     */
    virtual void mousePressEvent( QMouseEvent* event );

    /*!
     * \brief mouseReleaseEvent
     * \param event
     */
    virtual void mouseReleaseEvent( QMouseEvent* event );

    /*!
     * \brief scaleView
     * \param scaleFactor
     */
    virtual void scaleView( qreal scaleFactor );

    /*!
     * \brief wheelEvent
     * \param event
     */
    virtual void wheelEvent( QWheelEvent* event );

  private slots:
    void onConnectionCreated();
    void onConnectionDeleted();
    void onConnectionStarted();
    void onConnectToEmpty( Socket* socket, QPointF pos );
    void onDisconnectToEmpty( Socket* socket, QPointF pos );
    void onSelectionChanged();
};

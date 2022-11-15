// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/NodeView_global.h"
#include "NodeView/socket.h"

#include <QObject>

class NodeView;

/*!
 * \brief A subclass of QGraphicsPolygonItem that is used
 * to represent a node within a NodeView view.
 */
class NodeView_API FullLineSocket : public Socket {
    Q_OBJECT

  public:
    enum { Type = UserType + 123 };

    enum LabelPos { Left, Right, Center };

  protected:
    LabelPos m_labelPos;

  public:
    /*!
     * \brief Constructor.
     * \param type Whether this is an input or output Socket.
     * \param label The label that will appear on the Socket.
     * \param parent The parent item of the Socket.
     */
    FullLineSocket( NodeView* graphWidget, const SocketType type = Invalid, const QString label = "",
                    QGraphicsItem* parent = 0 );

    /*!
     * \brief Returns the position of the label within the node
     * \return the position of the label within the node
     */
    LabelPos labelPos();

    /*!
     * \brief Sets the position of the label within the node
     * \param LabelPos The position of the label within the node
     */
    void setLabelPos( LabelPos pos );

    /*!
     * \brief type
     * \return
     */
    virtual int type() const;

  protected:
    /*!
     * \brief constructGroup
     */
    virtual void constructGroup();

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

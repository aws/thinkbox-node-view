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
class NodeView_API TextlessSocket : public Socket {
    Q_OBJECT

  public:
    enum { Type = UserType + 6 };

    /*!
     * \brief Constructor.
     * \param type Whether this is an input or output Socket.
     * \param parent The parent item of the Socket.
     */
    TextlessSocket( NodeView* graphWidget, const SocketType type = Invalid, QGraphicsItem* parent = 0 );

    /*!
     * \brief Does nothing since there is no text (will be removed when we Redo the system
     * \return an empty QBrush
     */
    virtual QBrush labelBrush() const;

    /*!
     * \brief Does nothing since there is no text (will be removed when we Redo the system
     * \return an empty QPen
     */
    virtual QPen labelPen() const;

    /*!
     * \brief Does nothing since there is no text (will be removed when we Redo the system
     */
    virtual void setFont( const QFont font );

    /*!
     * \brief Does nothing since there is no text (will be removed when we Redo the system
     */
    virtual void setLabelBrush( const QBrush brush );

    /*!
     * \brief Does nothing since there is no text (will be removed when we Redo the system
     */
    virtual void setLabelPen( const QPen pen );

  protected:
    /*!
     * \brief constructGroup
     */
    virtual void constructGroup();

    /*!
     * \brief resizeGroup
     * \param width
     */
    virtual void resizeGroup( qreal width );
};

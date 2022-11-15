// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/NodeView_global.h"
#include "NodeView/node.h"

class NodeView_API SimpleNode : public Node {
    Q_OBJECT

  protected:
    qreal m_minHeight;

  public:
    enum { Type = UserType + 7 };

    SimpleNode( NodeView* graphWidget, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0 );

    /*!
     * \brief Adds a new input Socket to the Node.
     * \return The index of the new input Socket created.
     * \see getInputSocket(), getInputSocketCount(), deleteInputSocket()
     */
    virtual int addInputSocket();

    /*!
     * \brief Adds a new output Socket to the Node.
     * \return The index of the new output Socket created.
     * \see getOutputSocket(), getOutputSocketCount(), deleteOutputSocket()
     */
    virtual int addOutputSocket();

    /*!
     * \brief Gets the bounding rectangle of the Node
     * \return The bounding rectangle of the Node as a QRectF.
     */
    QRectF boundingRect() const;

    qreal minimumHeight() const;

    void setMinimumHeight( qreal height );

    /*!
     * \brief shape
     * \return
     */
    QPainterPath shape() const;

  protected:
    /*!
     * \brief resizeNode
     */
    virtual void resizeNode();
};

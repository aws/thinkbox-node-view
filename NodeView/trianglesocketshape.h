// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/NodeView_global.h"

#include <QGraphicsPolygonItem>

class NodeView_API TriangleSocketShape : public QGraphicsPolygonItem {
  public:
    enum { Type = UserType + 25 };

    enum Orientation { North, East, South, West, None };

  protected:
    Orientation m_orientation;
    QRectF m_rect;

  public:
    /*!
     * \brief A shape for that allows us to have triangular shaped Sockets
     * \param parent
     */
    TriangleSocketShape( QGraphicsItem* parent = 0 );

    /*!
     * \brief Returns the orientation in which the triangle has a point facing
     * \return The Orientaiton
     */
    Orientation orientation() const;

    /*!
     * \brief QRectF that the Triangle will be drawn within
     * \return The QRectf
     */
    QRectF rect() const;

    /*!
     * \brief Sets the orientation in which the triangle has a point facing
     * \param orientation
     */
    void setOrientation( Orientation orientation );

    /*!
     * \brief Sets the QrectF that the Triangle will be drawn within
     * \param The QRectF
     */
    void setRect( const QRectF& rect );

    /*!
     * \brief Sets the QrectF that the Triangle will be drawn within
     * \param x X Coordinate of the left side of the rect
     * \param y Y Coordiante of the top side of the rect
     * \param width Width of the rect
     * \param height Height of the rect
     */
    void setRect( qreal x, qreal y, qreal width, qreal height );

    /*!
     * \brief type
     * \return
     */
    virtual int type() const;

  protected:
    /*!
     * \brief generateTriangle
     */
    void generateTriangle();
};

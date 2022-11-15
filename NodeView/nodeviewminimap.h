// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QGraphicsView>
#include <QLabel>

class NodeView;
class MinimapDragLabel;

class NodeViewMiniMap : public QGraphicsView {
    Q_OBJECT

  private:
    bool m_mouseDrag;
    MinimapDragLabel* m_dragArea;
    NodeView* m_magmaView;

  public:
    /*!
     * \brief The NodeViewMiniMap is a minimap that can be displayed onto a NodeView
     * \param The MagmaFlux it can be displayed onto
     * \param The parent object of the NodeViewMiniMap
     */
    explicit NodeViewMiniMap( NodeView* magmaView, QWidget* parent = 0 );

    /*!
     * \brief magmaView
     * \return
     */
    NodeView* magmaView();

    /*!
     * \brief Move the dragArea of the minimap to the correct corner
     */
    void moveDragArea();

  protected:
    /*!
     * \brief drawForeground
     * \param painter
     * \param rect
     */
    virtual void drawForeground( QPainter* painter, const QRectF& rect );

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
     * \brief resizeEvent
     * \param event
     */
    virtual void resizeEvent( QResizeEvent* event );

  private:
    QList<QRectF> calculateUnusedAreas();

  private slots:
    void adjust();
};
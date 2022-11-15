// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QLabel>
#include <QPoint>

class NodeView;
class NodeViewMiniMap;

class MinimapDragLabel : public QWidget {
    Q_OBJECT

  private:
    NodeViewMiniMap* m_miniMap;
    QPoint m_currentPos;
    bool m_drag;

  public:
    /*!
     * \brief MinimapDragLabel The dragable area of a NodeViewMiniMap which appears in the central most corner of the
     * minimap
     * \param NodeViewMiniMap The minimap that the drag label is attached to
     * \param QWidget The Parent object of the minimap drag label
     */
    explicit MinimapDragLabel( NodeViewMiniMap* miniMap, QWidget* parent = 0 );

  protected:
    /*!
     * \brief enterEvent
     * \param event
     */
    void enterEvent( QEvent* event );

    /*!
     * \brief leaveEvent
     * \param event
     */
    void leaveEvent( QEvent* event );

    /*!
     * \brief mouseMoveEvent
     * \param event
     */
    void mouseMoveEvent( QMouseEvent* event );

    /*!
     * \brief mousePressEvent
     * \param event
     */
    void mousePressEvent( QMouseEvent* event );

    /*!
     * \brief mouseReleaseEvent
     * \param event
     */
    void mouseReleaseEvent( QMouseEvent* event );
};

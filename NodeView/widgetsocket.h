// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/socket.h"

#include <QGraphicsProxyWidget>

class QWidget;
class NodeView;

class WidgetSocket : public Socket {
    Q_OBJECT
  protected:
    QGraphicsProxyWidget m_proxyWidget;

  public:
    /*!
     * \brief WidgetSocket
     * \param graphWidget
     * \param widget
     * \param type
     * \param label
     * \param parent
     */
    explicit WidgetSocket( NodeView* graphWidget, QWidget* widget = 0, const SocketType type = Invalid,
                           const QString label = "", QGraphicsItem* parent = 0 );

    /*!
     * \brief Sets the Widget contained within the Socket
     * \param The widget contained within the socket
     */
    void setWidget( QWidget* widget );

    /*!
     * \brief update
     * \param rect
     */
    void update( const QRectF& rect );

    /*!
     * \brief Return a pointer to the widget within the socket
     * \return A pointer to the widget within the socket
     */
    QWidget* widget() const;

  protected:
    /*!
     * \brief constructGroup
     */
    virtual void constructGroup();
};

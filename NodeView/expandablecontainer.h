// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/NodeView_global.h"
#include "NodeView/stylemanager.h"

#include <QFont>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QString>

class Connection;
class NodeView;

/*!
 * \brief [Sockets](\ref Socket) are used to make connections between [Nodes](\ref Node) using Connection objects.
 */
class NodeView_API ExpandableContainer : public QGraphicsItemGroup {
  public:
    /*!
     * \brief Definition of type ID for Socket objects. All QGrpahicsItems need a type ID.
     */
    enum { Type = UserType + 95 };

    /*!
     * \brief Locations of where the expansion button can appear
     */
    enum ExpansionButtonPosition { ButtonLeft, ButtonRight };

    /*!
     * \brief Title justify for the title text
     */
    enum TitleLocation { TitleLeft, TitleRight, TitleCenter };

  private:
    QGraphicsItem* m_containedItem;
    QGraphicsSimpleTextItem m_label;
    QString m_labelText;
    bool m_contentsVisible;
    QGraphicsPixmapItem m_picture;
    int m_width;

    TitleLocation m_titleLoc;
    ExpansionButtonPosition m_expansionPos;
    int m_leftAdjust;
    int m_rightAdjust;

  public:
    /*!
     * \brief Constructor.
     * \param type Whether this is an input or output Socket.
     * \param label The label that will appear on the Socket.
     * \param parent The parent item of the Socket.
     */
    ExpandableContainer( QGraphicsItem* containedItem = NULL, const QString label = "", QGraphicsItem* parent = 0,
                         ExpansionButtonPosition expPos = ButtonRight, TitleLocation titleLoc = TitleLeft );

    /*!
     * \brief Returns the side at which the expansion button will appear
     * \return returns the location according to the enum ExpansionButtonPosition
     */
    ExpansionButtonPosition buttonPosition();

    /*!
     * \brief Expands/shrinks the central area of the container
     * \param expand or contract (true = expand)
     */
    void expandArea( bool expanded );

    /*!
     * \brief Returns the currently set font.
     * \return The QFont obeject that describes the currently set font being used for the text.
     */
    QFont font() const;

    /*!
     * \brief Returns the current item contained in the container
     * \return The QGraphicsItem contained
     */
    QGraphicsItem* getContainedItem();

    /*!
     * \brief Returns true if the expnasion button is currently under the mouses pointer
     * \return true if expansion button is currently under the mouse
     */
    bool isPixmapUnderMouse() const;

    /*!
     * \brief Returns the text from the label(contents) of the container
     * \return the text from the label of the container
     */
    QString label() const;

    /*!
     * \brief Sets the location of the button either top left or top right
     * \param The position to put the button in.
     */
    void setButtonLocation( ExpansionButtonPosition expansionPos );

    /*!
     * \brief Sets the font used by the text label.
     * \param font The QFont object that desbribes the font desired to be used for the text in the Socket.
     */
    void setFont( const QFont font );

    /*!
     * \brief Sets the text for the label (contents) of the container
     * \param QString The string you want to the label to be set to
     */
    void setLabel( const QString label );

    /*!
     * \brief Sets the amount that the inner data is pushed in from the sides (adjustment for sockets)
     * \param int The amuount to push the data in.
     */
    void setLeftAdjust( int adjust );

    /*!
     * \brief Sets teh brush that will be used by the text in the title
     * \param The brush to be used
     */
    void setTitleBrush( QBrush brush );

    /*!
     * \brief Justify the title within the container
     * \param The direction in which you wish the title to be justified
     */
    void setTitleLocation( TitleLocation titleLoc );

    /*!
     * \brief Sets the Brush for the label (title) of the container
     * \param QBrush The Brush you want to the label to use
     */
    QBrush titleBrush() const;

    /*!
     * \brief Set the maximum width the expandable container can take
     * \param width
     */
    void setWidth( int width );

    /*!
     * \brief Title height retrieves the height of the "title section" (button and title text)
     * \return The height of the tile section
     */
    qreal titleHeight();

    /*!
     * \brief Retrieve the justification placed on the title
     * \return The position of the title
     */
    TitleLocation titleLocation();

    /*!
     * \brief type
     * \return
     */
    int type() const;

    /*!
     * \brief update
     */
    void update();

  protected:
    /*!
     * \brief mouseDoubleClickEvent
     * \param event
     */
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief mousePressEvent
     * \param event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief mouseReleaseEvent
     * \param event
     */
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );

    /*!
     * \brief constructGroup
     */
    void constructGroup();
};

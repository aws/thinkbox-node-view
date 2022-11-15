// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "NodeView/NodeView_global.h"
#include "NodeView/node.h"

class NodeView;
class ExpandableContainer;

/*!
 * \brief A subclass of QGraphicsPolygonItem that is used
 * to represent a node within a NodeView view.
 */
class NodeView_API SideSocketNode : public Node {
    Q_OBJECT

  public:
    enum { Type = UserType + 5 };

    /*!
     * \brief Constructor
     * \param graphWidget The NodeView class that the Node will exist whthin.
     * \param parent The QGraphicsItem parent item of the Node.
     * \param scene The QGraphicsScene that the Node will be
     * displayed within.
     */
    SideSocketNode( NodeView* graphWidget, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0 );

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
     * \brief Returns the font currently being used for the detail text.
     * \return The QFont the the detail text is using.
     */
    virtual QFont detailTextFont() const;

    /*!
     * \brief Returns the expandable container contained
     * \return the ExpandableContainer contained in the node
     */
    ExpandableContainer* expandableArea();

    /*!
     * \brief Returns the title of the expandabelContainer within
     * \return THe title of the expandable title
     */
    QString expandableTitle() const;

    /*!
     * \brief Returns the font currently being used for the expandableContainer.
     * \return The QFont the the expandableContainer is using.
     */
    QFont expandableTitleFont() const;

    /*!
     * \brief Expand the expandable container
     * \param Expanded or closed
     */
    void expandArea( bool expanded );

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
     * \brief Sets the ExpandableContainer contained
     * \param ExpandableContainer The expandableContainer you want the node to contain
     */
    void setExpandableArea( ExpandableContainer* container );

    /*!
     * \brief Sets the text to be displayed as the Title of the Expandable container text.
     * \param text The QString that contains the text to be displayed.
     */
    void setExpandableTitle( const QString& text );

    void setAreaExpanded();

    /*!
     * \brief Sets the text font for the Expandable Container title text.
     * \param font The QFont that you want the font to be.
     */
    void setExpandableTitleFont( const QFont& font );

    /*!
     * \brief This returns the user defined type of a Node
     * QGraphicsItem. All
     * QGrpahicsItems need to have a type in order to properly
     * function within a QGraphicsScene.
     * \return The Node object type identifier.
     */
    virtual int type() const;

  protected:
    /*!
     * \brief This function is called whenever a new input or output Socket is added/removed from the Node. It
     * calculates its new size and puts all the components in the proper position.
     */
    virtual void resizeNode();

    /*!
     * \brief resizeTitleBar
     */
    virtual void resizeTitleBar();

  private:
    bool m_areaExpanded;
    ExpandableContainer* m_container;
};

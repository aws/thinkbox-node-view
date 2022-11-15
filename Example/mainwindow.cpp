// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFontDialog>

#include <NodeView/connection.h>
#include <NodeView/expandablecontainer.h>
#include <NodeView/fulllinesocket.h>
#include <NodeView/node.h>
#include <NodeView/nodeview.h>
#include <NodeView/sidesocketnode.h>
#include <NodeView/simplenode.h>
#include <NodeView/socket.h>

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent )
    , ui( new Ui::MainWindow ) {
    ui->setupUi( this );

    m_selectedNode = 0;
    m_selectedSocket = 0;

    // Connect up the 'Node Count' label
    onNodeAddedOrDeleted();
    connect( ui->magmaView, SIGNAL( nodeAdded( Node* ) ), this, SLOT( onNodeAddedOrDeleted() ) );
    connect( ui->magmaView, SIGNAL( nodeDeleted() ), this, SLOT( onNodeAddedOrDeleted() ) );

    // Connect up the add and delete buttons.
    connect( ui->addNodeButton, SIGNAL( clicked() ), this, SLOT( onAddNode() ) );
    connect( ui->addSimpleNodeButton, SIGNAL( clicked() ), this, SLOT( onAddSimpleNode() ) );
    connect( ui->addSideNodeButton, SIGNAL( clicked() ), this, SLOT( onAddSideNode() ) );
    connect( ui->deleteSelectionBtn, SIGNAL( clicked() ), ui->magmaView, SLOT( deleteSelectedNodes() ) );

    // Set up 'Snap to Grid' checkbox.
    ui->snapToGridBox->setChecked( ui->magmaView->snapToGrid() );
    connect( ui->snapToGridBox, SIGNAL( toggled( bool ) ), this, SLOT( onSnapToGridBoxToggled( bool ) ) );

    connect( ui->dropShadowBox, SIGNAL( toggled( bool ) ), this, SLOT( onDropShadowBoxToggled( bool ) ) );

    // Set up 'Grid Lines' checkbox.
    ui->gridLinesBox->setChecked( ui->magmaView->gridLines() );
    connect( ui->gridLinesBox, SIGNAL( toggled( bool ) ), this, SLOT( onGridLinesBoxToggled( bool ) ) );

    ui->creationPointBox->setChecked( ui->magmaView->isCreationPointMarkerVisible() );
    connect( ui->creationPointBox, SIGNAL( toggled( bool ) ), this, SLOT( onCreationPointBoxToggled( bool ) ) );

    // Set up the color change buttons.
    connect( ui->changeGridColorBtn, SIGNAL( clicked() ), this, SLOT( onChangeGridColorBtnClicked() ) );
    connect( ui->changeMagmaBackgroundColorBtn, SIGNAL( clicked() ), this,
             SLOT( onChangeMagmaBackgroundColorBtnClicked() ) );

    connect( ui->moveMiniButton, SIGNAL( clicked() ), this, SLOT( onMoveMiniButtonClicked() ) );
    connect( ui->zoomBox, SIGNAL( valueChanged( double ) ), this, SLOT( onZoomChanged() ) );

    ui->gridSizeSpinBox->setValue( ui->magmaView->gridSize() );
    connect( ui->gridSizeSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( onGridSizeChanged() ) );

    // Connect up the 'Selection Count' label.
    onSelectionChanged();
    connect( ui->magmaView, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );

    connect( ui->magmaView, SIGNAL( scrollWheel( QWheelEvent* ) ), this, SLOT( onScroll() ) );

    /*** Socket Properties Setup ***/
    // Connect basic color properties and Node name.
    connect( ui->magmaView, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
    connect( ui->nodeNameEdit, SIGNAL( returnPressed() ), this, SLOT( onNodeNameEditReturnPressed() ) );
    connect( ui->detailTextEdit, SIGNAL( textChanged() ), this, SLOT( onDetailTextEditTextChanged() ) );
    connect( ui->enableDetailText, SIGNAL( toggled( bool ) ), this, SLOT( onEnableDetailText( bool ) ) );
    connect( ui->changeBackgroundColorBtn, SIGNAL( clicked() ), this, SLOT( onChangeBackgroundColorBtnClicked() ) );
    connect( ui->changeBackgroundHighlightColorBtn, SIGNAL( clicked() ), this,
             SLOT( onChangeBackgroundHighlightColorBtnClicked() ) );
    connect( ui->changeForegroundColorBtn, SIGNAL( clicked() ), this, SLOT( onChangeForegroundColorBtnClicked() ) );
    connect( ui->changeForegroundHighlightColorBtn, SIGNAL( clicked() ), this,
             SLOT( onChangeForegroundHighlightColorBtnClicked() ) );

    connect( ui->changeBorderColorBtn, SIGNAL( clicked() ), this, SLOT( onChangeBorderColorBtnClicked() ) );
    connect( ui->changeBorderHighlightColorBtn, SIGNAL( clicked() ), this,
             SLOT( onChangeBorderColorHighlightBtnClicked() ) );

    connect( ui->changeDetailColorBtn, SIGNAL( clicked() ), this, SLOT( onChangeDetailColorBtnClicked() ) );
    connect( ui->changeDetailFontBtn, SIGNAL( clicked() ), this, SLOT( onChangeDetailFontBtnClicked() ) );

    connect( ui->changeTitleTextFontBtn, SIGNAL( clicked() ), this, SLOT( onChangeTitleTextFont() ) );
    connect( ui->changeTitleTextColorBtn, SIGNAL( clicked() ), this, SLOT( onChangeTitleTextColor() ) );

    // Connect adding/deleting of input/output sockets.
    connect( ui->addInputSocketBtn, SIGNAL( clicked() ), this, SLOT( onAddInputSocketBtnClicked() ) );
    connect( ui->addOutputSocketBtn, SIGNAL( clicked() ), this, SLOT( onAddOutputSocketBtnClicked() ) );
    connect( ui->deleteInputSocketBtn, SIGNAL( clicked() ), this, SLOT( onDeleteInputSocketBtnClicked() ) );
    connect( ui->deleteOutputSocketBtn, SIGNAL( clicked() ), this, SLOT( onDeleteOutputSocketBtnClicked() ) );
    connect( ui->fullLineInputSocketBtn, SIGNAL( clicked() ), this, SLOT( onAddFullLineInputSocketBtnClicked() ) );
    connect( ui->fullLineOutputSocketBtn, SIGNAL( clicked() ), this, SLOT( onAddFullLineOutputSocketBtnClicked() ) );

    /*** Socket Properties Setup ***/
    // Connect row change notifications for getting Socket properties.
    connect( ui->inputSocketListWidget, SIGNAL( itemClicked( QListWidgetItem* ) ), this,
             SLOT( onSocketItemClicked( QListWidgetItem* ) ) );
    connect( ui->outputSocketListWidget, SIGNAL( itemClicked( QListWidgetItem* ) ), this,
             SLOT( onSocketItemClicked( QListWidgetItem* ) ) );

    // Connect name change for Socket.
    connect( ui->socketNameEdit, SIGNAL( returnPressed() ), this, SLOT( onSocketNameEditReturnPressed() ) );

    // Connect Socket shape properties
    connect( ui->circleRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onSocketTypeToggled( bool ) ) );
    connect( ui->squareRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onSocketTypeToggled( bool ) ) );
    connect( ui->triangleRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onSocketTypeToggled( bool ) ) );

    connect( ui->northRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onChangeTriangleOrientation( bool ) ) );
    connect( ui->eastRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onChangeTriangleOrientation( bool ) ) );
    connect( ui->westRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onChangeTriangleOrientation( bool ) ) );
    connect( ui->southRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onChangeTriangleOrientation( bool ) ) );

    // Connect change Socket color properties
    connect( ui->changeOutlineColorBtn, SIGNAL( clicked() ), this, SLOT( onChangeOutlineColorBtnClicked() ) );
    connect( ui->changeFillColorBtn, SIGNAL( clicked() ), this, SLOT( onChangeFillColorBtnClicked() ) );
    connect( ui->changeTextColorBtn, SIGNAL( clicked() ), this, SLOT( onChangeTextColorBtnClicked() ) );

    connect( ui->cornerXRadiusBox, SIGNAL( valueChanged( double ) ), this, SLOT( onRadiusXChanged() ) );
    connect( ui->cornerYRadiusBox, SIGNAL( valueChanged( double ) ), this, SLOT( onRadiusYChanged() ) );

    connect( ui->enablePixmapButton, SIGNAL( toggled( bool ) ), this, SLOT( onEnablePixmapButtonToggled( bool ) ) );
    connect( ui->changePixmapButton, SIGNAL( clicked() ), this, SLOT( onChangePixmap() ) );

    connect( ui->topPosRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onPixmapPosToggled( bool ) ) );
    connect( ui->leftPosRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onPixmapPosToggled( bool ) ) );
    connect( ui->rightPosRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onPixmapPosToggled( bool ) ) );
    connect( ui->bottomPosRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onPixmapPosToggled( bool ) ) );
    connect( ui->centerPosRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onPixmapPosToggled( bool ) ) );
    connect( ui->setPixmapSizebtn, SIGNAL( clicked() ), this, SLOT( onSetPixmapSize() ) );
    connect( ui->minNodeWidthSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( onMinWidthChanged() ) );
    connect( ui->socketBufferSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( onChangeSocketBuffer() ) );

    connect( ui->socketShapeSizeBox, SIGNAL( valueChanged( int ) ), this, SLOT( onSocketShapeSizeChanged( int ) ) );

    connect( ui->deleteAllBtn, SIGNAL( pressed() ), this, SLOT( onDeleteAll() ) );
    connect( ui->leftJustifyRadio, SIGNAL( clicked( bool ) ), this, SLOT( onJustifyChanged( bool ) ) );
    connect( ui->centerJustifyRadio, SIGNAL( clicked( bool ) ), this, SLOT( onJustifyChanged( bool ) ) );
    connect( ui->rightJustifyRadio, SIGNAL( clicked( bool ) ), this, SLOT( onJustifyChanged( bool ) ) );

    connect( ui->elideCenterRadio, SIGNAL( clicked( bool ) ), this, SLOT( onElideChanged( bool ) ) );
    connect( ui->elideLeftRadio, SIGNAL( clicked( bool ) ), this, SLOT( onElideChanged( bool ) ) );
    connect( ui->elideCenterRadio, SIGNAL( clicked( bool ) ), this, SLOT( onElideChanged( bool ) ) );
    connect( ui->elideRightRadio, SIGNAL( clicked( bool ) ), this, SLOT( onElideChanged( bool ) ) );

    connect( ui->minimapBox, SIGNAL( clicked( bool ) ), this, SLOT( minimapChanged( bool ) ) );

    connect( ui->animationDurationBox, SIGNAL( valueChanged( int ) ), this, SLOT( onSocketDurationChanged() ) );
    connect( ui->animationLightnessBox, SIGNAL( valueChanged( int ) ), this, SLOT( onSocketLightnessChanged() ) );
    connect( ui->animationEnabledBox, SIGNAL( clicked( bool ) ), this, SLOT( onSocketAnimtionEnabledChanged() ) );

    connect( ui->multipleConnectionsCheckBox, SIGNAL( clicked() ), this, SLOT( onMultipleConnectionsChanged() ) );

    connect( ui->connectionDurationBox, SIGNAL( valueChanged( int ) ), this,
             SLOT( onConnectionAnimationDurationChanged() ) );
    connect( ui->connectionLightnessBox, SIGNAL( valueChanged( int ) ), this,
             SLOT( onConnectionAnimationLightnessChanged() ) );
    connect( ui->arrowPercentBox, SIGNAL( valueChanged( double ) ), this, SLOT( onConnectionArrowPositionChanged() ) );
    connect( ui->arrowSizeBox, SIGNAL( valueChanged( int ) ), this, SLOT( onConnectionArrowSizeChanged() ) );
    connect( ui->curvatureBox, SIGNAL( valueChanged( double ) ), this, SLOT( onConnectionCurvatureChanged() ) );
    connect( ui->dragDistanceBox, SIGNAL( valueChanged( double ) ), this, SLOT( onConnectionDragDistanceChanged() ) );
    connect( ui->connectionColorBtn, SIGNAL( clicked() ), this, SLOT( onConnectionChangeColor() ) );
    connect( ui->connectionBrushBtn, SIGNAL( clicked() ), this, SLOT( onConnectionBrushChanged() ) );
    connect( ui->connectionPenBtn, SIGNAL( clicked() ), this, SLOT( onConnectionPenChanged() ) );
    connect( ui->connectionSelectedColorBtn, SIGNAL( clicked() ), this, SLOT( onConnectionSelectedColorChanged() ) );
    connect( ui->connectionSelectedBrushBtn, SIGNAL( clicked() ), this, SLOT( onConnectionSelectedBrushChanged() ) );
    connect( ui->connectionSelectedPenBtn, SIGNAL( clicked() ), this, SLOT( onConnectionSelectedPenChanged() ) );
    connect( ui->connectionAnimationEnabled, SIGNAL( clicked( bool ) ), this,
             SLOT( onConnectionAnimationEnabledChanged() ) );

    connect( ui->StyleSheetEdit, SIGNAL( textChanged() ), this, SLOT( onStyleSheetChanged() ) );

    connect( ui->sideSocketTitle, SIGNAL( returnPressed() ), this, SLOT( onDetailTitleTextChanged() ) );
    connect( ui->sideSocketTitleFont, SIGNAL( clicked() ), this, SLOT( onDetailTitleFontChanged() ) );
    connect( ui->sideSocketTitleColor, SIGNAL( clicked() ), this, SLOT( onDetailTitleColorChanged() ) );

    connect( ui->expandableTitleLeftBtn, SIGNAL( clicked( bool ) ), this,
             SLOT( onDetailTitleLocationChanged( bool ) ) );
    connect( ui->expandableTitleCenterBtn, SIGNAL( clicked( bool ) ), this,
             SLOT( onDetailTitleLocationChanged( bool ) ) );
    connect( ui->expandableTitleRightBtn, SIGNAL( clicked( bool ) ), this,
             SLOT( onDetailTitleLocationChanged( bool ) ) );

    connect( ui->expansionButtonLeftBtn, SIGNAL( clicked( bool ) ), this,
             SLOT( onExpansionButtonLocationChanged( bool ) ) );
    connect( ui->expansionButtonRightBtn, SIGNAL( clicked( bool ) ), this,
             SLOT( onExpansionButtonLocationChanged( bool ) ) );

    connect( ui->nodeObjectNameEdit, SIGNAL( textChanged( QString ) ), this, SLOT( onNodeObjectNameChanged() ) );
    connect( ui->socketObjectNameEdit, SIGNAL( textChanged( QString ) ), this, SLOT( onSocketObjectNameChanged() ) );
    connect( ui->connectionObjectNameEdit, SIGNAL( textChanged( QString ) ), this,
             SLOT( onConnectionObjectNameChanged() ) );

    connect( ui->topAlignmentRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onSocketAlignmentChanged( bool ) ) );
    connect( ui->centerAlignmentRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onSocketAlignmentChanged( bool ) ) );
    connect( ui->bottomAlignmentRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onSocketAlignmentChanged( bool ) ) );
    connect( ui->dragEnabledCheckBox, SIGNAL( clicked( bool ) ), this, SLOT( onDragEnabledChanged( bool ) ) );

    connect( ui->elideRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onLabelStyleChanged( bool ) ) );
    connect( ui->wrapRadioBtn, SIGNAL( clicked( bool ) ), this, SLOT( onLabelStyleChanged( bool ) ) );

    ui->magmaView->setGridSize( 20 );
    ui->gridSizeSpinBox->setValue( 20 );
    ui->magmaView->setNodeViewStyleSheet( "({\"Node#test\":{\"backgroundBrush\":{\"color\":{rgb:[200,0,0]}}}})" );
    ui->StyleSheetEdit->setText( "({\"Node#test\":{\"backgroundBrush\":{\"color\":{rgb:[200,0,0]}}}})" );

    connect( ui->nodeCountTestBtn, SIGNAL( clicked() ), this, SLOT( OnPerformNodeCountTest() ) );
    connect( ui->connectionTestBtn, SIGNAL( clicked() ), this, SLOT( OnPerformConnectionTest() ) );
    connect( ui->meshTestBtn, SIGNAL( clicked() ), this, SLOT( OnPerformMeshTest() ) );

    /*Node *node1 = ui->magmaView->createNode();
    node1->setObjectName("test");
    int outSocket = node1->addOutputSocket();

    Node *node2 = ui->magmaView->createNode();
    node2->setObjectName("test");
    int inSocket = node2->addInputSocket();
    node1->getOutputSocket(outSocket)->createConnection(node2->getInputSocket(inSocket));

    connect(ui->magmaView,SIGNAL(doubleClick(QMouseEvent*)),this,SLOT(printCheck()));
    */

    // ui->magmaView->setRubberBandSelectionMode(Qt::ContainsItemShape);*/
    ui->propertyTabs->setCurrentIndex( 0 );
}

void MainWindow::printCheck() {}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onDragEnabledChanged( bool checked ) {
    if( this->m_selectedSocket ) {
        m_selectedSocket->setDragEnabled( checked );
    }
}

void MainWindow::onNodeObjectNameChanged() {
    if( this->m_selectedNode ) {
        m_selectedNode->setObjectName( ui->nodeObjectNameEdit->text() );
    }
}

void MainWindow::onSocketObjectNameChanged() {
    if( this->m_selectedSocket ) {
        m_selectedSocket->setObjectName( ui->socketObjectNameEdit->text() );
    }
}

void MainWindow::onConnectionObjectNameChanged() {
    if( this->m_selectedConnection ) {
        m_selectedConnection->setObjectName( ui->connectionObjectNameEdit->text() );
    }
}

void MainWindow::onMinWidthChanged() {
    if( this->m_selectedNode ) {
        m_selectedNode->setMinimumWidth( this->ui->minNodeWidthSpinBox->value() );
    }
}

void MainWindow::onDeleteAll() { ui->magmaView->deleteAllNodes(); }

void MainWindow::onChangePixmap() {
    if( this->m_selectedNode ) {
        QString fileName = QFileDialog::getOpenFileName( this, "Pixmap Selection", "", "Any File Type (*.*)" );
        m_selectedNode->setPixmap( QPixmap( fileName ) );
        this->ui->pixmapWidthSpinBox->setValue( m_selectedNode->pixmapSize().width() );
        this->ui->pixmapHeightSpinBox->setValue( m_selectedNode->pixmapSize().height() );
    }
}

void MainWindow::onChangeSocketBuffer() {
    if( this->m_selectedNode ) {
        this->m_selectedNode->setSocketBuffer( this->ui->socketBufferSpinBox->value() );
    }
}

void MainWindow::onSetPixmapSize() {
    if( this->m_selectedNode ) {
        this->m_selectedNode->setPixmapSize(
            QSize( this->ui->pixmapWidthSpinBox->value(), this->ui->pixmapHeightSpinBox->value() ) );
    }
}

void MainWindow::onRadiusXChanged() {
    if( this->m_selectedNode ) {
        m_selectedNode->setCornerXRadius( ui->cornerXRadiusBox->value() );
    }
}

void MainWindow::onRadiusYChanged() {
    if( this->m_selectedNode ) {
        m_selectedNode->setCornerYRadius( ui->cornerYRadiusBox->value() );
    }
}

void MainWindow::onAddNode() { ui->magmaView->createNode(); }

void MainWindow::onScroll() { this->ui->zoomBox->setValue( this->ui->magmaView->transform().m11() ); }

void MainWindow::onAddSideNode() {
    SideSocketNode* newNode = new SideSocketNode( ui->magmaView );
    newNode->setDetailText( "This is a test" );
    newNode->setExpandableTitle( "this is a title" );
    newNode->expandableArea()->setButtonLocation( ExpandableContainer::ButtonLeft );
    // newNode->expandableArea()->setTitleLocation(ExpandableContainer::TitleCenter);
    ui->magmaView->addNode( newNode );

    newNode->expandArea( false );
}

void MainWindow::onAddSimpleNode() {
    SimpleNode* newNode = new SimpleNode( ui->magmaView );
    ui->magmaView->addNode( newNode );
}

void MainWindow::onStyleSheetChanged() {
    QString styleSheet = ui->StyleSheetEdit->toPlainText();
    ui->magmaView->setNodeViewStyleSheet( ui->StyleSheetEdit->toPlainText() );
    QList<QGraphicsItem*> selectedItems = ui->magmaView->scene()->selectedItems();
    foreach( QGraphicsItem* item, selectedItems ) {
        item->setSelected( false );
    }
}

void MainWindow::onAddInputSocketBtnClicked() {
    if( this->m_selectedNode ) {
        Node* dNode = this->m_selectedNode;
        int socketIndex = dNode->addInputSocket();
        Socket* socket = dNode->getInputSocket( socketIndex );
        ui->inputSocketListWidget->addItem( socket->socketName() );
    }
}

void MainWindow::onAddOutputSocketBtnClicked() {
    if( this->m_selectedNode ) {
        Node* dNode = this->m_selectedNode;
        int socketIndex = dNode->addOutputSocket();
        Socket* socket = dNode->getOutputSocket( socketIndex );
        ui->outputSocketListWidget->addItem( socket->socketName() );
    }
}

void MainWindow::onAddFullLineInputSocketBtnClicked() {
    if( this->m_selectedNode ) {
        Node* dNode = this->m_selectedNode;
        int socketIndex = dNode->addInputFullLineSocket();
        FullLineSocket* sock = qgraphicsitem_cast<FullLineSocket*>( dNode->getInputSocket( socketIndex ) );
        ui->inputSocketListWidget->addItem( sock->socketName() );
    }
}

void MainWindow::onAddFullLineOutputSocketBtnClicked() {
    if( this->m_selectedNode ) {
        Node* dNode = this->m_selectedNode;
        int socketIndex = dNode->addOutputFullLineSocket();
        FullLineSocket* sock = qgraphicsitem_cast<FullLineSocket*>( dNode->getOutputSocket( socketIndex ) );
        ui->outputSocketListWidget->addItem( sock->socketName() );
    }
}

void MainWindow::onChangeOutlineColorBtnClicked() {
    if( this->m_selectedSocket ) {
        QColor color =
            QColorDialog::getColor( this->m_selectedSocket->outlineColor(), this, "", QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedSocket->setOutlineColor( color );
    }
}

void MainWindow::onChangeFillColorBtnClicked() {
    if( this->m_selectedSocket ) {
        QColor color =
            QColorDialog::getColor( this->m_selectedSocket->fillColor(), this, "", QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedSocket->setFillColor( color );
    }
}

void MainWindow::onChangeTextColorBtnClicked() {
    if( this->m_selectedSocket ) {
        QColor color =
            QColorDialog::getColor( this->m_selectedSocket->labelColor(), this, "", QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedSocket->setLabelColor( color );
    }
}

void MainWindow::onChangeBackgroundColorBtnClicked() {
    if( this->m_selectedNode ) {
        QColor color = QColorDialog::getColor( this->m_selectedNode->backgroundBrush().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedNode->setBackgroundBrush( QBrush( color ) );
    }
}

void MainWindow::onChangeBackgroundHighlightColorBtnClicked() {
    if( this->m_selectedNode ) {
        QColor color = QColorDialog::getColor( this->m_selectedNode->selectedBrush().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedNode->setSelectedBrush( QBrush( color ) );
    }
}

void MainWindow::onChangeForegroundColorBtnClicked() {
    if( this->m_selectedNode ) {
        QColor color = QColorDialog::getColor( this->m_selectedNode->titleBarBrush().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() ) {
            this->m_selectedNode->setTitleBarBrush( QBrush( color ) );
            // this->m_selectedNode->setTitleBarPen(QPen(color));
        }
    }
}

void MainWindow::onChangeBorderColorBtnClicked() {
    if( this->m_selectedNode ) {
        QPen pen = this->m_selectedNode->outlinePen();
        QColor color = QColorDialog::getColor( this->m_selectedNode->outlinePen().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() ) {
            pen.setColor( color );
            this->m_selectedNode->setOutlinePen( pen );
            // this->m_selectedNode->setTitleBarPen(QPen(color));
        }
    }
}

void MainWindow::onChangeDetailColorBtnClicked() {
    if( this->m_selectedNode ) {
        QColor color =
            QColorDialog::getColor( this->m_selectedNode->detailTextColor(), this, "", QColorDialog::ShowAlphaChannel );
        if( color.isValid() ) {
            this->m_selectedNode->setDetailTextColor( color );
            // this->m_selectedNode->setTitleBarPen(QPen(color));
        }
    }
}

void MainWindow::onChangeTitleTextColor() {
    if( this->m_selectedNode ) {
        QBrush brush = this->m_selectedNode->titleTextBrush();
        QColor color = QColorDialog::getColor( brush.color(), this, "", QColorDialog::ShowAlphaChannel );
        if( color.isValid() ) {
            brush.setColor( color );
            this->m_selectedNode->setTitleTextBrush( brush );
            // this->m_selectedNode->setTitleBarPen(QPen(color));
        }
    }
}

void MainWindow::onChangeTitleTextFont() {
    if( this->m_selectedNode ) {
        bool ok;
        QFont font = QFontDialog::getFont( &ok, this->m_selectedNode->titleTextFont(), this );
        if( ok ) {
            this->m_selectedNode->setTitleTextFont( font );
            // this->m_selectedNode->setTitleBarPen(QPen(color));
        }
    }
}

void MainWindow::onDetailTitleFontChanged() {
    if( this->m_selectedNode ) {

        SideSocketNode* ssNode = dynamic_cast<SideSocketNode*>( this->m_selectedNode );
        if( ssNode ) {
            bool ok;
            QFont font = QFontDialog::getFont( &ok, ssNode->expandableTitleFont(), this );
            if( ok ) {
                ssNode->setExpandableTitleFont( font );
                // this->m_selectedNode->setTitleBarPen(QPen(color));
            }
        }
    }
}

void MainWindow::onChangeDetailFontBtnClicked() {
    if( this->m_selectedNode ) {
        bool ok;
        QFont font = QFontDialog::getFont( &ok, this->m_selectedNode->detailTextFont(), this );
        if( ok ) {
            this->m_selectedNode->setDetailTextFont( font );
            // this->m_selectedNode->setTitleBarPen(QPen(color));
        }
    }
}

void MainWindow::onChangeBorderColorHighlightBtnClicked() {
    if( this->m_selectedNode ) {
        QPen pen = this->m_selectedNode->selectedPen();
        QColor color = QColorDialog::getColor( this->m_selectedNode->selectedPen().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() ) {
            pen.setColor( color );
            this->m_selectedNode->setSelectedPen( pen );
            // this->m_selectedNode->setTitleBarPen(QPen(color));
        }
    }
}

void MainWindow::onChangeForegroundHighlightColorBtnClicked() {
    if( this->m_selectedNode ) {
        QColor color = QColorDialog::getColor( this->m_selectedNode->titleBarSelectedBrush().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() ) {
            this->m_selectedNode->setTitleBarSelectedBrush( QBrush( color ) );
            // this->m_selectedNode->setTitleBarPen(QPen(color));
        }
    }
}

void MainWindow::onChangeMagmaBackgroundColorBtnClicked() {
    QColor color = QColorDialog::getColor( ui->magmaView->backgroundColor(), this, "", QColorDialog::ShowAlphaChannel );
    if( color.isValid() )
        ui->magmaView->setBackgroundColor( color );
}

void MainWindow::onChangeGridColorBtnClicked() {
    QColor color = QColorDialog::getColor( ui->magmaView->gridColor(), this, "", QColorDialog::ShowAlphaChannel );
    if( color.isValid() )
        ui->magmaView->setGridColor( color );
}

void MainWindow::onMoveMiniButtonClicked() {
    NodeView::MinimapPosition minipos = ui->magmaView->minimapPosition();

    switch( minipos ) {
    case NodeView::TopLeft:
        minipos = NodeView::TopRight;
        break;
    case NodeView::TopRight:
        minipos = NodeView::BottomRight;
        break;
    case NodeView::BottomRight:
        minipos = NodeView::BottomLeft;
        break;
    case NodeView::BottomLeft:
        minipos = NodeView::TopLeft;
        break;
    }
    ui->magmaView->setMiniMapPosition( minipos );
}

void MainWindow::minimapChanged( bool checked ) {
    if( checked )
        ui->magmaView->showMiniMap();
    else
        ui->magmaView->hideMiniMap();
}

void MainWindow::onZoomChanged() { ui->magmaView->setZoom( ui->zoomBox->value() ); }
void MainWindow::onGridSizeChanged() { ui->magmaView->setGridSize( ui->gridSizeSpinBox->value() ); }

void MainWindow::onConnectionCreated( Socket* sock ) { sock->setSocketBrush( QBrush( Qt::red ) ); }

void MainWindow::onDeleteInputSocketBtnClicked() {
    if( this->m_selectedNode ) {
        int socketIndex = ui->inputSocketListWidget->currentRow();
        this->m_selectedNode->deleteInputSocket( socketIndex );
        QListWidgetItem* listItem = ui->inputSocketListWidget->takeItem( socketIndex );
        delete listItem;
        ui->socketGroupBox->setEnabled( false );
        ui->inputSocketListWidget->clearSelection();
    }
}

void MainWindow::onDropShadowBoxToggled( bool checked ) {
    if( this->m_selectedNode ) {
        this->m_selectedNode->setDropShadow( checked );
    }
}

void MainWindow::onDeleteOutputSocketBtnClicked() {
    if( this->m_selectedNode ) {
        int socketIndex = ui->outputSocketListWidget->currentRow();
        this->m_selectedNode->deleteOutputSocket( socketIndex );
        QListWidgetItem* listItem = ui->outputSocketListWidget->takeItem( socketIndex );
        delete listItem;
        ui->socketGroupBox->setEnabled( false );
        ui->outputSocketListWidget->clearSelection();
    }
}

void MainWindow::onGridLinesBoxToggled( bool checked ) { ui->magmaView->setGridLines( checked ); }

void MainWindow::onCreationPointBoxToggled( bool checked ) { ui->magmaView->setCreationPointMarkerVisible( checked ); }

void MainWindow::onNodeAddedOrDeleted() {
    ui->nodeCountLabel->setText( "Node Count: " + QString::number( ui->magmaView->nodeCount() ) );
}

void MainWindow::onNodeNameEditReturnPressed() {
    if( this->m_selectedNode )
        this->m_selectedNode->setTitleText( ui->nodeNameEdit->text() );
}

void MainWindow::onDetailTitleTextChanged() {
    if( this->m_selectedNode ) {
        SideSocketNode* ssNode = dynamic_cast<SideSocketNode*>( this->m_selectedNode );
        if( ssNode )
            ssNode->setExpandableTitle( ui->sideSocketTitle->text() );
    }
}

void MainWindow::onDetailTitleColorChanged() {
    if( this->m_selectedNode ) {
        SideSocketNode* ssNode = dynamic_cast<SideSocketNode*>( this->m_selectedNode );
        if( ssNode ) {
            ExpandableContainer* container = ssNode->expandableArea();
            QBrush brush = container->titleBrush();
            QColor color = QColorDialog::getColor( brush.color(), this, "", QColorDialog::ShowAlphaChannel );
            if( color.isValid() ) {
                brush.setColor( color );
                container->setTitleBrush( brush );
            }
        }
        // ssNode->setExpandableTitle(ui->sideSocketTitle->text());
    }
}
void MainWindow::onDetailTitleLocationChanged( bool checked ) {
    if( this->m_selectedNode && checked ) {
        SideSocketNode* ssNode = dynamic_cast<SideSocketNode*>( this->m_selectedNode );
        if( ssNode ) {
            ExpandableContainer* container = ssNode->expandableArea();

            if( this->sender() == ui->expandableTitleLeftBtn ) {
                container->setTitleLocation( ExpandableContainer::TitleLeft );
            } else if( this->sender() == ui->expandableTitleCenterBtn ) {
                container->setTitleLocation( ExpandableContainer::TitleCenter );
            } else if( this->sender() == ui->expandableTitleRightBtn ) {
                container->setTitleLocation( ExpandableContainer::TitleRight );
            }
        }
    }
}

void MainWindow::onExpansionButtonLocationChanged( bool checked ) {
    if( this->m_selectedNode && checked ) {
        SideSocketNode* ssNode = dynamic_cast<SideSocketNode*>( this->m_selectedNode );
        if( ssNode ) {
            ExpandableContainer* container = ssNode->expandableArea();

            if( this->sender() == ui->expansionButtonLeftBtn ) {
                container->setButtonLocation( ExpandableContainer::ButtonLeft );
            } else if( this->sender() == ui->expansionButtonRightBtn ) {
                container->setButtonLocation( ExpandableContainer::ButtonRight );
            }
        }
    }
}

void MainWindow::onDetailTextEditTextChanged() {
    if( this->m_selectedNode )
        this->m_selectedNode->setDetailText( ui->detailTextEdit->toPlainText() );
}

void MainWindow::onEnableDetailText( bool enabled ) {
    if( this->m_selectedNode )
        this->m_selectedNode->setDetailTextEnabled( enabled );
}

void MainWindow::onSnapToGridBoxToggled( bool checked ) { ui->magmaView->setSnapToGrid( checked ); }

void MainWindow::onEnablePixmapButtonToggled( bool checked ) {
    if( this->m_selectedNode ) {
        this->m_selectedNode->setPixmapEnabled( checked );
        this->ui->changePixmapButton->setEnabled( checked );
        this->ui->pixmapPosGroupBox->setEnabled( checked );
        this->ui->pixmapSizeBox->setEnabled( checked );
        switch( m_selectedNode->pixmapPosition() ) {
        case Node::Top:
            ui->topPosRadioBtn->setChecked( true );
            break;
        case Node::Left:
            ui->leftPosRadioBtn->setChecked( true );
            break;
        case Node::Center:
            ui->centerPosRadioBtn->setChecked( true );
            break;
        case Node::Right:
            ui->rightPosRadioBtn->setChecked( true );
            break;
        case Node::Bottom:
            ui->bottomPosRadioBtn->setChecked( true );
            break;
        }
    }
}

void MainWindow::onSelectionChanged() {
    this->m_selectedConnection = 0;
    this->m_selectedNode = 0;

    ui->selectionCountLabel->setText( "Selection Count: " + QString::number( ui->magmaView->selectionCount() ) );

    ui->inputSocketListWidget->clear();
    ui->outputSocketListWidget->clear();
    ui->nodeNameEdit->clear();

    ui->nodeGroupBox->setEnabled( false );
    ui->pixmapGroupBox->setEnabled( false );
    ui->pixmapPosGroupBox->setEnabled( false );
    ui->socketGroupBox->setEnabled( false );
    ui->connectionGroupBox->setEnabled( false );
    ui->sideSocketGroupBox->setEnabled( false );

    this->m_selectedNode = 0;
    ui->detailTextEdit->clear();

    QList<Node*> selectedNodes = ui->magmaView->selectedNodes();
    QList<Connection*> selectedConnections = ui->magmaView->selectedConnections();
    if( selectedNodes.count() == 1 && selectedConnections.count() == 0 ) {
        this->m_selectedNode = selectedNodes[0];
        onNodeSelected();
    } else if( selectedNodes.count() == 0 && selectedConnections.count() == 1 ) {
        this->m_selectedConnection = selectedConnections[0];
        onConnectionSelected();
    }
}

void MainWindow::onConnectionSelected() {
    ui->connectionGroupBox->setEnabled( true );
    ui->connectionDurationBox->setValue( this->m_selectedConnection->animationDuration() );
    ui->connectionLightnessBox->setValue( this->m_selectedConnection->animationLightness() );
    ui->arrowPercentBox->setValue( this->m_selectedConnection->arrowPositionPercent() );
    ui->arrowSizeBox->setValue( this->m_selectedConnection->arrowSize() );
    ui->curvatureBox->setValue( this->m_selectedConnection->curvature() );
    ui->dragDistanceBox->setValue( this->m_selectedConnection->dragDistance() );
    ui->connectionAnimationEnabled->setChecked( this->m_selectedConnection->animationEnabled() );
    ui->connectionObjectNameEdit->setText( this->m_selectedConnection->objectName() );
}

void MainWindow::onNodeSelected() {

    ui->pixmapPosGroupBox->setEnabled( m_selectedNode->pixmapEnabled() );
    ui->pixmapSizeBox->setEnabled( m_selectedNode->pixmapEnabled() );
    ui->pixmapWidthSpinBox->setValue( m_selectedNode->pixmapSize().width() );
    ui->pixmapWidthSpinBox->setValue( m_selectedNode->pixmapSize().height() );
    ui->minNodeWidthSpinBox->setValue( m_selectedNode->minimumWidth() );
    ui->socketBufferSpinBox->setValue( m_selectedNode->socketBuffer() );
    Node* testNode = qgraphicsitem_cast<Node*>( m_selectedNode );
    if( testNode != 0 ) {
        ui->fullLineInputSocketBtn->setEnabled( true );
        ui->fullLineOutputSocketBtn->setEnabled( true );
    } else {
        ui->fullLineInputSocketBtn->setEnabled( false );
        ui->fullLineOutputSocketBtn->setEnabled( false );
    }
    switch( m_selectedNode->pixmapPosition() ) {
    case Node::Top:
        ui->topPosRadioBtn->setChecked( true );
        break;
    case Node::Left:
        ui->leftPosRadioBtn->setChecked( true );
        break;
    case Node::Center:
        ui->centerPosRadioBtn->setChecked( true );
        break;
    case Node::Right:
        ui->rightPosRadioBtn->setChecked( true );
        break;
    case Node::Bottom:
        ui->bottomPosRadioBtn->setChecked( true );
        break;
    }

    switch( m_selectedNode->titleJustify() ) {
    case Node::TitleLeft:
        ui->leftJustifyRadio->setChecked( true );
        break;
    case Node::TitleCenter:
        ui->centerJustifyRadio->setChecked( true );
        break;
    case Node::TitleRight:
        ui->rightJustifyRadio->setChecked( true );
        break;
    }

    switch( m_selectedNode->elidedTitle() ) {
    case Qt::ElideNone:
        ui->elideNoneRadio->setChecked( true );
        break;
    case Qt::ElideLeft:
        ui->elideLeftRadio->setChecked( true );
        break;
    case Qt::ElideMiddle:
        ui->elideCenterRadio->setChecked( true );
        break;
    case Qt::ElideRight:
        ui->elideRightRadio->setChecked( true );
        break;
    }

    ui->nodeGroupBox->setEnabled( true );
    ui->pixmapGroupBox->setEnabled( true );
    ui->nodeNameEdit->setText( this->m_selectedNode->titleText() );
    ui->detailTextEdit->setPlainText( this->m_selectedNode->detailText() );
    ui->dropShadowBox->setChecked( this->m_selectedNode->dropShadow() );

    ui->cornerXRadiusBox->setValue( this->m_selectedNode->cornerXRadius() );
    ui->cornerYRadiusBox->setValue( this->m_selectedNode->cornerYRadius() );

    ui->enablePixmapButton->setChecked( this->m_selectedNode->pixmapEnabled() );
    ui->changePixmapButton->setChecked( !this->m_selectedNode->pixmapEnabled() );
    QList<Socket*> inSockets = this->m_selectedNode->getInputSockets();
    QList<Socket*> outSockets = this->m_selectedNode->getOutputSockets();
    ui->enableDetailText->setChecked( this->m_selectedNode->detailTextEnabled() );
    ui->nodeObjectNameEdit->setText( this->m_selectedNode->objectName() );
    foreach( Socket* socket, inSockets ) {
        ui->inputSocketListWidget->addItem( socket->socketName() );
    }

    foreach( Socket* socket, outSockets ) {
        ui->outputSocketListWidget->addItem( socket->socketName() );
    }
    SideSocketNode* ssNode = dynamic_cast<SideSocketNode*>( this->m_selectedNode );
    if( ssNode ) {
        ui->sideSocketGroupBox->setEnabled( true );
        ui->sideSocketTitle->setText( ssNode->expandableTitle() );

        ExpandableContainer* container = ssNode->expandableArea();
        switch( container->titleLocation() ) {
        case ExpandableContainer::TitleLeft:
            ui->expandableTitleLeftBtn->setChecked( true );
            break;
        case ExpandableContainer::TitleCenter:
            ui->expandableTitleCenterBtn->setChecked( true );
            break;
        case ExpandableContainer::TitleRight:
            ui->expandableTitleRightBtn->setChecked( true );
            break;
        }

        switch( container->buttonPosition() ) {
        case ExpandableContainer::ButtonLeft:
            ui->expansionButtonLeftBtn->setChecked( true );
            break;
        case ExpandableContainer::ButtonRight:
            ui->expansionButtonRightBtn->setChecked( true );
            break;
        }
    }
}

void MainWindow::onSocketNameEditReturnPressed() {
    if( this->m_selectedSocket )
        this->m_selectedSocket->setSocketName( ui->socketNameEdit->text() );
}

void MainWindow::onSocketShapeSizeChanged( int size ) {
    if( this->m_selectedSocket )
        this->m_selectedSocket->setSocketShapeSize( size );
}

void MainWindow::onSocketItemClicked( QListWidgetItem* item ) {
    ui->socketNameEdit->clear();

    // if (socketIndex != -1) {
    ui->socketGroupBox->setEnabled( true );

    if( this->sender() == ui->inputSocketListWidget )
        this->m_selectedSocket = this->m_selectedNode->getInputSocket( ui->inputSocketListWidget->row( item ) );
    else
        this->m_selectedSocket = this->m_selectedNode->getOutputSocket( ui->outputSocketListWidget->row( item ) );

    ui->socketNameEdit->setText( this->m_selectedSocket->socketName() );

    ui->socketShapeSizeBox->setValue( this->m_selectedSocket->socketShapeSize() );

    if( this->m_selectedSocket->socketShape() == Socket::Circle )
        ui->circleRadioBtn->setChecked( true );
    else if( this->m_selectedSocket->socketShape() == Socket::Square )
        ui->squareRadioBtn->setChecked( true );
    else
        ui->triangleRadioBtn->setChecked( true );

    if( this->m_selectedSocket->verticalAlignment() == Socket::Top )
        ui->topAlignmentRadioBtn->setChecked( true );
    else if( this->m_selectedSocket->verticalAlignment() == Socket::Center )
        ui->centerAlignmentRadioBtn->setChecked( true );
    else
        ui->bottomAlignmentRadioBtn->setChecked( true );

    if( this->m_selectedSocket->labelStyle() == Socket::Elide )
        ui->elideRadioBtn->setChecked( true );
    else if( this->m_selectedSocket->labelStyle() == Socket::Wrap )
        ui->wrapRadioBtn->setChecked( true );

    ui->animationDurationBox->setValue( this->m_selectedSocket->animationDuration() );
    ui->animationLightnessBox->setValue( this->m_selectedSocket->animationLightness() );
    ui->multipleConnectionsCheckBox->setChecked( this->m_selectedSocket->multipleConnections() );
    ui->animationEnabledBox->setChecked( this->m_selectedSocket->animationEnabled() );
    ui->socketObjectNameEdit->setText( this->m_selectedSocket->objectName() );
    ui->dragEnabledCheckBox->setChecked( this->m_selectedSocket->dragEnabled() );
    if( this->m_selectedSocket->socketShape() == Socket::Triangle ) {
        ui->triangleOrientationBox->setEnabled( true );
        switch( this->m_selectedSocket->triangleOrientation() ) {
        case TriangleSocketShape::North:
            ui->northRadioBtn->setChecked( true );
            break;
        case TriangleSocketShape::South:
            ui->southRadioBtn->setChecked( true );
            break;
        case TriangleSocketShape::East:
            ui->eastRadioBtn->setChecked( true );
            break;
        case TriangleSocketShape::West:
            ui->westRadioBtn->setChecked( true );
            break;
        }
    } else {
        ui->triangleOrientationBox->setEnabled( false );
    }
    //} else
    //	ui->socketGroupBox->setEnabled(false);
}

void MainWindow::onChangeTriangleOrientation( bool checked ) {
    if( checked ) {
        if( this->m_selectedSocket ) {
            if( this->sender() == ui->northRadioBtn ) {
                m_selectedSocket->setTriangleOrientation( TriangleSocketShape::North );
            } else if( this->sender() == ui->westRadioBtn ) {
                m_selectedSocket->setTriangleOrientation( TriangleSocketShape::West );
            } else if( this->sender() == ui->eastRadioBtn ) {
                m_selectedSocket->setTriangleOrientation( TriangleSocketShape::East );
            } else if( this->sender() == ui->southRadioBtn ) {
                m_selectedSocket->setTriangleOrientation( TriangleSocketShape::South );
            }
        }
    }
}

void MainWindow::onSocketAlignmentChanged( bool checked ) {
    if( checked ) {
        if( this->m_selectedSocket ) {
            if( this->sender() == ui->topAlignmentRadioBtn ) {
                m_selectedSocket->setVerticalAlignment( Socket::Top );
            } else if( this->sender() == ui->centerAlignmentRadioBtn ) {
                m_selectedSocket->setVerticalAlignment( Socket::Center );
            } else if( this->sender() == ui->bottomAlignmentRadioBtn ) {
                m_selectedSocket->setVerticalAlignment( Socket::Bottom );
            }
        }
    }
}

void MainWindow::onLabelStyleChanged( bool checked ) {
    if( checked ) {
        if( this->m_selectedSocket ) {
            if( this->sender() == ui->elideRadioBtn ) {
                this->m_selectedSocket->setLabelStyle( Socket::Elide );
            } else if( this->sender() == ui->wrapRadioBtn ) {
                this->m_selectedSocket->setLabelStyle( Socket::Wrap );
            }
        }
    }
}

void MainWindow::onMultipleConnectionsChanged() {
    if( this->m_selectedSocket ) {
        this->m_selectedSocket->setMultipleConnections( ui->multipleConnectionsCheckBox->isChecked() );
    }
}

void MainWindow::onConnectionChangeColor() {
    if( this->m_selectedConnection ) {
        QColor color =
            QColorDialog::getColor( this->m_selectedConnection->color(), this, "", QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedConnection->setColor( color );
    }
}

void MainWindow::onConnectionBrushChanged() {
    if( this->m_selectedConnection ) {
        QColor color = QColorDialog::getColor( this->m_selectedConnection->brush().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedConnection->setBrush( QBrush( color ) );
    }
}

void MainWindow::onConnectionPenChanged() {
    if( this->m_selectedConnection ) {
        QColor color = QColorDialog::getColor( this->m_selectedConnection->pen().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedConnection->setPen( QPen( color ) );
    }
}

void MainWindow::onConnectionSelectedColorChanged() {
    if( this->m_selectedConnection ) {
        QColor color = QColorDialog::getColor( this->m_selectedConnection->selectedColor(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedConnection->setSelectedColor( color );
    }
}

void MainWindow::onConnectionSelectedBrushChanged() {
    if( this->m_selectedConnection ) {
        QColor color = QColorDialog::getColor( this->m_selectedConnection->selectedBrush().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedConnection->setSelectedBrush( QBrush( color ) );
    }
}

void MainWindow::onSocketAnimtionEnabledChanged() {
    if( this->m_selectedSocket ) {
        this->m_selectedSocket->setAnimationEnabled( ui->animationEnabledBox->isChecked() );
    }
}

void MainWindow::onConnectionAnimationEnabledChanged() {
    if( this->m_selectedConnection ) {
        this->m_selectedConnection->setAnimationEnabled( ui->connectionAnimationEnabled->isChecked() );
    }
}

void MainWindow::onConnectionSelectedPenChanged() {
    if( this->m_selectedConnection ) {
        QColor color = QColorDialog::getColor( this->m_selectedConnection->selectedPen().color(), this, "",
                                               QColorDialog::ShowAlphaChannel );
        if( color.isValid() )
            this->m_selectedConnection->setSelectedPen( QPen( color ) );
    }
}

void MainWindow::onConnectionAnimationDurationChanged() {
    if( this->m_selectedConnection ) {
        this->m_selectedConnection->setAnimationDuration( ui->connectionDurationBox->value() );
    }
}

void MainWindow::onConnectionArrowPositionChanged() {
    if( this->m_selectedConnection ) {
        this->m_selectedConnection->setArrowPositionPercent( ui->arrowPercentBox->value() );
    }
}

void MainWindow::onConnectionArrowSizeChanged() {
    if( this->m_selectedConnection ) {
        this->m_selectedConnection->setArrowSize( ui->arrowSizeBox->value() );
    }
}

void MainWindow::onConnectionCurvatureChanged() {
    if( this->m_selectedConnection ) {
        this->m_selectedConnection->setCurvature( ui->curvatureBox->value() );
    }
}

void MainWindow::onConnectionDragDistanceChanged() {
    if( this->m_selectedConnection ) {
        this->m_selectedConnection->setDragDistance( ui->dragDistanceBox->value() );
    }
}

void MainWindow::onConnectionAnimationLightnessChanged() {
    if( this->m_selectedConnection ) {
        this->m_selectedConnection->setAnimationLightness( ui->connectionLightnessBox->value() );
    }
}

void MainWindow::onSocketDurationChanged() {
    if( this->m_selectedSocket ) {
        this->m_selectedSocket->setAnimationDuration( ui->animationDurationBox->value() );
    }
}

void MainWindow::onSocketLightnessChanged() {
    if( this->m_selectedSocket ) {
        this->m_selectedSocket->setAnimationLightness( ui->animationLightnessBox->value() );
    }
}

void MainWindow::onSocketTypeToggled( bool checked ) {
    if( checked ) {
        if( this->m_selectedSocket ) {
            if( this->sender() == ui->circleRadioBtn ) {
                this->m_selectedSocket->setSocketShape( Socket::Circle );
                ui->triangleOrientationBox->setEnabled( false );
            } else if( this->sender() == ui->squareRadioBtn ) {
                this->m_selectedSocket->setSocketShape( Socket::Square );
                ui->triangleOrientationBox->setEnabled( false );
            } else {
                this->m_selectedSocket->setSocketShape( Socket::Triangle );
                ui->triangleOrientationBox->setEnabled( true );
                switch( this->m_selectedSocket->triangleOrientation() ) {
                case TriangleSocketShape::North:
                    ui->northRadioBtn->setChecked( true );
                    break;
                case TriangleSocketShape::South:
                    ui->southRadioBtn->setChecked( true );
                    break;
                case TriangleSocketShape::East:
                    ui->eastRadioBtn->setChecked( true );
                    break;
                case TriangleSocketShape::West:
                    ui->westRadioBtn->setChecked( true );
                    break;
                }
            }
        }
    }
}

void MainWindow::onPixmapPosToggled( bool checked ) {
    if( checked ) {
        if( this->m_selectedNode ) {
            if( this->sender() == ui->topPosRadioBtn )
                this->m_selectedNode->setPixmapPosition( Node::Top );
            else if( this->sender() == ui->leftPosRadioBtn )
                this->m_selectedNode->setPixmapPosition( Node::Left );
            else if( this->sender() == ui->rightPosRadioBtn )
                this->m_selectedNode->setPixmapPosition( Node::Right );
            else if( this->sender() == ui->bottomPosRadioBtn )
                this->m_selectedNode->setPixmapPosition( Node::Bottom );
            else if( this->sender() == ui->centerPosRadioBtn )
                this->m_selectedNode->setPixmapPosition( Node::Center );
        }
    }
}

void MainWindow::onJustifyChanged( bool checked ) {
    if( checked ) {
        if( this->m_selectedNode ) {
            if( this->sender() == ui->leftJustifyRadio )
                this->m_selectedNode->setTitleJustify( Node::TitleLeft );
            else if( this->sender() == ui->centerJustifyRadio )
                this->m_selectedNode->setTitleJustify( Node::TitleCenter );
            else if( this->sender() == ui->rightJustifyRadio )
                this->m_selectedNode->setTitleJustify( Node::TitleRight );
        }
    }
}

void MainWindow::onElideChanged( bool checked ) {
    if( checked ) {
        if( this->m_selectedNode ) {
            if( this->sender() == ui->elideNoneRadio )
                this->m_selectedNode->setElidedTitle( Qt::ElideNone );
            else if( this->sender() == ui->elideLeftRadio )
                this->m_selectedNode->setElidedTitle( Qt::ElideLeft );
            else if( this->sender() == ui->elideCenterRadio )
                this->m_selectedNode->setElidedTitle( Qt::ElideMiddle );
            else if( this->sender() == ui->elideRightRadio )
                this->m_selectedNode->setElidedTitle( Qt::ElideRight );
        }
    }
}

void MainWindow::OnPerformNodeCountTest() {
    int count = ui->stressCount1Box->value();
    int radius = ui->stressRadiusBox->value();
    NodeCountTest( count, radius );
}

void MainWindow::OnPerformConnectionTest() {
    int count = ui->stressCount1Box->value();
    int radius = ui->stressRadiusBox->value();
    ConnectionTest( count, radius );
}

void MainWindow::OnPerformMeshTest() {
    int count1 = ui->stressCount1Box->value();
    int count2 = ui->stressCount2Box->value();
    int radius = ui->stressRadiusBox->value();
    MeshTest( count1, count2, radius );
}

int MainWindow::randInt( int low, int high ) {
    // Random number between low and high
    return qrand() % ( ( high + 1 ) - low ) + low;
}

void MainWindow::NodeCountTest( int Count, int Radius ) {
    ui->magmaView->deleteAllNodes();
    for( int i = 0; i < Count; i++ ) {
        Node* node = ui->magmaView->createNode();
        node->setPos( randInt( -Radius, Radius ), randInt( -Radius, Radius ) );
    }
}

void MainWindow::ConnectionTest( int Count, int Radius ) {
    ui->magmaView->deleteAllNodes();
    for( int i = 0; i < Count; i++ ) {
        Node* node = ui->magmaView->createNode( 1 );
        node->setPos( randInt( -Radius, Radius ), randInt( -Radius, Radius ) );
    }
    QList<Node*> nodeList = ui->magmaView->nodes();
    Node* connectionNode = ui->magmaView->createNode( 0, 1, "test" );
    Socket* outputSocket = connectionNode->getOutputSocket( 0 );
    foreach( Node* node, nodeList ) {
        outputSocket->createConnection( node->getInputSocket( 0 ) );
    }
}

void MainWindow::MeshTest( int Count1, int Count2, int Radius ) {
    ui->magmaView->deleteAllNodes();
    for( int i = 0; i < Count1; i++ ) {
        Node* node = ui->magmaView->createNode( 1 );
        node->setPos( randInt( -Radius, Radius ), randInt( -Radius, Radius ) );
        node->getInputSocket( 0 )->setMultipleConnections( true );
    }
    QList<Node*> nodeList = ui->magmaView->nodes();
    for( int i = 0; i < Count2; i++ ) {
        Node* connectionNode = ui->magmaView->createNode( 0, 1, "test" );
        connectionNode->setPos( randInt( -Radius, Radius ), randInt( -Radius, Radius ) );
        Socket* outputSocket = connectionNode->getOutputSocket( 0 );
        foreach( Node* node, nodeList ) {
            outputSocket->createConnection( node->getInputSocket( 0 ) );
        }
    }
}

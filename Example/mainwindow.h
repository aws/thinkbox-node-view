// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Node;
class Socket;
class Connection;
class QListWidgetItem;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow( QWidget* parent = 0 );
    ~MainWindow();

  private slots:

    void onChangePixmap();
    void onAddNode();

    void onAddSideNode();
    void onAddSimpleNode();

    void onAddInputSocketBtnClicked();

    void onAddOutputSocketBtnClicked();

    void onAddFullLineOutputSocketBtnClicked();

    void onAddFullLineInputSocketBtnClicked();

    void onChangeOutlineColorBtnClicked();

    void onChangeFillColorBtnClicked();

    void onChangeTextColorBtnClicked();

    void onChangeBackgroundColorBtnClicked();

    void onChangeForegroundColorBtnClicked();

    void onChangeBorderColorBtnClicked();

    void onChangeDetailFontBtnClicked();

    void onChangeDetailColorBtnClicked();

    void onChangeBorderColorHighlightBtnClicked();

    void onChangeBackgroundHighlightColorBtnClicked();

    void onChangeForegroundHighlightColorBtnClicked();

    void onChangeMagmaBackgroundColorBtnClicked();

    void onDropShadowBoxToggled( bool checked );

    void onChangeGridColorBtnClicked();

    void onConnectionCreated( Socket* sock );

    void onDeleteInputSocketBtnClicked();

    void onDeleteOutputSocketBtnClicked();

    void onGridLinesBoxToggled( bool checked );

    void onNodeAddedOrDeleted();

    void onNodeNameEditReturnPressed();

    void onDetailTextEditTextChanged();

    void onEnableDetailText( bool enabled );

    void onNodeSelected();

    void onConnectionSelected();

    void onSelectionChanged();

    void onConnectionAnimationEnabledChanged();

    void onConnectionAnimationDurationChanged();

    void onConnectionAnimationLightnessChanged();

    void onConnectionArrowPositionChanged();

    void onConnectionArrowSizeChanged();

    void onConnectionCurvatureChanged();

    void onConnectionDragDistanceChanged();

    void onConnectionChangeColor();

    void onConnectionBrushChanged();

    void onConnectionPenChanged();

    void onConnectionSelectedColorChanged();

    void onConnectionSelectedBrushChanged();

    void onConnectionSelectedPenChanged();

    void onScroll();

    void onSnapToGridBoxToggled( bool checked );

    void onCreationPointBoxToggled( bool checked );

    void onSocketNameEditReturnPressed();

    void onSocketItemClicked( QListWidgetItem* item );

    void onSocketTypeToggled( bool checked );

    void onMoveMiniButtonClicked();

    void onZoomChanged();

    void onGridSizeChanged();

    void onRadiusXChanged();

    void onRadiusYChanged();

    void onEnablePixmapButtonToggled( bool checked );

    void onPixmapPosToggled( bool checked );

    void onSetPixmapSize();

    void onMinWidthChanged();

    void onChangeSocketBuffer();

    void onSocketShapeSizeChanged( int value );

    void onDeleteAll();

    void onJustifyChanged( bool checked );

    void onElideChanged( bool checked );

    void printCheck();

    void minimapChanged( bool checked );

    void onChangeTitleTextColor();

    void onChangeTitleTextFont();

    void onSocketAnimtionEnabledChanged();

    void onSocketDurationChanged();

    void onSocketLightnessChanged();

    void onMultipleConnectionsChanged();

    void onChangeTriangleOrientation( bool checked );

    void onStyleSheetChanged();

    void onDetailTitleTextChanged();

    void onDetailTitleFontChanged();

    void onDetailTitleLocationChanged( bool checked );

    void onExpansionButtonLocationChanged( bool checked );

    void onDetailTitleColorChanged();

    void onNodeObjectNameChanged();

    void onSocketObjectNameChanged();

    void onConnectionObjectNameChanged();

    void onSocketAlignmentChanged( bool checked );

    void onDragEnabledChanged( bool checked );

    void onLabelStyleChanged( bool checked );

    int randInt( int low, int high );

    void NodeCountTest( int Count, int Radius );

    void OnPerformNodeCountTest();

    void ConnectionTest( int Count, int Radius );

    void OnPerformConnectionTest();

    void MeshTest( int Count1, int Count2, int Radius );

    void OnPerformMeshTest();

  private:
    Ui::MainWindow* ui;
    Node* m_selectedNode;
    Socket* m_selectedSocket;
    Connection* m_selectedConnection;
};

#endif // MAINWINDOW_H

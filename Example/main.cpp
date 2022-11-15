// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include "mainwindow.h"
#include <QApplication>

int main( int argc, char* argv[] ) {
    QApplication::setStyle( "plastique" );
    QApplication::setPalette( QPalette( QColor( 64, 64, 64 ) ) );
    QApplication a( argc, argv );
    MainWindow w;
    w.show();

    // w.pan(-1000,0);

    return a.exec();
}

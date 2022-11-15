// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#include <NodeView/stylemanager.h>

#include <NodeView/node.h>

#include <QBrush>
#include <QDebug>
#include <QFont>
#include <QGradient>
#include <QMetaProperty>
#include <QPen>

#if QT_VERSION >= 0x050301
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#endif

/***********************Public Members***********************/

StyleManager::StyleManager( QObject* parent )
    : QObject( parent ) {
    m_styleSheet = "";
}

void StyleManager::registerStyleableItem( Styleable* styleItem ) {
    // if (!this->m_styleItems.contains(styleItem)) {
    connect( dynamic_cast<QObject*>( styleItem ), SIGNAL( requiresStyling( QObject* ) ), this,
             SLOT( onRequiresStyling( QObject* ) ) );
    connect( dynamic_cast<QObject*>( styleItem ), SIGNAL( destroyed( QObject* ) ), this,
             SLOT( onStyleItemDestroyed( QObject* ) ) );

    // this->m_styleItems.append(styleItem);
    this->m_styleItems.insert( dynamic_cast<QObject*>( styleItem ) );
    this->applyStyleSheet( dynamic_cast<QObject*>( styleItem ) );
    //}
}

void StyleManager::setStyleMap( const QVariantMap map ) {
    this->m_styleMap = map;
    this->applyStyleSheet();
}

void StyleManager::setStyleSheet( const QString style ) {
    this->m_styleSheet = style;

    if( this->m_styleSheet != "" ) {
        this->m_styleMap.clear();
#if QT_VERSION >= 0x050301
        this->parseJSON();
#endif
        this->applyStyleSheet();
    }
}

QString StyleManager::styleSheet() const { return this->m_styleSheet; }

/***********************Public Slots*************************/

void StyleManager::onRequiresStyling( QObject* item ) { this->applyStyleSheet( item ); }

void StyleManager::onStyleItemDestroyed( QObject* item ) { this->m_styleItems.remove( item ); }

/***********************Private Members**********************/
void StyleManager::applyStyleSheet() {
    foreach( auto styleItem, this->m_styleItems )
        this->applyStyleSheet( styleItem );
}

void StyleManager::applyStyleSheet( QObject* item ) {
    QString itemKey = ( item->objectName().length() )
                          ? QString( item->metaObject()->className() ).append( "#" ).append( item->objectName() )
                          : QString( item->metaObject()->className() );
    QVariantMap itemStyleMap; // = this->m_styleMap[itemKey].toMap();

    if( this->m_styleMap.contains( itemKey ) )
        itemStyleMap = this->m_styleMap[itemKey].toMap();
    else if( this->m_styleMap.contains( QString( item->metaObject()->className() ) ) )
        itemStyleMap = this->m_styleMap[QString( item->metaObject()->className() )].toMap();
    if( !itemStyleMap.isEmpty() ) {
        QVariantMap itemStyleMap = this->m_styleMap[itemKey].toMap();
        int propertyCount = item->metaObject()->propertyCount();

        for( int i = 0; i < propertyCount; i++ ) {
            const char* propertyName = item->metaObject()->property( i ).name();
            if( itemStyleMap.contains( QString( propertyName ) ) ) {
                item->setProperty( propertyName, itemStyleMap[QString( propertyName )] );
            }
        }
    }
}

#if QT_VERSION >= 0x050301
QVariantMap StyleManager::parseConnection( const QJsonObject& conn ) {
    QVariantMap map;

    if( conn["animationDuration"].isDouble() )
        map["animationDuration"] = conn["animationDuration"].toInt();

    if( conn["animationLightness"].isDouble() )
        map["animationLightness"] = conn["animationLightness"].toInt();

    if( conn["animationEnabled"].isBool() )
        map["animationEnabled"] = conn["animationEnabled"].toBool();

    if( conn["brush"].isObject() )
        map["brush"] = this->parseQBrush( conn["brush"].toObject() );

    if( conn["pen"].isObject() )
        map["pen"] = this->parseQPen( conn["pen"].toObject() );

    if( conn["selectedBrush"].isObject() )
        map["selectedBrush"] = this->parseQBrush( conn["selectedBrush"].toObject() );

    if( conn["selectedPen"].isObject() )
        map["selectedPen"] = this->parseQPen( conn["selectedPen"].toObject() );

    if( conn["arrowPositionPercent"].isDouble() )
        map["arrowPositionPercent"] = static_cast<qreal>( conn["arrowPositionPercent"].toDouble() );

    if( conn["arrowSize"].isDouble() )
        map["arrowSize"] = static_cast<qreal>( conn["arrowSize"].toDouble() );

    if( conn["curvature"].isDouble() )
        map["curvature"] = static_cast<qreal>( conn["curvature"].toDouble() );

    return map;
}

void StyleManager::parseJSON() {
    auto parseError = QJsonParseError();
    auto json = QJsonDocument::fromJson( m_styleSheet.toUtf8(), &parseError );

    if( json.isObject() ) {
        auto styleSheetObject = json.object();
        foreach( const auto& key, styleSheetObject.keys() ) {
            if( key.startsWith( "Node" ) )
                this->m_styleMap[key] = this->parseNode( styleSheetObject[key].toObject() );
            else if( key.startsWith( "Socket" ) )
                this->m_styleMap[key] = this->parseSocket( styleSheetObject[key].toObject() );
            else if( key.startsWith( "Connection" ) )
                this->m_styleMap[key] = this->parseConnection( styleSheetObject[key].toObject() );
        }
    }
}

QVariantMap StyleManager::parseNode( const QJsonObject& node ) {
    QVariantMap map;

    if( node["backgroundBrush"].isObject() )
        map["backgroundBrush"] = this->parseQBrush( node["backgroundBrush"].toObject() );

    if( node["selectedBrush"].isObject() )
        map["selectedBrush"] = this->parseQBrush( node["selectedBrush"].toObject() );

    if( node["titleBarBrush"].isObject() )
        map["titleBarBrush"] = this->parseQBrush( node["titleBarBrush"].toObject() );

    if( node["titleBarSelectedBrush"].isObject() )
        map["titleBarSelectedBrush"] = this->parseQBrush( node["titleBarSelectedBrush"].toObject() );

    if( node["titleTextBrush"].isObject() )
        map["titleTextBrush"] = this->parseQBrush( node["titleTextBrush"].toObject() );

    if( node["detailTextFont"].isObject() )
        map["detailTextFont"] = this->parseQFont( node["detailTextFont"].toObject() );

    if( node["titleTextFont"].isObject() )
        map["titleTextFont"] = this->parseQFont( node["titleTextFont"].toObject() );

    if( node["outlinePen"].isObject() )
        map["outlinePen"] = this->parseQPen( node["outlinePen"].toObject() );

    if( node["selectedPen"].isObject() )
        map["selectedPen"] = this->parseQPen( node["selectedPen"].toObject() );

    if( node["titleTextPen"].isObject() )
        map["titleTextPen"] = this->parseQPen( node["titleTextPen"].toObject() );

    if( node["detailTextColor"].isObject() )
        map["detailTextColor"] = this->parseQColor( node["detailTextColor"].toObject() );

    if( node["pixmap"].isObject() )
        map["pixmap"] = this->parseQPixmap( node["pixmap"].toString() );

    if( node["pixmapSize"].isObject() )
        map["pixmapSize"] = this->parseQSize( node["pixmapSize"].toArray() );

    if( node["detailTextEnabled"].isBool() )
        map["detailTextEnabled"] = node["detailTextEnabled"].toBool();

    if( node["dropShadow"].isBool() )
        map["dropShadow"] = node["dropShadow"].toBool();

    if( node["pixmapEnabled"].isBool() )
        map["pixmapEnabled"] = node["pixmapEnabled"].toBool();

    if( node["cornerXRadius"].isDouble() )
        map["cornerXRadius"] = static_cast<qreal>( node["cornerXRadius"].toDouble() );

    if( node["cornerYRadius"].isDouble() )
        map["cornerYRadius"] = static_cast<qreal>( node["cornerYRadius"].toDouble() );

    if( node["minimumWidth"].isDouble() )
        map["minimumWidth"] = static_cast<qreal>( node["minimumWidth"].toDouble() );

    if( node["pixmapPosition"].isDouble() )
        map["pixmapPosition"] = node["pixmapPosition"].toInt();

    return map;
}

QBrush StyleManager::parseQBrush( const QJsonObject& qBrush ) {

    if( qBrush["color"].isObject() ) {
        QColor color = this->parseQColor( qBrush["color"].toObject() );

        if( qBrush["brushStyle"].isDouble() ) {
            Qt::BrushStyle style = static_cast<Qt::BrushStyle>( qBrush["brushStyle"].toInt() );
            return QBrush( color, style );

        } else
            return QBrush( color );

    } else if( qBrush["pixmap"].isObject() ) {
        QPixmap pixmap = this->parseQPixmap( qBrush["pixmap"].toString() );
        return QBrush( pixmap );

    } else {
        return QBrush();
    }
}

QColor StyleManager::parseQColor( const QJsonObject& qColor ) {

    // Case for RGB
    if( qColor["rgb"].isArray() ) {
        qint32 rgb[3];

        auto i = 0;
        foreach( const auto& value, qColor["rgb"].toArray() ) {
            if( value.isDouble() ) {
                rgb[i] = value.toInt();
                i++;
            } else {
                return QColor();
            }
        }
        return QColor::fromRgb( rgb[0], rgb[1], rgb[2] );
    }

    // Case for RGBA
    if( qColor["rgba"].isArray() ) {
        qint32 rgba[4];

        auto i = 0;
        foreach( const auto& value, qColor["rgba"].toArray() ) {
            if( value.isDouble() ) {
                rgba[i] = value.toInt();
                i++;
            } else {
                return QColor();
            }
        }
        return QColor::fromRgb( rgba[0], rgba[1], rgba[2], rgba[3] );
    }

    // Case for HSV
    if( qColor["hsv"].isArray() ) {
        qint32 hsv[3];

        auto i = 0;
        foreach( const auto& value, qColor["hsv"].toArray() ) {
            if( value.isDouble() ) {
                hsv[i] = value.toInt();
                i++;
            } else {
                return QColor();
            }
        }
        return QColor::fromHsv( hsv[0], hsv[1], hsv[2] );
    }

    // Case for HSVA
    if( qColor["hsva"].isArray() ) {
        qint32 hsva[4];

        auto i = 0;
        foreach( const auto& value, qColor["hsva"].toArray() ) {
            if( value.isDouble() ) {
                hsva[i] = value.toInt();
                i++;
            } else {
                return QColor();
            }
        }
        return QColor::fromHsv( hsva[0], hsva[1], hsva[2], hsva[3] );
    }

    // Case for color string
    if( qColor["string"].isString() ) {
        return QColor( qColor["string"].toString() );
    }

    return QColor();
}

QFont StyleManager::parseQFont( const QJsonObject& qFont ) {
    QFont font;

    if( qFont["fontFamily"].isString() )
        font.setFamily( qFont["fontFamily"].toString() );

    if( qFont["pointSize"].isDouble() )
        font.setPointSize( qFont["pointSize"].toInt() );

    if( qFont["bold"].isBool() )
        font.setBold( qFont["bold"].toBool() );

    if( qFont["italic"].isBool() )
        font.setItalic( qFont["italic"].toBool() );

    if( qFont["underline"].isBool() )
        font.setUnderline( qFont["underline"].toBool() );

    if( qFont["strikeOut"].isBool() )
        font.setUnderline( qFont["strikeOut"].toBool() );

    return font;
}

QPen StyleManager::parseQPen( const QJsonObject& qPen ) {
    QPen pen;

    if( qPen["brush"].isObject() )
        pen.setBrush( this->parseQBrush( qPen["brush"].toObject() ) );

    if( qPen["color"].isObject() )
        pen.setColor( this->parseQColor( qPen["color"].toObject() ) );

    if( qPen["penStyle"].isDouble() )
        pen.setStyle( static_cast<Qt::PenStyle>( qPen["penStyle"].toInt() ) );

    if( qPen["penCapStyle"].isDouble() )
        pen.setCapStyle( static_cast<Qt::PenCapStyle>( qPen["penCapStyle"].toInt() ) );

    if( qPen["penJoinStyle"].isDouble() )
        pen.setJoinStyle( static_cast<Qt::PenJoinStyle>( qPen["penJoinStyle"].toInt() ) );

    if( qPen["width"].isDouble() )
        pen.setWidth( qPen["width"].toInt() );

    return pen;
}

QPixmap StyleManager::parseQPixmap( const QString& qPixmap ) { return QPixmap( qPixmap ); }

QSize StyleManager::parseQSize( const QJsonArray& qSize ) {
    qint32 size[2];

    auto i = 0;
    foreach( const auto& value, qSize ) {
        if( value.isDouble() ) {
            size[i] = value.toInt();
            i++;
        } else {
            return QSize();
        }
    }

    return QSize( size[0], size[1] );
}

QVariantMap StyleManager::parseSocket( const QJsonObject& sock ) {
    QVariantMap map;

    if( sock["animationDuration"].isDouble() )
        map["animationDuration"] = sock["animationDuration"].toInt();

    if( sock["animationLightness"].isDouble() )
        map["animationLightness"] = sock["animationLightness"].toInt();

    if( sock["animationEnabled"].isBool() )
        map["animationEnabled"] = sock["animationEnabled"].toBool();

    if( sock["fillColor"].isObject() )
        map["fillColor"] = this->parseQColor( sock["fillColor"].toObject() );

    if( sock["outlineColor"].isObject() )
        map["outlineColor"] = this->parseQColor( sock["outlineColor"].toObject() );

    if( sock["labelBrush"].isObject() )
        map["labelBrush"] = this->parseQBrush( sock["labelBrush"].toObject() );

    if( sock["socketBrush"].isObject() )
        map["socketBrush"] = this->parseQBrush( sock["socketBrush"].toObject() );

    if( sock["font"].isObject() )
        map["font"] = this->parseQFont( sock["font"].toObject() );

    if( sock["socketPen"].isObject() )
        map["socketPen"] = this->parseQPen( sock["socketPen"].toObject() );

    if( sock["multipleConnections"].isBool() )
        map["multipleConnections"] = sock["multipleConnections"].toBool();

    if( sock["socketShapeSize"].isDouble() )
        map["socketShapeSize"] = static_cast<qreal>( sock["socketShapeSize"].toDouble() );

    if( sock["socketShape"].isDouble() )
        map["socketShape"] = sock["socketShape"].toInt();

    return map;
}
#endif

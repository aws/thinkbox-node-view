// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QSet>
#include <QVariant>

class QGradient;

#if QT_VERSION >= 0x050301
// these are Qt5 features that aren't available in Krakatoa for Maya 2015 & 2016
class QJsonObject;
class QJsonArray;
#endif

class Styleable {
  public:
    virtual ~Styleable(){};

  protected:
    virtual void requiresStyling( QObject* item ) = 0;
};

class StyleManager : public QObject {
    Q_OBJECT
  private:
    QString m_styleSheet;
    QVariantMap m_styleMap;
    QSet<QObject*> m_styleItems;

  public:
    /*!
     * \brief StyleManager
     * \param parent
     */
    explicit StyleManager( QObject* parent = 0 );

    /*!
     * \brief registerStyleableItem
     * \param styleItem
     */
    void registerStyleableItem( Styleable* styleItem );

    /*!
     * \brief setStyleMap
     * \param map
     */
    void setStyleMap( const QVariantMap map );

    /*!
     * \brief setStyleSheet
     * \param style
     */
    void setStyleSheet( const QString style );

    /*!
     * \brief styleSheet
     * \return
     */
    QString styleSheet() const;

  public slots:
    /*!
     * \brief onRequiresStyling
     */
    void onRequiresStyling( QObject* item );

    /*!
     * \brief onStyleItemDestroyed
     */
    void onStyleItemDestroyed( QObject* item );

  private:
    /*!
     * \brief applyStyleSheet
     */
    void applyStyleSheet();

    /*!
     * \brief applyStyleSheet
     * \param styleItem
     */
    void applyStyleSheet( QObject* styleItem );

    //	void applyStyleToConnection(Connection *conn);

    //	void applyStyleToNode(Node *node);

    //	void applyStyleToSocket(Socket *sock);

#if QT_VERSION >= 0x050301
    /*!
     * \brief parseConnection
     * \param conn
     * \return
     */
    QVariantMap parseConnection( const QJsonObject& conn );

    /*!
     * \brief parseJSON
     */
    void parseJSON();

    /*!
     * \brief parseNode
     * \param node
     * \return
     */
    QVariantMap parseNode( const QJsonObject& node );

    /*!
     * \brief parseQBrush
     * \param qBrush
     * \return
     */
    QBrush parseQBrush( const QJsonObject& qBrush );

    /*!
     * \brief parseQColor
     * \param qColor
     * \return
     */
    QColor parseQColor( const QJsonObject& qColor );

    /*!
     * \brief parseQFont
     * \param qFont
     * \return
     */
    QFont parseQFont( const QJsonObject& qFont );

    //	const QGradient *parseQGradient(const QScriptValue &qGradient);

    /*!
     * \brief parseQPen
     * \param qPen
     * \return
     */
    QPen parseQPen( const QJsonObject& qPen );

    /*!
     * \brief parseQPixmap
     * \param qPixmap
     * \return
     */
    QPixmap parseQPixmap( const QString& qPixmap );

    /*!
     * \brief parseQSize
     * \param qSize
     * \return
     */
    QSize parseQSize( const QJsonArray& qSize );

    /*!
     * \brief parseSocket
     * \param sock
     * \return
     */
    QVariantMap parseSocket( const QJsonObject& sock );
#endif
};

#endif // STYLEMANAGER_H

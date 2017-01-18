#ifndef GTFSSERVICE_H
#define GTFSSERVICE_H

#include <QObject>

#include <QStringList>
#include <QPointF>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QCache>

#include "gtfsstop.h"

class GTFSService : public QObject
{
    Q_OBJECT
    Q_ENUMS(RouteTypes)
public:
    explicit GTFSService(QSqlDatabase db, QObject *parent = 0);

    //Q_INVOKABLE getRoute(QString route_name);
    //Q_INVOKABLE getRoutes();

    Q_INVOKABLE QString findStop(const QString stop_name);
    Q_INVOKABLE QStringList findStops(const QString stop_name);
    Q_INVOKABLE QString findNearestStop(const QPointF &pos, int type, double range);
    Q_INVOKABLE QStringList findNearestStops(const QPointF &pos, int type, double range);

    GTFSStop * getStop(const QString stop_id);

    QVariantMap getRoute(const QString route_id);

    QVariantList getShape(const QString shape_id);

    Q_INVOKABLE QVariantList getStops();

    enum RouteTypes {
        LightRail=0, // : Tram, Streetcar, Light rail. Any light rail or street level system within a metropolitan area.
        Subway=1, // Subway, Metro. Any underground rail system within a metropolitan area.
        Rail=2, // Rail. Used for intercity or long-distance travel.
        Bus=3, // Bus. Used for short- and long-distance bus routes.
        Ferry=4, // Ferry. Used for short- and long-distance boat service.
        CableCar=5, // Cable car. Used for street-level cable cars where the cable runs beneath the car.
        Gondola=6, // Gondola, Suspended cable car. Typically used for aerial cable cars where the car is suspended from the cable.
        Funicular=7 // Funicular. Any rail system designed for steep inclines.
    };

signals:

public slots:

protected:    
    QString getOne(QSqlQuery &q);
    QStringList getAllOne(QSqlQuery &q);
    QVariantMap getOneMap(QSqlQuery &q);

    QVariantList getAllListMap(QSqlQuery &q);
private:
    QSqlDatabase m_db;
    bool m_has_fares;
    bool m_has_shapes;
    bool m_has_dates;
    bool m_has_freq;
    bool m_has_transfers;

    QCache<QString, GTFSStop>m_stops;
    QVariantMap r2m(const QSqlRecord &record);
};

#endif // GTFSSERVICE_H

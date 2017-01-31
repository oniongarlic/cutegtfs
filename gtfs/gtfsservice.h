#ifndef GTFSSERVICE_H
#define GTFSSERVICE_H

#include <QObject>

#include <QStringList>
#include <QPointF>
#include <QDebug>

#include <QDate>
#include <QDateTime>

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

    enum PickupTypes {
        RegularPickup=0, // Regularly scheduled pickup
        NoPickup=1, // No pickup available
        PhonePickup=2, // Must phone agency to arrange pickup
        DriverPickup=3 // Must coordinate with driver to arrange pickup
    };

    enum StopTypes {
        LocationStop=0, // Stop. A location where passengers board or disembark from a transit vehicle.
        LocationStation=1 // Station. A physical structure or area that contains one or more stop.
    };

    //Q_INVOKABLE getRoute(QString route_name);
    //Q_INVOKABLE getRoutes();

    Q_INVOKABLE QString findStop(const QString stop_name);
    Q_INVOKABLE QStringList findStops(const QString stop_name);
    Q_INVOKABLE QString findNearestStop(const QPointF &pos, int type, double range);
    Q_INVOKABLE QStringList findNearestStops(const QPointF &pos, int type, double range);

    GTFSStop * getStop(const QString stop_id);

    Q_INVOKABLE QVariantMap getRoute(const QString route_id);

    Q_INVOKABLE QVariantList getShape(const QString shape_id);

    Q_INVOKABLE QVariantList getStopTrips(const QString stop_id, const QDate date, const int after=0);

    Q_INVOKABLE QVariantList getAgencies();

    Q_INVOKABLE QVariantList getStops();
    // Q_INVOKABLE QVariantList getStopsInside(const QPointF &a, const QPointF &b);

    bool isServiceOnDate(const QString &service_id, const QDate &date);
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
    QString dateToGTFSDate(const QDate &date);

    QHash<int, QByteArray> days() const {
        QHash<int, QByteArray> days;
        days[Qt::Monday] = "monday";
        days[Qt::Tuesday] = "tuesday";
        days[Qt::Wednesday] = "wednesday";
        days[Qt::Thursday] = "thursday";
        days[Qt::Friday] = "friday";
        days[Qt::Saturday] = "saturday";
        days[Qt::Sunday] = "sunday";
        return days;
    }

    int timeStringToSeconds(const QString &f);
};

#endif // GTFSSERVICE_H

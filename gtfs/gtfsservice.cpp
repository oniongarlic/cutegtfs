#include "gtfsservice.h"

#define SELECT_NEAREST_STOP "SELECT stop_id,stop_code,stop_name,((:lat-stop_lat)*(:lat-stop_lat))+((:lon-stop_lon)*(:lon-stop_lon)) AS d," \
        " stop_lat,stop_lon FROM stops WHERE location_type=:type "\
        " AND stop_lat BETWEEN :lat-:r AND :lat+:r "\
        " AND stop_lon BETWEEN :lon-:r AND :lon+:r "\
        " ORDER BY d LIMIT 1"

#define SELECT_NEAREST_STOPS "SELECT stop_id,stop_code,stop_name,((:lat-stop_lat)*(:lat-stop_lat))+((:lon-stop_lon)*(:lon-stop_lon)) AS d," \
        " stop_lat,stop_lon FROM stops WHERE location_type=:type "\
        " AND stop_lat BETWEEN :lat-:r AND :lat+:r "\
        " AND stop_lon BETWEEN :lon-:r AND :lon+:r "\
        " ORDER BY d LIMIT 50"

GTFSService::GTFSService(QSqlDatabase db, QObject *parent) : QObject(parent)
{
    QStringList tables;
    m_db=db;

    tables=db.tables();
    m_has_shapes=tables.contains("shapes");
    m_has_fares=tables.contains("fares");
    m_has_transfers=tables.contains("transfers");
    m_has_freq=tables.contains("frequencies");
}

QString GTFSService::findStop(QString stop_name)
{
    QSqlQuery q(m_db);

    q.setForwardOnly(true);
    QString r;

    q.prepare("SELECT stop_id FROM stops WHERE stop_id=? OR stop_code=? OR stop_name=?");
    q.bindValue(0, stop_name);
    q.bindValue(1, stop_name);
    q.bindValue(2, stop_name);
    if (!q.exec()) {
        qWarning("Query failed");
        return r;
    }
    if (q.isActive() && q.first()) {
        r.append(q.value(0).toString());
    } else {
        qDebug("No results");
        qDebug() << q.lastQuery();
    }
    return r;
}

QString GTFSService::findStops(QString stop_name)
{
    QSqlQuery q(m_db);

    q.prepare("SELECT stop_id,stop_code,stop_name,stop_lat,stop_lon FROM stops WHERE stop_name like'%?%'");
    q.bindValue(0, stop_name);
    q.exec();
}

QString GTFSService::findNearestStop(QPointF pos, int type, double range)
{
    QSqlQuery q(m_db);

    q.prepare(SELECT_NEAREST_STOP);
    q.bindValue(":lat", pos.x());
    q.bindValue(":lon", pos.y());
    q.bindValue(":r", range);
    q.bindValue(":type", type);
    q.exec();
}

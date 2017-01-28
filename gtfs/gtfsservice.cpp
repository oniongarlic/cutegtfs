#include "gtfsservice.h"

GTFSService::GTFSService(QSqlDatabase db, QObject *parent) : QObject(parent)
{
    QStringList tables;
    m_db=db;

    tables=db.tables();
    m_has_shapes=tables.contains("shapes");
    m_has_fares=tables.contains("fares");
    m_has_transfers=tables.contains("transfers");
    m_has_freq=tables.contains("frequencies");
    m_has_dates=tables.contains("calendar_dates");

    qDebug() << "Shapes" << m_has_shapes;
    qDebug() << "Dates" << m_has_dates;
}

QString GTFSService::dateToGTFSDate(const QDate &date)
{
    return date.toString("yyyyMMdd");
}

QString GTFSService::getOne(QSqlQuery &q)
{
    QString r;

    if (!q.exec()) {
        qDebug() << "Query failed" << q.lastError().text();
        return r;
    }
    if (!q.isActive()) {
        qDebug() << "Query failed, not active"<< q.lastQuery() << q.lastError().text();
        return r;
    }
    if (q.first())
        r.append(q.value(0).toString());

    return r;
}

QVariantMap GTFSService::getOneMap(QSqlQuery &q)
{
    QVariantMap r;

    if (!q.exec()) {
        qWarning("Query failed");
        qDebug() << q.lastError().text();
        return r;
    }
    if (q.isActive() && q.first())
        r=r2m(q.record());

    return r;
}

QVariantMap GTFSService::r2m(const QSqlRecord &record)
{
    QVariantMap map;

    for(int i = 0; i < record.count(); i++) {
        QString f=record.fieldName(i);
        QVariant v=record.value(i);

        if (f.endsWith("_time__")) {
            QTime t=QTime::fromString(v.toString(), "hh:mm:ss");
            map.insert(f, t);
        } else if (f.endsWith("_date")) {
            QDate t=QDate::fromString(v.toString(), "yyyyMMdd");
            map.insert(f, t);
        } else {
            map.insert(f, record.value(i));
        }
    }

    return map;
}

QStringList GTFSService::getAllOne(QSqlQuery &q)
{
    QStringList r;

    if (!q.exec()) {
        qWarning("Query failed");
        qDebug() << q.lastError().text() << q.lastQuery();
        return r;
    }
    if (q.isActive()) {
        while (q.next()) {
            qDebug() << q.value(0).toString() << q.value(1).toString();
            r << q.value(0).toString();
        }
    } else {
        qDebug() << q.lastQuery() << q.lastError().text();
    }

    return r;
}

QVariantList GTFSService::getAllListMap(QSqlQuery &q)
{
    QVariantList r;

    if (!q.exec()) {
        qWarning("Query failed");
        qDebug() << q.lastError().text() << q.lastQuery();
        return r;
    }
    if (q.isActive()) {
        while (q.next()) {
            QVariantMap m=r2m(q.record());
            r << m;
        }
    } else {
        qDebug() << "Error" << q.lastQuery() << q.lastError().text();
    }

    return r;
}

QVariantList GTFSService::getStops()
{
    QSqlQuery q(m_db);

    q.setForwardOnly(true);
    q.prepare("SELECT stop_id,stop_code,stop_name,stop_desc,stop_lat,stop_lon,zone_id,stop_url,location_type,parent_station,stop_timezone FROM stops ORDER BY stop_id");

    return getAllListMap(q);
}

GTFSStop *GTFSService::getStop(const QString stop_id)
{
    QSqlQuery q(m_db);
    GTFSStop *stop=NULL;

#if 0
    if (m_stops.contains(stop_id)) {
        stop=m_stops.take(stop_id);
    }
#endif

    q.setForwardOnly(true);
    q.prepare("SELECT stop_id,stop_code,stop_name,stop_desc,stop_lat,stop_lon,zone_id,location_type,parent_station,stop_timezone FROM stops WHERE stop_id=?");
    q.bindValue(0, stop_id);

    if (!q.exec()) {
        qWarning("Query failed");
        qDebug() << q.lastError().text();
        return stop;
    }

    if (q.isActive() && q.first()) {
        const QString id=q.value(0).toString();
        stop=new GTFSStop(
                    id,
                    q.value(2).toString(),
                    q.value(3).toString(),
                    q.value(4).toDouble(),
                    q.value(5).toDouble(),
                    q.value(7).toInt(),
                    q.value(8).toString()
                    );
        //m_stops.insert(id, stop);
    }

    return stop;
}

QVariantMap GTFSService::getRoute(const QString route_id)
{
    QSqlQuery q(m_db);

    q.setForwardOnly(true);
    q.prepare("SELECT route_id,agency_id,route_short_name,route_long_name,route_desc,route_type,route_url,route_color,route_text_color FROM routes WHERE route_id=?");
    q.bindValue(0, route_id);

    return getOneMap(q);
}

QVariantList GTFSService::getShape(const QString shape_id)
{
    QSqlQuery q(m_db);

    q.setForwardOnly(true);
    q.prepare("SELECT shape_pt_lat AS lat,shape_pt_lon as LON,shape_pt_sequence AS seq,shape_dist_traveled AS dist FROM shapes WHERE shape_id=? ORDER BY seq");
    q.bindValue(0, shape_id);

    return getAllListMap(q);
}

QVariantList GTFSService::getStopTrips(const QString stop_id, const QDate date)
{
    QSqlQuery q(m_db);
    QVariantList stops;

    q.setForwardOnly(true);
    q.prepare("SELECT t.route_id AS route_id,t.service_id,t.trip_id,t.shape_id,direction_id,block_id,route_short_name,arrival_time,departure_time FROM stop_times AS st,trips AS t,routes AS r WHERE stop_id=? AND st.trip_id=t.trip_id AND t.route_id=r.route_id ORDER BY arrival_time");
    q.bindValue(0, stop_id);

    stops=getAllListMap(q);

    QMutableListIterator<QVariant> i(stops);
    while (i.hasNext()) {
        QVariantMap s=i.next().toMap();
        //qDebug() << "STOP DATA:" << s;

        bool is=isServiceOnDate(s.value("service_id").toString(), date);
        if (is==false)
            i.remove();
    }

    return stops;
}

/**
 * @brief GTFSService::isServiceOnDate
 * @param service_id
 * @param date
 * @return
 *
 * Check if given service_id is available on the given date by looking into the calendar table for day specific details.
 *
 */
bool GTFSService::isServiceOnDate(const QString &service_id, const QDate &date)
{
    QSqlQuery q(m_db);
    QString dayStr;
    int day=date.dayOfWeek();

    dayStr=days().value(day);

    // First check calendar table for service entry
    q.prepare("SELECT "+dayStr+" FROM calendar WHERE start_date<=? AND ?<=end_date AND service_id=?");
    q.bindValue(0, dateToGTFSDate(date));
    q.bindValue(1, dateToGTFSDate(date));
    q.bindValue(2, service_id);

    QString qr=getOne(q);

    //qDebug() << service_id << date << qr;

    if (qr.isEmpty()) {
        qWarning() << "Calendar information not available for service " << service_id << " on day " << date;
        // return true;
    }

    // Check return value, in case value is 1 it
    if (qr=="1" && !m_has_dates)
        return true;
    else if (qr=="0" && !m_has_dates)
        return false;

    if (m_has_dates) {
        QSqlQuery q(m_db);

        qDebug() << "Exception check " << service_id << date;

        q.prepare("SELECT exception_type FROM calendar_dates WHERE service_id=? AND date=?");
        q.bindValue(0, service_id);
        q.bindValue(1, dateToGTFSDate(date));

        QString et=getOne(q);

        if (et=="1")
            return true;
        if (et=="0")
            return false;
    }


    return false;
}

QVariantList GTFSService::getAgencies()
{
    QSqlQuery q(m_db);

    q.setForwardOnly(true);
    q.prepare("SELECT agency_id,agency_name,agency_url,agency_timezone,agency_lang,agency_phone FROM agency ORDER BY agency_id");

    return getAllListMap(q);
}

QString GTFSService::findStop(const QString stop_name)
{
    QSqlQuery q(m_db);

    q.setForwardOnly(true);
    q.prepare("SELECT stop_id FROM stops WHERE stop_id=? OR stop_code=? OR stop_name like ?");
    q.bindValue(0, stop_name);
    q.bindValue(1, stop_name);
    q.bindValue(2, "%"+stop_name+"%");

    return getOne(q);
}

QStringList GTFSService::findStops(const QString stop_name)
{
    QSqlQuery q(m_db);

    q.prepare("SELECT stop_id,stop_code,stop_name,stop_lat,stop_lon FROM stops WHERE stop_name like ?");
    q.bindValue(0, "%"+stop_name+"%");
    return getAllOne(q);
}

#define SELECT_NEAREST_STOP "SELECT stop_id,((?-stop_lat)*(?-stop_lat))+((?-stop_lon)*(?-stop_lon)) AS d" \
    " FROM stops WHERE location_type=? "\
    " AND stop_lat BETWEEN ? AND ? "\
    " AND stop_lon BETWEEN ? AND ? "\
    " ORDER BY d LIMIT 1"

QString GTFSService::findNearestStop(const QPointF &pos, int type, double range)
{
    QSqlQuery q(m_db);

    q.prepare(SELECT_NEAREST_STOP);
    q.bindValue(0, pos.x());
    q.bindValue(1, pos.x());

    q.bindValue(2, pos.y());
    q.bindValue(3, pos.y());

    q.bindValue(4, type);

    q.bindValue(5, pos.x()-range);
    q.bindValue(6, pos.x()+range);

    q.bindValue(7, pos.y()-range);
    q.bindValue(8, pos.y()+range);

    return getOne(q);
}

#define SELECT_NEAREST_STOPS "SELECT stop_id,((?-stop_lat)*(?-stop_lat))+((?-stop_lon)*(?-stop_lon)) AS d" \
    " FROM stops WHERE location_type=? "\
    " AND stop_lat BETWEEN ? AND ? "\
    " AND stop_lon BETWEEN ? AND ? "\
    " ORDER BY d ASC LIMIT 20"

QStringList GTFSService::findNearestStops(const QPointF &pos, int type, double range)
{
    QSqlQuery q(m_db);

    q.prepare(SELECT_NEAREST_STOPS);
    q.bindValue(0, pos.x());
    q.bindValue(1, pos.x());

    q.bindValue(2, pos.y());
    q.bindValue(3, pos.y());

    q.bindValue(4, type);

    double x1=pos.x()-range,x2=pos.x()+range;

    qDebug() << x1 << x2;

    q.bindValue(5, QVariant(x1));
    q.bindValue(6, QVariant(x2));

    q.bindValue(7, pos.y()-range);
    q.bindValue(8, pos.y()+range);

    return getAllOne(q);
}

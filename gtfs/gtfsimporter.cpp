#include "gtfsimporter.h"

#define GTFS_STOPS_TABLE "CREATE TABLE IF NOT EXISTS stops ( \
stop_id text primary key, \
stop_code text, \
stop_name text, \
stop_desc text, \
stop_lat double not null, \
stop_lon double not null, \
zone_id text, \
stop_url text, \
location_type int, \
parent_station int, \
stop_timezone text, \
wheelchair_boarding int);"

#define GTFS_AGENCY_TABLE "CREATE TABLE IF NOT EXISTS agency ( \
agency_id text primary key, \
agency_name text not null, \
agency_url text not null, \
agency_timezone text not null, \
agency_lang text, \
agency_phone text, \
agency_fare_url text, \
agency_email text);"

#define GTFS_STOPTIMES_TABLE "CREATE TABLE IF NOT EXISTS stop_times (\
trip_id text not null,\
arrival_time text not null,\
departure_time text not null,\
stop_id text not null,\
stop_sequence int not null,\
stop_headsign text,\
pickup_type int,\
drop_off_type int,\
shape_dist_traveled double,\
timepoint int);"

#define GTFS_ROUTES_TABLE "CREATE TABLE IF NOT EXISTS routes (\
route_id text primary key,\
agency_id text,\
route_short_name text not null,\
route_long_name text not null,\
route_desc text,\
route_type int not null,\
route_url text,\
route_color text,\
route_text_color);"

#define GTFS_TRIPS_TABLE "CREATE TABLE IF NOT EXISTS trips (\
route_id text not null,\
service_id text not null,\
trip_id text primary key,\
trip_headsign text,\
trip_short_name text,\
direction_id int,\
block_id text,\
shape_id text,\
wheelchair_accessible int,\
bikes_allowed int);"

#define GTFS_CALENDAR_TABLE "CREATE TABLE IF NOT EXISTS calendar (\
service_id text primary key,\
monday int not null,\
tuesday int not null,\
wednesday int not null,\
thursday int not null,\
friday int not null,\
saturday int not null,\
sunday int not null,\
start_date text not null,\
end_date text not null);"

#define GTFS_CALENDARDATES_TABLE "CREATE TABLE IF NOT EXISTS calendar_dates (\
service_id text not null,\
date text not null,\
exception_type int not null, primary key(service_id, date));"

#define GTFS_SHAPES_TABLE "CREATE TABLE  IF NOT EXISTS shapes (\
shape_id text not null,\
shape_pt_lat real not null,\
shape_pt_lon real not null,\
shape_pt_sequence int not null,\
shape_dist_traveled real,\
primary key (shape_id,shape_pt_sequence));"

GTFSImporter::GTFSImporter(QSqlDatabase db, QObject *parent) : QObject(parent)
{
    m_db=db;
    m_tables.insert("agency", GTFS_AGENCY_TABLE);
    m_tables.insert("stops", GTFS_STOPS_TABLE);
    m_tables.insert("stop_times", GTFS_STOPTIMES_TABLE);
    m_tables.insert("routes", GTFS_ROUTES_TABLE);
    m_tables.insert("trips", GTFS_TRIPS_TABLE);
    m_tables.insert("calendar", GTFS_CALENDAR_TABLE);
    m_tables.insert("calendar_dates", GTFS_CALENDARDATES_TABLE);
    m_tables.insert("shapes", GTFS_SHAPES_TABLE);
    //m_tables.insert("transfers", GTFS_TRANSFERS_TABLE);
}

bool GTFSImporter::initialize()
{
    return true; // XXX
}

bool GTFSImporter::createTable(const QString table)
{
    QSqlQuery query(m_db);
    bool r;
    QString tdef=m_tables.value(table);

    if (tdef.isEmpty()) {
        qWarning() << "Definition not found for table " << table;
        return false;
    }

    r=query.exec(tdef);
    if (!r)
        qWarning("Table query failed");
    return r;
}

bool GTFSImporter::import(const QString table, const QStringList headers, const QMap<uint, QStringList> &data, bool update)
{
    QMapIterator<uint, QStringList> i(data);
    QString f=headers.join(",");
    QString v;

    if (data.size()==0) {
        qWarning() << "No data for table " << table << " ignoring.";
        return true;
    }

    if (createTable(table)==false) {
        qWarning() << "Failed to create table " << table;
        return false;
    }

    int hc=headers.size();
    for (int h=1;h<=hc;h++) {
        v.append('?');
        if (h<hc)
            v.append(',');
    }

    if (!m_db.transaction()) {
        qWarning("Failed to start transaction!");
        return false;
    } else {
        qDebug("Removing old data");
    }

    if (update) {
        QSqlQuery q(m_db);
        q.exec("DELETE FROM "+table+"");
    }

    while (i.hasNext()) {
        i.next();

        // catch invalid rows
        if (hc!=i.value().size()) {
            qWarning("Invalid GTFS data row");
            continue;
        }

        QSqlQuery query(m_db);
        query.prepare("INSERT INTO "+table+" ("+f+") VALUES ("+v+")");
        for (int h=0;h<hc;h++) {
            // qDebug() << h << ":" << i.value().at(h);
            query.bindValue(h, i.value().at(h));
        }
        if (!query.exec()) {
            qWarning() << "Query failed: " << query.lastError() ;
            qDebug() << query.lastQuery();
        }
    }
    return m_db.commit();
}

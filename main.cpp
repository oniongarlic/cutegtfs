#include <QCoreApplication>
#include <QFile>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QPointF>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "csv/readcsv.h"
#include "gtfs/gtfsimporter.h"
#include "gtfs/gtfsarchiveparser.h"
#include "gtfs/gtfsservice.h"

QSqlDatabase db;

void testCSV()
{
    readCSV csv;
    QFile f("quotetest.csv");
    f.open(QIODevice::ReadOnly);
    bool r=csv.parseCSV(f);
    qDebug() << "Result is " << r;
    f.close();
    qDebug() << csv.getData();
}

bool importData()
{
    GTFSImporter gtfs_imp(db);
    gtfs_imp.initialize();

    GTFSArchiveParser ga(&gtfs_imp);
    return ga.readArchive("gtfs.zip");
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList tables;

    qDebug() << db.drivers();
    db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");
    if (!db.open()) {
        qWarning("Failed to open database");
        qDebug() << db.lastError();
        exit(1);
    }
    tables=db.tables();

    qDebug() << tables;

    if (tables.count()==0) {
        importData();
    }

    GTFSService gtfs(db);

    qDebug() << gtfs.findStop("T2");

    qDebug() << gtfs.findStop("T22");

    qDebug() << gtfs.findStop("1170");
    qDebug() << gtfs.findStop("Paattisten kirkko");

    qDebug() << gtfs.findStops("Paattis");

    //1127,,Paattisten kirkko,,60.59149206168896,22.38063743684942,FÖLI,,0,,Europe/Helsinki
    //1170,,Paattisten kirkko,,60.59145527814784,22.38009434217617,FÖLI,,0,,Europe/Helsinki
    //1171,,Paattisten kirkko,,60.5907081332966,22.37798961826015,FÖLI,,0,,Europe/Helsinki

    qDebug() << gtfs.findNearestStop(QPointF(60.59198f, 22.38185f), 0, 0.01f);

    GTFSStop *s=gtfs.getStop("1170");

    if (s) {
        qDebug() << s->id << s->location.x() << s->location.y() << s->name << s->desc << s->type;

        //
        qDebug() << gtfs.findNearestStops(s->location, 0, 0.009f);

        delete s;
    }

    qDebug() << gtfs.findNearestStops(QPointF(60.5914f,22.3800f), 0, 0.005f);
    qDebug() << gtfs.getRoute("23");

    qDebug() << gtfs.getStops();

    qDebug() << gtfs.getShape("0_358");

    db.close();

    exit(0);

    return a.exec();
}

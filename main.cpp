#include <QCoreApplication>
#include <QFile>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "readcsv.h"
#include "gtfsimporter.h"
#include "gtfsarchiveparser.h"
#include "gtfsservice.h"

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
        GTFSImporter gtfs_imp(db);
        gtfs_imp.initialize();

        GTFSArchiveParser ga(&gtfs_imp);
        ga.readArchive("gtfs.zip");
    }

    GTFSService gtfs(db);
    qDebug() << gtfs.findStop("1170");
    qDebug() << gtfs.findStop("Paattisten kirkko");

    db.close();

    exit(0);

    return a.exec();
}

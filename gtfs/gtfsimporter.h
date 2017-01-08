#ifndef GTFSIMPORTER_H
#define GTFSIMPORTER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class GTFSImporter : public QObject
{
    Q_OBJECT
public:
    explicit GTFSImporter(QSqlDatabase db, QObject *parent = 0);
    bool initialize();
    bool import(const QString table, const QStringList headers, const QMap<uint, QStringList> &data, bool update=false);

signals:

public slots:

private:
    bool createTable(const QString table);
    QSqlDatabase m_db;    
    QMap<QString, QString> m_tables;
};

#endif // GTFSIMPORTER_H

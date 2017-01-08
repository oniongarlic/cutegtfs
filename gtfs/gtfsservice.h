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

class GTFSService : public QObject
{
    Q_OBJECT
public:
    explicit GTFSService(QSqlDatabase db, QObject *parent = 0);

    //Q_INVOKABLE QString findStop(QString stop_name);
    //Q_INVOKABLE getRoute(QString route_name);
    //Q_INVOKABLE findNearestStop();
    //Q_INVOKABLE findNearestStops();
    //Q_INVOKABLE getRoutes();

    QString findStop(QString stop_name);
    QString findStops(QString stop_name);    
    QString findNearestStop(const QPointF &pos, int type, double range);
signals:

public slots:

protected:    

    QString getOne(QSqlQuery &q);
private:
    QSqlDatabase m_db;
    bool m_has_fares;
    bool m_has_shapes;
    bool m_has_dates;
    bool m_has_freq;
    bool m_has_transfers;
};

#endif // GTFSSERVICE_H

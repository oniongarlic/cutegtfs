#ifndef GTFSSTOP_H
#define GTFSSTOP_H

#include <QObject>
#include <QPointF>
#include <QString>

#include <QDebug>

class GTFSStop
{
    Q_GADGET
public:
    GTFSStop();
    explicit GTFSStop(QString _id, QString _name, QString _desc, double _lat, double _lon, uint _type, QString _parent);
    const QString id;
    const QString name;
    const QString desc;
    const QPointF location;
    const uint type;
    const QString parent;

    bool operator==(const GTFSStop &s) const;
};

Q_CORE_EXPORT QDebug operator<<(QDebug debug, const GTFSStop &s);

//Q_DECLARE_METATYPE(GTFSStop)

#endif // GTFSSTOP_H

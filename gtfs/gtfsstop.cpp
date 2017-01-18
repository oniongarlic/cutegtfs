#include "gtfsstop.h"

GTFSStop::GTFSStop()
    : id("")
    , name("")
    , location(0,0)
    , type(0)
    , parent("")
{

}

GTFSStop::GTFSStop(QString _id, QString _name, QString _desc, double _lat, double _lon, uint _type, QString _parent)
    : id(_id)
    , name(_name)
    , desc(_desc)
    , location(_lat, _lon)
    , type(_type)
    , parent(_parent)
{

}

bool GTFSStop::operator==(const GTFSStop &s) const
{
    return (s.id==id);
}

QDebug operator<<(QDebug debug, const GTFSStop &s)
{
    debug.nospace() << s.id << '(' << s.location.x() << ", " << s.location.y() << ')';

    return debug.maybeSpace();
}

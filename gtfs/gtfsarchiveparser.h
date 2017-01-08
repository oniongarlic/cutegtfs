#ifndef GTFSARCHIVEPARSER_H
#define GTFSARCHIVEPARSER_H

#include <QObject>
#include <QDebug>

#include "gtfsimporter.h"

class GTFSArchiveParser : public QObject
{
    Q_OBJECT
public:
    explicit GTFSArchiveParser(GTFSImporter *importer, QObject *parent = 0);
    bool readArchive(QString file);

signals:
    void progress(const QString file);

public slots:

private:
    GTFSImporter *m_gtfsimport;
};

#endif // GTFSARCHIVEPARSER_H

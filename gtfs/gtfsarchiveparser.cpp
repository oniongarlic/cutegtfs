#include "gtfsarchiveparser.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "readcsv.h"

GTFSArchiveParser::GTFSArchiveParser(GTFSImporter *importer, QObject *parent) : QObject(parent)
{
    m_gtfsimport=importer;
}

bool GTFSArchiveParser::readArchive(QString file)
{
    readCSV csv;
    QuaZip z(file);
    QuaZipFileInfo info;
    GTFSImporter *gtfs=m_gtfsimport;

    if (!z.open(QuaZip::mdUnzip)) {
        qWarning("Failed to open GTFS zip archive");
        return false;
    }

    QuaZipFile zf(&z);
    for(bool more=z.goToFirstFile(); more; more=z.goToNextFile()) {
        QString file=z.getCurrentFileName();
        QString t=file;
        t.chop(4);

        qDebug() << "File is: " << file;

        emit progress(file);

        z.getCurrentFileInfo(&info);
        zf.open(QIODevice::ReadOnly);
        csv.parseCSV(zf);
        zf.close();        

        gtfs->import(t, csv.getHeaders(), csv.getData());
    }

    z.close();
    return true;
}

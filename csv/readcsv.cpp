#include <QByteArray>
#include <QFile>
#include <QDebug>

#include "readcsv.h"

readCSV::readCSV(QObject *parent) : QObject(parent)
{

}

bool readCSV::parseCSV(QString sourceFile)
{
    QFile f(sourceFile);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    bool r=parseCSV(f);

    f.close();

    return r;
}

bool readCSV::parseCSV(QIODevice &source, bool append)
{
    enum TokenState { Normal, PossiblyQuoted, Quoted, DoubleQuoted };
    m_rows=0;

    if (!source.isOpen())
        return false;

    if (!source.isReadable())
        return false;

    if (!append)
        clear();

    QTextStream in(&source);

    TokenState s=Normal;

    QStringList row;
    QString value;

    while (!in.atEnd()) {
        QString data=in.read(65535);

        for (int i=0;i<data.size();i++) {
            QChar v=data.at(i);
            switch (s) {
            case Normal:
                switch (v.toLatin1()) {
                case '\n':
                    row.append(value);
                    if (m_rows==0 && m_header) {
                        m_headers=row;
                    } else {
                        m_data.insert(m_rows, row);
                    }
                    m_rows++;
                    row.clear();
                    value.clear();
                    emit progress(m_rows);
                    break;
                case '\r':
                    break;
                case ',':
                    row.append(value);
                    value.clear();
                    break;
                case '"':
                    s=PossiblyQuoted;
                    break;
                default:
                    value.append(v);
                    break;
                }
                break;
            case PossiblyQuoted:
                switch (v.toLatin1()) {
                case '"':
                    value.append(v);
                    s=Normal;
                    break;
                default:
                    value.append(v);
                    s=Quoted;
                }
                break;
            case Quoted:
                switch (v.toLatin1()) {
                case '"':
                    // XXX: Handle "" properly
                    s=Normal;
                    break;
                default:
                    value.append(v);
                    break;
                }
                break;
            }
        }
    }

    qDebug() << "Loaded rows: " << m_rows;
    if (m_header)
        qDebug() << m_headers;

    return true;
}

void readCSV::setHasHeader(bool v)
{
    m_header=v;
}

void readCSV::clear()
{
    m_data.clear();
    m_headers.clear();
    m_rows=0;
}

QStringList readCSV::getRow(uint row)
{
    if (m_data.contains(row))
        return m_data.value(row);
    return QStringList();
}

QStringList readCSV::getHeaders()
{
    return m_headers;
}

QMap<uint, QStringList> &readCSV::getData()
{
    return m_data;
}

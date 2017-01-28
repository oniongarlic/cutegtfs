#ifndef READCSV_H
#define READCSV_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QIODevice>

class readCSV : public QObject
{
    Q_OBJECT
public:
    explicit readCSV(QObject *parent = 0);
    Q_INVOKABLE bool parseCSV(QString sourceFile);
    Q_INVOKABLE bool parseCSV(QIODevice &source, bool append=false);
    Q_INVOKABLE void setHasHeader(bool v);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QStringList getRow(uint row);
    Q_INVOKABLE QStringList getHeaders();
    Q_INVOKABLE QMap<uint, QStringList> &getData() ;

signals:
    void progress(uint row);

public slots:

private:
    bool m_header;
    uint m_rows;
    QStringList m_headers;
    QMap<uint, QStringList>m_data;
};

#endif // READCSV_H

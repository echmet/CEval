#ifndef NETCDFFILELOADER_H
#define NETCDFFILELOADER_H

#include <tuple>
#include <vector>
#include <QString>


class NetCDFFileLoader
{
public:
    typedef std::vector<std::tuple<float, float>> Scans;
    class Data {
    public:
        Data() :
            scans(Scans()),
            xUnits(QString()),
            yUnits(QString())
        {}
        Data(const Scans &&scans, const QString &xUntis, const QString &yUnits) :
            scans(scans),
            xUnits(xUntis),
            yUnits(yUnits)
        {}

        const Scans scans;
        const QString xUnits;
        const QString yUnits;
    };

    NetCDFFileLoader() = delete;

    static Data load(const QString &path);

};

#endif // NETCDFFILELOADER_H

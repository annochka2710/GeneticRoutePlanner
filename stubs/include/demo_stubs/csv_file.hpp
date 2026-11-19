#pragma once
#include "unavailable.hpp"
#include <QStringList>
#include <QVector>
// Only the read/iteration API used in settings.cpp is represented here.
class CSVFile {
public:
    enum SyncMode { SYNC_OFF };
    using Rows = QVector<QStringList>;
    using iterator = Rows::iterator;
    using const_iterator = Rows::const_iterator;
    CSVFile(const QString &, SyncMode, char) {
        demo_stubs::unavailable("FileSystemUtils::CSVFile read");
    }
    iterator begin() { return rows_.begin(); }
    iterator end() { return rows_.end(); }
    const_iterator begin() const { return rows_.begin(); }
    const_iterator end() const { return rows_.end(); }
private:
    Rows rows_;
};

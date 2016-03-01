
#include "ZipDirIterator.h"
#include "AbZip.h"
#include "abzip_p.h"
#include "CentralDirFileHeader.h"
#include "Utils.h"

#include <QVector>
#include <QRegExp>

class ZipDirIteratorPrivate
{

public:
    ZipDirIteratorPrivate( const AbZip& zip, const QString& path, const QStringList &nameFilters,
                        AbZip::ZipOptions options );

    void findFirst();
    void advance();

    bool entryMatches(const QString & fileName, CentralDirFileHeader* header);
    bool matchesFilters(const QString &fileName, CentralDirFileHeader* header ) const;
    bool hasNext() const;

    const AbZip& zip;
    QString path;
    const QStringList nameFilters;
    const AbZip::ZipOptions options;

    QVector<QRegExp> nameRegExps;

    CentralDirFileHeader* currentHeader;
    CentralDirFileHeader* nextHeader;
};


ZipDirIteratorPrivate::ZipDirIteratorPrivate(const AbZip& zip, const QString& path, const QStringList &nameFilters,
                                         AbZip::ZipOptions options )
    : zip(zip)
      ,path(path)
      , nameFilters(nameFilters.contains(QLatin1String("*")) ? QStringList() : nameFilters)
      , options(options)
{
    currentHeader = NULL;
    nextHeader = NULL;

    nameRegExps.reserve(nameFilters.size());
    for (int i = 0; i < nameFilters.size(); ++i)
        nameRegExps.append( QRegExp(nameFilters.at(i),
                    (options & AbZip::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive,
                    QRegExp::Wildcard));


    // Populate headers for hasNext() and next()
    findFirst();
}


void ZipDirIteratorPrivate::findFirst()
{
    CentralDirFileHeader* header = zip.d_ptr->centralDir->getFirstFile();

    while ( header )
    {
        if (path.isEmpty() ||
                header->sFileName.startsWith( path, (options & AbZip::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive ) )
        {
            // Check our filters
            if (entryMatches( header->sFileName, header ))
            {
                currentHeader = nextHeader;
                return;
            }
        }

        // Try next file
        header = zip.d_ptr->centralDir->getNextFile();
    }
}

// This gets the next header if one is found
void ZipDirIteratorPrivate::advance()
{
    if ( !nextHeader )
    {
        currentHeader = NULL;
        return;
    }

    CentralDirFileHeader* header;
    while ( (header = zip.d_ptr->centralDir->getNextFile()) )
    {
        if (!path.isEmpty() &&
                !header->sFileName.startsWith( path, (options & AbZip::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive ) )
            break;  // reached the end of the 'path' we are searching for

        // Check our filters
        if (entryMatches( header->sFileName, header ))
            return;
    }

    // no more items to iterate, take final 'nextHeader'
    if ( nextHeader )
        currentHeader = nextHeader;
    nextHeader = NULL;
}


bool ZipDirIteratorPrivate::hasNext() const
{
    return nextHeader ? true : false;
}

inline bool ZipDirIteratorPrivate::entryMatches(const QString & fileName, CentralDirFileHeader* header)
{
    if (matchesFilters(fileName, header))
    {
        //We found a matching entry.
        currentHeader = nextHeader;
        nextHeader = header;
        return true;
    }

    return false;
}

bool ZipDirIteratorPrivate::matchesFilters(const QString &fileName, CentralDirFileHeader* header ) const
{
    Q_ASSERT(!fileName.isEmpty());

    // Pass all entries through name filters
    if ( !nameFilters.isEmpty() )
    {
        bool matched = false;
        for (QVector<QRegExp>::const_iterator iter = nameRegExps.constBegin(), end = nameRegExps.constEnd(); iter != end; ++iter)
        {
            QRegExp copy = *iter;
            if (copy.exactMatch(fileName))
            {
                matched = true;
                break;
            }
        }
        if (!matched)
            return false;
    }

    // skip directories
    if ( (options & AbZip::IgnoreFolders) && header->isDirectory())
        return false;

    // skip files
    if ( (options & AbZip::IgnoreFiles) && !header->isDirectory())
        return false;

    return true;
}


ZipDirIterator::ZipDirIterator(const AbZip& zip, const QString &path, AbZip::ZipOptions options)
    : d_ptr(new ZipDirIteratorPrivate(zip, path, QStringList(), options ))
{
}


ZipDirIterator::ZipDirIterator(const AbZip& zip, const QString &path, const QStringList &nameFilters,
                           AbZip::ZipOptions options)
    : d_ptr(new ZipDirIteratorPrivate(zip, path, nameFilters, options))
{
}

ZipDirIterator::~ZipDirIterator()
{
    delete d_ptr;
}

// Private get functions
CentralDirFileHeader* ZipDirIterator::getCurrentHeader()
{
    if ( d_ptr->currentHeader )
        return d_ptr->currentHeader;

    return NULL;
}


QString ZipDirIterator::next()
{
    d_ptr->advance();
    if ( d_ptr->currentHeader )
        return d_ptr->currentHeader->sFileName;

    return QString();
}

bool ZipDirIterator::hasNext() const
{
    return d_ptr->hasNext();
}

QString ZipDirIterator::fileName() const
{
    if ( d_ptr->currentHeader )
        return ZipUtils::getFileName( d_ptr->currentHeader->sFileName );
    return QString();
}

QString ZipDirIterator::filePath() const
{
    if ( d_ptr->currentHeader )
        return d_ptr->currentHeader->sFileName;
    return QString();
}

ZipFileInfo ZipDirIterator::fileInfo() const
{
    return ZipFileInfo( d_ptr->currentHeader );
}

QString ZipDirIterator::path() const
{
    if ( d_ptr->currentHeader )
        return QDir( d_ptr->currentHeader->sFileName ).path();
    return QString();
}


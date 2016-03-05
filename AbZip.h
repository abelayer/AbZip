/****************************************************************************
**
** Copyright (C) 2016 Andy Bray
**
** This file is part of AbZip.
**
** AbZip is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** AbZip is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with AbZip.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef ABZIP_H
#define ABZIP_H

#include "zipglobal.h"

#include <QObject>
#include <QIODevice>
#include <QDir>

class AbZipPrivate;
class ZipInfo;
class ZipFileInfo;

class ZIP_EXPORT AbZip : public QObject
{
    Q_OBJECT

    AbZipPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(AbZip)

    friend class ZipDirIteratorPrivate;

public:
    /// Constructs the default AbZip object.  You must use setZipFileName() before calling any other member functions
    AbZip(QObject *parent = 0);

    /// Constructs a AbZip for a specified Archive name
    AbZip(const QString& zipName, QObject *parent = 0);

    /// Constructs a AbZip for a given QIODevice
    AbZip(QIODevice *ioDevice, QObject *parent = 0);

    /// Destroys the AbZip and closes the archive.  Central Directory changes are save at when close() is called.
    virtual ~AbZip();

    enum Mode { modeOpenRead, ///< Opens the archive for reading only. You will not be able to perform any modification
                              /// operations whilst in this mode.
                modeOpenWrite, ///< Open the archive for both reading and writing.
                modeCreateNew  ///< Creates a new archive and opens it for reading and writing.
                               /// \b Warning: Unless you specify the BackupExisting option when creating the file
                               /// then if an archive with same name exists then it will be overwritten!
                               /// If BackupExisting is set, then the existing archive will be renamed.
              };

    /// Error codes. For all errors other than Ok, call errorString() to retrieve the error message
    enum ErrorCode
    {
        Ok,             ///< No Error
        GeneralError,   ///< General error message
        CreateDirError, ///< Failed to create a new folder
        OpenFileError,  ///< Unable to open the archive file or the file being added
        CreateFileError,///< Unable to create a temporary file for extraction
        InvalidArchive, ///< Unable to read the Central Directory form the archive file. It's either correcpt or not a valid Zip file
        SeekError,      ///< Failed to locate Local File Header in the zip file.  Most likely a corrupt Central Directory record.
        OpenInReadOnly, ///< The Archive is open in Read mode and you are trying to perform a write action.  Close and re-open for writing.
        FileExists,     ///< The file to be extracted already exists and you have specified the AbZip::DontOverwrite option.
        ExtractFailed,  ///< An error occurred extracting the file data from the archive. See errorString() for more information.
        AddFileFailed,  ///< A error occurred compressing and adding the data to the archive. See errorString() for more information.
        CopyError,      ///< An error occurred copying the temporary extracted file to the destination folder.
        SetModTimeFailed, ///< Unable to set the last modified time for the extracted file to that stored in the archive
        InvalidCompressor, ///< The decompression method required to extract the file is not supported by this library
        InvalidEncryption, ///< The decryption method required to extract the file is not supported by this library
        DeleteError,    ///< An error occurred writing to a temp archive while deleting files from the current archive
        Corrupted,      ///< Corrupt data found during extraction.  CRC or AES validation failed.
        InvalidPassword,///< Invalid password specified for decryption
        InvalidAesData, ///< Invalid WinZip AES extra data specified
        FileNotFound,   ///< The file to be added or extracted was not found
        WriteFailed,    ///< An error occurred writing the Central Directory to file
        HeaderIntegrityError, ///< The Central Directory Header file does not match the Local File Header
        CentralDirError ///< An error has occurred in reading the Central Directory
    };

    /// Archive options
    enum Option
    {
        None                 = 0x0000,
        VerifyOnly           = 0x0001,      ///< Used to verity the integrity of the archive. This is only valis on all 'extract' function calls
                                            /// See also checkIntegrity().
        CaseSensitive        = 0x0002,      ///< When searching for files in the archive or to add to the archive, use Case Sensitive matching
        Recursive            = 0x0004,      ///< When searching or adding directories, recurse into all sub folders
        DontOverwrite        = 0x0008,      ///< If specified, existing local files will not be overwritten.
        IgnoreFolders        = 0x0010,      ///< Ignore folders when searching the archive
        IgnoreFiles          = 0x0020,      ///< Ignore files when searching the archive

        BackupExisting       = 0x0040,      ///< If set and the open mode modeCreateNew is used, then if the archive exists it will be
                                            /// rename with an incremental number added to the file name
                                            /// (e.g. 'file.zip' becomes 'file (1).zip' and a new 'file.zip' created)

        ExtractPaths         = 0x0080,      ///< Recreate the relative path from the archive when extracting files

        AddRelativePaths       = 0x0100,    ///< Add files including relative paths
        AddAbsolutePaths       = 0x0200,    ///< Add files to the archive and store using the files absolute path
        AddIgnorePaths         = 0x0400,    ///< All files are put in the root of the archive file.  This is not a good options to use as
                                            /// you may get files with the same name being added and if SkipExistingFiles is not set
                                            /// then the original file will be overwritten!
        AddReplaceFiles        = 0x1000,    ///< Always add and replace files that exists and ignore if different
        SkipExistingFiles      = 0x2000,    ///< Don't overwrite any files of the same file name and location in archive
        ContinueOnError        = 0x4000,    ///< On operations that add or extract multiple files, then continue if an error occures

        IgnoreStoreOnlyChecks  = 0x8000,    ///< AbZip will detect any files that it thinks should not be compressed and store them raw
                                            /// (e.g. files that are already compressed).  If you want to ignore this feature, then specify this option.

        useBZip2Compression    = 0x10000,   ///< For the use of BZip2 compression over the default Deflate compression
        useAESEncryption       = 0x20000,   ///< Use WinZip AES strong encryption over the default crc encryption
        useLzmaCompression     = 0x40000    ///< Use Lzma compression over the default. This gives higher compression
                                            /// ratios but take a little longer than the default.
        };
        Q_DECLARE_FLAGS(ZipOptions, Option)

    /**
     * @brief Opens the archive for either reading or writing.
     * \b Note: If you don't call open() before calling one of the main add or extract functions, then AbZip will
     * automatically open the archive in the required mode. (e.g If you call addDirectory() then it opens in modeOpenWrite,
     * or if you call extractAll() and the archive is not already open, then it will be open with modeOpenRead.
     * This mode will remain untill you call close().
     * @param mode The open mode for the archive (Default: modeOpenRead)
     * @param options This is either None (default) or use BackupExisting with modeCreateNew
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     *
     * \sa modeOpenRead, modeOpenWrite, modeCreateNew
     * \sa Options: BackupExisting
     * \sa close();
     */
    bool open( Mode mode = modeOpenRead, ZipOptions options = None );

    /**
     * @brief Closes the archive if open.  If any file delete operations are pending, then these are commited at this point.
     *
     * \sa open();
     */
    void close();

    /**
     * @brief Checks if the archive is open or not.
     * @return \c true if the archive is open, else \c false
     */
    bool isOpen();

    /**
     * @brief Adds the specified file to the archive. If the file already exists and has the same last modified date,
     * then the file will not be added. If SkipExistingFiles option is specified, then existing files will not be
     * overwritten even if they are older. If AddReplaceFiles option is specified, then the file will be added
     * even if it is older then the existing file.
     * @param file The full path and file name for the file to be added.
     * @param root (optional) The directory in the archive to store this file.
     * @param options Options to apply during adding.
     * @param level (optional) The level to use for compression
     * from 0 = no compression to 9 high compression but slower (default is 8).
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     *
     * \sa Options: AddRelativePaths, AddAbsolutePaths, AddIgnorePaths, SkipExistingFiles, AddReplaceFiles
     */
    bool addFile(const QString& file, const QString& root, ZipOptions options = AddRelativePaths, int level = -1 );

    /**
     * @brief Adds the specified file to the archive. If the file already exists and has the same last modified date,
     * then the file will not be added. If SkipExistingFiles option is specified, then existing files will not be
     * overwritten even if they are older. If AddReplaceFiles option is specified, then the file will be added
     * even if it is older then the existing file.
     * @param file The full path and file name for the file to be added.
     * @param options Options to apply during adding.
     * @param level (optional) The level to use for compression
     * from 0 = no compression to 9 high compression but slower (default is 8).
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     *
     * \sa Options: AddRelativePaths, AddAbsolutePaths, AddIgnorePaths, SkipExistingFiles, AddReplaceFiles
     */
    bool addFile(const QString& file, ZipOptions options = AddRelativePaths, int level = -1 );

    /**
     * @brief Add all files in a directory to the archive.
     * You can set filters to only add files that match a specified mask (See setNameFilters() ).
     * By specifying the Recursive option, you can add all files from all sub folders.
     * @param srcPath The full path to the folder to add files
     * @param root Use this as the root folder when adding to the archive.
     * e.g.  If root is \c 'backup/files' and we are adding \c 'c:/folder1/folder2/file1.txt' and \c 'c:/folder1/folder2/folder3/file2.txt' then
     * - if AddRelativePath option specified, files will be added as \c 'backup/files/folder2/file1.txt' and \c 'backup/files/folder2/folder3/file2.txt'
     * - if AddAbsolutePaths option specified, files will be added as \c 'backup/files/folder1/folder2/file1.txt' and
     * \c 'backup/files/folder1/folder2/folder3/file2.txt'
     * - if AddIgnorePaths option specified, files will be added as \c 'backup/files/file1.txt' and \c 'backup/files/file2.txt'
     * @param options Options to apply during adding.
     * @param level (optional) The level to use for compression from 0 = no compression to 9 high compression but slower (default is 8).
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     *
     * \sa Options: Recursive, AddRelativePaths, AddAbsolutePaths, AddIgnorePaths, SkipExistingFiles, AddReplaceFiles, ContinueOnError
     * \sa setNameFilters();
     */
    bool addDirectory(const QString& srcPath, const QString&, ZipOptions options = AddRelativePaths, int level = -1 );

    /**
     * @brief Add all files in a directory to the archive.
     * You can set filters to only add files that match a specified mask (See setNameFilters() ).
     * By specifying the Recursive option, you can add all fiels from all sub folders.
     * See options for further
     * @param srcPath The full path to the folder to add files
     * @param options Options to apply during adding.
     * @param level (optional) The level to use for compression from 0 = no compression to 9 high compression but slower (default is 8).
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     *
     * \sa Options: Recursive, AddRelativePaths, AddAbsolutePaths, AddIgnorePaths, SkipExistingFiles, AddReplaceFiles, ContinueOnError
     * \sa setNameFilters();
     */
    bool addDirectory(const QString& srcPath, ZipOptions options = AddRelativePaths, int level = -1 );

    /**
     * @brief Extract all files from the archive to a specified destination path
     * If ContinueOnError option is not specified, then the operation will stop if any errors are encountered.
     * @param destPath The full path name to where to extract the files
     * @param options Options use for extracting
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     *
     * \sa Options: Recursive, ExtractPaths, SkipExistingFiles, ContinueOnError, DontOverwrite
     * \sa setNameFilters();
     */
    bool extractAll(const QString& destPath, ZipOptions options = ExtractPaths);
    /**
     * @brief Extract all files from the specified root archive folder to a specified destination path.
     * If ContinueOnError option is not specified, then the operation will stop if any errors are encountered.
     * @param destPath The full path name to where to extract the files
     * @param fromRoot Extract all file from this root folder within the archive
     * @param options Options use for extracting
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     */
    bool extractAll(const QString& destPath, const QString& fromRoot, ZipOptions options = ExtractPaths);

    /**
     * @brief Extract the specified file from the archive to a specified destination folder.
     * @param filename This is the full path and filename of the file to extract.
     * @param destPath The full path name to where to extract the file.
     * @param options Options use for extracting
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     *
     * \sa Options: ExtractPaths, SkipExistingFiles, DontOverwrite, CaseSensitive
     */
    bool extractFile(const QString& filename, const QString& destPath, ZipOptions options = ExtractPaths);

    /**
     * @brief Extract all files from the specified listFiles and save them to the destination folder.
     * If ContinueOnError option is not specified, then the operation will stop if any errors are encountered.
     * @param listFiles A QstringList containing all the files to extract
     * @param destPath The full path name to where to extract the file.
     * @param options Options use for extracting
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     *
     * \sa Options: ExtractPaths, SkipExistingFiles, ContinueOnError, DontOverwrite, CaseSensitive
     */
    bool extractFiles(const QStringList& listFiles, const QString& destPath, ZipOptions options = ExtractPaths);

    /**
     * @brief Delete the specified file from the archive.
     * \b Note: The file data is only removed from the archive when you call close()
     * @param filename The full path and filename to the file within the archive to be deleted. Wildcard specifications
     * camn be used to delete all files matching a wildcard spec. (e.g. "*.pdf" will delete \b all files in the archive
     * with a '.pdf' file extention)
     * @param options Options use for extracting
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     *
     * \sa Options: Recursive, CaseSensitive
     * \sa setNameFilters();
     */
    bool deleteFile(const QString& filename, ZipOptions options = AbZip::None);

    /**
     * @brief This function performs a full integrity check of the archive file.
     * Including badly formed Central Directories, mismating local headers and corrupted file data.
     * This function is the equivalent to calling extractAll( "", AbZip::VerifyOnly );
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     */
    bool checkIntegrity();

    /**
     * @brief This searches the archive for a matching file spec. If found, then a ZipFileInfo structure is filled
     * containing the zip file information about that file.
     * @param filename The filename to search for. Filename wildcards can be used (e.g. '*.pdf' or '*accounts* )
     * @param options Options use for the search (e.g. Recursive, CaseSensitive )
     * @return If one or more matches are found, then a Qlist of ZipFileInfo entries are returned, else an empty list is returned.
     *
     * \sa Options: Recursive, ContinueOnError, CaseSensitive
     * \sa setNameFilters();
     */
    QList<ZipFileInfo> findFile( const QString& filename, ZipOptions options = Recursive );

    /**
     * @brief This searches the archive for a matching file spec. If found, then a ZipFileInfo structure is filled
     * containing the zip file information about that file.
     * @param filename The filename to search for. Filename wildcards can be used (e.g. '*.pdf' or '*accounts* )
     * @param root This is the archive folder to search from.
     * @param options Options use for the search (e.g. Recursive, CaseSensitive )
     * @return If one or more matches are found, then a Qlist of ZipFileInfo entries are returned, else an empty list is returned.
     *
     * \sa Options: Recursive, ContinueOnError, CaseSensitive
     * \sa setNameFilters();
     */
    QList<ZipFileInfo> findFile( const QString& filename, const QString& root, ZipOptions options = Recursive );

    /**
     * @brief This allows you to set specific name filters when adding, extractinmg or searching for files.
     * Each name filter is a wildcard filter that understands * and ? wildcards. (See QRegExp wildcard matching.)
     * @param nameFilters A QStringList containing one or more name filters (e.g. QStringList() << "*.pdf" << "*.doc" )
     *
     * \sa clearNameFilters(), addDirectory(), extractAll(), findFile(), deleteFile()
     */
    void setNameFilters( const QStringList& nameFilters );

    /**
     * @brief Clears and previously set name filters.
     *
     * \sa setNameFilters()
     */
    void clearNameFilters( );

    /**
     * @brief If a password is set during an add operation, then the file will be encrypted. If you
     * extracting an existing encrypted file, then you must call this function to set the password for decryption.
     *
     * \b Note: The default encryption uses the standard PKWARE encryption which is considered weak by todays standards and
     * its use is recommended only for situations with low security needs or for compatibility with older .ZIP applications.
     * For higher security situations it's recommended that you use the option useAESEncryption.
     * @param password The password use for encryption
     *
     * \sa useAESEncryption
     * \sa clearPassword();
     */
    void setPassword( const QString& password );

    /**
     * @brief Clears a previously set encryption/decryption password.
     *
     * \sa setPassword();
     */
    void clearPassword( );

    /**
     * @brief All functions return either true for success or false if an error occurred. Call this
     * function to retrieve the last known error code.
     * \b Note: Errors are accumilative whilst the archive is open, call clearErrors() to reset the
     * error code, count and message.
     * @return The last ErrorCode that occurred.
     *
     * \sa errorCount(), errorString(), clearErrors(), ErrorCode
     */
    int errorCode() const;

    /**
     * @brief On multiple file operations (e.g. addDirectory() or extractAll() ) and the option
     * ContinueOnError has been set, then this will return the total number of errors that occurred.
     * \b Note: Errors are accumilative whilst the archive is open, call clearErrors() to reset the
     * error code, count and message.
     * @return The total number of errors that occurred.
     *
     * \sa errorCode(), errorString(), clearErrors(), ErrorCode
     */
    int errorCount() const;

    /**
     * @brief Returns a human-readable description of the last error that occurred.
     * On operations that perform multiple tasks, then multiple error messsages are returned
     * separated by a new line character.
     * \b Note: Errors are accumilative whilst the archive is open, call clearErrors() to reset the
     * error code, count and message.
     * @return The last error message that occured
     *
     * \sa errorCode(), errorCount(), clearErrors() ErrorCode
     */
    const QString& errorString() const;

    /**
     * @brief This resets the error code, error count and last error message.
     */
    void clearErrors();

    /**
     * @brief Call this function to add your own comment stored in the archives central directory.
     * @param comment The comment to add to the archive.
     * @return \c true if successful otherwise \c false. See errorCode() and errorString() for information on the error
     */
    bool setComment(const QString& comment);

    /**
     * @brief Gets any comment that may be present in the archives central directory.
     * This is also returned with getInfo()
     * @return Returns the comment otherwise an empty string is returned.
     *
     * \sa getInfo()
     */
    QString getComment()const;

    /**
     * @brief This function will return core information about the archive (e.g. Comment, Total compressed size,
     * total uncompressed size, file count, version, total files added, extracted and deleted)
     * @return A ZipInfo structure containing the information
     *
     * \sa ZipInfo
     */
    ZipInfo getInfo() const;

    void setZipFileName( const QString& name );
};
Q_DECLARE_OPERATORS_FOR_FLAGS(AbZip::ZipOptions)

#endif // ABZIP_H

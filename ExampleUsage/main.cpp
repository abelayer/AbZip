#include <QCoreApplication>
#include <QDebug>

#include <AbZip.h>
#include <ZipFileInfo.h>
#include <ZipDirIterator.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // See UserGuide.html for more information and details on all functions.

	// All main functions will return true or false depending on success or fail.
	// You should check for this and react accordingly
	
    // Example 1: ADDING
    // Open and create a new Zip archive and add a file and a folder and all sub folders to it
    // The files are added using the default 'AbZip::AddRelativePaths' flag
    AbZip zip("c:/MyData/Backup1.zip" );
    if ( !zip.addDirectory( "c:/MyData/Folder1", AbZip::Recursive ) )
		qDebug() << zip.errorCount() << "errors occured:" << zip.errorString();

    if ( !zip.addFile( "c:/MyData/File1.pdf" ))
		qDebug() << "error occured:" << zip.errorString();

    // Add an encrypted file
    zip.setPassword("somepassword");
    if ( !zip.addFile( "c:/MyData/File2.pdf" ))
		qDebug() << "error occured:" << zip.errorString();
    if ( !zip.addFile( "c:/MyData/File3.pdf", AbZip::useAESEncryption  ))     // Use a more secure encryption method
		qDebug() << "error occured:" << zip.errorString();

    // Disable encryption (Only needed if adding more files unencrypted)
    zip.clearPassword();

    // Add all PDF files found in a folder and continue if an error occurs.  
	// This is not recursive but will store the files absolute path in the zip
    zip.setNameFilters(QStringList() << "*.pdf");
    if (!zip.addDirectory( "c:/MyData/Folder2", AbZip::ContinueOnError | AbZip::AddAbsolutePaths ))
		qDebug() << zip.errorCount() << "errors occured:" << zip.errorString();

    zip.close();

    // Example 2: EXTRACTING
    // Open and extract ALL files from an existing archive
    /// AbZip zip("c:/MyData/Backup1.zip" );

    // Set the decryption password (Only needed if you have encrypted files in this archive)
    zip.setPassword("somepassword");

    // Extract ALL files in the archive to a folder called 'c:/RestoreFolder1'
    if ( !zip.extractAll( "c:/RestoreFolder1" ))
		qDebug() << zip.errorCount() << "errors occured:" << zip.errorString();

    // Extract only PDF files from an existing archive
    zip.setNameFilters(QStringList() << "*.pdf");
    if ( !zip.extractAll( "c:/RestoreFolder2" ))
		qDebug() << zip.errorCount() << "errors occured:" << zip.errorString();

    zip.clearNameFilters();
    // Extract explicit files
    if (!zip.extractFile( "File1.pdf", "c:/RestoreFolder2" ))
		qDebug() << "error occured:" << zip.errorString();
	
    if (!zip.extractFile( "Folder5/File2.pdf", "c:/RestoreFolder2" ))
		qDebug() << "error occured:" << zip.errorString();

    zip.close();

    // Example 3: DELETING
    // Open and delete files from an existing archive
    /// AbZip zip("c:/MyData/Backup1.zip" );

    if ( !zip.deleteFile("File1.pdf"))
        qDebug() << "error occured:" << zip.errorString();

    // Delete all files matching a wildcard spec
    if ( !zip.deleteFile("*.pdf"))
        qDebug() << zip.errorCount() << "errors occured:" << zip.errorString();

    // Or use name filters to locate and delete files
    zip.setNameFilters(QStringList() << "*.jpg" << "*.png");
    if ( !zip.deleteFile(""))
        qDebug() << zip.errorCount() << "errors occured:" << zip.errorString();

    zip.close();

    // Example 4: SEARCHING
    // Open and find files in and existing archive
    /// AbZip zip("c:/MyData/Backup1.zip" );

    QList<ZipFileInfo> list = zip.findFile( "*.pdf" );
    qDebug() << "Found: " << list.count() << " matches";

    // Find files but search from a set folder in the archive
    list = zip.findFile( "*.pdf", "folder1" );
    qDebug() << "Found: " << list.count() << " matches";


    // Or use name filters to locate all JPG and PNG files and iterate through each file found
    zip.setNameFilters(QStringList() << "*.jpg" << "*.png");
    ZipDirIterator it(zip);
    while (it.hasNext())
    {
        it.next();
        ZipFileInfo info  = it.fileInfo();
        qDebug() << info.filePath << info.lastModifiedDate << info.uncompressedSize;
    }
	zip.close();
	
    // Example 5: INFO and CHECKING
    /// AbZip zip("c:/MyData/Backup1.zip" );
	
    // Validate the archive.  errors are returned in errorString()
    if (!zip.checkIntegrity())
         qDebug() << zip.errorCount() << "errors occured:" << zip.errorString();


	// Get the global zip comment if any
    QString comment = zip.getComment();
    qDebug() << "Archive comment:" << comment;

	// Get information about the archive
    ZipInfo info = zip.getInfo();

    // General info
    qDebug() << "Files: " << info.fileCount << ", Size:" << info.fileSize <<
                ", Packed size: " << info.packedSize << ", Unpacked size:" << info.unpackedSize;

    // Info when added or extracting files
    qDebug() << "Files added: " << info.filesAdded << ", Files extracted:" << info.filesExtracted <<
                     ", Files deleted: " << info.filesDeleted << ", Errors:" << info.errorCount;


    zip.close();


    return 0; //a.exec();
}


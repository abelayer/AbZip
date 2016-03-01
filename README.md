# AbZip
Cross platform Zip/Unzip Archive class written in Qt

AbZip is a Qt based C++ class for creating Zip archives. It's not a wrapper for 
other Zip/Unzip code but accesses the archives directly via QIODevice

I created this code after I needed to archive files for another project I was working on. 
I started to use QuaZip but quickly found problems with this. The main one being that you
could not delete or update files in an existing archive. This was a big problem for me, hence
I wrote this code.

So far I've only tested it on Windows, so I would appreciate help with compatibility with
other platforms.

Why Qt?  Well if you are a C++ programmer and you want to create cross-platform
applications or utilities, then Qt is simply the best out their!
To learn more about Qt goto <a href="http://qt-project.org/doc/">official Qt documentation</a>

AbZip features include:
 - Creating new or opening existing (Pkware v2.0) Zip archives.
 - Add files or entire folders to the archive
 - File name filters can be applied
 - Update existing files
 - Delete files from the archive (not many Zip classes do this as I found out, which
 is the reason I wrote these classes!)
 - Extract one or more files
 - Extract based on file name filters
 - Search for files, again using wildcard filters
 - Use Strong AES encryption instead of the default Pkware CRC base encryption
 - Supports 64bit archives.
 - Additional compression methods can easily be created and added (Currently I've
 added BZip2 but intend to start to add others later).
 
Currently there is no support for splitting archives. 
(Lets face it, how often these days do you need this feature!)

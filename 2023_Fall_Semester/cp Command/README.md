tucp.c README:

Objective: recreate the cp command in linux. tucp will copy files or a group of files.

Usage:
- tucp [Source File Path] [Destination File Path] : when tucp has two file name arguments, the program copies the contents of the 1st file to the 2nd file. If the 2nd file does not exist, the program will create a file with the specificed name and copies the content of the 1st file.
- tucp [Source File Path] [Directory Path] : when tucp has a file name and a directory as arguments, the program copies the contents of the file into the directory.
- tucp [Source File Path-1] [Source File Path-2] [Source File Path-n] [Directory Path] : when tucp has one or more arguments specifying source file names and the last argument specifies a directory, the program copies each file into the destination directory. The destination file is created if it does not exist, and overwrites if the file already exists.


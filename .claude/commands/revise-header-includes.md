Iterate over all headers(.h) and cpp(.cpp) files and edit header includes, which involves "#include" preprocessor.

0) Ask the name of the .vcxproj file to get the Additional Include Directories from.

1) Define a list: NullPath

2) Get the list of Additional Include Directories from .vcxproj file

3) Find files recursively. The file or sub-directory must be located under "src" or "include" folder.

4) Assess if header file after "#include" exists relative to any of the Additional Include Directories.

5) If not, find the relative path of where the header file with the same name exists.

6) If relative path exists, Edit the header includes path coming after "#include". Use the shortest relative path to Additional Include Directories.

7) If relative path doesn't exist, add the current file name being edited to the list NullPath.

8) Print out NullPath

Don't ask for permission to proceed, unless problem that requires to abort occurs.
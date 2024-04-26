# Search-directory-tree

## Directory Tree to XML Converter

Create a program that will traverse through the entire directory tree for a given initial directory and generate an XML document. 
Each new directory should become a new subelement with a tag name equal to the name of that directory. 

Additionally, each element in this XML should have attributes 'nfiles' whose value should be set to the number of files in that directory, and an attribute 'size' which should have a value equal to the total sum of the sizes of all files in that directory plus the sizes of all files in all its subdirectories.

<b>NOTE</b>: Handle exceptions separately for cases where access to a directory is not possible; ignore such directories and do not process or include them in the final XML.


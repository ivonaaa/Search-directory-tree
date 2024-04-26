#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PATH_LENGTH 1000

typedef struct DirectoryInfo {
    char name[MAX_PATH_LENGTH];
    int nFiles;
    long long totalSize;
    struct DirectoryInfo* subdirectories;
    int nSubdirectories;
} DirectoryInfo;

void traverseDirectory(const char* path, DirectoryInfo* parent);
void buildXML(DirectoryInfo* dir, FILE* xmlFile, int depth);
void freeDirectoryInfo(DirectoryInfo* dir);
void addSubdirectory(DirectoryInfo* parent, const char* name);
bool isDotOrDotDot(const char* name);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Directory path is not provided.\n");
        return 1;
    }

    FILE* xmlFile = fopen("directory_tree.xml", "w");
    if (xmlFile == NULL) {
        printf("Failed to create XML file.\n");
        return 2;
    }

    fprintf(xmlFile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(xmlFile, "<directory_tree>\n");

    DirectoryInfo root;
    strcpy(root.name, argv[1]);
    root.nFiles = 0;
    root.totalSize = 0;
    root.subdirectories = NULL;
    root.nSubdirectories = 0;

    traverseDirectory(argv[1], &root);
    buildXML(&root, xmlFile, 1);

    fprintf(xmlFile, "</directory_tree>\n");
    fclose(xmlFile);

    printf("XML file generated successfully.\n");

    freeDirectoryInfo(&root);

    return 0;
}

void traverseDirectory(const char* path, DirectoryInfo* parent) {
    char searchPath[MAX_PATH_LENGTH];
    sprintf(searchPath, "%s\\*", path);

    WIN32_FIND_DATA fileData;
    HANDLE hFind = FindFirstFile(searchPath, &fileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Failed to open directory: %s\n", path);
        return;
    }

    long long directorySize = 0;

    do {
        if (!isDotOrDotDot(fileData.cFileName)) {
            char fullPath[MAX_PATH_LENGTH];
            sprintf(fullPath, "%s\\%s", path, fileData.cFileName);
        
            if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                DirectoryInfo subdir;
                strcpy(subdir.name, fullPath);
                subdir.nFiles = 0;
                subdir.totalSize = 0;
                subdir.subdirectories = NULL;
                subdir.nSubdirectories = 0;

                addSubdirectory(parent, fullPath);
                traverseDirectory(fullPath, &(parent->subdirectories[parent->nSubdirectories - 1]));

                directorySize += subdir.totalSize;
                directorySize += parent->subdirectories[parent->nSubdirectories - 1].totalSize;
            } else {
                parent->nFiles++;
                directorySize += fileData.nFileSizeLow;
            }
        }
    } while (FindNextFile(hFind, &fileData));

    FindClose(hFind);

    parent->totalSize += directorySize;
}


void buildXML(DirectoryInfo* dir, FILE* xmlFile, int depth) {
    fprintf(xmlFile, "%*s<directory name=\"%s\" nfiles=\"%d\" size=\"%lld\">\n",
            depth * 4, "", dir->name, dir->nFiles, dir->totalSize);
    
    for (int i = 0; i < dir->nSubdirectories; ++i) {
        buildXML(&(dir->subdirectories[i]), xmlFile, depth + 1);
    }

    fprintf(xmlFile, "%*s</directory>\n", depth * 4, "");
}

void addSubdirectory(DirectoryInfo* parent, const char* name) {
    parent->subdirectories = (DirectoryInfo*)realloc(parent->subdirectories, (parent->nSubdirectories + 1) * sizeof(DirectoryInfo));
    DirectoryInfo* subdir = &(parent->subdirectories[parent->nSubdirectories]);
    strcpy(subdir->name, name);
    subdir->nFiles = 0;
    subdir->totalSize = 0;
    subdir->subdirectories = NULL;
    subdir->nSubdirectories = 0;
    parent->nSubdirectories++;
}

void freeDirectoryInfo(DirectoryInfo* dir) {
    for (int i = 0; i < dir->nSubdirectories; ++i) {
        freeDirectoryInfo(&(dir->subdirectories[i]));
    }
    free(dir->subdirectories);
}

bool isDotOrDotDot(const char* name) {
    return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0);
}

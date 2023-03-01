#include "FilesExplorer.h"

FilesExplorer::FilesExplorer(const std::filesystem::path &filePath, QObject *parent)
    : QObject(parent)
    , m_currentDirPath(filePath)
{
	readDirectory(m_currentDirPath);
}

std::vector<filestructs::FileData> FilesExplorer::getFiles() const
{
	return m_filesData;
}

void FilesExplorer::updateRequested()
{
	m_filesData.clear();
	readDirectory(m_currentDirPath);

	emit filesListUpdated();
}

void FilesExplorer::readDirectory(const std::filesystem::path &dirPath)
{
	for (const auto &entry : std::filesystem::directory_iterator(dirPath)) {
		if (entry.is_regular_file() && (entry.path().extension() == ".bmp" || entry.path().extension() == ".barch")) {
			std::string filePath{entry.path().u8string()};
			std::string fileName{entry.path().filename().u8string()};
			m_filesData.push_back(filestructs::FileData(fileName, filePath, entry.file_size()));
		}
	}
}

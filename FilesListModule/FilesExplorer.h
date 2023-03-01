#pragma once

#include <QObject>

#include "FileDataStruct.h"

class FilesExplorer : public QObject
{
	Q_OBJECT

public:
	FilesExplorer(const std::filesystem::path &filePath, QObject *parent = nullptr);

	std::vector<filestructs::FileData> getFiles() const;

signals:
	void filesListUpdated();

public slots:
	void updateRequested();

private:
	void readDirectory(const std::filesystem::path &filePath);

	std::filesystem::path m_currentDirPath;
	std::vector<filestructs::FileData> m_filesData;
};

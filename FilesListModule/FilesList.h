#pragma once

#include "qqmlintegration.h"
#include <QAbstractListModel>
#include <QPointer>

#include "FileDataStruct.h"
#include "FilesExplorer.h"

class FilesList : public QAbstractListModel
{
	Q_OBJECT
	QML_ELEMENT

public:
	enum Roles {
		FileNameRole = Qt::UserRole + 1,
		FilePathRole,
		FileSizeRole,
		FileExtensionRole
	};

	FilesList(const std::filesystem::path &dirPath, QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

	void addFile(filestructs::FileData &data);

	std::shared_ptr<FilesExplorer> explorer() const;

signals:
	void nameChanged();
	void compressionRequested(const std::filesystem::path &path);
	void decompressionRequested(const std::filesystem::path &path);
	void compressionFinished();
	void decompressionFinished();
	void updateFilesListRequested();

public slots:
	void requestCompression(int index);
	void requestDecompression(int index);

private:
	std::shared_ptr<FilesExplorer> m_explorer;
	QVector<filestructs::FileData> m_files;
};

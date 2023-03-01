#include "FilesList.h"

FilesList::FilesList(const std::filesystem::path &dirPath, QObject *parent)
    : QAbstractListModel(parent)
    , m_explorer(std::make_shared<FilesExplorer>(dirPath))
{	
	auto update = [this] () {
		beginResetModel();
		m_files.clear();
		auto data = m_explorer->getFiles();

		for (auto &f : data) {
			m_files.push_back(filestructs::FileData(f.name, f.path, f.size));
		}
		endResetModel();
	};

	QObject::connect(this, &FilesList::updateFilesListRequested, m_explorer.get(), &FilesExplorer::updateRequested);
	QObject::connect(m_explorer.get(), &FilesExplorer::filesListUpdated, this, update);

	update();
}

int FilesList::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) {
		return 0;
	}

	return m_files.size();
}

QVariant FilesList::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	if (index.row() < 0 || index.row() >= m_files.count()) {
		return QVariant();
	}

	switch (role) {
	case Roles::FileNameRole:
		return QString::fromStdString(m_files.at(index.row()).name);
	case Roles::FilePathRole:
		return QString::fromStdString(m_files.at(index.row()).path.u8string());
	case Roles::FileSizeRole:
		return m_files.at(index.row()).size / 1024;
	case Roles::FileExtensionRole:
		return QString::fromStdString(m_files.at(index.row()).path.extension().u8string());
	default:
		break;
	}

	return QVariant();
}

QHash<int, QByteArray> FilesList::roleNames() const
{
	return {
		{ Roles::FileNameRole, "fileName" },
		{ Roles::FileSizeRole, "filePath"},
		{ Roles::FileSizeRole, "fileSize"},
		{ Roles::FileExtensionRole, "fileExtension"}
	};
}

void FilesList::addFile(filestructs::FileData &data)
{
	m_files.push_back(data);
}

std::shared_ptr<FilesExplorer> FilesList::explorer() const
{
	return m_explorer;
}

void FilesList::requestCompression(int index)
{
	auto path = m_files.at(index).path;
	emit compressionRequested(path);
}

void FilesList::requestDecompression(int index)
{
	auto path = m_files.at(index).path;
	emit decompressionRequested(path);
}

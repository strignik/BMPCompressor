#pragma once

#include <QObject>
#include <filesystem>

namespace Compressor {

typedef struct tagBITMAPFILEHEADER {
	unsigned short  bfType;
	unsigned int	bfSize;
	unsigned short  bfReserved1;
	unsigned short  bfReserved2;
	unsigned int	bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

class BMPCompressor : public QObject
{
	Q_OBJECT

public:
	struct Bitmap {
		int width;
		int height;
		unsigned char * data;
	};

	BMPCompressor(QObject *parent = nullptr);
public slots:
	void compress(const std::filesystem::path &filePath);
	void decompress(const std::filesystem::path &filePath);
signals:
	void compressionFinished();
	void decompressionFinished();
private:
	std::pair<Bitmap*, std::vector<unsigned char>> createBitmap(const std::filesystem::path &filePath);
	std::pair<int, int> getImageResolution(const std::filesystem::path &filePath) const;
	size_t getFileSize(const std::filesystem::path &filePath) const;

	std::pair<std::vector<unsigned char>, std::vector<unsigned char>> encode(const Bitmap &bitmap) const;
	std::vector<unsigned char> decode(const std::vector<char> &emptyRows,
	                                  const std::vector<char> &encodedData,
	                                  const int width,
	                                  const int height) const;
};

}

#pragma pack(1)
#include "Compressor.h"

#include <fstream>
#include <bitset>

namespace Compressor {

template<typename First, typename ... T>
bool is_equal(First &&first, T && ... t)
{
	return ((first == t) && ...);
}

BMPCompressor::BMPCompressor(QObject *parent) : QObject(parent)
{

}

void BMPCompressor::compress(const std::filesystem::path &filePath)
{
	auto [bitmap, header] = createBitmap(filePath);
	auto [encodedData, emptyIndices] = encode(*bitmap);

	auto dirPath = filePath.parent_path();
	auto outFilePath = dirPath / std::string(filePath.stem().u8string()).append(std::string("_packed.barch"));

	std::ofstream outFile(outFilePath, std::ios::binary);

	if (outFile.good()) {
		size_t emptySize = emptyIndices.size();
		size_t headerSize = header.size();

		outFile.write(reinterpret_cast<char*>(&emptySize), sizeof(emptySize));
		std::copy(emptyIndices.cbegin(), emptyIndices.cend(), std::ostreambuf_iterator<char>(outFile));

		outFile.write(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
		std::copy(header.cbegin(), header.cend(), std::ostreambuf_iterator<char>(outFile));

		outFile.write(reinterpret_cast<char*>(&bitmap->width), sizeof(bitmap->width));
		outFile.write(reinterpret_cast<char*>(&bitmap->height), sizeof(bitmap->height));

		std::copy(encodedData.cbegin(), encodedData.cend(), std::ostreambuf_iterator<char>(outFile));
	}

	delete bitmap;
	outFile.close();

	emit compressionFinished();
}

void BMPCompressor::decompress(const std::filesystem::path &filePath)
{
	std::ifstream file(filePath, std::ios::binary);

	if (file.good()) {
		size_t fileSize = getFileSize(filePath);
		size_t emptyRowsSize = 0;
		size_t headerSize = 0;
		int imageWidth = 0;
		int imageHeight = 0;

		file.read(reinterpret_cast<char*>(&emptyRowsSize), sizeof(emptyRowsSize));
		std::vector<char> emptyRows(emptyRowsSize);
		file.read(&emptyRows[0], emptyRowsSize);

		file.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
		std::vector<char> header(headerSize);
		file.read(&header[0], headerSize);

		file.read(reinterpret_cast<char*>(&imageWidth), sizeof(imageWidth));
		file.read(reinterpret_cast<char*>(&imageHeight), sizeof(imageHeight));

		int imageSize = fileSize - emptyRowsSize - headerSize;
		std::vector<char> encodedData(imageSize);

		file.read(&encodedData[0], imageSize);

		auto decodedData = decode(emptyRows, encodedData, imageWidth, imageHeight);

		auto dirPath = filePath.parent_path();
		auto outFilePath = dirPath / std::string(filePath.stem().u8string()).append(std::string("_unpacked.bmp"));
		std::ofstream outFile(outFilePath, std::ios::binary);

		copy(header.cbegin(), header.cend(), std::ostreambuf_iterator<char>(outFile));
		copy(decodedData.cbegin(), decodedData.cend(), std::ostreambuf_iterator<char>(outFile));
		outFile.close();
	}
	file.close();
	emit decompressionFinished();
}

std::pair<BMPCompressor::Bitmap*, std::vector<unsigned char>> BMPCompressor::createBitmap(const std::filesystem::path &filePath)
{
	Bitmap *bmp = new Bitmap;
	PBITMAPFILEHEADER file_header;

	auto [width, height] = getImageResolution(filePath);

	bmp->width = width;
	bmp->height = height;

	std::ifstream file(filePath, std::ios::binary);

	if (!file.good()) {
		return {};
	}

	size_t fileSize = getFileSize(filePath);

	std::vector<unsigned char> first100(100);
	file.read(reinterpret_cast<char*>(&first100[0]), 100);
	file_header = (PBITMAPFILEHEADER)(&first100[0]);

	auto headerSize = file_header->bfOffBits;
	auto bodySize = fileSize - headerSize;

	file.seekg(headerSize);
	bmp->data = new unsigned char[bodySize];
	file.read(reinterpret_cast<char*>(bmp->data), bodySize);

	file.seekg(0);
	std::vector<unsigned char> header(headerSize);
	file.read(reinterpret_cast<char*>(&header[0]), headerSize);

	file.close();

	return {bmp, header};
}

std::pair<int, int> BMPCompressor::getImageResolution(const std::filesystem::path &filePath) const
{
	std::ifstream file(filePath, std::ios::binary);

	file.seekg(18);

	int width, height;

	file.read(reinterpret_cast<char*>(&width), sizeof(width));
	file.read(reinterpret_cast<char*>(&height), sizeof(height));

	file.close();

	return {width, height};
}

size_t BMPCompressor::getFileSize(const std::filesystem::path &filePath) const
{
	size_t fileSize = 0;
	std::ifstream file(filePath, std::ios::binary);

	if (file.good()) {
		file.seekg(0, std::ios::end);
		fileSize = file.tellg();
		file.seekg(0, std::ios::beg);
	}

	return fileSize;
}

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> BMPCompressor::encode(const Bitmap &bitmap) const
{
	std::vector<int> encoded = {};
	std::vector<unsigned char> emptyRowsIndices(bitmap.height);

	const auto &data = bitmap.data;
	bool prevRowWasEmpty = false;

	for(auto h = 0; h < bitmap.height; h++) {
		std::vector<unsigned char> row = {};
		row.reserve(bitmap.width + 3);

		auto begin = h * bitmap.width;
		auto end = begin + bitmap.width;

		if (h > 0) {
			begin += 3 * h;
			end += 3 * h;
		}

		row.insert(row.cend(), &bitmap.data[begin], &bitmap.data[end]);

		if (std::all_of(row.begin(),
		                row.end(),
		                [&emptyRowsIndices, &prevRowWasEmpty, &row]
		                (unsigned char &ch)
		{ return ch == 0xFF && ch == row[0];})) {
			emptyRowsIndices[h] = 1;
			prevRowWasEmpty = true;
		} else {
			row.insert(row.end(), {0x00, 0x00, 0x00});
			prevRowWasEmpty = false;

			for(auto w = 0; w < row.size(); w++) {
				const auto &pixel = row.data()[w];

				if (is_equal(row[w], row[w+1], row[w+2], row[w+3])) {
					if (pixel == 0xff) {
						encoded.push_back(0);
						w+=3;
						continue;
					} else if (pixel == 0x00) {
						encoded.insert(encoded.cend(), {1, 0});
					} else {
						encoded.insert(encoded.cend(), {1, 1});

						for (int i = 0; i < 4; i++) {
							std::bitset<8> byte(row[w + i]);
							auto bitCount = 7;
							do {
								encoded.push_back(byte.test(bitCount));
							} while (bitCount--);
						}
					}
					w+=3;
				} else {
					encoded.insert(encoded.cend(), {1, 1});

					for (int i = 0; i < 4; i++) {
						std::bitset<8> byte(row[w + i]);
						auto bitCount = 7;
						do {
							encoded.push_back(byte.test(bitCount));
						} while (bitCount--);
					}
					w+=3;
				}
			}
		}
	}

	std::vector<unsigned char> result;
	int bitCounter = 7;
	std::bitset<8> byte;
	for (auto i = 0; i < encoded.size(); i++) {
		byte.set(bitCounter--, encoded[i]);
		if (bitCounter == 0) {
			byte.set(bitCounter, encoded[++i]);
			bitCounter = 7;
			unsigned char data = byte.to_ulong();
			result.push_back(static_cast<unsigned char>(data));
			byte.reset();
		}
	}

	return {result, emptyRowsIndices};
}


std::vector<unsigned char> BMPCompressor::decode(const std::vector<char> &emptyRows,
                                                 const std::vector<char> &encodedData,
                                                 const int width,
                                                 const int height) const
{
	std::vector<int> bits;
	std::vector<unsigned char> decodedData;

	int offset = 0;

	for (auto i = 0; i < encodedData.size(); i++) {
		const unsigned char data = encodedData[i];
		std::bitset<8> byte(data);

		for (auto j = 7; j >= 0; j--) {
			bits.push_back(byte.test(j));
		}
	}

	for (auto h = 0; h < height; h++) {
		if (emptyRows[h] == 1) {
			int counter = width;
			while (counter--) {
				decodedData.push_back(0xFF);
			}
			decodedData.insert(decodedData.end(), {0x00, 0x00, 0x00});
			continue;
		} else {
			std::vector<unsigned char> currentRow;
			while (currentRow.size() < width) {
				if (bits[offset] == 0) {
					currentRow.insert(currentRow.cend(), {0xFF, 0xFF, 0xFF, 0xFF});
					offset++;
				} else if (bits[offset] == 1 && bits[offset+1] == 0) {
					currentRow.insert(currentRow.cend(), {0x00, 0x00, 0x00, 0x00});
					offset+=2;
				} else if (bits[offset] == 1 && bits[offset+1] == 1) {
					offset+=2;

					for (int i = 0; i < 4; i++) {
						std::bitset<8> color;
						auto bitCount = 7;
						do {
							color.set(bitCount, bits[offset++]);
						} while (bitCount--);

						currentRow.push_back(static_cast<unsigned char>(color.to_ulong()));
					}
				}
			}
			decodedData.insert(decodedData.cend(), currentRow.cbegin(), currentRow.cend());
		}
	}

	return decodedData;
}

}

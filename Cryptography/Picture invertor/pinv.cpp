#define _CRT_SECURE_NO_WARNINGS
#include "pinv.hpp"
#include <iostream>

bool pinv::is_BMP(const string& patch) {
	return (patch.rfind(".bmp") != string::npos);
}

void pinv::transform(const string& patch, BYTE bits, const char& color) {
	bits = (bits > 7 ? 7 : bits);

	if (!is_BMP(patch)) {
		std::cerr << "Incorrect format." << std::endl << "Please, use bitmap-picture format" << std::endl;
		return; // Incorrect format
	}

	inverseColorBits(patch, bits, color);
}

FILE* pinv::getPictureHandle(const string& patch) {
	FILE *stream = fopen(patch.c_str(), "rb");
	return stream;
}

FILE* pinv::getDropHandle(string patch) {
	patch += "_new.bmp";
	FILE* drop = fopen(patch.c_str(), "wb+");
	return drop;
}

void pinv::inverseColorBits(const string& patch, const BYTE& bits, const char& RGB) {
	FILE* stream = getPictureHandle(patch);
	if (!stream) return; // The file was not opened

	FILE* drop = getDropHandle(patch);

	// Read fileheader

	tagBITMAPFILEHEADER* file_header = new tagBITMAPFILEHEADER();
	fread(&file_header->bfType, sizeof(WORD), 1, stream);
	fread(&file_header->bfSize, sizeof(DWORD), 1, stream);
	fread(&file_header->bfReserved1, sizeof(WORD), 1, stream);
	fread(&file_header->bfReserved2, sizeof(WORD), 1, stream);
	fread(&file_header->bfOffBits, sizeof(DWORD), 1, stream);

	// Read infoheader

	tagBITMAPINFOHEADER* file_infoheader = new tagBITMAPINFOHEADER();
	fread(&file_infoheader->biSize, sizeof(DWORD), 1, stream);
	fread(&file_infoheader->biWidth, sizeof(DWORD), 1, stream);
	fread(&file_infoheader->biHeight, sizeof(DWORD), 1, stream);
	fread(&file_infoheader->biPlanes, sizeof(WORD), 1, stream);
	fread(&file_infoheader->biBitCount, sizeof(WORD), 1, stream);
	fread(&file_infoheader->biCompression, sizeof(DWORD), 1, stream);
	fread(&file_infoheader->biSizeImage, sizeof(DWORD), 1, stream);
	fread(&file_infoheader->biXPelsPerMeter, sizeof(DWORD), 1, stream);
	fread(&file_infoheader->biYPelsPerMeter, sizeof(DWORD), 1, stream);
	fread(&file_infoheader->biClrUsed, sizeof(DWORD), 1, stream);
	fread(&file_infoheader->biClrImportant, sizeof(DWORD), 1, stream);

	// Read pixels

	int size = file_infoheader->biWidth * file_infoheader->biHeight * file_infoheader->biBitCount / 8;
	tagRGBQUAD* data = new tagRGBQUAD[size];
	fread(data, sizeof(BYTE), size, stream);

	for (auto i = 0; i < size; i++) {
		for (auto bit = 0; bit < bits; bit++) {
			switch (RGB) {
			case 'R':
				data[i].rgbRed ^= (1 << bit);
				break;
			case 'G':
				data[i].rgbGreen ^= (1 << bit);
				break;
			case 'B':
				data[i].rgbBlue ^= (1 << bit);
				break;
			}
		}
	}

	// Write image data
	size /= file_infoheader->biBitCount / 8;

	fwrite(file_header, sizeof(tagBITMAPFILEHEADER), 1, drop);
	fwrite(file_infoheader, sizeof(tagBITMAPINFOHEADER), 1, drop);
	fwrite(data, sizeof(tagRGBQUAD), size, drop);

	fclose(stream);
	fclose(drop);
}
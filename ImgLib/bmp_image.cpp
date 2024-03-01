#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib { // BGR24 без сжатия

	PACKED_STRUCT_BEGIN BitmapFileHeader{
	// 14 bytes
	char B; 
	char M; // 2 bytes Символы BM
	uint32_t summ_headline_data; // 4 bytes Размер данных определяется как отступ, умноженный на высоту изображения
	uint32_t nulls;// 4 bytes: nulls Зарезервированное пространство
	uint32_t offset; // 4 bytes Отступ данных от начала файла равен размеру двух частей заголовка
	}
	PACKED_STRUCT_END

	PACKED_STRUCT_BEGIN BitmapInfoHeader{
	// 40 bytes
	uint32_t headline_size; // 4 bytes Размер второй части заголовка
	int32_t width;// 4 bytes Ширина изображения в пикселях
	int32_t height; // 4 bytes Высота изображения в пикселях
	uint16_t num_planes; // 2 bytes Количество плоскостей: всегда 1 — одна RGB плоскость
	uint16_t bytes_pro_pixel; // 2 bytes Количество бит на пиксель
	uint32_t compression; // 4 bytes Тип сжатия: отсутствие сжатия
	uint32_t byte_size; // 4 bytes Количество байт в данных = Произведение отступа на высоту
	int32_t hor_resolution; // 4 bytes Горизонтальное разрешение, пикселей на метр: примерно соответствует 300 DPI
	int32_t vert_resolution; // 4 bytes Вертикальное разрешение, пикселей на метр: примерно соответствует 300 DPI
	int32_t used_colors; // 4 bytes Количество использованных цветов: значение не определено
	int32_t sign_colors; // 4 bytes Количество значимых цветов 0x1000000
	}
	PACKED_STRUCT_END

	// функция вычисления отступа по ширине
	static int GetBMPStride(int w) {
		return 4 * ((w * 3 + 3) / 4);
	}

	ofstream& operator<<(ofstream& ofs, BitmapFileHeader& bfh) {
		ofs.write(reinterpret_cast<char*>(&bfh.B), sizeof(bfh.B));
		ofs.write(reinterpret_cast<char*>(&bfh.M), sizeof(bfh.M));
		ofs.write(reinterpret_cast<char*>(&bfh.summ_headline_data), sizeof(bfh.summ_headline_data));
		ofs.write(reinterpret_cast<char*>(&bfh.nulls), sizeof(bfh.nulls));
		ofs.write(reinterpret_cast<char*>(&bfh.offset), sizeof(bfh.offset));
		return ofs;
	}

	ifstream& operator >> (ifstream& ifs, BitmapFileHeader& bfh) {
		ifs.read(reinterpret_cast<char*>(&bfh.B), sizeof(bfh.B));
		ifs.read(reinterpret_cast<char*>(&bfh.M), sizeof(bfh.M));
		ifs.read(reinterpret_cast<char*>(&bfh.summ_headline_data), sizeof(bfh.summ_headline_data));
		ifs.read(reinterpret_cast<char*>(&bfh.nulls), sizeof(bfh.nulls));
		ifs.read(reinterpret_cast<char*>(&bfh.offset), sizeof(bfh.offset));
		return ifs;
	}

	ofstream& operator << (ofstream& ofs, BitmapInfoHeader & bih) {
		ofs.write(reinterpret_cast<char*>(&bih.headline_size), sizeof(bih.headline_size));
		ofs.write(reinterpret_cast<char*>(&bih.width), sizeof(bih.width));
		ofs.write(reinterpret_cast<char*>(&bih.height), sizeof(bih.height));
		ofs.write(reinterpret_cast<char*>(&bih.num_planes), sizeof(bih.num_planes));
		ofs.write(reinterpret_cast<char*>(&bih.bytes_pro_pixel), sizeof(bih.bytes_pro_pixel));
		ofs.write(reinterpret_cast<char*>(&bih.compression), sizeof(bih.compression));
		ofs.write(reinterpret_cast<char*>(&bih.byte_size), sizeof(bih.byte_size));
		ofs.write(reinterpret_cast<char*>(&bih.hor_resolution), sizeof(bih.hor_resolution));
		ofs.write(reinterpret_cast<char*>(&bih.vert_resolution), sizeof(bih.vert_resolution));
		ofs.write(reinterpret_cast<char*>(&bih.used_colors), sizeof(bih.used_colors));
		ofs.write(reinterpret_cast<char*>(&bih.sign_colors), sizeof(bih.sign_colors));
		return ofs;
	}

	ifstream& operator >> (ifstream& ifs, BitmapInfoHeader& bih) {
		ifs.read(reinterpret_cast<char*>(&bih.headline_size), sizeof(bih.headline_size));
		ifs.read(reinterpret_cast<char*>(&bih.width), sizeof(bih.width));
		ifs.read(reinterpret_cast<char*>(&bih.height), sizeof(bih.height));
		ifs.read(reinterpret_cast<char*>(&bih.num_planes), sizeof(bih.num_planes));
		ifs.read(reinterpret_cast<char*>(&bih.bytes_pro_pixel), sizeof(bih.bytes_pro_pixel));
		ifs.read(reinterpret_cast<char*>(&bih.compression), sizeof(bih.compression));
		ifs.read(reinterpret_cast<char*>(&bih.byte_size), sizeof(bih.byte_size));
		ifs.read(reinterpret_cast<char*>(&bih.hor_resolution), sizeof(bih.hor_resolution));
		ifs.read(reinterpret_cast<char*>(&bih.vert_resolution), sizeof(bih.vert_resolution));
		ifs.read(reinterpret_cast<char*>(&bih.used_colors), sizeof(bih.used_colors));
		ifs.read(reinterpret_cast<char*>(&bih.sign_colors), sizeof(bih.sign_colors));
		return ifs;
	}

	bool CheckBitmapFileHeader(BitmapFileHeader& bm_file) {
		if (bm_file.B != 'B' || bm_file.M != 'M') {
			return false;
		}

		if (bm_file.nulls != 0) {
			return false;
		}

		if (bm_file.offset != 54) {
			return false;
		}
		return true;
	}

	bool CheckBitmapInfoHeader(BitmapInfoHeader& bm_info) {
		if (bm_info.headline_size != 40) {
			return false;
		}

		if (bm_info.num_planes != 1) {
			return false;
		}

		if (bm_info.bytes_pro_pixel != 24) {
			return false;
		}

		if (bm_info.compression != 0) {
			return false;
		}

		if (bm_info.byte_size != GetBMPStride(bm_info.width) * bm_info.height) {
			return false;
		}
		return true;
	}

	BitmapFileHeader CreateBitmapFileHeader(int stride, int h) {
		return BitmapFileHeader{
			.B = 'B',
			.M = 'M',
			.summ_headline_data = static_cast<uint32_t>(stride * h + 54),
			.nulls = 0, 
			.offset = 54
		};
	}

	BitmapInfoHeader CreateBitmapInfoHeader(int stride, int w, int h) {
		return BitmapInfoHeader{
			.headline_size  = 40,
			.width = w,
			.height = h,
			.num_planes = 1,
			.bytes_pro_pixel = 24,
			.compression = 0,
			.byte_size = static_cast<uint32_t>(stride * h),
			.hor_resolution = 11811,
			.vert_resolution = 11811,
			.used_colors = 0,
			.sign_colors = 0x1000000
		};
	}

	// напишите эту функцию
	bool SaveBMP(const Path& file, const Image& image) {
		ofstream ofs(file, ios::binary);
		const int w = image.GetWidth();
		const int h = image.GetHeight();
		const int stride = GetBMPStride(w);

		auto bm_file = CreateBitmapFileHeader(stride, h);
		auto bm_info = CreateBitmapInfoHeader(stride, w, h);

		ofs << bm_file << bm_info;

		std::vector<char> buff(stride);
		for (int y = 0; y < h; ++y) {
			const Color* line = image.GetLine(h - y - 1);
			for (int x = 0; x < w; ++x) {
				buff[x * 3 + 2] = static_cast<char>(line[x].r);
				buff[x * 3 + 1] = static_cast<char>(line[x].g);
				buff[x * 3 + 0] = static_cast<char>(line[x].b);
			}
			for (int x = w * 3; x < stride; ++x) {
				buff[x] = static_cast<char>(0);
			}
			ofs.write(buff.data(), stride);
		}

		return ofs.good();
	}

	// напишите эту функцию
	Image LoadBMP(const Path& file) {
		ifstream ifs(file, ios::binary);

		//1st PART
		BitmapFileHeader bm_file;
		ifs >> bm_file;
		if (!CheckBitmapFileHeader(bm_file)) {
			return {};
		}

		//2nd PART
		BitmapInfoHeader bm_info;
		ifs >> bm_info;
		if (!CheckBitmapInfoHeader(bm_info)) {
			return {};
		}

		Image result(bm_info.width, bm_info.height, Color::Black());
		int stride = GetBMPStride(bm_info.width);
		std::vector<char> buff(stride);

		for (int y = 0; y < bm_info.height; ++y) {
			Color* line = result.GetLine(bm_info.height - y - 1);
			ifs.read(buff.data(), stride);

			for (int x = 0; x < bm_info.width; ++x) {
				line[x].r = static_cast<byte>(buff[x * 3 + 2]);
				line[x].g = static_cast<byte>(buff[x * 3 + 1]);
				line[x].b = static_cast<byte>(buff[x * 3 + 0]);
			}
		}
		return result;
	}

}  // namespace img_lib
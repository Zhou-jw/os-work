#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 640
#define HEIGHT 480

int main(int argc, char **argv) {
    FILE *fp;
    unsigned char *buf;
    int i, j;
    int x, y;
    int r, g, b;
    int size;

    if (argc != 2) {
        printf("Usage: %s hexdumpfile\n", argv[0]);
        return -1;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }

    buf = (unsigned char *)malloc(WIDTH * HEIGHT * 3);
    if (buf == NULL) {
        printf("Failed to allocate memory\n");
        fclose(fp);
        return -1;
    }

    memset(buf, 0, WIDTH * HEIGHT * 3);

    for (i = 0; i < WIDTH * HEIGHT; i++) {
        fscanf(fp, "%02x%02x%02x", &r, &g, &b);
        x = i % WIDTH;
        y = i / WIDTH;
        buf[(y * WIDTH + x) * 3 + 0] = r;
        buf[(y * WIDTH + x) * 3 + 1] = g;
        buf[(y * WIDTH + x) * 3 + 2] = b;
    }

    fclose(fp);

    fp = fopen("output.bmp", "wb");
    if (fp == NULL) {
        printf("Failed to create output file\n");
        free(buf);
        return -1;
    }

    size = WIDTH * HEIGHT * 3 + 54;

    fputc('B', fp);
    fputc('M', fp);
    fwrite(&size, sizeof(int), 1, fp);
    fwrite("\0\0\0\0", sizeof(int), 1, fp);
    fwrite("\x36\0\0\0", sizeof(int), 1, fp);
    fwrite("\x28\0\0\0", sizeof(int), 1, fp);
    fwrite(&WIDTH, sizeof(int), 1, fp);
    fwrite(&HEIGHT, sizeof(int), 1, fp);
    fwrite("\x01\0\x18\0", sizeof(short), 1, fp);
    fwrite("\0\0\0\0", sizeof(int), 1, fp);
    fwrite(&size, sizeof(int), 1, fp);
    fwrite("\x13\x0B\x00\x00", sizeof(int), 1, fp);
    fwrite("\x13\x0B\x00\x00", sizeof(int), 1, fp);
    fwrite("\0\0\0\0", sizeof(int), 1, fp);
    fwrite("\0\0\0\0", sizeof(int), 1, fp);

    for (i = HEIGHT - 1; i >= 0; i--) {
        for (j = 0; j < WIDTH; j++) {
            fputc(buf[(i * WIDTH + j) * 3 + 2], fp);
            fputc(buf[(i * WIDTH + j) * 3 + 1], fp);
            fputc(buf[(i * WIDTH + j) * 3 + 0], fp);
        }
        for (j = WIDTH * 3; j % 4 != 0; j++) {
            fputc(0x00, fp);
        }
    }

    fclose(fp);

    free(buf);

    return 0;
}

uint8_t* convert(const std::vector<std::string> lines, const uint8_t hex_len, uint16_t &width, uint16_t &height) {
	size_t max_len = 0;
	for (std::string line : lines) {
		max_len = std::max(max_len, line.length());
	}

	uint8_t channels = hex_len % 4 == 0 ? 4 : 3;

	bool ignore_linebreaks = true;
	double pixels_per_line = double(max_len) / hex_len;
	if (width == 0) {
		if (height == 0) {
			width = ceil(pixels_per_line);
			ignore_linebreaks = false;
			height = lines.size();
		} else {
			width = ceil(double(lines.size()) / height * pixels_per_line);
		}
	}

	if (height == 0) {
		height = ceil(lines.size() / (width / pixels_per_line));
	}

	printf("Output image size: %d x %d pixels.\n", width, height);

	uint8_t *pixel_colors = new uint8_t[width * height * channels];

	for (uint32_t i = 0; i < width * height * channels; i++) {
		pixel_colors[i] = 0;
	}

	std::string line;
	std::string temp;
	std::string color;
	std::istringstream stream;
	uint32_t array_pos = 0;
	uint32_t parsed;
	for (size_t y = 0; y < lines.size(); y++) {
		line = lines[y];

		if (ignore_linebreaks && temp.length() > 0) {
			line = temp + line;
			temp = "";
		}

		while (line.length() > 0) {
			for (uint16_t x = 0; x < width; x++) {
				if (line.length() < uint32_t((x + 1) * hex_len)) {
					temp = line.substr(x * hex_len,
							line.length() - x * hex_len);
					line = "";
					break;
				}

				if (ignore_linebreaks) {
					if (y != 0) {
						array_pos += channels;
					}
				} else {
					array_pos = y * width * channels + x * channels;
				}

				color = line.substr(x * hex_len, hex_len);
				if (hex_len <= 4) {
					color.resize(hex_len * 2);
					for (int8_t i = hex_len - 1; i >= 0; i--) {
						color[i * 2 + 1] = color[i * 2] = color[i];
					}
				}

				stream.str(color);
				stream >> std::hex >> parsed;

				if (stream.rdbuf()->in_avail() > 0) {
					cerr << line.substr(x * hex_len, hex_len)
							<< " can't be parsed as hex input!" << endl;
				} else {
					pixel_colors[array_pos] = (parsed >> 16) & 0xFF;
					pixel_colors[array_pos + 1] = (parsed >> 8) & 0xFF;
					pixel_colors[array_pos + 2] = parsed & 0xFF;
					if (channels == 4) {
						pixel_colors[array_pos + 3] = (parsed >> 24) & 0xFF;
					}
				}

				stream.clear();
			}

			if (line.length() > width * hex_len) {
				line = line.substr(width * hex_len,
						line.length() - width * hex_len);
			} else {
				line = "";
			}
		}
	}

	return pixel_colors;
}
public static void byte2image(byte[] data,String path){
    if(data.length<3||path.equals("")) return;
    try{
    FileImageOutputStream imageOutput = new FileImageOutputStream(new File(path));
imageOutput.write(data, 0, data.length);
imageOutput.close();
System.out.println("Make Picture success,Please find image in " + path);
} catch(Exception ex) {
    System.out.println("Exception: " + ex);
    ex.printStackTrace();
}
}


public static void main(String[] args) {
    String str2="ffd8ffe000104a46494600010100000100010000f****ee3fdf4ff00d0857927f0cbf43fccd483";
    byte[] t=fromHexString(str2);
    byte2image(t, "d://1.jpg");//转成保存图片
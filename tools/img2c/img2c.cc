#include <QApplication>

#include <QImage>

#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	//  QApplication app(argc, argv);

	

	if(argc < 3) {
		printf("Missing parameter [inputfile] [imgname]\n");
		return 1;
	}

	QString file = argv[1];
	QString name = argv[2];

	QImage img;
	if(!img.load(file)) {
		printf("Could not open file %s\n", file.toStdString().c_str());
		return 1;
	}

	printf("struct __img_%s {\n  size_t width;\n  size_t height;\n  unsigned int pixels[%d];\n  unsigned int order;\n}", name.toStdString().c_str(), img.width() * img.height());
	printf(" img_%s = {\n", name.toStdString().c_str());
	printf("  %d,\n", img.width());
	printf("  %d,\n", img.height());
	printf("  {\n");
	
	for(int y = 0; y < img.height(); y++) {
		for(int x = 0; x < img.width(); x++) {
			printf("    0x%08x", htonl(img.pixel(x, y)));
			if(x != img.width() - 1 || y != img.height() - 1) {
 				printf(",");
			}
			printf("\n");
		}
	}
	printf("  },\n");
	printf("  0x%08x", htonl(0x00010203));
	printf("};\n");

	return 0;
	//	return app.exec();
}

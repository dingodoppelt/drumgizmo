#include <QApplication>

#include <QImage>

#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
  if(argc < 3) {
    printf("Missing parameter [inputfile] [outputfile]\n");
    return 1;
  }
  
  QString file = argv[1];
  QString name = argv[2];
  
  QImage img;
  if(!img.load(file)) {
    printf("Could not open file %s\n", file.toStdString().c_str());
    return 1;
  }
  
  FILE *fp = fopen(name.toStdString().c_str(), "w");
  
  unsigned int w = img.width();
  unsigned int h = img.height();
  (void)fwrite(&w, sizeof(unsigned int), 1, fp);
  (void)fwrite(&h, sizeof(unsigned int), 1, fp);
  
  for(int y = 0; y < img.height(); y++) {
    for(int x = 0; x < img.width(); x++) {
      unsigned int px = img.pixel(x, y);
      (void)fwrite(&px, 1, sizeof(unsigned int), fp);
    }
  }
  
  fclose(fp);
  return 0;
}

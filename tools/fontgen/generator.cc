/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            generator.cc
 *
 *  Mon Nov 21 20:16:00 CEST 2016
 *  Copyright 2016 Lars Muldjord
 *  muldjordlars@gmail.com
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include "generator.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QFont>

Generator::Generator()
{
  QLabel *fontLabel = new QLabel("Font family:");
  fontLineEdit = new QLineEdit("DejaVu");
  QLabel *sizeLabel = new QLabel("Font px size:");
  sizeLineEdit = new QLineEdit("12");
  QLabel *outputLabel = new QLabel("Output file:");
  outputLineEdit = new QLineEdit("output");

  QVBoxLayout *layout = new QVBoxLayout();
  QPushButton *renderButton = new QPushButton("Generate");
  connect(renderButton, SIGNAL(clicked()), this, SLOT(initGenerate()));

  layout->addWidget(fontLabel);
  layout->addWidget(fontLineEdit);
  layout->addWidget(sizeLabel);
  layout->addWidget(sizeLineEdit);
  layout->addWidget(outputLabel);
  layout->addWidget(outputLineEdit);
  layout->addWidget(renderButton);

  setLayout(layout);
}

Generator::~Generator()
{
}

void Generator::initGenerate()
{
  qDebug("Generating font to file '%s' using font '%s' at size '%s'...",
         outputLineEdit->text().toStdString().c_str(),
         fontLineEdit->text().toStdString().c_str(),
         sizeLineEdit->text().toStdString().c_str());

  QList<QImage> chars;

  maxSize = 50;

  int vertOffset = 0;
  int fontHeight = maxSize;
  
  font.setFamily(fontLineEdit->text());
  font.setPixelSize(sizeLineEdit->text().toInt());
  
  setVertLimits(vertOffset, fontHeight);
  
  for(int a = 0; a < 255; ++a) {
    int horizOffset = 0;
    int charWidth = 0;
    QImage image(maxSize, maxSize, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    setHorizLimits(horizOffset, charWidth, a, image);
    QImage finalChar(image.copy(horizOffset, vertOffset, charWidth + 2, fontHeight));
    QPainter p(&finalChar);
    p.setPen(QPen(QColor(255, 0, 255)));
    p.drawLine(finalChar.width() - 1, 0, finalChar.width() - 1, finalChar.height());
    chars.append(finalChar);
  }
  
  int imageWidth = 0;
  for(int a = 0; a < chars.length(); ++a) {
    imageWidth += chars.at(a).width();
  }

  QImage finalFont(imageWidth, fontHeight, QImage::Format_ARGB32);
  finalFont.fill(Qt::transparent);
  QPainter p(&finalFont);
  int curOffset = 0;
  for(int a = 0; a < chars.length(); ++a) {
    p.drawImage(curOffset, 0, chars.at(a));
    curOffset += chars.at(a).width();
  }
  finalFont.save(outputLineEdit->text() + ".png");
}

void Generator::setVertLimits(int &vertOffset, int &fontHeight)
{
  QImage image(maxSize, maxSize, QImage::Format_ARGB32);
  image.fill(Qt::transparent);
    
  QPainter p(&image);
  p.setPen(QPen(QColor(0, 0, 0, 255)));
  p.setFont(font);
    
  // Render the two chars that will give us a maximum overall font height
  p.drawText(maxSize / 2, maxSize / 2, "Õ");
  p.drawText(maxSize / 2, maxSize / 2, "j");

  vertOffset = getVertOffset(image);
  fontHeight = getFontHeight(image, vertOffset);
  qDebug("Vertical offset: %d\nHeight: %d\n", vertOffset, fontHeight);
}

void Generator::setHorizLimits(int &horizOffset, int &charWidth,
                               const int &curChar, QImage &image)
{
  QPainter p(&image);
  p.setPen(QPen(QColor(0, 0, 0, 255)));
  p.setFont(font);
    
  p.drawText(maxSize / 2, maxSize / 2, QChar(curChar));

  horizOffset = getHorizOffset(image);
  charWidth = getFontWidth(image, horizOffset);
  qDebug("Horizontal offset: %d\nWidth: %d\n", horizOffset, charWidth);
}

int Generator::getVertOffset(const QImage &image)
{
  for(int y = 0; y < maxSize; ++y) {
    for(int x = 0; x < maxSize; ++x) {
      if(image.pixelColor(x, y) != Qt::transparent) {
        return y;
      }
    }
  }
  return 0;
}

int Generator::getFontHeight(const QImage &image, const int &vertOffset)
{
  for(int y = maxSize - 1; y > vertOffset; --y) {
    for(int x = 0; x < maxSize; ++x) {
      if(image.pixelColor(x, y) != Qt::transparent) {
        return y + 1 - vertOffset;
      }
    }
  }
  return 0;
}

int Generator::getHorizOffset(const QImage &image)
{
  for(int x = 0; x < maxSize; ++x) {
    for(int y = 0; y < maxSize; ++y) {
      if(image.pixelColor(x, y) != Qt::transparent) {
        return x;
      }
    }
  }
  return 0;
}

int Generator::getFontWidth(const QImage &image, const int &horizOffset)
{
  for(int x = maxSize - 1; x > horizOffset; --x) {
    for(int y = 0; y < maxSize; ++y) {
      if(image.pixelColor(x, y) != Qt::transparent) {
        return x + 1 - horizOffset;
      }
    }
  }
  return 0;
}

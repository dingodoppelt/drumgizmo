/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            generator.h
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

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>

class Generator : public QWidget
{
  Q_OBJECT;

public:
  Generator();
  ~Generator();

private slots:
  void initGenerate();

private:
  QLineEdit *fontLineEdit;
  QLineEdit *sizeLineEdit;
  QLineEdit *outputLineEdit;
  QCheckBox *embossEnabled;
  
  int maxSize;
  QFont font;

  void setVertLimits(int &vertOffset, int &fontHeight);
  int getVertOffset(const QImage &image);
  int getFontHeight(const QImage &image, const int &vertOffset);

  void setHorizLimits(int &horizOffset, int &fontHeight,
                      const int &curChar, QImage &image);
  int getHorizOffset(const QImage &image);
  int getFontWidth(const QImage &image, const int &horizOffset);

  void assembleFinalFont(const QList<QImage> &chars, const int &fontHeight);
};

#endif // _GENERATOR_H

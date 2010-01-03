/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audioextractor.cc
 *
 *  Sat Nov 21 13:09:35 CET 2009
 *  Copyright 2009 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "audioextractor.h"

#include <QDomDocument>
#include <QFile>

#include <sndfile.h>

AudioExtractor::AudioExtractor(QObject *parent)
  : QObject(parent)
{
}

float *AudioExtractor::load(QString file, size_t *size)
{
  float *data;

  SF_INFO sf_info;
	SNDFILE *fh = sf_open(file.toStdString().c_str(), SFM_READ, &sf_info);
  if(!fh) {
    printf("Load error...\n");
    *size = 0;
    return NULL;
  }

	*size = sf_seek(fh, 0, SEEK_END);
  data = new float[*size];

	sf_seek(fh, 0, SEEK_SET);
	sf_read_float(fh, data, *size); 

	sf_close(fh);

  return data;
}

void AudioExtractor::exportSelection(QString filename,
                                     int index,
                                     float *data, size_t size,
                                     Selection sel)
{
  printf("Writing: %s (sz: %d, from %d to %d)\n",
         filename.toStdString().c_str(), size, sel.from, sel.to);
  
  if(sel.from > (int)size || sel.to > (int)size || sel.to < 0 || sel.from < 0 || sel.to < sel.from) {
    printf("Out of bounds\n");
    return;
  }

  // Apply linear fadein
  for(int i = 0; i < sel.fadein; i++) {
    float val = ((float)i / (float)sel.fadein);
    data[i + sel.from] *= val;
  }

  // Apply fadeout
  for(int i = 0; i < sel.fadeout; i++) {
    float val = ((float)i / (float)sel.fadeout);
    data[sel.to - i] *= val;
  }

  SF_INFO sf_info;
  sf_info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
  sf_info.samplerate = 44100;
  sf_info.channels = 1;

  SNDFILE *fh = sf_open(filename.toStdString().c_str(), SFM_WRITE, &sf_info);
  if(!fh) {
    printf("Open for write error...\n");
    return;
  }
  sf_write_float(fh, data + sel.from, sel.to - sel.from); 
  sf_close(fh);
}

void AudioExtractor::exportSelections(Selections selections, QVector<int> levels)
{
  // Do the actual exporting one file at the time.
  AudioFileList::iterator j = audiofiles.begin();
  while(j != audiofiles.end()) {

    QString file = j->first;
    QString name = j->second;
    size_t size;

    // TODO: Use sf_seek instead...
    float *data = load(file, &size);
    if(!data) continue;

    int index = 0;
    QMap<int, Selection>::iterator i = selections.begin();
    while(i != selections.end()) {
      index++;

      QString file = exportpath + "/" + prefix + "/samples/" +
        prefix + "-" + name + "-" + QString::number(index) + ".wav";

      exportSelection(file, index, data, size, i.value());
      i++;
    }

    delete[] data;
    
    j++;
  }

  QDomDocument doc;
  QDomProcessingInstruction header =
    doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
  doc.appendChild(header); 

  QDomElement instrument = doc.createElement("instrument");
  instrument.setAttribute("name", prefix);
  doc.appendChild(instrument);

  QDomElement samples = doc.createElement("samples");
  instrument.appendChild(samples);

  // Do the adding to the xml file one sample at the time.
  int index = 0;
  QMap<int, Selection>::iterator i = selections.begin();
  while(i != selections.end()) {
    index++;

    QDomElement sample = doc.createElement("sample");
    sample.setAttribute("name", prefix + "-" + QString::number(index));
    samples.appendChild(sample);

    AudioFileList::iterator j = audiofiles.begin();
    while(j != audiofiles.end()) {

      QString file = j->first;
      QString name = j->second;

      QDomElement audiofile = doc.createElement("audiofile");
      audiofile.setAttribute("file", "samples/" + prefix + "-" + name + "-"
                             + QString::number(index) + ".wav");
      audiofile.setAttribute("channel", name);
      sample.appendChild(audiofile);

      j++;
    }

    i++;
  }

  QDomElement velocities = doc.createElement("velocities");
  instrument.appendChild(velocities);

  QVector<int>::iterator k = levels.begin();
  while(k != levels.end()) {
    QDomElement velocity = doc.createElement("velocity");
    velocity.setAttribute("lower", "0");
    velocity.setAttribute("upper", "127");
    velocities.appendChild(velocity);

    QDomElement sampleref = doc.createElement("sampleref");
    sampleref.setAttribute("name", "bleh");
    sampleref.setAttribute("probability", "0.1");
    velocity.appendChild(sampleref);

    k++;
  }

  QFile xmlfile(exportpath + "/" + prefix + "/" + prefix + ".xml");
  xmlfile.open(QIODevice::WriteOnly);
  xmlfile.write(doc.toByteArray());
  xmlfile.close();
}

void AudioExtractor::addFile(QString file, QString name)
{
  QPair<QString, QString> pair;
  pair.first = file;
  pair.second = name;
  audiofiles.push_back(pair);
}

void AudioExtractor::removeFile(QString file, QString name)
{
  AudioFileList::iterator j = audiofiles.begin();
  while(j != audiofiles.end()) {
    if(file == j->first/* && name == j->second*/) {
      audiofiles.erase(j);
      return;
    }
    j++;
  }
}

void AudioExtractor::setOutputPrefix(const QString &p)
{
  prefix = p;
}

void AudioExtractor::setExportPath(const QString &path)
{
  exportpath = path;
}

void AudioExtractor::changeName(QString file, QString name)
{
  AudioFileList::iterator j = audiofiles.begin();
  while(j != audiofiles.end()) {
    if(file == j->first) {
      j->second = name;
      return;
    }
    j++;
  }
}

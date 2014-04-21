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
#include <QDir>
#include <QApplication>

#include <sndfile.h>

typedef struct {
  SNDFILE *fh;
  float *data;
} audiodata_t;

AudioExtractor::AudioExtractor(Selections &s, QObject *parent)
  : QObject(parent), selections(s)
{
}

void AudioExtractor::exportSelections()
{
  emit setMaximumProgress(selections.ids().size() + 1/* for xml writing*/);
  int progress = 0;
  emit progressUpdate(progress++);
  qApp->processEvents();

  // Open all input audio files:
  audiodata_t audiodata[audiofiles.size()];

  int idx = 0;
  AudioFileList::iterator j = audiofiles.begin();
  while(j != audiofiles.end()) {
    QString file = j->first;

    SF_INFO sf_info;
    audiodata[idx].fh = sf_open(file.toStdString().c_str(), SFM_READ, &sf_info);
    if(!audiodata[idx].fh) {
      printf("Load error '%s'\n", file.toStdString().c_str());
      return;
    }

    audiodata[idx].data = NULL;

    j++;
    idx++;
  }
  
  idx = 0;
  QVector<sel_id_t> sels = selections.ids();
  QVector<sel_id_t>::iterator si = sels.begin();
  while(si != sels.end()) {
    Selection sel = selections.get(*si);
    size_t offset = sel.from;
    size_t size = sel.to - sel.from;
    size_t fadein = sel.fadein;
    size_t fadeout = sel.fadeout;


    // Read all input audio file chunks:
    for(int i = 0; i < audiofiles.size(); i++) {

      // Clear out old buffer (if one exists)
      if(audiodata[i].data) {
        delete audiodata[i].data;
        audiodata[i].data = NULL;
      }

      SNDFILE *fh = audiodata[i].fh;

      sf_seek(fh, offset, SEEK_SET);

      float *data = new float[size];
      sf_read_float(fh, data, size); 

      // Apply linear fadein
      for(size_t fi = 0; fi < fadein; fi++) {
        float val = ((float)fi / (float)fadein);
        if(fi < size) data[fi] *= val;
      }

      // Apply fadeout
      for(size_t fo = 0; fo < fadeout; fo++) {
        float val = ((float)fo / (float)fadeout);
        if(size >= fo) data[size - fo] *= val;
      }

      audiodata[i].data = data;
    }

    // Create output path:
    QString path = exportpath + "/" + prefix + "/samples";
    QDir d;
    d.mkpath(path);

    // Write all sample chunks to single output file:
    QString file = path + "/" + QString::number(idx) + "-" + prefix + ".wav";
    
    SF_INFO sf_info;
    sf_info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    sf_info.samplerate = 44100;
    sf_info.channels = audiofiles.size();

    SNDFILE *ofh = sf_open(file.toStdString().c_str(), SFM_WRITE, &sf_info);
    if(!ofh) {
      printf("Open for write error...\n");
      return;
    }

    for(size_t ob = 0; ob < size; ob++) {
      float obuf[audiofiles.size()];
      for(int ai = 0; ai < audiofiles.size(); ai++) {
        obuf[ai] = audiodata[ai].data[ob];
      }
      sf_write_float(ofh, obuf, audiofiles.size()); 
    }
    sf_close(ofh);

    idx++;
    si++;

    emit progressUpdate(progress++);
    qApp->processEvents();
  }

  // Close all input audio files:
  for(int i = 0; i < audiofiles.size(); i++) {
    if(audiodata[i].data) {
      delete audiodata[i].data;
      audiodata[i].data = NULL;
    }

    sf_close(audiodata[i].fh);
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

  {
  // Do the adding to the xml file one sample at the time.
  int index = 0;
  QVector<sel_id_t>::iterator si = sels.begin();
  while(si != sels.end()) {
    index++;

    Selection i = selections.get(*si);
    i.name = prefix + "-" + QString::number(index);

    QDomElement sample = doc.createElement("sample");
    sample.setAttribute("name", i.name);
    sample.setAttribute("power", QString::number(i.energy));
    samples.appendChild(sample);

    selections.update(*si, i);

    int channelnum = 1; // Filechannel numbers are 1-based.
    AudioFileList::iterator j = audiofiles.begin();
    while(j != audiofiles.end()) {

      QString file = j->first;
      QString name = j->second;

      QDomElement audiofile = doc.createElement("audiofile");
      audiofile.setAttribute("file", "samples/" + 
                             QString::number(index) + "-" + prefix + ".wav");
      audiofile.setAttribute("channel", name);
      audiofile.setAttribute("filechannel", QString::number(channelnum));
      sample.appendChild(audiofile);
      channelnum++;
      j++;
    }

    si++;
  }
  }
  
  QFile xmlfile(exportpath + "/" + prefix + "/" + prefix + ".xml");
  xmlfile.open(QIODevice::WriteOnly);
  xmlfile.write(doc.toByteArray());
  xmlfile.close();

  emit progressUpdate(progress++);
  qApp->processEvents();
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

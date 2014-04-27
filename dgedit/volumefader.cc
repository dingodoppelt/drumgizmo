/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            volumefader.cc
 *
 *  Tue Apr 15 15:46:00 CEST 2014
 *  Copyright 2014 Jonas Suhr Christensen
 *  jsc@umbraculum.org
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
#include "volumefader.h"

#include <QSlider>
#include <QVBoxLayout>

#include <math.h>

#define SCALAR 10
#define P 0.5

VolumeFader::VolumeFader() 
{
  peak = 0;
  volslider = new QSlider();
  volslider->setRange(-60 * SCALAR , 10 * SCALAR);
  volslider->setOrientation(Qt::Horizontal);

  volumepeak = new QLabel();

  volume = new QLabel();

  connect(volslider, SIGNAL(valueChanged(int)), this, SLOT(handleValueChanged()));

  QVBoxLayout* lo = new QVBoxLayout();
  lo->addWidget(volslider);
  
  lo->addWidget(volumepeak);
  lo->addWidget(volume);

  setLayout(lo);
}

VolumeFader::~VolumeFader()
{

}

void VolumeFader::updatePeakDb(double) 
{
}

void VolumeFader::updatePeakPower(double newpeak)
{
  peak = (newpeak * (1-P) + peak * P);
  volumepeak->setText("Peak " + QString::number(peak, 'f', 5));
  handleValueChanged();
}

void VolumeFader::setVolumeDb(double db)
{
  volslider->setValue(db*SCALAR); 
  handleValueChanged();
}

void VolumeFader::setVolumePower(double power)
{
  double db = 20 * log10(power);
  setVolumeDb(db);
  handleValueChanged();
}

void VolumeFader::handleValueChanged() 
{
  double db = ((double)volslider->value())/((double)SCALAR);
  double power = pow(10, db/20);
  emit volumeChangedDb(db);
  emit volumeChangedPower(power);

  volume->setText("Gain " + QString::number(volslider->value()/SCALAR) + " dB");
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jackclient.cc
 *
 *  Sun Jul 20 21:48:44 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#include "jackclient.h"

extern "C"
{
  void _wrap_jack_shutdown(void *arg){
    ((JackClient*)arg)->shutdown();}
  int _wrap_jack_process(jack_nframes_t nframes, void *arg){
    return ((JackClient*)arg)->process(nframes);}
  void _wrap_jack_thread_init(void *arg){
    ((JackClient*)arg)->thread_init();}
  void _wrap_jack_freewheel_mode(int freewheel_mode, void *arg){
    ((JackClient*)arg)->freewheel_mode(freewheel_mode);}
  int _wrap_jack_buffer_size(jack_nframes_t nframes, void *arg){
    return ((JackClient*)arg)->buffer_size(nframes);}
  int _wrap_jack_sample_rate(jack_nframes_t nframes, void *arg){
    return ((JackClient*)arg)->sample_rate(nframes);}
  void _wrap_jack_port_registration(jack_port_id_t port, int i, void *arg){
    ((JackClient*)arg)->port_registration(port, i);}
  int _wrap_jack_graph_order(void *arg){
    return ((JackClient*)arg)->graph_order();}
  int _wrap_jack_xrun(void *arg){
    return ((JackClient*)arg)->xrun();}
}  // extern "C"

JackClient::JackClient(size_t num_inputs, size_t num_outputs)
{
	jack_status_t status;

	jack_client = jack_client_open("DrumGizmo", JackNullOption, &status);

	// Setup input port
	midi_port = jack_port_register(jack_client,
                                 "Midi in",
                                 JACK_DEFAULT_MIDI_TYPE,
                                 JackPortIsInput,// | JackPortIsTerminal,
                                 0);

	// Setup input ports
	for(size_t i = 0; i < num_inputs; i++) {
		char port_name[32];
		sprintf(port_name, "Port %i", i);
		jack_port_t *port = jack_port_register(jack_client,
                                           port_name,
                                           JACK_DEFAULT_AUDIO_TYPE,
                                           JackPortIsInput | JackPortIsTerminal,
                                           0);
    input_ports.push_back(port);
	}

	// Setup output ports
	for(size_t i = 0; i < num_outputs; i++) {
		char port_name[32];
		sprintf(port_name, "Port %i", i);
		jack_port_t *port = jack_port_register(jack_client,
                                           port_name,
                                           JACK_DEFAULT_AUDIO_TYPE,
                                           JackPortIsOutput | JackPortIsTerminal,
                                           0);
    output_ports.push_back(port);
	}

	jack_on_shutdown(jack_client, _wrap_jack_shutdown, this);
  jack_set_process_callback(jack_client, _wrap_jack_process, this);
	jack_set_thread_init_callback(jack_client, _wrap_jack_thread_init, this);
	jack_set_freewheel_callback(jack_client, _wrap_jack_freewheel_mode, this);
	jack_set_buffer_size_callback(jack_client, _wrap_jack_buffer_size, this);
	jack_set_sample_rate_callback(jack_client, _wrap_jack_sample_rate, this);
	jack_set_port_registration_callback(jack_client, _wrap_jack_port_registration, this);
	jack_set_graph_order_callback(jack_client, _wrap_jack_graph_order, this);
	jack_set_xrun_callback(jack_client, _wrap_jack_xrun, this);
}

JackClient::~JackClient()
{
	jack_client_close(jack_client);
}

void JackClient::activate()
{
	jack_activate(jack_client);
}

void JackClient::shutdown()
{
}

int JackClient::process(jack_nframes_t nframes)
{
  /*
	int pos = -1;
	Params *params = (Params*)arg;

	void *midibuffer = jack_port_get_buffer(params->midi_port, nframes);
	jack_nframes_t midievents = jack_midi_get_event_count(midibuffer);
	for(jack_nframes_t i = 0; i < midievents; i++) {
		jack_midi_event_t event;
		jack_midi_event_get(&event, midibuffer, i);
		printf("[ Time: %d  Size: %d  ", event.time, event.size);
		for(int j = 0; j < event.size; j++) {
			jack_midi_data_t m = event.buffer[j];
			printf("  Data: %d ", m);
		}
		printf("]\n");
		for(int i = 0; i < NUM_PORTS; i++) {
			pos = event.time;
		}
	}

	for(int i = 0; i < NUM_PORTS; i++) {
		jack_default_audio_sample_t *buffer;
		buffer = (jack_default_audio_sample_t *)jack_port_get_buffer(params->drums[i].port, nframes);
		for(int j = 0; j < nframes; j++) {
			if(j == pos) params->drums[i].samples[0].p = -j;
			offset_t p = params->drums[i].samples[0].p;
			size_t data_size = params->drums[i].samples[0].data_size;
			jack_default_audio_sample_t *samples = params->drums[i].samples[0].data;
			if(p+j > data_size) buffer[j] = 0;
			else buffer[j] = samples[(j + p) % data_size];
			//      params->drums[i].samples[0].p ++;
		}
		params->drums[i].samples[0].p += nframes;
	}
  */
	return 0;
}

void JackClient::thread_init()
{
}

void JackClient::freewheel_mode(int freewheel_mode)
{
}

int JackClient::buffer_size(jack_nframes_t nframes)
{
  return 0;
}

int JackClient::sample_rate(jack_nframes_t nframes)
{
  return 0;
}

void JackClient::port_registration(jack_port_id_t port, int i)
{
}

int JackClient::graph_order()
{
  return 0;
}

int JackClient::xrun()
{
  return 0;
}

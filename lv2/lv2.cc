/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lv2.cc
 *
 *  Wed Jul 13 13:50:33 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

#include <stdlib.h>

#include "lv2_gui.h"

#include "lv2_instance.h"

#include <hugin.hpp>

#define MIDI_EVENT_URI "http://lv2plug.in/ns/ext/midi#MidiEvent"

#define NS_ATOM "http://lv2plug.in/ns/ext/atom#"
#define NS_DG "http://drumgizmo.org/lv2/atom#"

/*
 * Stuff to handle DrumGizmo* transmission from instance to GUI.
 */
static LV2_DrumGizmo_Descriptor dg_descriptor;

static DrumGizmo *dg_get_pci(LV2_Handle instance)
{
  DGLV2 *dglv2 = (DGLV2 *)instance;
  return dglv2->dg;
}

/*
 * Stuff to save/restore plugin state.
 */
/*
void dg_save(LV2_Handle                 instance,
             LV2_State_Store_Function   store,
             void*                      callback_data,
             uint32_t                   flags,
             const LV2_Feature *const * features)
{
  DGLV2 *dglv2 = (DGLV2 *)instance;
  printf("dg_save\n");

  std::string config = dglv2->dg->configString();
  printf("%s\n", config.c_str());

  store(callback_data,
        dglv2->urimap->uri_to_id(dglv2->urimap->callback_data,
                                 NULL, NS_DG "config"),
        config.data(), config.length(),
        dglv2->urimap->uri_to_id(dglv2->urimap->callback_data,
                                 NULL, NS_ATOM "String"),
        LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
}

void dg_restore(LV2_Handle                  instance,
                LV2_State_Retrieve_Function retrieve,
                void*                       callback_data,
                uint32_t                    flags,
                const LV2_Feature *const *  features)
{
  DGLV2 *dglv2 = (DGLV2 *)instance;
  printf("dg_restore\n");

  size_t      size;
  uint32_t    type;
  //  uint32_t    flags;

  const char* data =
    (const char*)retrieve(callback_data,
                          dglv2->urimap->uri_to_id(dglv2->urimap->callback_data,
                                                   NULL, NS_DG "config"),
                          &size, &type, &flags);
  std::string config;
  config.append(data, size);
  dglv2->dg->setConfigString(config);

  dglv2->in->loadMidiMap(dglv2->dg->midimapfile);
}
*/
LV2_State_Status
dg_save(LV2_Handle                 instance,
        LV2_State_Store_Function   store,
        LV2_State_Handle           handle,
        uint32_t                   flags,
        const LV2_Feature *const * features)
{
  DGLV2 *dglv2 = (DGLV2 *)instance;
  printf("dg_save\n");

  std::string config = dglv2->dg->configString();
  printf("%s\n", config.c_str());

  store(handle,
        dglv2->urimap->uri_to_id(dglv2->urimap->callback_data,
                                 NULL, NS_DG "config"),
        config.c_str(),
        config.length() + 1, // Careful!  Need space for terminator
        dglv2->urimap->uri_to_id(dglv2->urimap->callback_data,
                                 NULL, NS_ATOM "chunk"),
        LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
/*
    MyPlugin*   plugin   = (MyPlugin*)instance;
    const char* greeting = plugin->state.greeting;

    store(handle,
          plugin->uris.my_greeting,
          greeting,
          strlen(greeting) + 1,  // Careful!  Need space for terminator
          plugin->uris.atom_String,
          LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
*/
  printf("dg_save\n");
    return LV2_STATE_SUCCESS;
}

LV2_State_Status
dg_restore(LV2_Handle                  instance,
           LV2_State_Retrieve_Function retrieve,
           LV2_State_Handle            handle,
           uint32_t                    flags,
           const LV2_Feature *const *  features)
{
  DGLV2 *dglv2 = (DGLV2 *)instance;
  DEBUG(lv2, "dg_restore begin\n");

  size_t      size;
  uint32_t    type;
  //  uint32_t    flags;

  const char* data =
    (const char*)retrieve(handle,
                          dglv2->urimap->uri_to_id(dglv2->urimap->callback_data,
                                                   NULL, NS_DG "config"),
                          &size, &type, &flags);
  DEBUG(lv2, "Config string size: %d, data*: %p\n", size, data);

  if(data && size) {
    std::string config;
    config.append(data, size - 1);
    dglv2->dg->setConfigString(config);
    //dglv2->in->loadMidiMap(dglv2->dg->midimapfile);
  }
 
  /*
    MyPlugin* plugin = (MyPlugin*)instance;

    size_t      size;
    uint32_t    type;
    uint32_t    flags;
    const char* greeting = retrieve(
        handle, plugin->uris.my_greeting, &size, &type, &flags);

    if (greeting) {
        free(plugin->state->greeting);
        plugin->state->greeting = strdup(greeting);
    } else {
        plugin->state->greeting = strdup(DEFAULT_GREETING);
    }
  */
  DEBUG(lv2, "dg_restore done\n");

  return LV2_STATE_SUCCESS;
}

static LV2_State_Interface dg_persist = {
  dg_save,
  dg_restore
};

/** A globally unique, case-sensitive identifier for this plugin type.
 *
 * All plugins with the same URI MUST be compatible in terms of 'port
 * signature', meaning they have the same number of ports, same port
 * shortnames, and roughly the same functionality.  URIs should
 * probably contain a version number (or similar) for this reason.
 *
 * Rationale:  When serializing session/patch/etc files, hosts MUST
 * refer to a loaded plugin by the plugin URI only.  In the future
 * loading a plugin with this URI MUST yield a plugin with the
 * same ports (etc) which is 100% compatible. */
#define DRUMGIZMO_URI "http://drumgizmo.org/lv2"

/** Function pointer that instantiates a plugin.
 *
 * A handle is returned indicating the new plugin instance. The
 * instantiation function accepts a sample rate as a parameter as well
 * as the plugin descriptor from which this instantiate function was
 * found. This function must return NULL if instantiation fails.
 *
 * bundle_path is a string of the path to the LV2 bundle which contains
 * this plugin binary.  It MUST include the trailing directory separator
 * (e.g. '/') so that BundlePath + filename gives the path to a file
 * in the bundle.
 *
 * features is a NULL terminated array of LV2_Feature structs which
 * represent the features the host supports. Plugins may refuse to
 * instantiate if required features are not found here (however hosts
 * SHOULD NOT use this as a discovery mechanism, instead reading the
 * data file before attempting to instantiate the plugin).  This array
 * must always exist; if a host has no features, it MUST pass a single
 * element array containing NULL (to simplify plugins).
 *
 * Note that instance initialisation should generally occur in
 * activate() rather than here.  If a host calls instantiate, it MUST
 * call cleanup() at some point in the future. */
LV2_Handle instantiate(const struct _LV2_Descriptor *descriptor,
                       double sample_rate,
                       const char *bundle_path,
                       const LV2_Feature *const *features)
{
  DGLV2 *dglv2 = new DGLV2;

  dglv2->urimap = NULL;
  for (int i = 0 ; features[i] ; i++) {
    printf("DG: feature: %s\n", features[i]->URI);
    if (!strcmp(features[i]->URI, LV2_URI_MAP_URI)) {
      dglv2->urimap = (LV2_URI_Map_Feature*)features[i]->data;
    }
    if (!strcmp(features[i]->URI, LV2_STATE__makePath)) {
      dglv2->makepath = (LV2_State_Make_Path*)features[i]->data;
    }
 }

  dg_descriptor.get_pci = dg_get_pci;

  dglv2->in = new InputLV2();
  dglv2->out = new OutputLV2();

  dglv2->buffer = NULL;
  dglv2->buffer_size = 0;

  /*
  char* path = dglv2->makepath->path(dglv2->makepath->handle,
                                      "hello.txt");
  FILE* myfile = fopen(path, "w");
  fprintf(myfile, "world");
  fclose(myfile);
  */

  dglv2->dg = new DrumGizmo(dglv2->out, dglv2->in);
  //  dglv2->dg->loadkit(getenv("DRUMGIZMO_DRUMKIT"));
  //  dglv2->dg->init(true);

  return (LV2_Handle)dglv2;
}

/** Function pointer that connects a port on a plugin instance to a memory
 * location where the block of data for the port will be read/written.
 *
 * The data location is expected to be of the type defined in the
 * plugin's data file (e.g. an array of float for an lv2:AudioPort).
 * Memory issues are managed by the host. The plugin must read/write
 * the data at these locations every time run() is called, data
 * present at the time of this connection call MUST NOT be
 * considered meaningful. 
 *
 * The host MUST NOT try to connect a data buffer to a port index 
 * that is not defined in the RDF data for the plugin. If it does, 
 * the plugin's behaviour is undefined.
 *
 * connect_port() may be called more than once for a plugin instance
 * to allow the host to change the buffers that the plugin is reading
 * or writing. These calls may be made before or after activate()
 * or deactivate() calls.  Note that there may be realtime constraints
 * on connect_port (see lv2:hardRTCapable in lv2.ttl).
 *
 * connect_port() MUST be called at least once for each port before
 * run() is called.  The plugin must pay careful attention to the block
 * size passed to the run function as the block allocated may only just
 * be large enough to contain the block of data (typically samples), and
 * is not guaranteed to be constant.
 *
 * Plugin writers should be aware that the host may elect to use the
 * same buffer for more than one port and even use the same buffer for
 * both input and output (see lv2:inPlaceBroken in lv2.ttl).
 * However, overlapped buffers or use of a single buffer for both
 * audio and control data may result in unexpected behaviour.
 *
 * If the plugin has the feature lv2:hardRTCapable then there are 
 * various things that the plugin MUST NOT do within the connect_port()
 * function (see lv2.ttl). */
void connect_port(LV2_Handle instance,
                  uint32_t port,
                  void *data_location)
{
  DGLV2 *dglv2 = (DGLV2 *)instance;

  if(port == 0) {// MIDI in
    dglv2->in->eventPort = (LV2_Event_Buffer*)data_location;
  } else {// Audio Port
    if(port - 1 < NUM_OUTPUTS) {
      dglv2->out->outputPorts[port - 1].samples = (sample_t*)data_location;
      dglv2->out->outputPorts[port - 1].size = 0;
    }
  }
}

/** Function pointer that initialises a plugin instance and activates
 * it for use.
 * 
 * This is separated from instantiate() to aid real-time support and so
 * that hosts can reinitialise a plugin instance by calling deactivate()
 * and then activate(). In this case the plugin instance must reset all
 * state information dependent on the history of the plugin instance
 * except for any data locations provided by connect_port(). If there
 * is nothing for activate() to do then the plugin writer may provide
 * a NULL rather than an empty function.
 *
 * When present, hosts MUST call this function once before run()
 * is called for the first time. This call SHOULD be made as close
 * to the run() call as possible and indicates to real-time plugins
 * that they are now live, however plugins MUST NOT rely on a prompt
 * call to run() after activate().  activate() may not be called again
 * unless deactivate() is called first (after which activate() may be
 * called again, followed by deactivate, etc. etc.).  If a host calls
 * activate, it MUST call deactivate at some point in the future.
 *
 * Note that connect_port() may be called before or after a call to
 * activate(). */
void activate(LV2_Handle instance)
{
  DGLV2 *dglv2 = (DGLV2 *)instance;
  //dglv2->dg->run();
  (void)dglv2;
}

/** Function pointer that runs a plugin instance for a block.
 *
 * Two parameters are required: the first is a handle to the particular
 * instance to be run and the second indicates the block size (in
 * samples) for which the plugin instance may run.
 *
 * Note that if an activate() function exists then it must be called
 * before run(). If deactivate() is called for a plugin instance then
 * the plugin instance may not be reused until activate() has been
 * called again.
 *
 * If the plugin has the feature lv2:hardRTCapable then there are 
 * various things that the plugin MUST NOT do within the run()
 * function (see lv2.ttl). */
void run(LV2_Handle instance,
         uint32_t sample_count)
{
  static size_t pos = 0;
  DGLV2 *dglv2 = (DGLV2 *)instance;

  // The buffer is not used anymore - declared NULL in 'instantiate'.
  /*
  if(dglv2->buffer_size != sample_count) {
    if(dglv2->buffer) free(dglv2->buffer);
    dglv2->buffer_size = sample_count;
    dglv2->buffer = (sample_t*)malloc(sizeof(sample_t) * dglv2->buffer_size);
    printf("(Re)allocate buffer: %d samples\n", dglv2->buffer_size);
  }
  */
  dglv2->dg->run(pos, dglv2->buffer, sample_count);

  pos += sample_count;
}

/** This is the counterpart to activate() (see above). If there is
 * nothing for deactivate() to do then the plugin writer may provide
 * a NULL rather than an empty function.
 *
 * Hosts must deactivate all activated units after they have been run()
 * for the last time. This call SHOULD be made as close to the last
 * run() call as possible and indicates to real-time plugins that
 * they are no longer live, however plugins MUST NOT rely on prompt
 * deactivation. Note that connect_port() may be called before or
 * after a call to deactivate().
 *
 * Note that deactivation is not similar to pausing as the plugin
 * instance will be reinitialised when activate() is called to reuse it.
 * Hosts MUST NOT call deactivate() unless activate() was previously
 * called. */
void deactivate(LV2_Handle instance)
{
  DGLV2 *dglv2 = (DGLV2 *)instance;
  dglv2->dg->stop();
}

/** This is the counterpart to instantiate() (see above).  Once an instance
 * of a plugin has been finished with it can be deleted using this
 * function. The instance handle passed ceases to be valid after
 * this call.
 * 
 * If activate() was called for a plugin instance then a corresponding
 * call to deactivate() MUST be made before cleanup() is called.
 * Hosts MUST NOT call cleanup() unless instantiate() was previously
 * called. */
void cleanup(LV2_Handle instance)
{
  DGLV2 *dglv2 = (DGLV2 *)instance;
  delete dglv2->dg;
  delete dglv2->in;
  delete dglv2->out;
}

/** Function pointer that can be used to return additional instance data for
 * a plugin defined by some extenion (e.g. a struct containing additional
 * function pointers).
 *
 * The actual type and meaning of the returned object MUST be specified 
 * precisely by the extension if it defines any extra data.  If a particular
 * extension does not define extra instance data, this function MUST return
 * NULL for that extension's URI.  If a plugin does not support any
 * extensions that define extra instance data, this function pointer may be
 * set to NULL rather than providing an empty function.
 * 
 * The only parameter is the URI of the extension. The plugin MUST return
 * NULL if it does not support the extension, but hosts SHOULD NOT use this
 * as a discovery method (e.g. hosts should only call this function for
 * extensions known to be supported by the plugin from the data file).
 *
 * The host is never responsible for freeing the returned value.
 * 
 * NOTE: This function should return a struct (likely containing function
 * pointers) and NOT a direct function pointer.  Standard C and C++ do not
 * allow type casts from void* to a function pointer type.  To provide
 * additional functions a struct should be returned containing the extra
 * function pointers (which is valid standard code, and a much better idea
 * for extensibility anyway). */
const void* extension_data(const char *uri)
{
  printf("extension_data(%s)\n", uri);

  if(!strcmp(uri, PLUGIN_INSTANCE_URI)) return &dg_descriptor;
  if(!strcmp(uri, LV2_STATE__interface)) return &dg_persist;
  return NULL;
}

#ifdef __cplusplus
extern "C" {
#endif

static const LV2_Descriptor descriptor = {
	DRUMGIZMO_URI,
	instantiate,
	connect_port,
  activate,
	run,
  deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	switch (index) {
	case 0:
		return &descriptor;
	default:
		return NULL;
	}
}

#ifdef __cplusplus
}
#endif

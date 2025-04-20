
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <pthread.h>

#include <spa/param/latency-utils.h>
#include <spa/pod/builder.h>

#include <pipewire/filter.h>
#include <pipewire/pipewire.h>

#include <spa/control/control.h>
#include <spa/param/audio/format-utils.h>
#include <spa/param/props.h>
#include <spa/utils/defs.h>
#include <spa/utils/result.h>

#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>

#include "constants.h"
#include "lph.h"
//#include "lph_lv2.h"
#include "midifile.h"
#include "ports.h"

struct data;

//struct port {
//    struct data* data;
//};

struct instance {
    LilvInstance* instance;
    LV2_Worker_Schedule work_schedule;
    LV2_Feature work_schedule_feature;
    LV2_Options_Option options[6];
    LV2_Feature options_feature;

    const LV2_Feature* features[7];

    const LV2_Worker_Interface* work_iface;

    int32_t block_length;
    LV2_Atom empty_atom;
    struct pw_thread_loop* loop;
};

struct data {
    struct pw_thread_loop* loop;
    struct pw_filter* filter;
    int64_t clock_time;
    const LilvPlugin* lilvPlugin;
    struct instance instance;
    struct lph_ports lph_ports;
};

const LV2_Feature buf_size_features[3] = {
    { LV2_BUF_SIZE__powerOf2BlockLength, NULL },
    { LV2_BUF_SIZE__fixedBlockLength, NULL },
    { LV2_BUF_SIZE__boundedBlockLength, NULL },
};

static LV2_Worker_Status work_schedule(LV2_Worker_Schedule_Handle handle, uint32_t size, const void* data)
{
printf("\nInvoke work schedule");fflush(stdout);
            //struct instance *i = (struct instance*)handle;
            //spa_loop_invoke(i->loop, do_schedule, 1, data, size, false, i);
    return LV2_WORKER_SUCCESS;
}


static void on_process(void* userdata, struct spa_io_position* position)
{
    struct data* data = userdata;
    uint32_t n_samples = position->clock.duration;
    uint64_t frame = data->clock_time;
    float denom = (float)position->clock.rate.denom;
    data->clock_time += position->clock.duration;


    int n_ports = data->lph_ports.n_ports;

    for (int n = 0; n < n_ports; n++) {
        struct lph_port* port = &data->lph_ports.ports[n];
        port->methods.pre_run(data->instance.instance, port, frame, denom,(uint64_t) n_samples);
    }

    lilv_instance_run(data->instance.instance, n_samples);

    for (int n = 0; n < n_ports; n++) {
        struct lph_port* port = &data->lph_ports.ports[n];
        port->methods.post_run(data->instance.instance, port);
    }

}

static void on_destroy(void* data)
{
    pw_log_debug("lph on destroy");
}

static void on_state_changed(void* data, enum pw_filter_state old, enum pw_filter_state state, const char* error)
{
    pw_log_debug("lph on state changed");
}

static void on_io_changed(void* data, void* port_data, uint32_t id, void* area, uint32_t size)
{
    pw_log_debug("lph on io changed");
}

static void on_param_changed(void* data, void* port_data, uint32_t id, const struct spa_pod* param)
{
    pw_log_debug("lph on param changed");
}

static void on_add_buffer(void* data, void* port_data, struct pw_buffer* buffer)
{
    pw_log_debug("lph on add buffer");
}

static void on_remove_buffer(void* data, void* port_data, struct pw_buffer* buffer)
{
    pw_log_debug("lph on remove buffer");
}

static void on_drained(void* data)
{
    pw_log_debug("lph on drained");
}

static void on_command(void* data, const struct spa_command* command)
{
    pw_log_debug("lph on command");
}

static const struct pw_filter_events filter_events = {
    PW_VERSION_FILTER_EVENTS,
    .process = on_process,
    .destroy = on_destroy,
    .state_changed = on_state_changed,
    .io_changed = on_io_changed,
    .param_changed = on_param_changed,
    .add_buffer = on_add_buffer,
    .remove_buffer = on_remove_buffer,
    .drained = on_drained,
    .command = on_command,
};

static pthread_mutex_t lock;

void lph_init()
{
    int argc = 0;
    char*** argv = NULL;

    pw_init(&argc, argv);
    initConstants();
    pthread_mutex_init(&lock, NULL);
}

static void do_quit(void* userdata, int signal_number)
{
    struct data* data = userdata;
    pw_thread_loop_signal(data->loop, false);
}


void *lph_plugin(const char *pluginUri) {
    LilvNode* uri = lilv_new_uri(constants.world, pluginUri);
    LilvPlugin *plugin = NULL;
    if (uri != NULL) {
        const LilvPlugins* plugins = lilv_world_get_all_plugins(constants.world);
        plugin = lilv_plugins_get_by_uri(plugins, uri);
        lilv_node_free(uri);
        if (plugin == NULL) {
            printf("\ncan't load plugin %s", pluginUri);
        }
    } else {
        printf("\nerror in URI %s", pluginUri);
        return NULL;
    }

  return (void *)plugin;

}


void *lph_preset(const void *plugin, const char *presetUri) {
    //LilvNode* uri = lilv_new_uri(constants.world, presetUri);
    LilvPlugin *plug = (LilvPlugin *) plugin;

   LilvNode *selected = NULL;

  LilvNodes* presets = lilv_plugin_get_related(plug, constants.pset_Preset);
  LILV_FOREACH (nodes, i, presets) {
    const LilvNode* preset = lilv_nodes_get(presets, i);
    lilv_world_load_resource(constants.world, preset);
       if (!strcmp(presetUri,lilv_node_as_string(preset))) selected = preset;
  }
  lilv_nodes_free(presets);

  return (void *)selected;

}



void lph_thread_loop(void* plugin, void *preset, char* instanceName, int sampleRate)
{
    struct data data = {
        0,
    };
    char nodeName[100];

    if (instanceName)
        strcpy(nodeName, instanceName);
    else
        strcpy(nodeName, strdup(lilv_node_as_string(lilv_plugin_get_name(data.lilvPlugin))));

    pthread_mutex_lock(&lock);
    data.lilvPlugin = (LilvPlugin *)plugin;


    data.loop = pw_thread_loop_new(instanceName, NULL);
    data.instance.loop = data.loop;

    pw_loop_add_signal(pw_thread_loop_get_loop(data.loop), SIGINT, do_quit, &data);
    pw_loop_add_signal(pw_thread_loop_get_loop(data.loop), SIGTERM, do_quit, &data);

    pw_thread_loop_lock(data.loop);
    pw_thread_loop_start(data.loop);

    data.filter = pw_filter_new_simple(
        pw_thread_loop_get_loop(data.loop), nodeName,
        pw_properties_new(PW_KEY_MEDIA_TYPE, "Audio", PW_KEY_MEDIA_CATEGORY, "Filter", PW_KEY_MEDIA_ROLE, "DSP", NULL),
        &filter_events, &data);

    init_ports(data.lilvPlugin, &data.lph_ports);

    uint8_t buffer[1024];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    const struct spa_pod* params[1];

    params[0] = spa_process_latency_build(&b, SPA_PARAM_ProcessLatency,
        &SPA_PROCESS_LATENCY_INFO_INIT(.ns = 10 * SPA_NSEC_PER_MSEC));

    if (pw_filter_connect(data.filter, PW_FILTER_FLAG_RT_PROCESS, params, 1) < 0) {
        fprintf(stderr, "can't connect\n");
        return;
    }

    {

        uint32_t n_features = 0;
        static const int32_t min_block_length = 1;
        static const int32_t max_block_length = 8192;
        static const int32_t seq_size = 32768;
        float fsample_rate = (float)sampleRate;

        data.instance.block_length = 1024;
        data.instance.features[n_features++] = &constants.map_feature;
        data.instance.features[n_features++] = &constants.unmap_feature;

        data.instance.features[n_features++] = &buf_size_features[0];
        data.instance.features[n_features++] = &buf_size_features[1];
        data.instance.features[n_features++] = &buf_size_features[2];
        if (lilv_plugin_has_feature(data.lilvPlugin, constants.worker_schedule)) {
            data.instance.work_schedule.handle = data.instance.instance;
            data.instance.work_schedule.schedule_work = work_schedule;
            data.instance.work_schedule_feature.URI = LV2_WORKER__schedule;
            data.instance.work_schedule_feature.data = &data.instance.work_schedule;
            data.instance.features[n_features++] = &data.instance.work_schedule_feature;
        }

        data.instance.options[0] = (LV2_Options_Option) { LV2_OPTIONS_INSTANCE,
            0,
            constants_map(constants, LV2_BUF_SIZE__minBlockLength),
            sizeof(int32_t),
            constants.atom_Int,
            &min_block_length };
        data.instance.options[1] = (LV2_Options_Option) { LV2_OPTIONS_INSTANCE,
            0,
            constants_map(constants, LV2_BUF_SIZE__maxBlockLength),
            sizeof(int32_t),
            constants.atom_Int,
            &max_block_length };
        data.instance.options[2] = (LV2_Options_Option) {
            LV2_OPTIONS_INSTANCE, 0, constants_map(constants, LV2_BUF_SIZE__sequenceSize), sizeof(int32_t),
            constants.atom_Int, &seq_size
        };
        data.instance.options[3] = (LV2_Options_Option) { LV2_OPTIONS_INSTANCE,
            0,
            constants_map(constants, "http://lv2plug.in/ns/ext/buf-size#nominalBlockLength"),
            sizeof(int32_t),
            constants.atom_Int,
            &data.instance.block_length };
        data.instance.options[4] = (LV2_Options_Option) { LV2_OPTIONS_INSTANCE,
            0,
            constants_map(constants, LV2_PARAMETERS__sampleRate),
            sizeof(float),
            constants.atom_Float,
            &fsample_rate };
        data.instance.options[5] = (LV2_Options_Option) { LV2_OPTIONS_INSTANCE, 0, 0, 0, 0, NULL };

        data.instance.options_feature.URI = LV2_OPTIONS__options;
        data.instance.options_feature.data = data.instance.options;
        data.instance.features[n_features++] = &data.instance.options_feature;


	if (preset != NULL) {
		lilv_state_restore(preset, NULL, NULL, NULL, 0, data.instance.features);
	}

        data.instance.instance = lilv_plugin_instantiate(data.lilvPlugin, sampleRate, data.instance.features);
        if (data.instance.instance != NULL) {
            if (lilv_plugin_has_extension_data(data.lilvPlugin, constants.worker_iface)) {
                data.instance.work_iface = (const LV2_Worker_Interface*)lilv_instance_get_extension_data(
                    data.instance.instance, LV2_WORKER__interface);
            }
        }
    }

//return;
    // setup port, i.e. create pw ports and some permanent port buffer "connections"
    int n_ports = data.lph_ports.n_ports;
    for (int n = 0; n < n_ports; n++) {
        struct lph_port* port = &data.lph_ports.ports[n];
        port->methods.setup(data.instance.instance, port, data.filter);
    }

    lilv_instance_activate(data.instance.instance);
    pthread_mutex_unlock(&lock);

    pw_thread_loop_wait(data.loop);

    pthread_mutex_lock(&lock);
    lilv_instance_deactivate(data.instance.instance);
    pthread_mutex_unlock(&lock);

    pw_thread_loop_unlock(data.loop);
    pw_thread_loop_stop(data.loop);
    pw_filter_destroy(data.filter);
    pw_thread_loop_destroy(data.loop);
    pw_deinit();
}


/*


int
jalv_apply_preset(Jalv* jalv, const LilvNode* preset)
{
  lilv_state_free(jalv->preset);
  jalv->preset = lilv_state_new_from_world(
    jalv->world, jalv_mapper_urid_map(jalv->mapper), preset);
  if (jalv->preset) {
    jalv_apply_state(jalv, jalv->preset);
  }
  return 0;
}


  } else if (strcmp(cmd, "presets\n") == 0) {
    jalv_unload_presets(jalv);
    jalv_load_presets(jalv, jalv_print_preset, NULL);
  } else if (sscanf(cmd, "preset %1023[a-zA-Z0-9_:/-.#]\n", sym) == 1) {
    LilvNode* preset = lilv_new_uri(jalv->world, sym);
    lilv_world_load_resource(jalv->world, preset);
    jalv_apply_preset(jalv, preset);
    lilv_node_free(preset);
    print_controls(jalv, true, false);



int
jalv_load_presets(Jalv* jalv, PresetSink sink, void* data)
{
  LilvNodes* presets =
    lilv_plugin_get_related(jalv->plugin, jalv->nodes.pset_Preset);
  LILV_FOREACH (nodes, i, presets) {
    const LilvNode* preset = lilv_nodes_get(presets, i);
    lilv_world_load_resource(jalv->world, preset);
    if (!sink) {
      continue;
    }

    LilvNodes* labels =
      lilv_world_find_nodes(jalv->world, preset, jalv->nodes.rdfs_label, NULL);
    if (labels) {
      const LilvNode* label = lilv_nodes_get_first(labels);
      sink(jalv, preset, label, data);
      lilv_nodes_free(labels);
    } else {
      jalv_log(JALV_LOG_WARNING,
               "Preset <%s> has no rdfs:label\n",
               lilv_node_as_string(lilv_nodes_get(presets, i)));
    }
  }
  lilv_nodes_free(presets);

  return 0;
}

int
jalv_unload_presets(Jalv* jalv)
{
  LilvNodes* presets =
    lilv_plugin_get_related(jalv->plugin, jalv->nodes.pset_Preset);
  LILV_FOREACH (nodes, i, presets) {
    const LilvNode* preset = lilv_nodes_get(presets, i);
    lilv_world_unload_resource(jalv->world, preset);
  }
  lilv_nodes_free(presets);

  return 0;
}


static int
jalv_print_preset(Jalv*           ZIX_UNUSED(jalv),
                  const LilvNode* node,
                  const LilvNode* title,
                  void*           ZIX_UNUSED(data))
{
  printf("%s (%s)\n", lilv_node_as_string(node), lilv_node_as_string(title));
  return 0;
}


*/




/*


static void
jalv_create_controls(Jalv* jalv, bool writable)
{
  const LilvPlugin* plugin         = jalv->plugin;
  LilvWorld*        world          = jalv->world;
  LilvNode*         patch_writable = lilv_new_uri(world, LV2_PATCH__writable);
  LilvNode*         patch_readable = lilv_new_uri(world, LV2_PATCH__readable);

  LilvNodes* properties =
    lilv_world_find_nodes(world,
                          lilv_plugin_get_uri(plugin),
                          writable ? patch_writable : patch_readable,
                          NULL);
  LILV_FOREACH (nodes, p, properties) {
    const LilvNode* property = lilv_nodes_get(properties, p);
    ControlID*      record   = NULL;

    if (!writable &&
        lilv_world_ask(
          world, lilv_plugin_get_uri(plugin), patch_writable, property)) {
      // Find existing writable control
      for (size_t i = 0; i < jalv->controls.n_controls; ++i) {
        if (lilv_node_equals(jalv->controls.controls[i]->node, property)) {
          record              = jalv->controls.controls[i];
          record->is_readable = true;
          break;
        }
      }

      if (record) {
        continue;
      }
    }

    record = new_property_control(jalv->world,
                                  property,
                                  &jalv->nodes,
                                  jalv_mapper_urid_map(jalv->mapper),
                                  &jalv->forge);

    if (writable) {                                                                                                                               
      record->is_writable = true;                                                                                                                 
    } else {                                                                                                                                      
      record->is_readable = true;                                                                                                                 
    }                                                                                                                                             
                                                                                                                                                  
    if (record->value_type) {                                                                                                                     
      add_control(&jalv->controls, record);                                                                                                       
} else {
      jalv_log(JALV_LOG_WARNING,
               "Parameter <%s> has unknown value type, ignored\n",
               lilv_node_as_string(record->node));
      free(record);
    }
  }
  lilv_nodes_free(properties);

  lilv_node_free(patch_readable);
  lilv_node_free(patch_writable);
}


ControlID*                                                                                                                                        
new_property_control(LilvWorld* const       world,                                                                                                
                     const LilvNode*        property,
                     const JalvNodes* const nodes,
                     LV2_URID_Map* const    map,
                     LV2_Atom_Forge* const  forge)
{
  ControlID* id = (ControlID*)calloc(1, sizeof(ControlID));
  id->type      = PROPERTY;
  id->node      = lilv_node_duplicate(property);
  id->symbol    = lilv_world_get_symbol(world, property);
  id->forge     = forge;
  id->property  = map->map(map->handle, lilv_node_as_uri(property));                                                                              

  id->label = lilv_world_get(world, property, nodes->rdfs_label, NULL);                                                                           
  id->min   = lilv_world_get(world, property, nodes->lv2_minimum, NULL);                                                                          
  id->max   = lilv_world_get(world, property, nodes->lv2_maximum, NULL);                                                                          
  id->def   = lilv_world_get(world, property, nodes->lv2_default, NULL);                                                                          

  const char* const types[] = {LV2_ATOM__Int,
                               LV2_ATOM__Long,                                                                                                    
                               LV2_ATOM__Float,                                                                                                   
                               LV2_ATOM__Double,
                               LV2_ATOM__Bool,
                               LV2_ATOM__String,
                               LV2_ATOM__Path,                                                                                                    
NULL};

  for (const char* const* t = types; *t; ++t) {
    if (has_range(world, nodes, property, *t)) {
      id->value_type = map->map(map->handle, *t);
      break;
    }
  }

  id->is_toggle = (id->value_type == forge->Bool);
  id->is_integer =
    (id->value_type == forge->Int || id->value_type == forge->Long);

  if (!id->value_type) {
    jalv_log(JALV_LOG_WARNING,
             "Unknown value type for property <%s>\n",
             lilv_node_as_string(property));
  }

  return id;
}


*/




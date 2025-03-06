//#include <stdint.h>

extern void lph_init();
extern void *lph_plugin(const char *pluginUri);
extern void *lph_preset(const void *plugin, const char *presetUri);
extern void lph_thread_loop(void *plugin, void *preset, char *instanceName, int sampleRate);

/* future ?

extern void lph_init();
extern void *lph_thread_loop_create();
extern void lph_thread_loop_run(char *pluginUri, char *instanceName, int sampleRate);
extern void lph_thread_loop_stop(void *thread_loop);
extern void lph_thread_loop_destroy(void *thread_loop);

*/

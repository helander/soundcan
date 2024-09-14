
//#include <stdbool.h>
#include <stdlib.h>
//#include <string.h>



#include <lv2/core/lv2.h>
//#include <lv2/atom/atom.h>
//#include <lv2/atom/util.h>
//#include "lv2/atom/forge.h"
//#include <lv2/midi/midi.h>
//#include <lv2/urid/urid.h>
//#include "lv2/patch/patch.h"
//#include "lv2/options/options.h"
//#include "lv2/state/state.h"
//#include "lv2/worker/worker.h"


void lv2_instantiate();
void lv2_connect_port();
void lv2_activate();
void lv2_cleanup();
void lv2_run();

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    static const LV2_Descriptor sDescriptor = {
        .URI            = "http://helander.network/lv2soundfont/FOOBAR",
        .instantiate    = lv2_instantiate,
        .connect_port   = lv2_connect_port,
        .activate       = lv2_activate,
        .run            = lv2_run,
        .deactivate     = NULL,
        .cleanup        = lv2_cleanup,
        .extension_data = NULL,
    };

    return (index == 0) ? &sDescriptor : NULL;
}

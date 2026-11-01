#include "options.h"

#include "sklaffterm_adapter.h"
#include "sklaffterm_config.h"

void
sklaffterm_adapter_apply_config(void)
{
    set_option_value(
        Q_OPTION_DOWNLOAD_DIR,
        sklaffterm_config.download_dir
    );

    set_option_value(
        Q_OPTION_UPLOAD_DIR,
        sklaffterm_config.upload_dir
    );
}

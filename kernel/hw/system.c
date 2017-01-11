#include <inc/abstract.h>
#include <inc/console.h>

extern Driver ata_driver;

static System core = {
		.drivers = {
				&ata_driver,
		},
		.drivers_num = 1
};

int drivers_init(void)
{
	for(int i = 0; i < core.drivers_num; i++) {
		int res = core.drivers[i]->init();
		if(res) {
			kprintf("Error during init of driver [%s]: err_code = %d\n",
					core.drivers[i]->desc, res);
			return res;
		}
	}
	kprintf("Drivers inited successfully\n");
	return 0;
}

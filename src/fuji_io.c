
#ifdef BUILD_ADAM
#include "adam/io.h"
#endif /* BUILD_ADAM */
#ifdef BUILD_APPLE2
#include "apple2/io.h"
#endif /* BUILD_APPLE2 */
#ifdef BUILD_ATARI
#include "atari/io.h"
#endif /* BUILD_ATARI */
#ifdef BUILD_C64
#include "c64/io.h"
#endif /* BUILD_C64 */
#ifdef BUILD_PC8801
#include "pc8801/io.h"
#endif /* BUILD_PC8801 */
#ifdef BUILD_PC6001
#include "pc6001/io.h"
#endif /* BUILD_PC6001 */
#ifdef BUILD_RC2014
#include "rc2014/io.h"
#endif /* BUILD_RC2014 */

bool fuji_io_error(void) {return io_error();}
unsigned char fuji_io_status(void) {return io_status();}
void fuji_io_init(void) {io_init();}
unsigned char fuji_io_get_wifi_status(void) {return io_get_wifi_status();}
NetConfig* fuji_io_get_ssid(void) {return io_get_ssid();}
unsigned char fuji_io_scan_for_networks(void) {return io_scan_for_networks();}
SSIDInfo *fuji_io_get_scan_result(unsigned char n) {return io_get_scan_result(n);}
AdapterConfig *fuji_io_get_adapter_config(void) {return io_get_adapter_config();}
void fuji_io_set_ssid(NetConfig *nc) {io_set_ssid(nc);}
void fuji_io_get_device_slots(DeviceSlot *d) {io_get_device_slots(d);}
void fuji_io_get_host_slots(HostSlot *h) {io_get_host_slots(h);}
void fuji_io_put_host_slots(HostSlot *h) {io_put_host_slots(h);}
void fuji_io_put_device_slots(DeviceSlot *d) {io_put_device_slots(d);}
void fuji_io_mount_host_slot(unsigned char hs) {io_mount_host_slot(hs);}
void fuji_io_open_directory(unsigned char hs, char *p, char *f) {io_open_directory(hs, p, f);}
char *fuji_io_read_directory(unsigned char l, unsigned char a) {return io_read_directory(l, a);}
void fuji_io_close_directory(void) {io_close_directory();}
void fuji_io_set_directory_position(DirectoryPosition pos) {io_set_directory_position(pos);}
void fuji_io_set_device_filename(unsigned char ds, char* e) {io_set_device_filename(ds, e);}
char *fuji_io_get_device_filename(unsigned char ds) {return io_get_device_filename(ds);}
void fuji_io_mount_disk_image(unsigned char ds, unsigned char mode) {io_mount_disk_image(ds, mode);}
void fuji_io_set_boot_config(unsigned char toggle) {io_set_boot_config(toggle);}
void fuji_io_set_boot_mode(unsigned char mode) {
#ifdef BUILD_ATARI
    io_set_boot_mode(mode);
#endif
}
void fuji_io_umount_disk_image(unsigned char ds) {io_umount_disk_image(ds);}
void fuji_io_boot(void) {io_boot();}
void fuji_io_create_new(unsigned char selected_host_slot,unsigned char selected_device_slot,unsigned long selected_size,char *path) {io_create_new(selected_host_slot, selected_device_slot, selected_size, path);}
void fuji_io_build_directory(unsigned char ds, unsigned long numBlocks, char *v) {
#ifndef BUILD_APPLE2
    io_build_directory(ds, numBlocks, v);
#endif
}
bool fuji_io_get_device_enabled_status(unsigned char d) {return io_get_device_enabled_status(d);}
void fuji_io_update_devices_enabled(bool *e) {io_update_devices_enabled(e);}
void fuji_io_enable_device(unsigned char d) {io_enable_device(d);}
void fuji_io_disable_device(unsigned char d) {io_disable_device(d);}
void fuji_io_copy_file(unsigned char source_slot, unsigned char destination_slot) {io_copy_file(source_slot, destination_slot);}
unsigned char fuji_io_device_slot_to_device(unsigned char ds) {return io_device_slot_to_device(ds);}
void fuji_io_get_filename_for_device_slot(unsigned char slot, const char* filename) {
#ifdef BUILD_ATARI
    io_get_filename_for_device_slot(slot, filename);
#endif
}

unsigned char fuji_io_mount_all(void) {
#ifdef BUILD_ATARI
    return io_mount_all();
#endif
}
bool fuji_io_get_wifi_enabled(void) {return io_get_wifi_enabled();}


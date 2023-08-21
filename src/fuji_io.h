/**
 * FujiNet CONFIG for #Adam
 *
 * I/O Routines
 */

#ifndef FUJI_IO_H
#define FUJI_IO_H

#include <stdbool.h>
#include "../fuji_typedefs.h"

bool fuji_io_error(void);
unsigned char fuji_io_status(void);
void fuji_io_init(void);
unsigned char fuji_io_get_wifi_status(void);
NetConfig* fuji_io_get_ssid(void);
unsigned char fuji_io_scan_for_networks(void);
SSIDInfo *io_get_scan_result(unsigned char n);
AdapterConfig *io_get_adapter_config(void);
void fuji_io_set_ssid(NetConfig *nc);
void fuji_io_get_device_slots(DeviceSlot *d);
void fuji_io_get_host_slots(HostSlot *h);
void fuji_io_put_host_slots(HostSlot *h);
void fuji_io_put_device_slots(DeviceSlot *d);
void fuji_io_mount_host_slot(unsigned char hs);
void fuji_io_open_directory(unsigned char hs, char *p, char *f);
char *io_read_directory(unsigned char l, unsigned char a);
void fuji_io_close_directory(void);
void fuji_io_set_directory_position(DirectoryPosition pos);
void fuji_io_set_device_filename(unsigned char ds, char* e);
char *io_get_device_filename(unsigned char ds);
void fuji_io_mount_disk_image(unsigned char ds, unsigned char mode);
void fuji_io_set_boot_config(unsigned char toggle);
void fuji_io_set_boot_mode(unsigned char mode);
void fuji_io_umount_disk_image(unsigned char ds);
void fuji_io_boot(void);
void fuji_io_create_new(unsigned char selected_host_slot,unsigned char selected_device_slot,unsigned long selected_size,char *path);
void fuji_io_build_directory(unsigned char ds, unsigned long numBlocks, char *v);
bool fuji_io_get_device_enabled_status(unsigned char d);
void fuji_io_update_devices_enabled(bool *e);
void fuji_io_enable_device(unsigned char d);
void fuji_io_disable_device(unsigned char d);
void fuji_io_copy_file(unsigned char source_slot, unsigned char destination_slot);
unsigned char fuji_io_device_slot_to_device(unsigned char ds);
void fuji_io_get_filename_for_device_slot(unsigned char slot, const char* filename);
unsigned char fuji_io_mount_all(void);
bool fuji_io_get_wifi_enabled(void);

#endif /* FUJI_IO_H */

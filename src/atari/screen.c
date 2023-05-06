#ifdef BUILD_ATARI

/**
 * FujiNet Configuration Program: screen functions
 * The screen functions are a common set of graphics and text string functions to display information to the video output device.
 * These screen functions is comprised of two files; screen.c and screen.h.  Screen.c sets up the display dimensions, memory and
 * initializes the display as well as provide various text manipulations functions for proper display.  The screen.h file include
 * defines for various items such as keyboard codes, functions and screen memory addresses.
 *
 **/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <stdint.h>
#include <peekpoke.h>
#include "screen.h"
#include "io.h"
#include "globals.h"
#include "bar.h"
#include "input.h"

//unsigned char *video_ptr;  // a pointer to the memory address containing the screen contents
unsigned char *cursor_ptr; // a pointer to the current cursor position on the screen
char _visibleEntries;
extern bool copy_mode;
char text_empty[] = "Empty";
char fn[256];

// Patch to the character set to add things like the folder icon and the wifi-signal-strength bars.
// Each new character is 8-bytes.
//
unsigned char fontPatch[64] = {
    0    ,0    ,0    ,0    ,0    ,0    ,3    ,51,   // WIFI BARS 1
    0    ,0    ,3    ,3    ,51   ,51   ,51   ,51,   // WIFI BARS 2
    48   ,48   ,48   ,48   ,48   ,48   ,48   ,48,   // WIFI BARS 3
    0    ,120  ,135  ,255  ,255  ,255  ,255  ,0,    // CH_FOLDER
    0    ,204  ,205  ,195  ,255  ,255  ,255  ,0,    // CH_BINARY
    0    ,0    ,112  ,206  ,115  ,14    ,0   ,0,    // CH_LINK
    255  ,0    ,0    ,0    ,0    ,0    ,0    ,255,  // ?? where'd it go?
    0    ,0    ,255  ,0    ,255  ,0    ,255  ,0       // CH_OTHER
};

void set_cursor(unsigned char x, unsigned char y)
{
  gotoxy(x,y);
}

/**********************
 * Print ATASCII string to display memory.  Note: ATASCII is not a 1:1 mapping for ASCII.  It is a ven diagram with significant overlap.
 */
void put_char(char c)
{
  cputc(c);
}

void screen_append(char *s)
{
  cputs(s);
}

void screen_puts(unsigned char x, unsigned char y, char *s)
{
  cputsxy(x,y,s);
}

void font_init()
{
  // Copy ROM font
  memcpy((unsigned char *)FONT_MEMORY, (unsigned char *)0xE000, 1024);

  // And patch it.
  memcpy(FONT_MEMORY + 520, &fontPatch, sizeof(fontPatch));

  OS.chbas = FONT_MEMORY >> 8; // use the charset
}

void screen_mount_and_boot()
{
  screen_clear();
  bar_clear(false);
}

void screen_set_wifi(AdapterConfig *ac)
{
  char mactmp[3];
  unsigned char i = 0;
  unsigned char x = 13;

  screen_clear();
  bar_clear(false);
  screen_puts(0, 0, "WELCOME TO #FUJINET!");
  screen_puts(0, 22, "SCANNING NETWORKS...");
  screen_puts(0, 2, "MAC Address:");
  screen_puts(15, 2, ":");
  screen_puts(18, 2, ":");
  screen_puts(21, 2, ":");
  screen_puts(24, 2, ":");
  screen_puts(27, 2, ":");

  // screen_set_wifi_display_mac_address(ac);
  for (i = 0; i < 6; i++)
  {
    itoa(ac->macAddress[i], mactmp, 16);
    screen_puts(x, 2, mactmp);
    x += 3;
  }
}

void screen_set_wifi_print_rssi(SSIDInfo *s, unsigned char i)
{
  char out[4] = {0x20, 0x20, 0x20, 0x00};

  if (s->rssi > -40)
  {
    out[0] = 0x01;
    out[1] = 0x02;
    out[2] = 0x03;
  }
  else if (s->rssi > -60)
  {
    out[0] = 0x01;
    out[1] = 0x02;
  }
  else
  {
    out[0] = 0x01;
  }

  screen_puts(35, i + NETWORKS_START_Y, out);
}

void screen_set_wifi_display_ssid(char n, SSIDInfo *s)
{
  screen_puts(2, n + NETWORKS_START_Y, (char *)s->ssid);
  screen_set_wifi_print_rssi(s, n);
}

void screen_set_wifi_select_network(unsigned char nn)
{
  screen_clear_line(numNetworks + NETWORKS_START_Y);
  screen_puts(2, NETWORKS_START_Y + numNetworks, "<Enter a specific SSID>");

  screen_puts(0, 22, " SELECT NET, S SKIP "
                     "   ESC TO RE-SCAN   ");

  bar_show(NETWORKS_START_Y);
}

void screen_set_wifi_custom(void)
{
  screen_clear_line(20);
  screen_puts(0, 22, " ENTER NETWORK NAME "
                     "  AND PRESS return  ");
}

void screen_set_wifi_password(void)
{
  screen_clear_line(22);
  screen_puts(3, 22, "   ENTER PASSWORD");
}

void screen_print_ip(unsigned char x, unsigned char y, unsigned char *buf)
{
  unsigned char i = 0;
  unsigned char tmp[4];

  set_cursor(x, y);
  for (i = 0; i < 4; i++)
  {
    itoa(buf[i], tmp, 10);
    screen_append(tmp);
    if (i == 3)
      break;
    screen_append(".");
  }
}

/**
 * Convert hex to a string and print as a MAC address at position x, y
 */
/**
 * Convert hex to a string and print as a MAC address at position x, y
 */
void screen_print_mac(unsigned char x, unsigned char y, unsigned char *buf)
{
  unsigned char mactmp[18];

  sprintf(mactmp, "%02X:%02X:%02X:%02X:%02X:%02X", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
  screen_puts(x, y, mactmp);
}

/**
 * Convert hex to a string.  Special hex output of numbers under 16, e.g. 9 -> 09, 10 -> 0A
 */
void itoa_hex(unsigned char val, char *buf)
{

  if (val < 16)
  {
    *(buf++) = '0';
  }
  itoa(val, buf, 16);
}

/*
 * Display the 'info' screen
 */
void screen_show_info(int printerEnabled, AdapterConfig *ac)
{
  unsigned char i;
  screen_clear();
  bar_clear(false);

  screen_puts(0, 0, HORIZONTAL_LINE);
  screen_puts(23, 0 , " FUJINET CONFIG ");
  screen_puts(7, 17,
              CH_KEY_LABEL_L CH_INV_C CH_KEY_LABEL_R "RECONNECT " CH_KEY_LABEL_L CH_INV_S CH_KEY_LABEL_R "CHANGE SSID");
  screen_puts(9, 19, "Any other key to return");
  screen_puts(5, 7, "      SSID:");
  screen_puts(5, 8, "  Hostname:");
  screen_puts(5, 9, "IP Address:");
  screen_puts(5, 10, "   Gateway:");
  screen_puts(5, 11, "       DNS:");
  screen_puts(5, 12, "   Netmask:");
  screen_puts(5, 13, "       MAC:");
  screen_puts(5, 14, "     BSSID:");
  screen_puts(5, 15, "   Version:");

  screen_puts(17, 7, ac->ssid);
  screen_puts(17, 8, ac->hostname);
  screen_print_ip(17, 9, ac->localIP);
  screen_print_ip(17, 10, ac->gateway);
  screen_print_ip(17, 11, ac->dnsIP);
  screen_print_ip(17, 12, ac->netmask);
  screen_print_mac(17, 13, ac->macAddress);
  screen_print_mac(17, 14, ac->bssid);
  screen_puts(17, 15, ac->fn_version);
}

void screen_select_slot(char *e)
{
  unsigned int *s;
  unsigned char d[40];


  screen_clear();

  screen_puts(0, 22,
              CH_KEY_1TO8 "Slot" CH_KEY_RETURN "Select" CH_KEY_LABEL_L CH_INV_E CH_KEY_LABEL_R "ject" CH_KEY_ESC "Abort");

  screen_puts(0, 0, "MOUNT TO DRIVE SLOT");

  // Show file details if it's an existing file only.
  if ( create == false )
  {
    // Modified time 
    sprintf(d, "%8s %04u-%02u-%02u %02u:%02u:%02u", "MTIME:", (*e++) + 1970, *e++, *e++, *e++, *e++, *e++);
    screen_puts(0, DEVICES_END_MOUNT_Y + 5, d);

    // File size
    // only 2 bytes, so max size is 65535.. don't show for now until SIO method is changed to return more.
    // Result is unreliable since if the file was < 65535 bytes it will be ok, but if it was more we don't
    // know how to interpret the 2 bytes we have available to us.
    //s = (unsigned int *)e;
    //sprintf(d, "%8s %u bytes", "SIZE:", *s); 
    //sprintf(d, "%8s %u K", "SIZE:", *s >> 10); 
    //screen_puts(0, DEVICES_END_MOUNT_Y + 4, d);

    // Skip next 4 bytes to get to the filename (2 for the size, 2 for flags we don't care about)
    e += 4;

    // Filename
    screen_puts(3, DEVICES_END_MOUNT_Y + 2, "FILE:");
    screen_puts(9, DEVICES_END_MOUNT_Y + 2, e);
  }

  screen_hosts_and_devices_device_slots(DEVICES_START_MOUNT_Y, &deviceSlots, &deviceEnabled);

  bar_show(DEVICES_START_MOUNT_Y);
}

void screen_select_slot_mode(void)
{
  screen_clear_line(21);
  screen_puts(0, 22,
              CH_KEY_RETURN "Read Only" CH_KEY_LABEL_L CH_INV_W CH_KEY_LABEL_R "Read/Write" CH_KEY_ESC "Abort");
}

void screen_select_slot_choose(void)
{
}

void screen_select_slot_eject(unsigned char ds)
{
}

void screen_select_slot_build_eos_directory(void)
{
}

void screen_select_slot_build_eos_directory_label(void)
{
}

void screen_select_slot_build_eos_directory_creating(void)
{
}

void screen_select_file(void)
{
  screen_clear();
  bar_clear(false);

  screen_puts(0, 2, HORIZONTAL_LINE);
  screen_puts(0, 21, HORIZONTAL_LINE);

  screen_puts(28, 22, CH_KEY_NAV);
  screen_puts(28, 23, "|Move");

  screen_puts(33, 22, "|" CH_KEY_RETURN);
  screen_puts(33, 23, "|Select");

  if (copy_mode == false)
  {
    screen_puts(0, 22, CH_INV_F "ilter:");
    screen_puts(0, 23,
        CH_KEY_C "opy " CH_INV_N "ew " CH_KEY_OPTION " Boot " CH_KEY_ESC " Back");
  }
  else
  {
    screen_puts(0, 22, CH_KEY_C "Complete Copy");
    screen_puts(0, 23,
      CH_INV_C "opy " CH_INV_N "ew " CH_KEY_RETURN "Choose " CH_KEY_OPTION "Boot " CH_KEY_ESC "ape");
  }
}

void screen_select_file_display(char *p, char *f)
{
  unsigned char i;
  screen_clear_line(0);
  screen_clear_line(1);
  screen_puts(0, 0, CH_LINK);
  screen_puts(2, 0, selected_host_name);
  screen_puts(2 + strlen(selected_host_name),0, p);

  // Path - the path can wrap to line 4 (maybe 5?) so clear both to be safe.
  //screen_puts(0, 1, "Path:");
  //screen_puts(5, 1, p);

  // Filter
  //screen_puts(8, 22, CH_KEY_F "ilter:");
  screen_puts(7, 22, f);

  // Clear out the file area
  for (i = FILES_START_Y; i < FILES_START_Y + ENTRIES_PER_PAGE; i++)
  {
    screen_clear_line(i);
  }

  // clear Prev/next page ticks
  screen_puts(34,2,"\x12\x12\x12\x12");
}

void screen_select_file_display_long_filename(char *e)
{
  screen_puts(0, 24, e);
}

void screen_select_file_clear_long_filename(void)
{
  screen_clear_line(24);
  screen_clear_line(25);
}

void screen_select_file_filter(void)
{
  // No need to display anything additional, we're already showing the filter on the screen.
}

void screen_select_file_next(void)
{
  if (pos == 0)  screen_puts(34,2,"\x12  "); // clear previous page
  if (dir_eof == false)
  {
    screen_puts(35,2,"Pg\x7F"); // next page
  }
}

void screen_select_file_prev(void)
{
  if (dir_eof  == true) screen_puts(37,2,"\x12"); // clear next page
  if (pos > 0)
  {
    screen_puts(34,2,"\x7EPg"); // previous page
  }
}

void screen_select_file_display_entry(unsigned char y, char *e, unsigned entryType)
{
  if (entryType > 0)
  {
    if (entryType == 1) screen_puts(0,FILES_START_Y+y,CH_FOLDER);
    else if (entryType == 2) screen_puts(0,FILES_START_Y+y,CH_BINARY);
    else if (entryType == 3) screen_puts(0,FILES_START_Y+y,CH_LINK);
    else if (entryType == 4) screen_puts(0,FILES_START_Y+y,CH_MENU);
    else screen_puts(0,FILES_START_Y+y,CH_OTHER);
    screen_puts(2, FILES_START_Y + y, e);
  }
  else 
  {
    screen_puts(0, FILES_START_Y + y, e);
  }
}

void screen_select_file_choose(char visibleEntries)
{
  bar_show(FILES_START_Y);
  _visibleEntries = visibleEntries;
}

void screen_select_file_new_type(void)
{
  // Not used on Atari
}

void screen_select_file_new_size(unsigned char k)
{
  screen_clear_line(20);
  screen_clear_line(21);

  screen_puts(0, 20, "Size?" CH_KEY_LABEL_L CH_INV_1 CH_KEY_LABEL_R "90K  " CH_KEY_LABEL_L CH_INV_2 CH_KEY_LABEL_R "130K  " CH_KEY_LABEL_L CH_INV_3 CH_KEY_LABEL_R "180K  " CH_KEY_LABEL_L CH_INV_4 CH_KEY_LABEL_R "360K  ");
  screen_puts(5, 21,
              CH_KEY_LABEL_L CH_INV_5 CH_KEY_LABEL_R "720K " CH_KEY_LABEL_L CH_INV_6 CH_KEY_LABEL_R "1440K " CH_KEY_LABEL_L CH_INV_7 CH_KEY_LABEL_R "Custom ?");
}

void screen_select_file_new_custom(void)
{
  screen_clear_line(20);
  screen_clear_line(21);

  screen_puts(0, 20, "# Sectors?");
  screen_puts(0, 21, "Sector Size (128/256/512)?");
}

void screen_select_file_new_name(void)
{
  screen_clear_line(20);
  screen_clear_line(21);
  screen_puts(0, 20, "Enter name of new disk image file");
}

void screen_select_file_new_creating(void)
{
  screen_clear();
  screen_puts(3, 0, "Creating File");
}

void screen_clear_line(unsigned char y)
{
  cclearxy(0,y,40);
}

void screen_error(const char *msg)
{
  screen_clear_line(1);
  bar_show(1);
  bar_set_color(COLOR_SETTING_FAILED);
  screen_puts(0, 1, msg);
  while (!kbhit()) {}
  bar_set_color(COLOR_SETTING_SUCCESSFUL);
}

void screen_hosts_and_devices(HostSlot *h, DeviceSlot *d, unsigned char *e)
{
  unsigned char retry = 5;
  unsigned char i;
  char temp[10];


  screen_clear();
  bar_clear(false);

  screen_puts(0, 0, HORIZONTAL_LINE);
  screen_puts(23, 0, " TNFS HOST LIST ");
  screen_puts(0, 11, HORIZONTAL_LINE);
  screen_puts(26, 11, " DRIVE SLOTS ");

  while (retry > 0)
  {
    io_get_host_slots(&hostSlots[0]);

    if (io_error())
      retry--;
    else
      break;
  }

  if (io_error())
  {
    screen_error("ERROR READING HOST SLOTS");
    while (!kbhit())
    {
    }
    cold_start();
  }

  retry = 5;

  screen_hosts_and_devices_host_slots(&hostSlots[0]);

  while (retry > 0)
  {
    io_get_device_slots(&deviceSlots[0]);

    if (io_error())
      retry--;
    else
      break;
  }

  if (io_error())
  {
    screen_error("ERROR READING DEVICE SLOTS");
    // die();
    while (!kbhit())
    {
    }
    cold_start();
  }

  screen_hosts_and_devices_device_slots(DEVICES_START_Y, &deviceSlots[0], "");
}

void screen_clear()
{
  clrscr();
}

// Show the keys that are applicable when we are on the Hosts portion of the screen.
void screen_hosts_and_devices_hosts(void)
{
  screen_clear_line(22);
  screen_clear_line(23);
  screen_puts(0, 22,
              CH_KEY_1TO8 "Slot" CH_KEY_LABEL_L CH_INV_E CH_KEY_LABEL_R "dit Slot" CH_KEY_RETURN "Select Files");
  screen_puts(2, 23,
              CH_KEY_LABEL_L CH_INV_C CH_KEY_LABEL_R "onfig" CH_KEY_TAB "Drive Slots" CH_KEY_OPTION "Boot");

  // bar_show(2);
  bar_show(selected_host_slot + HOSTS_START_Y);
}

// Show the keys that are applicable when we are on the Devices portion of the screen.
void screen_hosts_and_devices_devices(void)
{
  screen_clear_line(22);
  screen_clear_line(23);

  screen_clear_line(11);
  screen_puts(0, 11, HORIZONTAL_LINE);
  screen_puts(26, 11, " DRIVE SLOTS ");

  screen_puts(3, 22,
              CH_KEY_1TO8 "Slot" CH_KEY_LABEL_L CH_INV_E CH_KEY_LABEL_R "ject" CH_KEY_LABEL_L CH_INV_C CH_INV_L CH_INV_E CH_INV_A CH_INV_R CH_KEY_LABEL_R "All Slots");
  screen_puts(3, 23,
              CH_KEY_TAB "Hosts" CH_KEY_LABEL_L CH_INV_R CH_KEY_LABEL_R "ead " CH_KEY_LABEL_L CH_INV_W CH_KEY_LABEL_R "rite" CH_KEY_LABEL_L CH_INV_C CH_KEY_LABEL_R "onfig");
  bar_show(selected_device_slot + DEVICES_START_Y);
}

void screen_hosts_and_devices_host_slots(HostSlot *h)
{
  unsigned char slotNum;

  for (slotNum = 0; slotNum < NUM_HOST_SLOTS; slotNum++)
  {
    set_cursor(2, slotNum + HOSTS_START_Y);
    put_char(slotNum + '1');
    set_cursor(5,  slotNum + HOSTS_START_Y);
    

    screen_append((hostSlots[slotNum][0] != 0x00) ? (char *)hostSlots[slotNum] : text_empty);
  }
}

// Since 'deviceSlots' is a global, do we need to access the input parameter at all?
// Maybe globals.h wasn't supposed in be part of screen? I needed it for something..
void screen_hosts_and_devices_device_slots(unsigned char y, DeviceSlot *dslot, unsigned char *e)
{
  unsigned char slotNum;
  unsigned char dinfo[6];

  // Get full filename for device slot 8
  if (deviceSlots[7].file[0] != 0x00)
    io_get_filename_for_device_slot(7, fn);

  // Display device slots
  for (slotNum = 0; slotNum < NUM_DEVICE_SLOTS; slotNum++)
  {
    dinfo[1] = 0x20;
    dinfo[2] = (slotNum == 7 && strstr(fn, ".cas") != NULL) ? 'C' : (0x31 + slotNum);
    dinfo[4] = 0x20;
    dinfo[5] = 0x00;

    if (deviceSlots[slotNum].file[0] != 0x00)
    {
      dinfo[0] = deviceSlots[slotNum].hostSlot + 0x31;
      dinfo[3] = (deviceSlots[slotNum].mode == 0x02 ? 'W' : 'R');
    }
    else
    {
      dinfo[0] = 0x20;
      dinfo[3] = 0x20;
    }

    screen_puts(0, slotNum + y, dinfo);

    screen_append(deviceSlots[slotNum].file[0] != 0x00 ? (char *)deviceSlots[slotNum].file : text_empty);
  }
}

void screen_hosts_and_devices_devices_clear_all(void)
{
  screen_clear_line(11);
  screen_puts(0, 11, "EJECTING ALL.. WAIT");
}

void screen_hosts_and_devices_clear_host_slot(unsigned char i)
{
  // i comes in as the place in the array for this host slot. To get the corresponding position on the screen, add HOSTS_START_Y
  screen_clear_line(i + HOSTS_START_Y);
}

void screen_hosts_and_devices_edit_host_slot(unsigned char i)
{
  char tmp[2] = {0, 0};
  int newloc = i + HOSTS_START_Y;

  screen_clear_line(newloc);
  tmp[0] = newloc - HOSTS_START_Y + 0x31;
  screen_puts(2, newloc, tmp);
}

void screen_hosts_and_devices_eject(unsigned char ds)
{
  char tmp[2] = {0, 0};
  unsigned char y;

  tmp[0] = ds + '1';

  if ( mounting ) 
  {
    y = DEVICES_START_MOUNT_Y;
  }
  else
  {
    y = DEVICES_START_Y;
  }
  bar_show(y + ds);
  screen_clear_line(y + ds);
  screen_puts(2, y + ds, tmp);
  screen_puts(5, y + ds, text_empty);
}

void screen_hosts_and_devices_host_slot_empty(unsigned char hs)
{
  // When this gets called it seems like the cursor is right where we want it to be.
  // so no need to move to a position first.
  screen_append(text_empty);
}

void screen_hosts_and_devices_long_filename(char *f)
{
  // show_line_nums();
}

void screen_init(void)
{
  font_init();
  bar_setup_regs();
  bar_clear(false);

  selected_host_slot = 0;
  selected_device_slot = 0;
}

void screen_destination_host_slot(char *h, char *p)
{
  screen_clear();
  screen_puts(0, 22,
              CH_KEY_1TO8 "Slot" CH_KEY_RETURN "Select" CH_KEY_ESC "Abort");

  screen_puts(0, 18, h);
  screen_puts(0, 19, p);

  screen_puts(0, 0, "COPY TO HOST SLOT");
  bar_show(HOSTS_START_Y);
}

void screen_destination_host_slot_choose(void)
{
  // show_line_nums();
}

void screen_perform_copy(char *sh, char *p, char *dh, char *dp)
{
  // show_line_nums();
  screen_clear();
  bar_clear(false);
  screen_puts(0, 0, "COPYING, PLEASE WAIT");
}

void screen_connect_wifi(NetConfig *nc)
{
  screen_clear();
  bar_clear(false);

  screen_puts(0, 0, "WELCOME TO #FUJINET! CONNECTING TO NET");
  screen_puts(2, 3, nc->ssid);
  bar_show(3);
}

int _screen_input(unsigned char x, unsigned char y, char *s, unsigned char maxlen)
{
  unsigned char k, o, start, cpos;

  cpos = x;
  o = strlen(s);                // assign to local var the size of s which contains the current string
  set_cursor(x, y);             // move the cusor to coordinates x,y
  screen_append(s);             // call screen_append function and pass by value (a copy) the contents of s

  cursor(1);
  // Start capturing the keyboard input into local var k
  do
  {
    k = cgetc(); // Capture keyboard input into k

    if (k == KCODE_ESCAPE) // KCODE_ESCAPE is the ATASCI code for the escape key which is commonly used to cancel.
    {
      cursor(0);
      return -1;
    }

    if (k == KCODE_BACKSP) // KCODE_BACKSP is the ATASCI backspace key.  This if clause test for backspace and updates cursor_ptr to the remainder of contents upto the last backspace
    {
      if (wherex() > x) // execute only if cursor_ptr is greater than input_start_ptr
      {
        s[--o] = 0; 
        cputcxy(wherex()-1,wherey(),' ');
        gotox(wherex()-1);
      }
    }
    else if ((k > 0x1F) && (k < 0x80)) // Display printable ascii to screen
    {
      if (o < maxlen - 1)
      {
        put_char(k);
        s[o++] = k;
      }
    }
  } while (k != KCODE_RETURN); // Continue to capture keyboard input until return (0x9B)
  cursor(0);
}


#ifdef DEBUG
// Debugging function to show line #'s, used to test if the Y coordinate calculations are working.
void show_line_nums(void)
{
  screen_puts(0, 0, "0");
  screen_puts(0, 1, "1");
  screen_puts(0, 2, "2");
  screen_puts(0, 3, "3");
  screen_puts(0, 4, "4");
  screen_puts(0, 5, "5");
  screen_puts(0, 6, "6");
  screen_puts(0, 7, "7");
  screen_puts(0, 8, "8");
  screen_puts(0, 9, "9");
  screen_puts(0, 10, "10");
  screen_puts(0, 11, "11");
  screen_puts(0, 12, "12");
  screen_puts(0, 13, "13");
  screen_puts(0, 14, "14");
  screen_puts(0, 15, "15");
  screen_puts(0, 16, "16");
  screen_puts(0, 17, "17");
  screen_puts(0, 18, "18");
  screen_puts(0, 19, "19");
  screen_puts(0, 20, "20");
  screen_puts(0, 21, "21");
  screen_puts(0, 22, "22");
  screen_puts(0, 23, "23");
  screen_puts(0, 24, "24");
  screen_puts(0, 25, "25");

  while (!kbhit())
  {
  }
  cgetc();
  screen_clear();
}

void screen_debug(char *message)
{
  screen_clear_line(24);
  screen_puts(0, 24, message);
}
#endif //DEBUG
#endif

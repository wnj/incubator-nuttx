/****************************************************************************
 * boards/arm/kinetis/teensy-36/src/teensy-36.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_ARM_KINETIS_TEENSY_36_SRC_TEENSY_36_H
#define __BOARDS_ARM_KINETIS_TEENSY_36_SRC_TEENSY_36_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>
#include <stdint.h>
#include <arch/kinetis/chip.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Application Configuration ************************************************/

/* Assume we have everything */

#define HAVE_PROC        1
#define HAVE_MMCSD       1
#define HAVE_AUTOMOUNTER 1
#define HAVE_SPI         1
#define HAVE_USBDEV      1

#if defined(CONFIG_KINETIS_RTC)
#define HAVE_RTC_DRIVER  1
#endif

/* Automount procfs */

#if !defined(CONFIG_FS_PROCFS)
#  undef HAVE_PROC
#endif

#if defined(HAVE_PROC) && defined(CONFIG_DISABLE_MOUNTPOINT)
#  warning Mountpoints disabled.  No procfs support
#  undef HAVE_PROC
#endif

#if defined(CONFIG_NSH_PROC_MOUNTPOINT)
#  define PROCFS_MOUNTPOUNT CONFIG_NSH_PROC_MOUNTPOINT
#else
#  define PROCFS_MOUNTPOUNT "/proc"
#endif

/* SD card support */

#define MMCSD_SLOTNO 0

/* Can't support MMC/SD features if mountpoints are disabled or if SDHC
 * support is not enabled.
 */

#if defined(CONFIG_DISABLE_MOUNTPOINT) || !defined(CONFIG_KINETIS_SDHC)
#  undef HAVE_MMCSD
#endif

#ifdef HAVE_MMCSD
#  if defined(CONFIG_NSH_MMCSDSLOTNO) && CONFIG_NSH_MMCSDSLOTNO != 0
#    error Only one MMC/SD slot, slot 0
#  endif

#  ifdef CONFIG_NSH_MMCSDMINOR
#    define MMSCD_MINOR CONFIG_NSH_MMCSDMINOR
#  else
#    define MMSCD_MINOR 0
#  endif

/* We expect to receive GPIO interrupts for card insertion events */

#  ifndef CONFIG_KINETIS_GPIOIRQ
#    error "CONFIG_KINETIS_GPIOIRQ required for card detect interrupt"
#  endif

#  ifndef CONFIG_KINETIS_PORTDINTS
#    error "CONFIG_KINETIS_PORTDINTS required for card detect interrupt"
#  endif

#endif

/* Automounter */

#if !defined(CONFIG_FS_AUTOMOUNTER) || !defined(HAVE_MMCSD)
#  undef HAVE_AUTOMOUNTER
#  undef CONFIG_FRDMK66F_SDHC_AUTOMOUNT
#endif

#ifndef CONFIG_FRDMK66F_SDHC_AUTOMOUNT
#  undef HAVE_AUTOMOUNTER
#endif

/* Automounter defaults */

#ifdef HAVE_AUTOMOUNTER

#  ifndef CONFIG_FRDMK66F_SDHC_AUTOMOUNT_FSTYPE
#    define CONFIG_FRDMK66F_SDHC_AUTOMOUNT_FSTYPE "vfat"
#  endif

#  ifndef CONFIG_FRDMK66F_SDHC_AUTOMOUNT_BLKDEV
#    define CONFIG_FRDMK66F_SDHC_AUTOMOUNT_BLKDEV "/dev/mmcds0"
#  endif

#  ifndef CONFIG_FRDMK66F_SDHC_AUTOMOUNT_MOUNTPOINT
#    define CONFIG_FRDMK66F_SDHC_AUTOMOUNT_MOUNTPOINT "/mnt/sdcard"
#  endif

#  ifndef CONFIG_FRDMK66F_SDHC_AUTOMOUNT_DDELAY
#    define CONFIG_FRDMK66F_SDHC_AUTOMOUNT_DDELAY 1000
#  endif

#  ifndef CONFIG_FRDMK66F_SDHC_AUTOMOUNT_UDELAY
#    define CONFIG_FRDMK66F_SDHC_AUTOMOUNT_UDELAY 2000
#  endif
#endif /* HAVE_AUTOMOUNTER */

/* Can't support USB features if USB is not enabled */

#ifndef CONFIG_USBDEV
#  undef HAVE_USBDEV
#endif

/* How many SPI modules does this chip support? The LM3S6918 supports 2 SPI
 * modules (others may support more -- in such case, the following must be
 * expanded).
 */

#if KINETIS_NSPI < 1
#  undef CONFIG_KINETIS_SPI0
#  undef CONFIG_KINETIS_SPI1
#  undef CONFIG_KINETIS_SPI2
#elif KINETIS_NSPI < 2
#  undef CONFIG_KINETIS_SPI1
#  undef CONFIG_KINETIS_SPI2
#elif KINETIS_NSPI < 3
#  undef CONFIG_KINETIS_SPI2
#endif

#if !defined(CONFIG_KINETIS_SPI0) && !defined(CONFIG_KINETIS_SPI1) && \
            !defined(CONFIG_KINETIS_SPI3)
#  undef HAVE_SPI
#endif

/* FREEDOM-K66F GPIOs *******************************************************/

/* A micro Secure Digital (SD) card slot is available on the Teensy 3.6
 * connected to the SD Host Controller (SDHC) signals of the MCU.
 * This slot will accept micro format SD memory cards.
 * The SD card detect pin (PTD10) is an open switch that shorts with VDD when
 * card is inserted.
 *
 *   ------------ ------------- --------
 *    SD Card Slot Board Signal  K66F Pin
 *    ------------ ------------- --------
 *    DAT0         SDHC0_D0      PTE1
 *    DAT1         SDHC0_D1      PTE0
 *    DAT2         SDHC0_D2      PTE5
 *    CD/DAT3      SDHC0_D3      PTE4
 *    CMD          SDHC0_CMD     PTE3
 *    CLK          SDHC0_DCLK    PTE2
 *    SWITCH       D_CARD_DETECT PTD10
 *    ------------ ------------- --------
 *
 * There is no Write Protect pin available to the K66F.
 */

#define GPIO_SD_CARDDETECT (GPIO_PULLDOWN | PIN_INT_BOTH | PIN_PORTD | PIN10)

/* An RGB LED is connected through GPIO as shown below:
 *
 *   LED    K66
 *   ------ -------------------------------------------------------
 *   RED    PTC5
 */

#define GPIO_LED         (GPIO_LOWDRIVE | GPIO_OUTPUT_ONE | PIN_PORTC | PIN5)

/* SPI1 on J6 */

#define PIN_CE           (GPIO_LOWDRIVE | GPIO_OUTPUT_ONE | PIN_PORTB | PIN20)
#define PIN_SPI1_PCS0    (GPIO_LOWDRIVE | GPIO_OUTPUT_ONE | PIN_PORTD | PIN4)

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Functions Definitions
 ****************************************************************************/

/****************************************************************************
 * Name: k66_spidev_initialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the FREEDOM-K66F
 *   board.
 *
 ****************************************************************************/

void weak_function k66_spidev_initialize(void);

/****************************************************************************
 * Name: k66_rtc_initialize
 *
 * Description:
 *   Called to initialize the RTC FREEDOM-K66F board.
 *
 ****************************************************************************/

#if defined(HAVE_RTC_DRIVER)
int k66_rtc_initialize(void);
#else
#  define k66_rtc_initialize() (OK)
#endif
/****************************************************************************
 * Name: k66_usbinitialize
 *
 * Description:
 *   Called to setup USB-related GPIO pins for the FREEDOM-K66F board.
 *
 ****************************************************************************/

void weak_function k66_usbinitialize(void);

/****************************************************************************
 * Name: k66_bringup
 *
 * Description:
 *   Bring up board features
 *
 ****************************************************************************/

#if defined(CONFIG_BOARDCTL) || defined(CONFIG_BOARD_LATE_INITIALIZE)
int k66_bringup(void);
#endif

/****************************************************************************
 * Name: k66_sdhc_initialize
 *
 * Description:
 *   Inititialize the SDHC SD card slot
 *
 ****************************************************************************/

#ifdef HAVE_MMCSD
int k66_sdhc_initialize(void);
#else
#  define k66_sdhc_initialize() (OK)
#endif

/****************************************************************************
 * Name: k66_cardinserted
 *
 * Description:
 *   Check if a card is inserted into the SDHC slot
 *
 ****************************************************************************/

#ifdef HAVE_AUTOMOUNTER
bool k66_cardinserted(void);
#else
#  define k66_cardinserted() (false)
#endif

/****************************************************************************
 * Name: k66_writeprotected
 *
 * Description:
 *   Check if the card in the MMC/SD slot is write protected
 *
 ****************************************************************************/

#ifdef HAVE_AUTOMOUNTER
bool k66_writeprotected(void);
#else
#  define k66_writeprotected() (false)
#endif

/****************************************************************************
 * Name:  k66_automount_initialize
 *
 * Description:
 *   Configure auto-mounter for the configured SDHC slot
 *
 * Input Parameters:
 *   None
 *
 *  Returned Value:
 *    None
 *
 ****************************************************************************/

#ifdef HAVE_AUTOMOUNTER
void k66_automount_initialize(void);
#endif

/****************************************************************************
 * Name:  k66_automount_event
 *
 * Description:
 *   The SDHC card detection logic has detected an insertion or removal
 *   event.
 *   It has already scheduled the MMC/SD block driver operations.
 *   Now we need to schedule the auto-mount event which will occur with a
 *   substantial delay to make sure that everything has settle down.
 *
 * Input Parameters:
 *   inserted - True if the card is inserted in the slot.  False otherwise.
 *
 *  Returned Value:
 *    None
 *
 *  Assumptions:
 *    Interrupts are disabled.
 *
 ****************************************************************************/

#ifdef HAVE_AUTOMOUNTER
void k66_automount_event(bool inserted);
#endif

/****************************************************************************
 * Name: k66_pwm_setup
 *
 * Description:
 *   Initialize PWM and register the PWM device.
 *
 ****************************************************************************/

#ifdef CONFIG_PWM
int k66_pwm_setup(void);
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_KINETIS_TEENSY_36_SRC_TEENSY_36_H */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libdragon.h>

#include "flashcart/flashcart.h"
#include "libs/toml/toml.h"
#include "boot/boot.h"

#include "settings.h"


void settings_load_from_file (settings_t *settings) {
    FILE *fp = NULL;
    char error_buffer[256];
    
    printf("Loading settings file %s\n", SC64_SETTINGS_FILEPATH);
    wait_ms(1000);

    fp = fopen(SC64_SETTINGS_FILEPATH, "r");
	if (!fp) {
		printf("Error loading config file %s\n", SC64_SETTINGS_FILEPATH);
        // generate a default config file.
        printf("Creating a new one!");
        wait_ms(10000);
        //assertf(!fp, "Couldn't open toml config file: %s", SC64_SETTINGS_FILEPATH);
        settings_save_default_state();
	}

    toml_table_t *conf = toml_parse_file(fp, error_buffer, sizeof(error_buffer));
    if (!conf) {
		printf("Error parsing config: %s\n", error_buffer);
        wait_ms(10000);
        printf("Attempt a repair!");
        settings_validate();
        printf("Creating a new one!");
        settings_save_default_state();
        //assertf(!conf, "Couldn't parse toml config: %s", error_buffer); // TODO: work out why and handle appropriately.
	}

    //fclose(fp);
    assertf(!fclose(fp), "Couldn't close toml config file"); // TODO: work out why and handle appropriately.
    fp = NULL;

    // Handle last_rom
    toml_table_t *last_rom = toml_table_in(conf, "last_rom");
    if (!last_rom) {
		printf("Missing '[last_rom]' header in config\n");
        wait_ms(10000);
	}
    else {
        toml_datum_t rom_path = toml_string_in(last_rom, "rom_path");
        if (!rom_path.ok) {
            printf("Couldn't read 'rom_path' value in config\n");
            wait_ms(10000);
        }
        else {
            printf("Found rom path: %s\n", rom_path.u.s);
            settings->last_rom.rom_path = rom_path.u.s;
        }

        toml_datum_t save_path = toml_string_in(last_rom, "save_path");
        if (!save_path.ok) {
            printf("Couldn't read 'save_path' value in config\n");
            wait_ms(10000);
        }
        else {
            printf("Found save path: %s\n", save_path.u.s );
            settings->last_rom.save_path = save_path.u.s;
        }

        toml_datum_t tmp_save_type = toml_int_in(last_rom, "save_type");
        if (!tmp_save_type.ok) {
            printf("Couldn't read 'save_type' value in config\n");
            wait_ms(10000);
        }
        else {
            assertf((int)tmp_save_type.u.i < __FLASHCART_SAVE_TYPE_END, "Invalid save type in config file");
            printf("Found save type: %d\n", (int)tmp_save_type.u.i );
            settings->last_rom.save_type = (int)tmp_save_type.u.i;
        }
    }


    // Handle last_state
    toml_table_t* last_state = toml_table_in(conf, "last_state");
    if (!last_state) {
		printf("Missing '[last_state]' header in config\n");
        wait_ms(10000);
	}
    else {

        // toml_datum_t current_directory = toml_string_in(last_state, "current_directory");
        // if (!current_directory.ok) {
        // 	printf("Couldn't read 'current_directory' value in config\n");
        //     printf("Defaulting to root directory.\n");
        //     settings->last_state.current_directory = "sd://";
        //     wait_ms(5000);
        // }
        // else {
        //     printf("Found current directory: %s\n", current_directory.u.s );
        //     settings->last_state.current_directory = current_directory.u.s;
        // }

        // toml_datum_t tmp_auto_load_last_rom = toml_bool_in(last_state, "auto_load");
        // if (!tmp_auto_load_last_rom.ok) {
        // 	printf("Couldn't read 'auto_load' value in config\n");
        //     printf("Defaulting to false.\n");
        //     wait_ms(5000);
        //     settings->last_state.auto_load_last_rom = false;
        // }
        // else {
        //     printf("Found autoload: %s\n", tmp_auto_load_last_rom.u.b ? "true" : "false");
        //     settings->last_state.auto_load_last_rom = tmp_auto_load_last_rom.u.b;
        // }
    }

    // Handle boot_params
    toml_table_t* boot_params = toml_table_in(conf, "boot_params");
    if (!boot_params) {
		printf("Missing '[boot_params]' header in config\n");
        wait_ms(10000);
	}
    else {

    }

    toml_free(conf);

}

void settings_save(void) {
    // TODO: if there is a failure, validate or write the default state.
    FILE *fp = NULL;
    printf("Saving settings file %s\n", SC64_SETTINGS_FILEPATH);
    wait_ms(1000);
    fp = fopen(SC64_SETTINGS_FILEPATH, "w");

    // TODO: convert and save the state to TOML

    fclose(fp);
    assertf(!fclose(fp), "Couldn't close toml settings file"); // TODO: work out why and handle appropriately.
    fp = NULL;
}

void settings_load_default_state(settings_t *settings) {
    // Happens on init
    // TODO: should also happen as a last resort
    // Mainly if the file does not exist, or is corrupted beyond repair.
//#ifdef SETTINGS_SCHEMA_VERSION 1
    settings->last_rom.rom_path = "";
    settings->last_rom.save_path = "";
    settings->last_rom.save_type = FLASHCART_SAVE_TYPE_NONE;
    settings->last_rom.save_writeback = false;

    settings->last_state.current_directory = "sd://";
    settings->last_state.auto_load_last_rom = false;

    settings->boot_params.device_type = BOOT_DEVICE_TYPE_ROM;
    settings->boot_params.reset_type = BOOT_RESET_TYPE_NMI;
    settings->boot_params.detect_tv_type = true;
    settings->boot_params.detect_cic_seed = true;
//#endif
}

void settings_save_default_state(void) {

}

void settings_validate(void) {
    // TODO: should validate against a file schema.
    // Must take into account that the settings will change in future, so should be backwards compatible.
}

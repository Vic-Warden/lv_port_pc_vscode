#include "lvgl/lvgl.h"
#include "hal/hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Image declarations
LV_IMG_DECLARE(album_cover);
LV_IMG_DECLARE(adaptive);
LV_IMG_DECLARE(autonomous);
LV_IMG_DECLARE(charging);
LV_IMG_DECLARE(front_fog_light);
LV_IMG_DECLARE(low_beam);
LV_IMG_DECLARE(parking_light);
LV_IMG_DECLARE(rear_fog_light);
LV_IMG_DECLARE(steering);

// Global widgets for right dial (infotainment)
static lv_obj_t *img_album_art;
static lv_obj_t *lbl_music_title;
static lv_obj_t *lbl_music_artist;
static lv_obj_t *cont_call_overlay;
static lv_obj_t *lbl_call_name;
static lv_obj_t *lbl_call_number;
static lv_obj_t *lbl_time_date;
static lv_obj_t *lbl_incoming_call;

// Global widgets for left dial (dashboard)
static lv_obj_t *arc_battery;
static lv_obj_t *lbl_range;
static lv_obj_t *lbl_bat_pct;
static lv_obj_t *lbl_left_blink;
static lv_obj_t *lbl_right_blink;
static lv_obj_t *lbl_center_text;
static lv_obj_t *img_lights;
static lv_obj_t *img_center_icon;
static uint8_t light_mode = 0;
static uint8_t blinker_mode = 0;

// Returns a color depending on battery percentage
static lv_color_t get_battery_color(int32_t pct) {
    lv_color_t color_green = lv_color_hex(0x2ECC71);
    lv_color_t color_yellow = lv_color_hex(0xE6B800);
    lv_color_t color_red = lv_color_hex(0xE74C3C);
    if (pct > 50) {
        uint8_t mix = (uint8_t)(((pct - 50) * 255) / 50);
        return lv_color_mix(color_green, color_yellow, mix);
    } else if (pct > 20) {
        uint8_t mix = (uint8_t)(((pct - 20) * 255) / 30);
        return lv_color_mix(color_yellow, color_red, mix);
    } else {
        return color_red;
    }
}

void create_left_dial(void) {
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0); // Black background
    lv_obj_t *dial = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dial, 420, 420);
    lv_obj_align(dial, LV_ALIGN_CENTER, -240, 0); // Left position
    lv_obj_set_style_radius(dial, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dial, lv_color_hex(0x0A0A0A), 0);
    lv_obj_set_style_border_color(dial, lv_color_hex(0x333333), 0);
    lv_obj_set_style_border_width(dial, 2, 0);
    img_lights = lv_image_create(dial);
    lv_image_set_src(img_lights, &parking_light);
    lv_obj_set_size(img_lights, 60, 60);
    lv_image_set_scale(img_lights, 30);
    lv_image_set_inner_align(img_lights, LV_IMAGE_ALIGN_CENTER);
    lv_obj_align(img_lights, LV_ALIGN_TOP_MID, 0, 35);
    lv_obj_set_style_image_recolor_opa(img_lights, LV_OPA_COVER, 0);
    lv_obj_set_style_image_recolor(img_lights, lv_color_hex(0x2ECC71), 0);
    lbl_range = lv_label_create(dial);
    lv_label_set_text(lbl_range, "48");
    lv_obj_set_style_text_color(lbl_range, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_range, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_range, LV_ALIGN_CENTER, 0, -40);
    lv_obj_t *lbl_km = lv_label_create(dial);
    lv_label_set_text(lbl_km, "km");
    lv_obj_set_style_text_color(lbl_km, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(lbl_km, &lv_font_montserrat_18, 0);
    lv_obj_align_to(lbl_km, lbl_range, LV_ALIGN_OUT_BOTTOM_MID, 0, -5);
    lbl_left_blink = lv_label_create(dial);
    lv_label_set_text(lbl_left_blink, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_font(lbl_left_blink, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lbl_left_blink, lv_color_hex(0x1A1A1A), 0);
    lv_obj_align_to(lbl_left_blink, lbl_range, LV_ALIGN_OUT_LEFT_MID, -35, 0);
    lbl_right_blink = lv_label_create(dial);
    lv_label_set_text(lbl_right_blink, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_font(lbl_right_blink, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lbl_right_blink, lv_color_hex(0x1A1A1A), 0);
    lv_obj_align_to(lbl_right_blink, lbl_range, LV_ALIGN_OUT_RIGHT_MID, 35, 0);
    lbl_bat_pct = lv_label_create(dial);
    lv_label_set_text(lbl_bat_pct, "30%");
    lv_obj_set_style_text_color(lbl_bat_pct, get_battery_color(30), 0);
    lv_obj_align(lbl_bat_pct, LV_ALIGN_BOTTOM_MID, 0, -60);
    img_center_icon = lv_image_create(dial);
    lv_image_set_src(img_center_icon, &autonomous);
    lv_obj_set_size(img_center_icon, 50, 50);
    lv_image_set_scale(img_center_icon, 25);
    lv_image_set_inner_align(img_center_icon, LV_IMAGE_ALIGN_CENTER);
    lv_obj_align(img_center_icon, LV_ALIGN_CENTER, 0, 45);
    lv_obj_set_style_image_recolor_opa(img_center_icon, LV_OPA_COVER, 0);
    lv_obj_set_style_image_recolor(img_center_icon, lv_color_hex(0x3498DB), 0);
    lbl_center_text = lv_label_create(dial);
    lv_label_set_text(lbl_center_text, "Full Autonomous");
    lv_obj_set_style_text_color(lbl_center_text, lv_color_hex(0x3498DB), 0);
    lv_obj_set_style_text_font(lbl_center_text, &lv_font_montserrat_12, 0);
    lv_obj_align_to(lbl_center_text, img_center_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, -2);
    arc_battery = lv_arc_create(dial);
    lv_obj_set_size(arc_battery, 370, 370);
    lv_obj_center(arc_battery);
    lv_arc_set_rotation(arc_battery, 135);
    lv_arc_set_bg_angles(arc_battery, 0, 270);
    lv_arc_set_value(arc_battery, 30);
    lv_obj_remove_style(arc_battery, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_opa(arc_battery, 0, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_battery, 8, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_battery, get_battery_color(30), LV_PART_INDICATOR);
    lbl_time_date = lv_label_create(dial);
    lv_label_set_text(lbl_time_date, "19:43  |  MAR 23");
    lv_obj_set_style_text_color(lbl_time_date, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_time_date, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_time_date, LV_ALIGN_BOTTOM_MID, 0, -25);
}

void create_infotainment_dial(void) {
    lv_obj_t *dial = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dial, 420, 420);
    lv_obj_align(dial, LV_ALIGN_CENTER, 240, 0); // Right position
    lv_obj_set_style_radius(dial, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dial, lv_color_hex(0x0A0A0A), 0);
    lv_obj_set_style_border_width(dial, 2, 0);
    lv_obj_set_style_border_color(dial, lv_color_hex(0x333333), 0);
    img_album_art = lv_image_create(dial);
    lv_image_set_src(img_album_art, &album_cover);
    lv_img_set_zoom(img_album_art, 64); 
    lv_obj_set_style_bg_color(img_album_art, lv_color_black(), 0);
    lv_obj_align(img_album_art, LV_ALIGN_CENTER, 0, -80);
    lbl_music_title = lv_label_create(dial);
    lv_label_set_text(lbl_music_title, "Title");
    lv_obj_set_style_text_font(lbl_music_title, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(lbl_music_title, lv_color_white(), 0);
    lv_obj_align(lbl_music_title, LV_ALIGN_CENTER, 0, 10);
    lbl_music_artist = lv_label_create(dial);
    lv_label_set_text(lbl_music_artist, "Artist");
    lv_obj_set_style_text_font(lbl_music_artist, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_music_artist, lv_color_hex(0xAAAAAA), 0);
    lv_obj_align(lbl_music_artist, LV_ALIGN_CENTER, 0, 40);
    cont_call_overlay = lv_obj_create(dial);
    lv_obj_set_size(cont_call_overlay, 1, 1);
    lv_obj_add_flag(cont_call_overlay, LV_OBJ_FLAG_HIDDEN);
    lbl_incoming_call = lv_label_create(dial);
    lv_label_set_text(lbl_incoming_call, "Incoming Call");
    lv_obj_set_style_text_font(lbl_incoming_call, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_incoming_call, lv_color_white(), 0);
    lv_obj_align(lbl_incoming_call, LV_ALIGN_CENTER, 0, -40);
    lv_obj_add_flag(lbl_incoming_call, LV_OBJ_FLAG_HIDDEN);
    lbl_call_name = lv_label_create(dial);
    lv_label_set_text(lbl_call_name, "");
    lv_obj_set_style_text_font(lbl_call_name, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_call_name, lv_color_white(), 0);
    lv_obj_align(lbl_call_name, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(lbl_call_name, LV_OBJ_FLAG_HIDDEN);
    lbl_call_number = lv_label_create(dial);
    lv_label_set_text(lbl_call_number, "+33(0)6 18 03 31 52");
    lv_obj_set_style_text_font(lbl_call_number, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_call_number, lv_color_white(), 0);
    lv_obj_align(lbl_call_number, LV_ALIGN_CENTER, 0, 30);
    lv_obj_add_flag(lbl_call_number, LV_OBJ_FLAG_HIDDEN);
    lbl_time_date = lv_label_create(dial);
    lv_label_set_text(lbl_time_date, "14:25  |  MAR 26");
    lv_obj_set_style_text_color(lbl_time_date, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_time_date, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_time_date, LV_ALIGN_BOTTOM_MID, 0, -25);
}

int main(void)
{
    lv_init();
    sdl_hal_init(960, 480); // Window size
    create_left_dial();
    create_infotainment_dial();
    uint32_t sim_timer = 0;
    int32_t battery_pct = 30;
    bool is_charging = false;
    bool blinker_state = false;
    char buf[32];
    while (1)
    {
        sim_timer++;
        // Toggle incoming call on right dial every ~10s
        if(sim_timer % 2000 == 0) {
            if(lv_obj_has_flag(lbl_incoming_call, LV_OBJ_FLAG_HIDDEN)) {
                lv_obj_clear_flag(lbl_incoming_call, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lbl_call_name, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lbl_call_number, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(img_album_art, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(lbl_music_title, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(lbl_music_artist, LV_OBJ_FLAG_HIDDEN);
                lv_label_set_text(lbl_call_name, "Jean Dupont");
                lv_label_set_text(lbl_call_number, "+33 6 12 34 56 78");
            } else {
                lv_obj_add_flag(lbl_incoming_call, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(lbl_call_name, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(lbl_call_number, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(img_album_art, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lbl_music_title, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lbl_music_artist, LV_OBJ_FLAG_HIDDEN);
            }
        }
        // Blinker and lights logic
        if(sim_timer % 100 == 0) {
            blinker_state = !blinker_state;
            if(sim_timer % 600 == 0) blinker_mode = (blinker_mode + 1) % 3;
            lv_obj_set_style_text_color(lbl_left_blink, lv_color_hex(0x1A1A1A), 0);
            lv_obj_set_style_text_color(lbl_right_blink, lv_color_hex(0x1A1A1A), 0);
            if(blinker_state) {
                if(blinker_mode == 0 || blinker_mode == 2) lv_obj_set_style_text_color(lbl_left_blink, lv_color_hex(0x2ECC71), 0);
                if(blinker_mode == 1 || blinker_mode == 2) lv_obj_set_style_text_color(lbl_right_blink, lv_color_hex(0x2ECC71), 0);
            }
            if(!is_charging && battery_pct < 21) {
                if(blinker_state) {
                    lv_obj_remove_flag(img_center_icon, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_remove_flag(lbl_center_text, LV_OBJ_FLAG_HIDDEN);
                } else {
                    lv_obj_add_flag(img_center_icon, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(lbl_center_text, LV_OBJ_FLAG_HIDDEN);
                }
            }
            if(sim_timer % 400 == 0) {
                light_mode = (light_mode + 1) % 5;
                switch(light_mode) {
                    case 0: lv_image_set_src(img_lights, &parking_light); lv_obj_set_style_image_recolor(img_lights, lv_color_hex(0x2ECC71), 0); break;
                    case 1: lv_image_set_src(img_lights, &low_beam); lv_obj_set_style_image_recolor(img_lights, lv_color_hex(0x2ECC71), 0); break;
                    case 2: lv_image_set_src(img_lights, &low_beam); lv_obj_set_style_image_recolor(img_lights, lv_color_hex(0x3498DB), 0); break;
                    case 3: lv_image_set_src(img_lights, &front_fog_light); lv_obj_set_style_image_recolor(img_lights, lv_color_hex(0x2ECC71), 0); break;
                    case 4: lv_image_set_src(img_lights, &rear_fog_light); lv_obj_set_style_image_recolor(img_lights, lv_color_hex(0xE67E22), 0); break;
                }
            }
        }
        // Battery and mode logic
        if(sim_timer % 400 == 0) {
            if(battery_pct > 0) battery_pct--;
            lv_arc_set_value(arc_battery, battery_pct);
            sprintf(buf, "%d%%", battery_pct);
            lv_label_set_text(lbl_bat_pct, buf);
            sprintf(buf, "%d", (battery_pct * 160) / 100);
            lv_label_set_text(lbl_range, buf);
            lv_color_t current_color = get_battery_color(battery_pct);
            lv_obj_set_style_arc_color(arc_battery, current_color, LV_PART_INDICATOR);
            lv_obj_set_style_text_color(lbl_bat_pct, current_color, 0);
            if (battery_pct >= 27) {
                lv_image_set_src(img_center_icon, &autonomous);
                lv_label_set_text(lbl_center_text, "Full Autonomous");
                lv_obj_set_style_image_recolor(img_center_icon, lv_color_hex(0x3498DB), 0);
                lv_obj_set_style_text_color(lbl_center_text, lv_color_hex(0x3498DB), 0);
                lv_obj_remove_flag(img_center_icon, LV_OBJ_FLAG_HIDDEN);
                lv_obj_remove_flag(lbl_center_text, LV_OBJ_FLAG_HIDDEN);
            } else if (battery_pct >= 24) {
                lv_image_set_src(img_center_icon, &adaptive);
                lv_label_set_text(lbl_center_text, "Driver assistance");
                lv_obj_set_style_image_recolor(img_center_icon, lv_color_hex(0x2ECC71), 0);
                lv_obj_set_style_text_color(lbl_center_text, lv_color_hex(0x2ECC71), 0);
                lv_obj_remove_flag(img_center_icon, LV_OBJ_FLAG_HIDDEN);
                lv_obj_remove_flag(lbl_center_text, LV_OBJ_FLAG_HIDDEN);
            } else if (battery_pct >= 21) {
                lv_image_set_src(img_center_icon, &steering);
                lv_label_set_text(lbl_center_text, "Manual Drive");
                lv_obj_set_style_image_recolor(img_center_icon, lv_color_hex(0xF39C12), 0);
                lv_obj_set_style_text_color(lbl_center_text, lv_color_hex(0xF39C12), 0);
                lv_obj_remove_flag(img_center_icon, LV_OBJ_FLAG_HIDDEN);
                lv_obj_remove_flag(lbl_center_text, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_image_set_src(img_center_icon, &charging);
                lv_label_set_text(lbl_center_text, "LOW BATTERY!");
                lv_obj_set_style_image_recolor(img_center_icon, lv_color_hex(0xE74C3C), 0);
                lv_obj_set_style_text_color(lbl_center_text, lv_color_hex(0xE74C3C), 0);
            }
        }
        lv_timer_handler();
        usleep(5 * 1000);
    }
    return 0;
}
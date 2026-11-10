#include <lvgl.h>
#include "nurture.h"

void lvgl_ui_entry_point()
{
    const uint16_t screen_padding = 10;
    lv_obj_t* screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_left(screen, screen_padding, 0);
    lv_obj_set_style_pad_right(screen, screen_padding, 0);
    lv_obj_set_style_pad_top(screen, screen_padding, 0);
    lv_obj_set_style_pad_bottom(screen, screen_padding, 0);

    const uint16_t album_cover_size = 220;
    lv_obj_t* album_cover = lv_image_create(screen);
    lv_image_set_src(album_cover, &nurture);
    lv_obj_set_size(album_cover, album_cover_size, album_cover_size);
    lv_image_set_inner_align(album_cover, LV_IMAGE_ALIGN_CENTER);

    lv_obj_t* song_name_text = lv_label_create(screen);
    lv_label_set_text(song_name_text, "Look At The Sky");
    lv_label_set_long_mode(song_name_text, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(song_name_text, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(song_name_text, album_cover_size, 20);

    lv_obj_t* play_next_previous_container = lv_obj_create(screen);
    lv_obj_set_size(play_next_previous_container, album_cover_size, 40);
    lv_obj_set_style_bg_opa(play_next_previous_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(play_next_previous_container, 0, 0);
    lv_obj_set_style_pad_all(play_next_previous_container, 0, 0);
    lv_obj_set_scrollbar_mode(play_next_previous_container, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_flex_flow(play_next_previous_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(play_next_previous_container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_left(play_next_previous_container, screen_padding, 0);
    lv_obj_set_style_pad_right(play_next_previous_container, screen_padding, 0);

    lv_obj_t* previous = lv_button_create(play_next_previous_container);
    lv_obj_t* play_pause = lv_button_create(play_next_previous_container);
    lv_obj_t* next = lv_button_create(play_next_previous_container);

    lv_obj_t* previous_icon = lv_label_create(previous);
    lv_label_set_text(previous_icon, LV_SYMBOL_PREV);
    lv_obj_set_flex_align(previous_icon, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* play_pause_icon = lv_label_create(play_pause);
    lv_label_set_text(play_pause_icon, LV_SYMBOL_PLAY);
    lv_obj_set_flex_align(play_pause_icon, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* next_icon = lv_label_create(next);
    lv_label_set_text(next_icon, LV_SYMBOL_NEXT);
    lv_obj_set_flex_align(next_icon, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* Set their sizes */
    lv_obj_set_size(previous, 40, 40);
    lv_obj_set_size(play_pause, 40, 40);
    lv_obj_set_size(next, 40, 40);
}
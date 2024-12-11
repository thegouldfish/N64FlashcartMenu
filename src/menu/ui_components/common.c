#include <stdarg.h>

#include "../ui_components.h"
#include "../fonts.h"
#include "constants.h"


void ui_components_box_draw (int x0, int y0, int x1, int y1, color_t color) {
    rdpq_mode_push();
        rdpq_set_mode_fill(color);

        rdpq_fill_rectangle(x0, y0, x1, y1);
    rdpq_mode_pop();
}


static void ui_components_border_draw_interal (int x0, int y0, int x1, int y1, color_t color) {
    rdpq_mode_push();
        rdpq_set_mode_fill(color);

        rdpq_fill_rectangle(x0 - BORDER_THICKNESS, y0 - BORDER_THICKNESS, x1 + BORDER_THICKNESS, y0);
        rdpq_fill_rectangle(x0 - BORDER_THICKNESS, y1, x1 + BORDER_THICKNESS, y1 + BORDER_THICKNESS);

        rdpq_fill_rectangle(x0 - BORDER_THICKNESS, y0, x0, y1);
        rdpq_fill_rectangle(x1, y0, x1 + BORDER_THICKNESS, y1);
    rdpq_mode_pop();
}

void ui_components_border_draw (int x0, int y0, int x1, int y1) {
    ui_components_border_draw_interal(x0, y0, x1, y1, BORDER_COLOR);
}

void ui_components_layout_draw (void) {
    ui_components_border_draw(
        VISIBLE_AREA_X0,
        VISIBLE_AREA_Y0,
        VISIBLE_AREA_X1,
        VISIBLE_AREA_Y1
    );
    ui_components_box_draw(
        VISIBLE_AREA_X0,
        LAYOUT_ACTIONS_SEPARATOR_Y,
        VISIBLE_AREA_X1,
        LAYOUT_ACTIONS_SEPARATOR_Y + BORDER_THICKNESS,
        BORDER_COLOR
    );
}

void ui_components_progressbar_draw (int x0, int y0, int x1, int y1, float progress) {    
    float progress_width = progress * (x1 - x0);

    ui_components_box_draw(x0, y0, x0 + progress_width, y1, PROGRESSBAR_DONE_COLOR);
    ui_components_box_draw(x0 + progress_width, y0, x1, y1, PROGRESSBAR_BG_COLOR);
}

void ui_components_seekbar_draw (float position) {
    int x0 = SEEKBAR_X;
    int y0 = SEEKBAR_Y;
    int x1 = SEEKBAR_X + SEEKBAR_WIDTH;
    int y1 = SEEKBAR_Y + SEEKBAR_HEIGHT;

    ui_components_border_draw(x0, y0, x1, y1);
    ui_components_progressbar_draw(x0, y0, x1, y1, position);
}

void ui_components_loader_draw (float progress) {
    int x0 = LOADER_X;
    int y0 = LOADER_Y;
    int x1 = LOADER_X + LOADER_WIDTH;
    int y1 = LOADER_Y + LOADER_HEIGHT;

    ui_components_border_draw(x0, y0, x1, y1);
    ui_components_progressbar_draw(x0, y0, x1, y1, progress);
}

void ui_components_scrollbar_draw (int x, int y, int width, int height, int position, int items, int visible_items) {
    if (items <= 1 || items <= visible_items) {
        ui_components_box_draw(x, y, x + width, y + height, SCROLLBAR_INACTIVE_COLOR);
    } else {
        int scroll_height = (int) ((visible_items / (float) (items)) * height);
        float scroll_position = ((position / (float) (items - 1)) * (height - scroll_height));

        ui_components_box_draw(x, y, x + width, y + height, SCROLLBAR_BG_COLOR);
        ui_components_box_draw(x, y + scroll_position, x + width, y + scroll_position + scroll_height, SCROLLBAR_POSITION_COLOR);
    }
}

void ui_components_list_scrollbar_draw (int position, int items, int visible_items) {
    ui_components_scrollbar_draw(
        LIST_SCROLLBAR_X,
        LIST_SCROLLBAR_Y,
        LIST_SCROLLBAR_WIDTH,
        LIST_SCROLLBAR_HEIGHT,
        position,
        items,
        visible_items
    );
}

void ui_components_dialog_draw (int width, int height) {
    int x0 = DISPLAY_CENTER_X - (width / 2);
    int y0 = DISPLAY_CENTER_Y - (height / 2);
    int x1 = DISPLAY_CENTER_X + (width / 2);
    int y1 = DISPLAY_CENTER_Y + (height / 2);

    ui_components_border_draw(x0, y0, x1, y1);
    ui_components_box_draw(x0, y0, x1, y1, DIALOG_BG_COLOR);
}

void ui_components_messagebox_draw (char *fmt, ...) {
    char buffer[512];
    size_t nbytes = sizeof(buffer);

    va_list va;
    va_start(va, fmt);
    char *formatted = vasnprintf(buffer, &nbytes, fmt, va);
    va_end(va);

    int paragraph_nbytes = nbytes;

    rdpq_paragraph_t *paragraph = rdpq_paragraph_build(&(rdpq_textparms_t) {
        .width = MESSAGEBOX_MAX_WIDTH,
        .height = VISIBLE_AREA_HEIGHT,
        .align = ALIGN_CENTER,
        .valign = VALIGN_CENTER,
        .wrap = WRAP_WORD,
        .line_spacing = TEXT_LINE_SPACING_ADJUST,
    }, FNT_DEFAULT, formatted, &paragraph_nbytes);

    if (formatted != buffer) {
        free(formatted);
    }

    ui_components_dialog_draw(
        paragraph->bbox.x1 - paragraph->bbox.x0 + MESSAGEBOX_MARGIN,
        paragraph->bbox.y1 - paragraph->bbox.y0 + MESSAGEBOX_MARGIN
    );

    rdpq_paragraph_render(paragraph, DISPLAY_CENTER_X - (MESSAGEBOX_MAX_WIDTH / 2), VISIBLE_AREA_Y0);

    rdpq_paragraph_free(paragraph);
}

void ui_components_main_text_draw (rdpq_align_t align, rdpq_valign_t valign, char *fmt, ...) {
    char buffer[1024];
    size_t nbytes = sizeof(buffer);

    va_list va;
    va_start(va, fmt);
    char *formatted = vasnprintf(buffer, &nbytes, fmt, va);
    va_end(va);

    rdpq_text_printn(
        &(rdpq_textparms_t) {
            .width = VISIBLE_AREA_WIDTH - (TEXT_MARGIN_HORIZONTAL * 2),
            .height = LAYOUT_ACTIONS_SEPARATOR_Y - OVERSCAN_HEIGHT - (TEXT_MARGIN_VERTICAL * 2),
            .align = align,
            .valign = valign,
            .wrap = WRAP_WORD,
            .line_spacing = TEXT_LINE_SPACING_ADJUST,
        },
        FNT_DEFAULT,
        VISIBLE_AREA_X0 + TEXT_MARGIN_HORIZONTAL,
        VISIBLE_AREA_Y0 + TEXT_MARGIN_VERTICAL + TEXT_OFFSET_VERTICAL,
        formatted,
        nbytes
    );

    if (formatted != buffer) {
        free(formatted);
    }
}

void ui_components_actions_bar_text_draw (rdpq_align_t align, rdpq_valign_t valign, char *fmt, ...) {
    char buffer[256];
    size_t nbytes = sizeof(buffer);

    va_list va;
    va_start(va, fmt);
    char *formatted = vasnprintf(buffer, &nbytes, fmt, va);
    va_end(va);

    rdpq_text_printn(
        &(rdpq_textparms_t) {
            .width = VISIBLE_AREA_WIDTH - (TEXT_MARGIN_HORIZONTAL * 2),
            .height = VISIBLE_AREA_Y1 - LAYOUT_ACTIONS_SEPARATOR_Y - BORDER_THICKNESS - (TEXT_MARGIN_VERTICAL * 2),
            .align = align,
            .valign = valign,
            .wrap = WRAP_ELLIPSES,
            .line_spacing = TEXT_LINE_SPACING_ADJUST,
        },
        FNT_DEFAULT,
        VISIBLE_AREA_X0 + TEXT_MARGIN_HORIZONTAL,
        LAYOUT_ACTIONS_SEPARATOR_Y + BORDER_THICKNESS + TEXT_MARGIN_VERTICAL + TEXT_OFFSET_VERTICAL,
        formatted,
        nbytes
    );

    if (formatted != buffer) {
        free(formatted);
    }
}

void ui_components_main_text_draw_location (float x, float y, char *fmt, ...) {
    char buffer[256];
    size_t nbytes = sizeof(buffer);

    va_list va;
    va_start(va, fmt);
    char *formatted = vasnprintf(buffer, &nbytes, fmt, va);
    va_end(va);

    rdpq_text_printn(
        &(rdpq_textparms_t) {
            .width = VISIBLE_AREA_WIDTH - (TEXT_MARGIN_HORIZONTAL * 2),
            .height = LAYOUT_ACTIONS_SEPARATOR_Y - OVERSCAN_HEIGHT - (TEXT_MARGIN_VERTICAL * 2),
            .align = ALIGN_LEFT,
            .valign = VALIGN_TOP,
            .wrap = WRAP_ELLIPSES,
            .line_spacing = TEXT_LINE_SPACING_ADJUST,
        },
        FNT_DEFAULT,
        x,
        y,
        ///VISIBLE_AREA_X0 + TEXT_MARGIN_HORIZONTAL + x,
        //VISIBLE_AREA_Y0 + TEXT_MARGIN_VERTICAL + TEXT_OFFSET_VERTICAL + y,
        formatted,
        nbytes
    );

    if (formatted != buffer) {
        free(formatted);
    }
}



static void ui_components_main_text_draw_tab (float x, float y, float width, char *fmt, ...) {
    char buffer[256];
    size_t nbytes = sizeof(buffer);

    va_list va;
    va_start(va, fmt);
    char *formatted = vasnprintf(buffer, &nbytes, fmt, va);
    va_end(va);

    rdpq_text_printn(
        &(rdpq_textparms_t) {
            .width = width,
            .height = LAYOUT_ACTIONS_SEPARATOR_Y - OVERSCAN_HEIGHT - (TEXT_MARGIN_VERTICAL * 2),
            .align = ALIGN_CENTER,
            .valign = VALIGN_TOP,
            .wrap = WRAP_ELLIPSES,
            .line_spacing = TEXT_OFFSET_VERTICAL,
        },
        FNT_DEFAULT,
        x,
        y,
        formatted,
        nbytes
    );

    if (formatted != buffer) {
        free(formatted);
    }
}

void ui_components_tabs_draw(const char** text, int count, int selected, int width ) {

    ui_components_box_draw(
        VISIBLE_AREA_X0,
        LAYOUT_ACTIONS_SEPARATOR_Y,
        VISIBLE_AREA_X1,
        LAYOUT_ACTIONS_SEPARATOR_Y + BORDER_THICKNESS,
        BORDER_COLOR
    );

    int starting_x = VISIBLE_AREA_X0 + 4;

    int x = starting_x;
    int y = OVERSCAN_HEIGHT;    
    int height = TAB_HEIGHT;

    // first draw the tabs that are not selected
    for(int i=0;i< count;i++) {
        if(i != selected) {

            ui_components_box_draw(
                x,
                y,
                x + width,
                y + height,
                TAB_INACTIVE_BACKGROUND_COLOR
            );

            ui_components_border_draw_interal(
                x,
                y,
                x + width,
                y + height,
                TAB_INACTIVE_BORDER_COLOR
            );

            ui_components_main_text_draw_tab(x + 4, y, width, (char*)text[i]);
        }

        x += width;
    }
    // draw the selected tab

    if(selected >= 0 && selected < count) {
        x = starting_x + (width * selected);

        ui_components_box_draw(
            x,
            y,
            x + width,
            y + height,
            TAB_ACTIVE_BACKGROUND_COLOR
        );

        ui_components_border_draw_interal(
            x,
            y,
            x + width,
            y + height,
            TAB_ACTIVE_BORDER_COLOR
        );


        ui_components_main_text_draw_tab(x + 4, y, width, (char*)text[selected]);
    }
}
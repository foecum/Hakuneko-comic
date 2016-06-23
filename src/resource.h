/************************************************************************
*             INCLUDE THIS HEADER FILE INTO YOUR PROJECT                *
*************************************************************************
* Accessing binary data of a member:           RESOURCES::MEMBER.data   *
* Getting the binary data size of a member:    RESOURCES::MEMBER.length *
************************************************************************/

#ifndef RESOURCES_H
#define RESOURCES_H

struct RCDATA
{
    unsigned int length;
    unsigned char* data;
};

struct RESOURCES
{
    // File: "..\res\untick.png"
    public: static const RCDATA UNTICK_PNG;
    // File: "..\res\btn_abort_disabled.png"
    public: static const RCDATA BTN_ABORT_DISABLED_PNG;
    // File: "..\res\btn_bookremove_disabled.png"
    public: static const RCDATA BTN_BOOKREMOVE_DISABLED_PNG;
    // File: "..\res\btn_bookmark_hover.png"
    public: static const RCDATA BTN_BOOKMARK_HOVER_PNG;
    // File: "..\res\btn_browse_disabled.png"
    public: static const RCDATA BTN_BROWSE_DISABLED_PNG;
    // File: "..\res\btn_bookremove_normal.png"
    public: static const RCDATA BTN_BOOKREMOVE_NORMAL_PNG;
    // File: "..\res\btn_sync_normal.png"
    public: static const RCDATA BTN_SYNC_NORMAL_PNG;
    // File: "..\res\btn_sync_hover.png"
    public: static const RCDATA BTN_SYNC_HOVER_PNG;
    // File: "..\res\btn_download_disabled.png"
    public: static const RCDATA BTN_DOWNLOAD_DISABLED_PNG;
    // File: "..\res\btn_abort_hover.png"
    public: static const RCDATA BTN_ABORT_HOVER_PNG;
    // File: "..\res\btn_browse_hover.png"
    public: static const RCDATA BTN_BROWSE_HOVER_PNG;
    // File: "..\res\app.ico"
    public: static const RCDATA APP_ICO;
    // File: "..\res\btn_download_hover.png"
    public: static const RCDATA BTN_DOWNLOAD_HOVER_PNG;
    // File: "..\res\btn_download_normal.png"
    public: static const RCDATA BTN_DOWNLOAD_NORMAL_PNG;
    // File: "..\res\btn_bookremove_hover.png"
    public: static const RCDATA BTN_BOOKREMOVE_HOVER_PNG;
    // File: "..\res\btn_bookmark_disabled.png"
    public: static const RCDATA BTN_BOOKMARK_DISABLED_PNG;
    // File: "..\res\tick.png"
    public: static const RCDATA TICK_PNG;
    // File: "..\res\btn_abort_normal.png"
    public: static const RCDATA BTN_ABORT_NORMAL_PNG;
    // File: "..\res\btn_browse_normal.png"
    public: static const RCDATA BTN_BROWSE_NORMAL_PNG;
    // File: "..\res\btn_bookmark_normal.png"
    public: static const RCDATA BTN_BOOKMARK_NORMAL_PNG;
    // File: "..\res\btn_sync_disabled.png"
    public: static const RCDATA BTN_SYNC_DISABLED_PNG;
};

#endif // RESOURCES_H

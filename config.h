/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int viewontag          = 1;        /* switch view on tag switch */
static const char *fonts[]          = {
    "SauceCodePro Nerd Font:size=13",
    "Noto Color Emoji:size=10",
    "PingFang SC:size=12"
};
static const char col_dark[]        = "#0F1419";
static const char col_gray1[]       = "#212733";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_purple1[]     = "#A37ACC";
static const char col_purple2[]     = "#D4BFFF";
static const char col_blue[]        = "#80D4FF";
static const char col_cyan[]        = "#005577";
static const unsigned int baralpha = 0xd0;
static const unsigned int titlealpha = 0xf0;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_dark, col_purple1, col_purple1 },
	[SchemeTitle]  = { col_blue, col_gray1, col_purple2 },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, titlealpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
	[SchemeTitle] = { OPAQUE, titlealpha, borderalpha },
};
static const XPoint stickyicon[]    = { {0,0}, {4,0}, {4,8}, {2,6}, {0,8}, {0,0} }; /* represents the icon as an array of vertices */
static const XPoint stickyiconbb    = {4,8};    /* defines the bottom right corner of the polygon's bounding box (speeds up scaling) */

/* tagging */
static const char *tags[] = { " ", " ", " ", " ", " ", " ", " ", " ", " " };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                 instance    title       tags mask     isfloating   monitor */
	{ "Gimp",                NULL,       NULL,       0,            1,           -1 },
	{ "Google-chrome",       NULL,       NULL,       1,            0,           -1 },
	{ "Alacritty",           NULL,       NULL,       1 << 1,       0,           -1 },
    { "netease-cloud-music", NULL,       NULL,       1 << 2,       1,           -1 },
	{ "Clashy",              NULL,       NULL,       1 << 3,       1,           -1 },
	{ "listen1",             NULL,       NULL,       1 << 7,       1,           -1 },
	{ "GoldenDict",          NULL,       NULL,       1 << 8,       1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "拓",       tile },    /* first entry is default */
	{ " ",       NULL },    /* no layout function means floating behavior */
	{ " ",       monocle },
    { " ",       deck },
    { NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd1[]  = { "alacritty", NULL };
static const char *termcmd2[]  = { "st", NULL };
static const char *browsercmd[]  = { "google-chrome-stable", NULL };
static const char *trayercmd[]  = { "/home/ustc-1314/.local/share/dwm/t-toggle.sh",  NULL };
static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%",    NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%",    NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "@DEFAULT_SINK@", "toggle", NULL };
static const char *screencmd[] = { "flameshot", "gui", NULL };
static const char *switchbar[] = { "/home/ustc-1314/.local/share/dwm/switch-bar.sh", NULL };
static const char *rofiapp[] = { "rofi", "-show", "combi", "-theme", "dracula-icon", NULL };
static const char *rofirun[] = { "rofi", "-show", "run", "-theme", "dracula-grid", NULL };
static const char *roficlip[] = { "rofi", "-modi", "clipboard:greenclip print", "-show", "clipboard", "-run-command", "'{cmd}'", "-theme", "dracula-line", NULL };
static const char *rofikdeconnect[] = { "/home/ustc-1314/.local/share/dwm/rofi_kdeconnect.sh", NULL };
static const char *lockcmd[] = { "/home/ustc-1314/.local/share/dwm/lock.sh", NULL };

static Key keys[] = {
	/* modifier                     key              function        argument */
	{ MODKEY,                       XK_Return,       spawn,          {.v = termcmd2 } },
	{ MODKEY|ShiftMask,             XK_Return,       spawn,          {.v = termcmd1 } },
	{ MODKEY,                       XK_w,            spawn,          {.v = browsercmd } },
	{ MODKEY,                       XK_a,            spawn,          {.v = trayercmd } },
	{ MODKEY|ShiftMask,             XK_t,            spawn,          {.v = switchbar } },
	{ MODKEY,                       XK_slash,        spawn,          {.v = rofiapp } },
	{ MODKEY,                       XK_p,            spawn,          {.v = rofirun } },
	{ MODKEY,                       XK_c,            spawn,          {.v = roficlip } },
	{ MODKEY|ShiftMask,             XK_d,            spawn,          {.v = rofikdeconnect } },
	{ MODKEY,                       XK_b,            togglebar,      {0} },
    { MODKEY|ShiftMask,             XK_j,            rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,            rotatestack,    {.i = -1 } },
	{ MODKEY,                       XK_j,            focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,            focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,            incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,            incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,            setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,            setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_z,            zoom,           {0} },
	{ MODKEY,                       XK_Tab,          view,           {0} },
	{ MODKEY,                       XK_q,            killclient,     {0} },
	{ MODKEY,                       XK_t,            setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,            setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,            setlayout,      {.v = &layouts[2]} },
    { MODKEY,                       XK_y,            setlayout,      {.v = &layouts[3]} },
    { MODKEY|ControlMask,           XK_comma,        cyclelayout,    {.i = -1 } },
    { MODKEY|ControlMask,           XK_period,       cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_semicolon,    setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_semicolon,    togglefloating, {0} },
    { MODKEY|ShiftMask,             XK_m,            togglefullscr,  {0} },
    { MODKEY,                       XK_s,            togglesticky,   {0} },
	{ MODKEY,                       XK_0,            view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,            tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_bracketleft,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_bracketright, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_bracketleft,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_bracketright, tagmon,         {.i = +1 } },
    { MODKEY,                       XK_comma,        viewtoleft,     {0} },
    { MODKEY,                       XK_period,       viewtoright,    {0} },
    { MODKEY|ShiftMask,             XK_comma,        tagtoleft,      {0} },
    { MODKEY|ShiftMask,             XK_period,       tagtoright,     {0} },
    { MODKEY|Mod1Mask,              XK_0,            togglegaps,     {0} },
    { MODKEY|Mod1Mask|ControlMask,  XK_0,            defaultgaps,    {0} },
    { MODKEY|Mod1Mask,              XK_equal,        incrgaps,       {.i = +1 } },
    { MODKEY|Mod1Mask,              XK_minus,        incrgaps,       {.i = -1 } },
    { MODKEY|Mod1Mask|ShiftMask,    XK_equal,        incrogaps,      {.i = +1 } },
 	{ MODKEY|Mod1Mask|ShiftMask,    XK_minus,        incrogaps,      {.i = -1 } },
    { MODKEY|Mod1Mask|ControlMask,  XK_equal,        incrigaps,      {.i = +1 } },
    { MODKEY|Mod1Mask|ControlMask,  XK_minus,        incrigaps,      {.i = -1 } },
	TAGKEYS(                        XK_1,                         0)
	TAGKEYS(                        XK_2,                         1)
	TAGKEYS(                        XK_3,                         2)
	TAGKEYS(                        XK_4,                         3)
	TAGKEYS(                        XK_5,                         4)
	TAGKEYS(                        XK_6,                         5)
	TAGKEYS(                        XK_7,                         6)
	TAGKEYS(                        XK_8,                         7)
	TAGKEYS(                        XK_9,                         8)
	{ MODKEY|ShiftMask,             XK_q,           quit,           {0} },
	{ 0,                            XK_Print,       spawn,          {.v = screencmd } },
    { 0,                       XF86XK_AudioLowerVolume, spawn, {.v = downvol } },
	{ 0,                       XF86XK_AudioMute, spawn, {.v = mutevol } },
	{ 0,                       XF86XK_AudioRaiseVolume, spawn, {.v = upvol   } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd2 } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};


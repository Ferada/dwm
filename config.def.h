/* See LICENSE file for copyright and license details. */

#include <XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "unifont:size=12" };
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { "#bbbbbb", "#111111", "#333333" },
	[SchemeSel]  = { "#eeeeee", "#222222", "#222222" },
};
static const char *alsa_card        = "default";
static const char *alsa_selem_name  = "Master";

/* tagging */
static const char *tags[] = { "一", "二", "三", "四", "五", "六", "七", "八", "九" };

static const Rule rules[] = {
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       1 << 5,       0,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 1,       0,           -1 },
	{ "feh",      NULL,       NULL,       0,            1,           -1 },
	{ "MPlayer",  NULL,       NULL,       0,            1,           -1 },
	{ "mplayer2", NULL,       NULL,       0,            1,           -1 },
	{ "mpv",      NULL,       NULL,       0,            1,           -1 },
};

/* layout(s) */
static const float mfact      = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const int resizehints  = 0;    /* 1 means respect size hints in tiled resizals */
static const int layoutaxis[] = {
	1,    /* layout axis: 1 = x, 2 = y; negative values mirror the layout */
	2,    /* master axis: 1 = x (left to right), 2 = y (top to bottom), 3 = z (monocle) */
	2,    /* stack  axis: 1 = x (left to right), 2 = y (top to bottom), 3 = z (monocle) */
};

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
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

void
exec(const Arg *arg) {
	if(dpy)
		close(ConnectionNumber(dpy));
	setsid();
	execvp(((char **)arg->v)[0], (char **)arg->v);
	fprintf(stderr, "dwm: execvp %s", ((char **)arg->v)[0]);
	perror(" failed");
	exit(EXIT_SUCCESS);
}

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]            = {"dmenu_run", "-b", "-m", dmenumon, NULL};
static const char *termcmd[]             = {"st", NULL};
static const char *pausecmd[]            = {"mpc", "toggle", NULL};
static const char *stopcmd[]             = {"mpc", "stop", NULL};
static const char *prevcmd[]             = {"mpc", "prev", NULL};
static const char *nextcmd[]             = {"mpc", "next", NULL};
static const char *clipmenucmd[]         = {"clipmenu", NULL};
static const char *actprimarycmd[]       = {"act", "-s", "primary", NULL};
static const char *actclipboardcmd[]     = {"act", "-s", "clipboard", NULL};
static const char *switchselectionscmd[] = {"xsel", "-x", NULL};
static const char *switchclipboardcmd[]  = {"sh", "-c", "PRIMARY=`xsel -op`; xsel -ob | xsel -ip; echo \"$PRIMARY\" | xsel -ib", NULL};
static const char *clearselectionscmd[]  = {"sh", "-c", "xsel -cp; xsel -cb; xsel -cs", NULL};
static const char *lockscreencmd[]       = {"sh", "-c", "slock slack away; slack auto", NULL};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_Tab,    rotatelayoutaxis, {.i = 0} },    /* 0 = layout axis */
	{ MODKEY|ControlMask,           XK_Tab,    rotatelayoutaxis, {.i = 1} },    /* 1 = master axis */
	{ MODKEY|ControlMask|ShiftMask, XK_Tab,    rotatelayoutaxis, {.i = 2} },    /* 2 = stack axis */
	{ MODKEY|ControlMask,           XK_Return, mirrorlayout,     {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{MODKEY|ControlMask|ShiftMask,  XK_q,      restart,        {0}},
	{MODKEY|ControlMask|Mod1Mask,   XK_q,      quit,           {0}},
	{0,               XF86XK_AudioRaiseVolume, alsaset,        {.i = 5}},
	{ShiftMask,       XF86XK_AudioRaiseVolume, alsaset,        {.i = 1}},
	{0,               XF86XK_AudioLowerVolume, alsaset,        {.i = -5}},
	{ShiftMask,       XF86XK_AudioLowerVolume, alsaset,        {.i = -1}},
	{0,                      XF86XK_AudioMute, alsaset,        {.i = 0}},
	{0,                      XF86XK_AudioPlay, spawn,          {.v = pausecmd}},
	{0,                      XF86XK_AudioStop, spawn,          {.v = stopcmd}},
	{0,                      XF86XK_AudioPrev, spawn,          {.v = prevcmd}},
	{0,                      XF86XK_AudioNext, spawn,          {.v = nextcmd}},
	{ MODKEY,                       XK_a,      spawn,          {.v = clipmenucmd}},
	{ MODKEY,                       XK_s,      spawn,          {.v = actprimarycmd}},
	{ MODKEY|ShiftMask,             XK_s,      spawn,          {.v = actclipboardcmd}},
	{ MODKEY,                       XK_z,      spawn,          {.v = switchclipboardcmd}},
	{ MODKEY|ControlMask,           XK_z,      spawn,          {.v = switchselectionscmd}},
	{ MODKEY|ShiftMask,             XK_z,      spawn,          {.v = clearselectionscmd}},
	{ MODKEY|ShiftMask,             XK_l,      spawn,          {.v = lockscreencmd}},
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};


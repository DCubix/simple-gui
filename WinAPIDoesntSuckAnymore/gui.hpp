#ifndef GUI_HPP
#define GUI_HPP

#include <Windows.h>

#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include <iostream>

// thanks MaGetzUb
#pragma comment(linker,"/manifestdependency:\"type='win32' "\
                       "name='Microsoft.Windows.Common-Controls' "\
                       "version='6.0.0.0' "\
                       "processorArchitecture='*' "\
                       "publicKeyToken='6595b64144ccf1df' "\
                       "language='*' "\
                       "\"")

// thanks MaGetzUb
#pragma region utils
const char* GetWMName(UINT msg) {
	switch (msg) {
		case 0: return "WM_NULL";
		case 1: return "WM_CREATE";
		case 2: return "WM_DESTROY";
		case 3: return "WM_MOVE";
		case 5: return "WM_SIZE";
		case 6: return "WM_ACTIVATE";
		case 7: return "WM_SETFOCUS";
		case 8: return "WM_KILLFOCUS";
		case 10: return "WM_ENABLE";
		case 11: return "WM_SETREDRAW";
		case 12: return "WM_SETTEXT";
		case 13: return "WM_GETTEXT";
		case 14: return "WM_GETTEXTLENGTH";
		case 15: return "WM_PAINT";
		case 16: return "WM_CLOSE";
		case 17: return "WM_QUERYENDSESSION";
		case 18: return "WM_QUIT";
		case 19: return "WM_QUERYOPEN";
		case 20: return "WM_ERASEBKGND";
		case 21: return "WM_SYSCOLORCHANGE";
		case 22: return "WM_ENDSESSION";
		case 24: return "WM_SHOWWINDOW";
		case 25: return "WM_CTLCOLOR";
		case 26: return "WM_WININICHANGE";
		case 27: return "WM_DEVMODECHANGE";
		case 28: return "WM_ACTIVATEAPP";
		case 29: return "WM_FONTCHANGE";
		case 30: return "WM_TIMECHANGE";
		case 31: return "WM_CANCELMODE";
		case 32: return "WM_SETCURSOR";
		case 33: return "WM_MOUSEACTIVATE";
		case 34: return "WM_CHILDACTIVATE";
		case 35: return "WM_QUEUESYNC";
		case 36: return "WM_GETMINMAXINFO";
		case 38: return "WM_PAINTICON";
		case 39: return "WM_ICONERASEBKGND";
		case 40: return "WM_NEXTDLGCTL";
		case 42: return "WM_SPOOLERSTATUS";
		case 43: return "WM_DRAWITEM";
		case 44: return "WM_MEASUREITEM";
		case 45: return "WM_DELETEITEM";
		case 46: return "WM_VKEYTOITEM";
		case 47: return "WM_CHARTOITEM";
		case 48: return "WM_SETFONT";
		case 49: return "WM_GETFONT";
		case 50: return "WM_SETHOTKEY";
		case 51: return "WM_GETHOTKEY";
		case 55: return "WM_QUERYDRAGICON";
		case 57: return "WM_COMPAREITEM";
		case 61: return "WM_GETOBJECT";
		case 65: return "WM_COMPACTING";
		case 68: return "WM_COMMNOTIFY";
		case 70: return "WM_WINDOWPOSCHANGING";
		case 71: return "WM_WINDOWPOSCHANGED";
		case 72: return "WM_POWER";
		case 73: return "WM_COPYGLOBALDATA";
		case 74: return "WM_COPYDATA";
		case 75: return "WM_CANCELJOURNAL";
		case 78: return "WM_NOTIFY";
		case 80: return "WM_INPUTLANGCHANGEREQUEST";
		case 81: return "WM_INPUTLANGCHANGE";
		case 82: return "WM_TCARD";
		case 83: return "WM_HELP";
		case 84: return "WM_USERCHANGED";
		case 85: return "WM_NOTIFYFORMAT";
		case 123: return "WM_CONTEXTMENU";
		case 124: return "WM_STYLECHANGING";
		case 125: return "WM_STYLECHANGED";
		case 126: return "WM_DISPLAYCHANGE";
		case 127: return "WM_GETICON";
		case 128: return "WM_SETICON";
		case 129: return "WM_NCCREATE";
		case 130: return "WM_NCDESTROY";
		case 131: return "WM_NCCALCSIZE";
		case 132: return "WM_NCHITTEST";
		case 133: return "WM_NCPAINT";
		case 134: return "WM_NCACTIVATE";
		case 135: return "WM_GETDLGCODE";
		case 136: return "WM_SYNCPAINT";
		case 160: return "WM_NCMOUSEMOVE";
		case 161: return "WM_NCLBUTTONDOWN";
		case 162: return "WM_NCLBUTTONUP";
		case 163: return "WM_NCLBUTTONDBLCLK";
		case 164: return "WM_NCRBUTTONDOWN";
		case 165: return "WM_NCRBUTTONUP";
		case 166: return "WM_NCRBUTTONDBLCLK";
		case 167: return "WM_NCMBUTTONDOWN";
		case 168: return "WM_NCMBUTTONUP";
		case 169: return "WM_NCMBUTTONDBLCLK";
		case 171: return "WM_NCXBUTTONDOWN";
		case 172: return "WM_NCXBUTTONUP";
		case 173: return "WM_NCXBUTTONDBLCLK";
		case 176: return "EM_GETSEL";
		case 177: return "EM_SETSEL";
		case 178: return "EM_GETRECT";
		case 179: return "EM_SETRECT";
		case 180: return "EM_SETRECTNP";
		case 181: return "EM_SCROLL";
		case 182: return "EM_LINESCROLL";
		case 183: return "EM_SCROLLCARET";
		case 185: return "EM_GETMODIFY";
		case 187: return "EM_SETMODIFY";
		case 188: return "EM_GETLINECOUNT";
		case 189: return "EM_LINEINDEX";
		case 190: return "EM_SETHANDLE";
		case 191: return "EM_GETHANDLE";
		case 192: return "EM_GETTHUMB";
		case 193: return "EM_LINELENGTH";
		case 194: return "EM_REPLACESEL";
		case 195: return "EM_SETFONT";
		case 196: return "EM_GETLINE";
		case 197: return "EM_LIMITTEXT";
			//case 197: return "EM_SETLIMITTEXT";
		case 198: return "EM_CANUNDO";
		case 199: return "EM_UNDO";
		case 200: return "EM_FMTLINES";
		case 201: return "EM_LINEFROMCHAR";
		case 202: return "EM_SETWORDBREAK";
		case 203: return "EM_SETTABSTOPS";
		case 204: return "EM_SETPASSWORDCHAR";
		case 205: return "EM_EMPTYUNDOBUFFER";
		case 206: return "EM_GETFIRSTVISIBLELINE";
		case 207: return "EM_SETREADONLY";
		case 209: return "EM_SETWORDBREAKPROC";
			//case 209: return "EM_GETWORDBREAKPROC";
		case 210: return "EM_GETPASSWORDCHAR";
		case 211: return "EM_SETMARGINS";
		case 212: return "EM_GETMARGINS";
		case 213: return "EM_GETLIMITTEXT";
		case 214: return "EM_POSFROMCHAR";
		case 215: return "EM_CHARFROMPOS";
		case 216: return "EM_SETIMESTATUS";
		case 217: return "EM_GETIMESTATUS";
		case 224: return "SBM_SETPOS";
		case 225: return "SBM_GETPOS";
		case 226: return "SBM_SETRANGE";
		case 227: return "SBM_GETRANGE";
		case 228: return "SBM_ENABLE_ARROWS";
		case 230: return "SBM_SETRANGEREDRAW";
		case 233: return "SBM_SETSCROLLINFO";
		case 234: return "SBM_GETSCROLLINFO";
		case 235: return "SBM_GETSCROLLBARINFO";
		case 240: return "BM_GETCHECK";
		case 241: return "BM_SETCHECK";
		case 242: return "BM_GETSTATE";
		case 243: return "BM_SETSTATE";
		case 244: return "BM_SETSTYLE";
		case 245: return "BM_CLICK";
		case 246: return "BM_GETIMAGE";
		case 247: return "BM_SETIMAGE";
		case 248: return "BM_SETDONTCLICK";
		case 255: return "WM_INPUT";
		case 256: return "WM_KEYDOWN";
			//case 256: return "WM_KEYFIRST";
		case 257: return "WM_KEYUP";
		case 258: return "WM_CHAR";
		case 259: return "WM_DEADCHAR";
		case 260: return "WM_SYSKEYDOWN";
		case 261: return "WM_SYSKEYUP";
		case 262: return "WM_SYSCHAR";
		case 263: return "WM_SYSDEADCHAR";
		case 265: return "WM_UNICHAR / WM_KEYLAST";
			//case 265: return "WM_WNT_CONVERTREQUESTEX";
		case 266: return "WM_CONVERTREQUEST";
		case 267: return "WM_CONVERTRESULT";
		case 268: return "WM_INTERIM";
		case 269: return "WM_IME_STARTCOMPOSITION";
		case 270: return "WM_IME_ENDCOMPOSITION";
		case 271: return "WM_IME_COMPOSITION";
			//case 271: return "WM_IME_KEYLAST";
		case 272: return "WM_INITDIALOG";
		case 273: return "WM_COMMAND";
		case 274: return "WM_SYSCOMMAND";
		case 275: return "WM_TIMER";
		case 276: return "WM_HSCROLL";
		case 277: return "WM_VSCROLL";
		case 278: return "WM_INITMENU";
		case 279: return "WM_INITMENUPOPUP";
		case 280: return "WM_SYSTIMER";
		case 287: return "WM_MENUSELECT";
		case 288: return "WM_MENUCHAR";
		case 289: return "WM_ENTERIDLE";
		case 290: return "WM_MENURBUTTONUP";
		case 291: return "WM_MENUDRAG";
		case 292: return "WM_MENUGETOBJECT";
		case 293: return "WM_UNINITMENUPOPUP";
		case 294: return "WM_MENUCOMMAND";
		case 295: return "WM_CHANGEUISTATE";
		case 296: return "WM_UPDATEUISTATE";
		case 297: return "WM_QUERYUISTATE";
		case 306: return "WM_CTLCOLORMSGBOX";
		case 307: return "WM_CTLCOLOREDIT";
		case 308: return "WM_CTLCOLORLISTBOX";
		case 309: return "WM_CTLCOLORBTN";
		case 310: return "WM_CTLCOLORDLG";
		case 311: return "WM_CTLCOLORSCROLLBAR";
		case 312: return "WM_CTLCOLORSTATIC";
		case 512: return "WM_MOUSEFIRST";
			//case 512: return "WM_MOUSEMOVE";
		case 513: return "WM_LBUTTONDOWN";
		case 514: return "WM_LBUTTONUP";
		case 515: return "WM_LBUTTONDBLCLK";
		case 516: return "WM_RBUTTONDOWN";
		case 517: return "WM_RBUTTONUP";
		case 518: return "WM_RBUTTONDBLCLK";
		case 519: return "WM_MBUTTONDOWN";
		case 520: return "WM_MBUTTONUP";
		case 521: return "WM_MBUTTONDBLCLK";
			//case 521: return "WM_MOUSELAST";
		case 522: return "WM_MOUSEWHEEL";
		case 523: return "WM_XBUTTONDOWN";
		case 524: return "WM_XBUTTONUP";
		case 525: return "WM_XBUTTONDBLCLK";
		case 526: return "WM_MOUSEHWHEEL";
		case 528: return "WM_PARENTNOTIFY";
		case 529: return "WM_ENTERMENULOOP";
		case 530: return "WM_EXITMENULOOP";
		case 531: return "WM_NEXTMENU";
		case 532: return "WM_SIZING";
		case 533: return "WM_CAPTURECHANGED";
		case 534: return "WM_MOVING";
		case 536: return "WM_POWERBROADCAST";
		case 537: return "WM_DEVICECHANGE";
		case 544: return "WM_MDICREATE";
		case 545: return "WM_MDIDESTROY";
		case 546: return "WM_MDIACTIVATE";
		case 547: return "WM_MDIRESTORE";
		case 548: return "WM_MDINEXT";
		case 549: return "WM_MDIMAXIMIZE";
		case 550: return "WM_MDITILE";
		case 551: return "WM_MDICASCADE";
		case 552: return "WM_MDIICONARRANGE";
		case 553: return "WM_MDIGETACTIVE";
		case 560: return "WM_MDISETMENU";
		case 561: return "WM_ENTERSIZEMOVE";
		case 562: return "WM_EXITSIZEMOVE";
		case 563: return "WM_DROPFILES";
		case 564: return "WM_MDIREFRESHMENU";
		case 640: return "WM_IME_REPORT";
		case 641: return "WM_IME_SETCONTEXT";
		case 642: return "WM_IME_NOTIFY";
		case 643: return "WM_IME_CONTROL";
		case 644: return "WM_IME_COMPOSITIONFULL";
		case 645: return "WM_IME_SELECT";
		case 646: return "WM_IME_CHAR";
		case 648: return "WM_IME_REQUEST";
		case 656: return "WM_IMEKEYDOWN";
			//case 656: return "WM_IME_KEYDOWN";
		case 657: return "WM_IMEKEYUP";
			//case 657: return "WM_IME_KEYUP";
		case 672: return "WM_NCMOUSEHOVER";
		case 673: return "WM_MOUSEHOVER";
		case 674: return "WM_NCMOUSELEAVE";
		case 675: return "WM_MOUSELEAVE";
		case 768: return "WM_CUT";
		case 769: return "WM_COPY";
		case 770: return "WM_PASTE";
		case 771: return "WM_CLEAR";
		case 772: return "WM_UNDO";
		case 773: return "WM_RENDERFORMAT";
		case 774: return "WM_RENDERALLFORMATS";
		case 775: return "WM_DESTROYCLIPBOARD";
		case 776: return "WM_DRAWCLIPBOARD";
		case 777: return "WM_PAINTCLIPBOARD";
		case 778: return "WM_VSCROLLCLIPBOARD";
		case 779: return "WM_SIZECLIPBOARD";
		case 780: return "WM_ASKCBFORMATNAME";
		case 781: return "WM_CHANGECBCHAIN";
		case 782: return "WM_HSCROLLCLIPBOARD";
		case 783: return "WM_QUERYNEWPALETTE";
		case 784: return "WM_PALETTEISCHANGING";
		case 785: return "WM_PALETTECHANGED";
		case 786: return "WM_HOTKEY";
		case 791: return "WM_PRINT";
		case 792: return "WM_PRINTCLIENT";
		case 793: return "WM_APPCOMMAND";
		case 856: return "WM_HANDHELDFIRST";
		case 863: return "WM_HANDHELDLAST";
		case 864: return "WM_AFXFIRST";
		case 895: return "WM_AFXLAST";
		case 896: return "WM_PENWINFIRST";
		case 897: return "WM_RCRESULT";
		case 898: return "WM_HOOKRCRESULT";
		case 899: return "WM_GLOBALRCCHANGE";
			//case 899: return "WM_PENMISCINFO";
		case 900: return "WM_SKB";
		case 901: return "WM_HEDITCTL";
			//case 901: return "WM_PENCTL";
		case 902: return "WM_PENMISC";
		case 903: return "WM_CTLINIT";
		case 904: return "WM_PENEVENT";
		case 911: return "WM_PENWINLAST";
		case 1024: return "DDM_SETFMT";
			//case 1024: return "DM_GETDEFID";
			//case 1024: return "NIN_SELECT";
			//case 1024: return "TBM_GETPOS";
			//case 1024: return "WM_PSD_PAGESETUPDLG";
			//case 1024: return "WM_USER";
		case 1025: return "CBEM_INSERTITEMA";
			//case 1025: return "DDM_DRAW";
			//case 1025: return "DM_SETDEFID";
			//case 1025: return "HKM_SETHOTKEY";
			//case 1025: return "PBM_SETRANGE";
			//case 1025: return "RB_INSERTBANDA";
			//case 1025: return "SB_SETTEXTA";
			//case 1025: return "TB_ENABLEBUTTON";
			//case 1025: return "TBM_GETRANGEMIN";
			//case 1025: return "TTM_ACTIVATE";
			//case 1025: return "WM_CHOOSEFONT_GETLOGFONT";
			//case 1025: return "WM_PSD_FULLPAGERECT";
		case 1026: return "CBEM_SETIMAGELIST";
			//case 1026: return "DDM_CLOSE";
			//case 1026: return "DM_REPOSITION";
			//case 1026: return "HKM_GETHOTKEY";
			//case 1026: return "PBM_SETPOS";
			//case 1026: return "RB_DELETEBAND";
			//case 1026: return "SB_GETTEXTA";
			//case 1026: return "TB_CHECKBUTTON";
			//case 1026: return "TBM_GETRANGEMAX";
			//case 1026: return "WM_PSD_MINMARGINRECT";
		case 1027: return "CBEM_GETIMAGELIST";
			//case 1027: return "DDM_BEGIN";
			//case 1027: return "HKM_SETRULES";
			//case 1027: return "PBM_DELTAPOS";
			//case 1027: return "RB_GETBARINFO";
			//case 1027: return "SB_GETTEXTLENGTHA";
			//case 1027: return "TBM_GETTIC";
			//case 1027: return "TB_PRESSBUTTON";
			//case 1027: return "TTM_SETDELAYTIME";
			//case 1027: return "WM_PSD_MARGINRECT";
		case 1028: return "CBEM_GETITEMA";
			//case 1028: return "DDM_END";
			//case 1028: return "PBM_SETSTEP";
			//case 1028: return "RB_SETBARINFO";
			//case 1028: return "SB_SETPARTS";
			//case 1028: return "TB_HIDEBUTTON";
			//case 1028: return "TBM_SETTIC";
			//case 1028: return "TTM_ADDTOOLA";
			//case 1028: return "WM_PSD_GREEKTEXTRECT";
		case 1029: return "CBEM_SETITEMA";
			//case 1029: return "PBM_STEPIT";
			//case 1029: return "TB_INDETERMINATE";
			//case 1029: return "TBM_SETPOS";
			//case 1029: return "TTM_DELTOOLA";
			//case 1029: return "WM_PSD_ENVSTAMPRECT";
		case 1030: return "CBEM_GETCOMBOCONTROL";
			//case 1030: return "PBM_SETRANGE32";
			//case 1030: return "RB_SETBANDINFOA";
			//case 1030: return "SB_GETPARTS";
			//case 1030: return "TB_MARKBUTTON";
			//case 1030: return "TBM_SETRANGE";
			//case 1030: return "TTM_NEWTOOLRECTA";
			//case 1030: return "WM_PSD_YAFULLPAGERECT";
		case 1031: return "CBEM_GETEDITCONTROL";
			//case 1031: return "PBM_GETRANGE";
			//case 1031: return "RB_SETPARENT";
			//case 1031: return "SB_GETBORDERS";
			//case 1031: return "TBM_SETRANGEMIN";
			//case 1031: return "TTM_RELAYEVENT";
		case 1032: return "CBEM_SETEXSTYLE";
			//case 1032: return "PBM_GETPOS";
			//case 1032: return "RB_HITTEST";
			//case 1032: return "SB_SETMINHEIGHT";
			//case 1032: return "TBM_SETRANGEMAX";
			//case 1032: return "TTM_GETTOOLINFOA";
		case 1033: return "CBEM_GETEXSTYLE";
			//case 1033: return "CBEM_GETEXTENDEDSTYLE";
			//case 1033: return "PBM_SETBARCOLOR";
			//case 1033: return "RB_GETRECT";
			//case 1033: return "SB_SIMPLE";
			//case 1033: return "TB_ISBUTTONENABLED";
			//case 1033: return "TBM_CLEARTICS";
			//case 1033: return "TTM_SETTOOLINFOA";
		case 1034: return "CBEM_HASEDITCHANGED";
			//case 1034: return "RB_INSERTBANDW";
			//case 1034: return "SB_GETRECT";
			//case 1034: return "TB_ISBUTTONCHECKED";
			//case 1034: return "TBM_SETSEL";
			//case 1034: return "TTM_HITTESTA";
			//case 1034: return "WIZ_QUERYNUMPAGES";
		case 1035: return "CBEM_INSERTITEMW";
			//case 1035: return "RB_SETBANDINFOW";
			//case 1035: return "SB_SETTEXTW";
			//case 1035: return "TB_ISBUTTONPRESSED";
			//case 1035: return "TBM_SETSELSTART";
			//case 1035: return "TTM_GETTEXTA";
			//case 1035: return "WIZ_NEXT";
		case 1036: return "CBEM_SETITEMW";
			//case 1036: return "RB_GETBANDCOUNT";
			//case 1036: return "SB_GETTEXTLENGTHW";
			//case 1036: return "TB_ISBUTTONHIDDEN";
			//case 1036: return "TBM_SETSELEND";
			//case 1036: return "TTM_UPDATETIPTEXTA";
			//case 1036: return "WIZ_PREV";
		case 1037: return "CBEM_GETITEMW";
			//case 1037: return "RB_GETROWCOUNT";
			//case 1037: return "SB_GETTEXTW";
			//case 1037: return "TB_ISBUTTONINDETERMINATE";
			//case 1037: return "TTM_GETTOOLCOUNT";
		case 1038: return "CBEM_SETEXTENDEDSTYLE";
			//case 1038: return "RB_GETROWHEIGHT";
			//case 1038: return "SB_ISSIMPLE";
			//case 1038: return "TB_ISBUTTONHIGHLIGHTED";
			//case 1038: return "TBM_GETPTICS";
			//case 1038: return "TTM_ENUMTOOLSA";
		case 1039: return "SB_SETICON";
			//case 1039: return "TBM_GETTICPOS";
			//case 1039: return "TTM_GETCURRENTTOOLA";
		case 1040: return "RB_IDTOINDEX";
			//case 1040: return "SB_SETTIPTEXTA";
			//case 1040: return "TBM_GETNUMTICS";
			//case 1040: return "TTM_WINDOWFROMPOINT";
		case 1041: return "RB_GETTOOLTIPS";
			//case 1041: return "SB_SETTIPTEXTW";
			//case 1041: return "TBM_GETSELSTART";
			//case 1041: return "TB_SETSTATE";
			//case 1041: return "TTM_TRACKACTIVATE";
		case 1042: return "RB_SETTOOLTIPS";
			//case 1042: return "SB_GETTIPTEXTA";
			//case 1042: return "TB_GETSTATE";
			//case 1042: return "TBM_GETSELEND";
			//case 1042: return "TTM_TRACKPOSITION";
		case 1043: return "RB_SETBKCOLOR";
			//case 1043: return "SB_GETTIPTEXTW";
			//case 1043: return "TB_ADDBITMAP";
			//case 1043: return "TBM_CLEARSEL";
			//case 1043: return "TTM_SETTIPBKCOLOR";
		case 1044: return "RB_GETBKCOLOR";
			//case 1044: return "SB_GETICON";
			//case 1044: return "TB_ADDBUTTONSA";
			//case 1044: return "TBM_SETTICFREQ";
			//case 1044: return "TTM_SETTIPTEXTCOLOR";
		case 1045: return "RB_SETTEXTCOLOR";
			//case 1045: return "TB_INSERTBUTTONA";
			//case 1045: return "TBM_SETPAGESIZE";
			//case 1045: return "TTM_GETDELAYTIME";
		case 1046: return "RB_GETTEXTCOLOR";
			//case 1046: return "TB_DELETEBUTTON";
			//case 1046: return "TBM_GETPAGESIZE";
			//case 1046: return "TTM_GETTIPBKCOLOR";
		case 1047: return "RB_SIZETORECT";
			//case 1047: return "TB_GETBUTTON";
			//case 1047: return "TBM_SETLINESIZE";
			//case 1047: return "TTM_GETTIPTEXTCOLOR";
		case 1048: return "RB_BEGINDRAG";
			//case 1048: return "TB_BUTTONCOUNT";
			//case 1048: return "TBM_GETLINESIZE";
			//case 1048: return "TTM_SETMAXTIPWIDTH";
		case 1049: return "RB_ENDDRAG";
			//case 1049: return "TB_COMMANDTOINDEX";
			//case 1049: return "TBM_GETTHUMBRECT";
			//case 1049: return "TTM_GETMAXTIPWIDTH";
		case 1050: return "RB_DRAGMOVE";
			//case 1050: return "TBM_GETCHANNELRECT";
			//case 1050: return "TB_SAVERESTOREA";
			//case 1050: return "TTM_SETMARGIN";
		case 1051: return "RB_GETBARHEIGHT";
			//case 1051: return "TB_CUSTOMIZE";
			//case 1051: return "TBM_SETTHUMBLENGTH";
			//case 1051: return "TTM_GETMARGIN";
		case 1052: return "RB_GETBANDINFOW";
			//case 1052: return "TB_ADDSTRINGA";
			//case 1052: return "TBM_GETTHUMBLENGTH";
			//case 1052: return "TTM_POP";
		case 1053: return "RB_GETBANDINFOA";
			//case 1053: return "TB_GETITEMRECT";
			//case 1053: return "TBM_SETTOOLTIPS";
			//case 1053: return "TTM_UPDATE";
		case 1054: return "RB_MINIMIZEBAND";
			//case 1054: return "TB_BUTTONSTRUCTSIZE";
			//case 1054: return "TBM_GETTOOLTIPS";
			//case 1054: return "TTM_GETBUBBLESIZE";
		case 1055: return "RB_MAXIMIZEBAND";
			//case 1055: return "TBM_SETTIPSIDE";
			//case 1055: return "TB_SETBUTTONSIZE";
			//case 1055: return "TTM_ADJUSTRECT";
		case 1056: return "TBM_SETBUDDY";
			//case 1056: return "TB_SETBITMAPSIZE";
			//case 1056: return "TTM_SETTITLEA";
		case 1057: return "MSG_FTS_JUMP_VA";
			//case 1057: return "TB_AUTOSIZE";
			//case 1057: return "TBM_GETBUDDY";
			//case 1057: return "TTM_SETTITLEW";
		case 1058: return "RB_GETBANDBORDERS";
		case 1059: return "MSG_FTS_JUMP_QWORD";
			//case 1059: return "RB_SHOWBAND";
			//case 1059: return "TB_GETTOOLTIPS";
		case 1060: return "MSG_REINDEX_REQUEST";
			//case 1060: return "TB_SETTOOLTIPS";
		case 1061: return "MSG_FTS_WHERE_IS_IT";
			//case 1061: return "RB_SETPALETTE";
			//case 1061: return "TB_SETPARENT";
		case 1062: return "RB_GETPALETTE";
		case 1063: return "RB_MOVEBAND";
			//case 1063: return "TB_SETROWS";
		case 1064: return "TB_GETROWS";
		case 1065: return "TB_GETBITMAPFLAGS";
		case 1066: return "TB_SETCMDID";
		case 1067: return "RB_PUSHCHEVRON";
			//case 1067: return "TB_CHANGEBITMAP";
		case 1068: return "TB_GETBITMAP";
		case 1069: return "MSG_GET_DEFFONT";
			//case 1069: return "TB_GETBUTTONTEXTA";
		case 1070: return "TB_REPLACEBITMAP";
		case 1071: return "TB_SETINDENT";
		case 1072: return "TB_SETIMAGELIST";
		case 1073: return "TB_GETIMAGELIST";
		case 1074: return "TB_LOADIMAGES";
			/*case 1074: return "EM_CANPASTE";
			case 1074: return "TTM_ADDTOOLW";*/
		case 1075: return "EM_DISPLAYBAND";
			//case 1075: return "TB_GETRECT";
			//case 1075: return "TTM_DELTOOLW";
		case 1076: return "EM_EXGETSEL";
			//case 1076: return "TB_SETHOTIMAGELIST";
			//case 1076: return "TTM_NEWTOOLRECTW";
		case 1077: return "EM_EXLIMITTEXT";
			//case 1077: return "TB_GETHOTIMAGELIST";
			//case 1077: return "TTM_GETTOOLINFOW";
		case 1078: return "EM_EXLINEFROMCHAR";
			//case 1078: return "TB_SETDISABLEDIMAGELIST";
			//case 1078: return "TTM_SETTOOLINFOW";
		case 1079: return "EM_EXSETSEL";
			//case 1079: return "TB_GETDISABLEDIMAGELIST";
			//case 1079: return "TTM_HITTESTW";
		case 1080: return "EM_FINDTEXT";
			//case 1080: return "TB_SETSTYLE";
			//case 1080: return "TTM_GETTEXTW";
		case 1081: return "EM_FORMATRANGE";
			//case 1081: return "TB_GETSTYLE";
			//case 1081: return "TTM_UPDATETIPTEXTW";
		case 1082: return "EM_GETCHARFORMAT";
			//case 1082: return "TB_GETBUTTONSIZE";
			//case 1082: return "TTM_ENUMTOOLSW";
		case 1083: return "EM_GETEVENTMASK";
			//case 1083: return "TB_SETBUTTONWIDTH";
			//case 1083: return "TTM_GETCURRENTTOOLW";
		case 1084: return "EM_GETOLEINTERFACE";
			//case 1084: return "TB_SETMAXTEXTROWS";
		case 1085: return "EM_GETPARAFORMAT";
			//case 1085: return "TB_GETTEXTROWS";
		case 1086: return "EM_GETSELTEXT";
			//case 1086: return "TB_GETOBJECT";
		case 1087: return "EM_HIDESELECTION";
			//case 1087: return "TB_GETBUTTONINFOW";
		case 1088: return "EM_PASTESPECIAL";
			//case 1088: return "TB_SETBUTTONINFOW";
		case 1089: return "EM_REQUESTRESIZE";
			//case 1089: return "TB_GETBUTTONINFOA";
		case 1090: return "EM_SELECTIONTYPE";
			//case 1090: return "TB_SETBUTTONINFOA";
		case 1091: return "EM_SETBKGNDCOLOR";
			//case 1091: return "TB_INSERTBUTTONW";
		case 1092: return "EM_SETCHARFORMAT";
			//case 1092: return "TB_ADDBUTTONSW";
		case 1093: return "EM_SETEVENTMASK";
			//case 1093: return "TB_HITTEST";
		case 1094: return "EM_SETOLECALLBACK";
			//case 1094: return "TB_SETDRAWTEXTFLAGS";
		case 1095: return "EM_SETPARAFORMAT";
			//case 1095: return "TB_GETHOTITEM";
		case 1096: return "EM_SETTARGETDEVICE";
			//case 1096: return "TB_SETHOTITEM";
		case 1097: return "EM_STREAMIN";
			//case 1097: return "TB_SETANCHORHIGHLIGHT";
		case 1098: return "EM_STREAMOUT";
			//case 1098: return "TB_GETANCHORHIGHLIGHT";
		case 1099: return "EM_GETTEXTRANGE";
			//case 1099: return "TB_GETBUTTONTEXTW";
		case 1100: return "EM_FINDWORDBREAK";
			//case 1100: return "TB_SAVERESTOREW";
		case 1101: return "EM_SETOPTIONS";
			//case 1101: return "TB_ADDSTRINGW";
		case 1102: return "EM_GETOPTIONS";
			//case 1102: return "TB_MAPACCELERATORA";
		case 1103: return "EM_FINDTEXTEX";
			//case 1103: return "TB_GETINSERTMARK";
		case 1104: return "EM_GETWORDBREAKPROCEX";
			//case 1104: return "TB_SETINSERTMARK";
		case 1105: return "EM_SETWORDBREAKPROCEX";
			//case 1105: return "TB_INSERTMARKHITTEST";
		case 1106: return "EM_SETUNDOLIMIT";
			//case 1106: return "TB_MOVEBUTTON";
		case 1107: return "TB_GETMAXSIZE";
		case 1108: return "EM_REDO";
			//case 1108: return "TB_SETEXTENDEDSTYLE";
		case 1109: return "EM_CANREDO";
			//case 1109: return "TB_GETEXTENDEDSTYLE";
		case 1110: return "EM_GETUNDONAME";
			//case 1110: return "TB_GETPADDING";
		case 1111: return "EM_GETREDONAME";
			//case 1111: return "TB_SETPADDING";
		case 1112: return "EM_STOPGROUPTYPING";
			//case 1112: return "TB_SETINSERTMARKCOLOR";
		case 1113: return "EM_SETTEXTMODE";
			//case 1113: return "TB_GETINSERTMARKCOLOR";
		case 1114: return "EM_GETTEXTMODE";
			//case 1114: return "TB_MAPACCELERATORW";
		case 1115: return "EM_AUTOURLDETECT";
			//case 1115: return "TB_GETSTRINGW";
		case 1116: return "EM_GETAUTOURLDETECT";
			//case 1116: return "TB_GETSTRINGA";
		case 1117: return "EM_SETPALETTE";
		case 1118: return "EM_GETTEXTEX";
		case 1119: return "EM_GETTEXTLENGTHEX";
		case 1120: return "EM_SHOWSCROLLBAR";
		case 1121: return "EM_SETTEXTEX";
		case 1123: return "TAPI_REPLY";
		case 1124: return "ACM_OPENA";
			//case 1124: return "BFFM_SETSTATUSTEXTA";
			//case 1124: return "CDM_FIRST";
			//case 1124: return "CDM_GETSPEC";
			//case 1124: return "EM_SETPUNCTUATION";
			//case 1124: return "IPM_CLEARADDRESS";
			//case 1124: return "WM_CAP_UNICODE_START";
		case 1125: return "ACM_PLAY";
			//case 1125: return "BFFM_ENABLEOK";
			//case 1125: return "CDM_GETFILEPATH";
			//case 1125: return "EM_GETPUNCTUATION";
			//case 1125: return "IPM_SETADDRESS";
			//case 1125: return "PSM_SETCURSEL";
			//case 1125: return "UDM_SETRANGE";
			//case 1125: return "WM_CHOOSEFONT_SETLOGFONT";
		case 1126: return "ACM_STOP";
			//case 1126: return "BFFM_SETSELECTIONA";
			//case 1126: return "CDM_GETFOLDERPATH";
			//case 1126: return "EM_SETWORDWRAPMODE";
			//case 1126: return "IPM_GETADDRESS";
			//case 1126: return "PSM_REMOVEPAGE";
			//case 1126: return "UDM_GETRANGE";
			//case 1126: return "WM_CAP_SET_CALLBACK_ERRORW";
			//case 1126: return "WM_CHOOSEFONT_SETFLAGS";
		case 1127: return "ACM_OPENW";
			//case 1127: return "BFFM_SETSELECTIONW";
			//case 1127: return "CDM_GETFOLDERIDLIST";
			//case 1127: return "EM_GETWORDWRAPMODE";
			//case 1127: return "IPM_SETRANGE";
			//case 1127: return "PSM_ADDPAGE";
			//case 1127: return "UDM_SETPOS";
			//case 1127: return "WM_CAP_SET_CALLBACK_STATUSW";
		case 1128: return "BFFM_SETSTATUSTEXTW";
			//case 1128: return "CDM_SETCONTROLTEXT";
			//case 1128: return "EM_SETIMECOLOR";
			//case 1128: return "IPM_SETFOCUS";
			//case 1128: return "PSM_CHANGED";
			//case 1128: return "UDM_GETPOS";
		case 1129: return "CDM_HIDECONTROL";
			//case 1129: return "EM_GETIMECOLOR";
			//case 1129: return "IPM_ISBLANK";
			//case 1129: return "PSM_RESTARTWINDOWS";
			//case 1129: return "UDM_SETBUDDY";
		case 1130: return "CDM_SETDEFEXT";
			//case 1130: return "EM_SETIMEOPTIONS";
			//case 1130: return "PSM_REBOOTSYSTEM";
			//case 1130: return "UDM_GETBUDDY";
		case 1131: return "EM_GETIMEOPTIONS";
			//case 1131: return "PSM_CANCELTOCLOSE";
			//case 1131: return "UDM_SETACCEL";
		case 1132: return "EM_CONVPOSITION";
			//case 1132: return "EM_CONVPOSITION";
			//case 1132: return "PSM_QUERYSIBLINGS";
			//case 1132: return "UDM_GETACCEL";
		case 1133: return "MCIWNDM_GETZOOM";
			//case 1133: return "PSM_UNCHANGED";
			//case 1133: return "UDM_SETBASE";
		case 1134: return "PSM_APPLY";
			//case 1134: return "UDM_GETBASE";
		case 1135: return "PSM_SETTITLEA";
			//case 1135: return "UDM_SETRANGE32";
		case 1136: return "PSM_SETWIZBUTTONS";
			//case 1136: return "UDM_GETRANGE32";
			//case 1136: return "WM_CAP_DRIVER_GET_NAMEW";
		case 1137: return "PSM_PRESSBUTTON";
			//case 1137: return "UDM_SETPOS32";
			//case 1137: return "WM_CAP_DRIVER_GET_VERSIONW";
		case 1138: return "PSM_SETCURSELID";
			//case 1138: return "UDM_GETPOS32";
		case 1139: return "PSM_SETFINISHTEXTA";
		case 1140: return "PSM_GETTABCONTROL";
		case 1141: return "PSM_ISDIALOGMESSAGE";
		case 1142: return "MCIWNDM_REALIZE";
			//case 1142: return "PSM_GETCURRENTPAGEHWND";
		case 1143: return "MCIWNDM_SETTIMEFORMATA";
			//case 1143: return "PSM_INSERTPAGE";
		case 1144: return "EM_SETLANGOPTIONS";
			//case 1144: return "MCIWNDM_GETTIMEFORMATA";
			//case 1144: return "PSM_SETTITLEW";
			//case 1144: return "WM_CAP_FILE_SET_CAPTURE_FILEW";
		case 1145: return "EM_GETLANGOPTIONS";
			//case 1145: return "MCIWNDM_VALIDATEMEDIA";
			//case 1145: return "PSM_SETFINISHTEXTW";
			//case 1145: return "WM_CAP_FILE_GET_CAPTURE_FILEW";
		case 1146: return "EM_GETIMECOMPMODE";
		case 1147: return "EM_FINDTEXTW";
			//case 1147: return "MCIWNDM_PLAYTO";
			//case 1147: return "WM_CAP_FILE_SAVEASW";
		case 1148: return "EM_FINDTEXTEXW";
			//case 1148: return "MCIWNDM_GETFILENAMEA";
		case 1149: return "EM_RECONVERSION";
			//case 1149: return "MCIWNDM_GETDEVICEA";
			//case 1149: return "PSM_SETHEADERTITLEA";
			//case 1149: return "WM_CAP_FILE_SAVEDIBW";
		case 1150: return "EM_SETIMEMODEBIAS";
			//case 1150: return "MCIWNDM_GETPALETTE";
			//case 1150: return "PSM_SETHEADERTITLEW";
		case 1151: return "EM_GETIMEMODEBIAS";
			//case 1151: return "MCIWNDM_SETPALETTE";
			//case 1151: return "PSM_SETHEADERSUBTITLEA";
		case 1152: return "MCIWNDM_GETERRORA";
			//case 1152: return "PSM_SETHEADERSUBTITLEW";
		case 1153: return "PSM_HWNDTOINDEX";
		case 1154: return "PSM_INDEXTOHWND";
		case 1155: return "MCIWNDM_SETINACTIVETIMER";
			//case 1155: return "PSM_PAGETOINDEX";
		case 1156: return "PSM_INDEXTOPAGE";
		case 1157: return "DL_BEGINDRAG";
			//case 1157: return "MCIWNDM_GETINACTIVETIMER";
			//case 1157: return "PSM_IDTOINDEX";
		case 1158: return "DL_DRAGGING";
			//case 1158: return "PSM_INDEXTOID";
		case 1159: return "DL_DROPPED";
			//case 1159: return "PSM_GETRESULT";
		case 1160: return "DL_CANCELDRAG";
			//case 1160: return "PSM_RECALCPAGESIZES";
		case 1164: return "MCIWNDM_GET_SOURCE";
		case 1165: return "MCIWNDM_PUT_SOURCE";
		case 1166: return "MCIWNDM_GET_DEST";
		case 1167: return "MCIWNDM_PUT_DEST";
		case 1168: return "MCIWNDM_CAN_PLAY";
		case 1169: return "MCIWNDM_CAN_WINDOW";
		case 1170: return "MCIWNDM_CAN_RECORD";
		case 1171: return "MCIWNDM_CAN_SAVE";
		case 1172: return "MCIWNDM_CAN_EJECT";
		case 1173: return "MCIWNDM_CAN_CONFIG";
		case 1174: return "IE_GETINK";
			//case 1174: return "IE_MSGFIRST";
			//case 1174: return "MCIWNDM_PALETTEKICK";
		case 1175: return "IE_SETINK";
		case 1176: return "IE_GETPENTIP";
		case 1177: return "IE_SETPENTIP";
		case 1178: return "IE_GETERASERTIP";
		case 1179: return "IE_SETERASERTIP";
		case 1180: return "IE_GETBKGND";
		case 1181: return "IE_SETBKGND";
		case 1182: return "IE_GETGRIDORIGIN";
		case 1183: return "IE_SETGRIDORIGIN";
		case 1184: return "IE_GETGRIDPEN";
		case 1185: return "IE_SETGRIDPEN";
		case 1186: return "IE_GETGRIDSIZE";
		case 1187: return "IE_SETGRIDSIZE";
		case 1188: return "IE_GETMODE";
		case 1189: return "IE_SETMODE";
		case 1190: return "IE_GETINKRECT";
			//case 1190: return "WM_CAP_SET_MCI_DEVICEW";
		case 1191: return "WM_CAP_GET_MCI_DEVICEW";
		case 1204: return "WM_CAP_PAL_OPENW";
		case 1205: return "WM_CAP_PAL_SAVEW";
		case 1208: return "IE_GETAPPDATA";
		case 1209: return "IE_SETAPPDATA";
		case 1210: return "IE_GETDRAWOPTS";
		case 1211: return "IE_SETDRAWOPTS";
		case 1212: return "IE_GETFORMAT";
		case 1213: return "IE_SETFORMAT";
		case 1214: return "IE_GETINKINPUT";
		case 1215: return "IE_SETINKINPUT";
		case 1216: return "IE_GETNOTIFY";
		case 1217: return "IE_SETNOTIFY";
		case 1218: return "IE_GETRECOG";
		case 1219: return "IE_SETRECOG";
		case 1220: return "IE_GETSECURITY";
		case 1221: return "IE_SETSECURITY";
		case 1222: return "IE_GETSEL";
		case 1223: return "IE_SETSEL";
		case 1224: return "CDM_LAST";
			//case 1224: return "EM_SETBIDIOPTIONS";
			//case 1224: return "IE_DOCOMMAND";
			//case 1224: return "MCIWNDM_NOTIFYMODE";
		case 1225: return "EM_GETBIDIOPTIONS";
			//case 1225: return "IE_GETCOMMAND";
		case 1226: return "EM_SETTYPOGRAPHYOPTIONS";
			//case 1226: return "IE_GETCOUNT";
		case 1227: return "EM_GETTYPOGRAPHYOPTIONS";
			//case 1227: return "IE_GETGESTURE";
			//case 1227: return "MCIWNDM_NOTIFYMEDIA";
		case 1228: return "EM_SETEDITSTYLE";
			//case 1228: return "IE_GETMENU";
		case 1229: return "EM_GETEDITSTYLE";
			//case 1229: return "IE_GETPAINTDC";
			//case 1229: return "MCIWNDM_NOTIFYERROR";
		case 1230: return "IE_GETPDEVENT";
		case 1231: return "IE_GETSELCOUNT";
		case 1232: return "IE_GETSELITEMS";
		case 1233: return "IE_GETSTYLE";
		case 1243: return "MCIWNDM_SETTIMEFORMATW";
		case 1244: return "EM_OUTLINE";
			//case 1244: return "MCIWNDM_GETTIMEFORMATW";
		case 1245: return "EM_GETSCROLLPOS";
		case 1246: return "EM_SETSCROLLPOS";
			//case 1246: return "EM_SETSCROLLPOS";
		case 1247: return "EM_SETFONTSIZE";
		case 1248: return "EM_GETZOOM";
			//case 1248: return "MCIWNDM_GETFILENAMEW";
		case 1249: return "EM_SETZOOM";
			//case 1249: return "MCIWNDM_GETDEVICEW";
		case 1250: return "EM_GETVIEWKIND";
		case 1251: return "EM_SETVIEWKIND";
		case 1252: return "EM_GETPAGE";
			//case 1252: return "MCIWNDM_GETERRORW";
		case 1253: return "EM_SETPAGE";
		case 1254: return "EM_GETHYPHENATEINFO";
		case 1255: return "EM_SETHYPHENATEINFO";
		case 1259: return "EM_GETPAGEROTATE";
		case 1260: return "EM_SETPAGEROTATE";
		case 1261: return "EM_GETCTFMODEBIAS";
		case 1262: return "EM_SETCTFMODEBIAS";
		case 1264: return "EM_GETCTFOPENSTATUS";
		case 1265: return "EM_SETCTFOPENSTATUS";
		case 1266: return "EM_GETIMECOMPTEXT";
		case 1267: return "EM_ISIME";
		case 1268: return "EM_GETIMEPROPERTY";
		case 1293: return "EM_GETQUERYRTFOBJ";
		case 1294: return "EM_SETQUERYRTFOBJ";
		case 1536: return "FM_GETFOCUS";
		case 1537: return "FM_GETDRIVEINFOA";
		case 1538: return "FM_GETSELCOUNT";
		case 1539: return "FM_GETSELCOUNTLFN";
		case 1540: return "FM_GETFILESELA";
		case 1541: return "FM_GETFILESELLFNA";
		case 1542: return "FM_REFRESH_WINDOWS";
		case 1543: return "FM_RELOAD_EXTENSIONS";
		case 1553: return "FM_GETDRIVEINFOW";
		case 1556: return "FM_GETFILESELW";
		case 1557: return "FM_GETFILESELLFNW";
		case 1625: return "WLX_WM_SAS";
		case 2024: return "SM_GETSELCOUNT";
			//case 2024: return "UM_GETSELCOUNT";
			//case 2024: return "WM_CPL_LAUNCH";
		case 2025: return "SM_GETSERVERSELA";
			//case 2025: return "UM_GETUSERSELA";
			//case 2025: return "WM_CPL_LAUNCHED";
		case 2026: return "SM_GETSERVERSELW";
			//case 2026: return "UM_GETUSERSELW";
		case 2027: return "SM_GETCURFOCUSA";
			//case 2027: return "UM_GETGROUPSELA";
		case 2028: return "SM_GETCURFOCUSW";
			//case 2028: return "UM_GETGROUPSELW";
		case 2029: return "SM_GETOPTIONS";
			//case 2029: return "UM_GETCURFOCUSA";
		case 2030: return "UM_GETCURFOCUSW";
		case 2031: return "UM_GETOPTIONS";
		case 2032: return "UM_GETOPTIONS2";
		case 4096: return "LVM_FIRST";
			//case 4096: return "LVM_GETBKCOLOR";
		case 4097: return "LVM_SETBKCOLOR";
		case 4098: return "LVM_GETIMAGELIST";
		case 4099: return "LVM_SETIMAGELIST";
		case 4100: return "LVM_GETITEMCOUNT";
		case 4101: return "LVM_GETITEMA";
		case 4102: return "LVM_SETITEMA";
		case 4103: return "LVM_INSERTITEMA";
		case 4104: return "LVM_DELETEITEM";
		case 4105: return "LVM_DELETEALLITEMS";
		case 4106: return "LVM_GETCALLBACKMASK";
		case 4107: return "LVM_SETCALLBACKMASK";
		case 4108: return "LVM_GETNEXTITEM";
		case 4109: return "LVM_FINDITEMA";
		case 4110: return "LVM_GETITEMRECT";
		case 4111: return "LVM_SETITEMPOSITION";
		case 4112: return "LVM_GETITEMPOSITION";
		case 4113: return "LVM_GETSTRINGWIDTHA";
		case 4114: return "LVM_HITTEST";
		case 4115: return "LVM_ENSUREVISIBLE";
		case 4116: return "LVM_SCROLL";
		case 4117: return "LVM_REDRAWITEMS";
		case 4118: return "LVM_ARRANGE";
		case 4119: return "LVM_EDITLABELA";
		case 4120: return "LVM_GETEDITCONTROL";
		case 4121: return "LVM_GETCOLUMNA";
		case 4122: return "LVM_SETCOLUMNA";
		case 4123: return "LVM_INSERTCOLUMNA";
		case 4124: return "LVM_DELETECOLUMN";
		case 4125: return "LVM_GETCOLUMNWIDTH";
		case 4126: return "LVM_SETCOLUMNWIDTH";
		case 4127: return "LVM_GETHEADER";
		case 4129: return "LVM_CREATEDRAGIMAGE";
		case 4130: return "LVM_GETVIEWRECT";
		case 4131: return "LVM_GETTEXTCOLOR";
		case 4132: return "LVM_SETTEXTCOLOR";
		case 4133: return "LVM_GETTEXTBKCOLOR";
		case 4134: return "LVM_SETTEXTBKCOLOR";
		case 4135: return "LVM_GETTOPINDEX";
		case 4136: return "LVM_GETCOUNTPERPAGE";
		case 4137: return "LVM_GETORIGIN";
		case 4138: return "LVM_UPDATE";
		case 4139: return "LVM_SETITEMSTATE";
		case 4140: return "LVM_GETITEMSTATE";
		case 4141: return "LVM_GETITEMTEXTA";
		case 4142: return "LVM_SETITEMTEXTA";
		case 4143: return "LVM_SETITEMCOUNT";
		case 4144: return "LVM_SORTITEMS";
		case 4145: return "LVM_SETITEMPOSITION32";
		case 4146: return "LVM_GETSELECTEDCOUNT";
		case 4147: return "LVM_GETITEMSPACING";
		case 4148: return "LVM_GETISEARCHSTRINGA";
		case 4149: return "LVM_SETICONSPACING";
		case 4150: return "LVM_SETEXTENDEDLISTVIEWSTYLE";
		case 4151: return "LVM_GETEXTENDEDLISTVIEWSTYLE";
		case 4152: return "LVM_GETSUBITEMRECT";
		case 4153: return "LVM_SUBITEMHITTEST";
		case 4154: return "LVM_SETCOLUMNORDERARRAY";
		case 4155: return "LVM_GETCOLUMNORDERARRAY";
		case 4156: return "LVM_SETHOTITEM";
		case 4157: return "LVM_GETHOTITEM";
		case 4158: return "LVM_SETHOTCURSOR";
		case 4159: return "LVM_GETHOTCURSOR";
		case 4160: return "LVM_APPROXIMATEVIEWRECT";
		case 4161: return "LVM_SETWORKAREAS";
		case 4162: return "LVM_GETSELECTIONMARK";
		case 4163: return "LVM_SETSELECTIONMARK";
		case 4164: return "LVM_SETBKIMAGEA";
		case 4165: return "LVM_GETBKIMAGEA";
		case 4166: return "LVM_GETWORKAREAS";
		case 4167: return "LVM_SETHOVERTIME";
		case 4168: return "LVM_GETHOVERTIME";
		case 4169: return "LVM_GETNUMBEROFWORKAREAS";
		case 4170: return "LVM_SETTOOLTIPS";
		case 4171: return "LVM_GETITEMW";
		case 4172: return "LVM_SETITEMW";
		case 4173: return "LVM_INSERTITEMW";
		case 4174: return "LVM_GETTOOLTIPS";
		case 4179: return "LVM_FINDITEMW";
		case 4183: return "LVM_GETSTRINGWIDTHW";
		case 4191: return "LVM_GETCOLUMNW";
		case 4192: return "LVM_SETCOLUMNW";
		case 4193: return "LVM_INSERTCOLUMNW";
		case 4211: return "LVM_GETITEMTEXTW";
		case 4212: return "LVM_SETITEMTEXTW";
		case 4213: return "LVM_GETISEARCHSTRINGW";
		case 4214: return "LVM_EDITLABELW";
		case 4235: return "LVM_GETBKIMAGEW";
		case 4236: return "LVM_SETSELECTEDCOLUMN";
		case 4237: return "LVM_SETTILEWIDTH";
		case 4238: return "LVM_SETVIEW";
		case 4239: return "LVM_GETVIEW";
		case 4241: return "LVM_INSERTGROUP";
		case 4243: return "LVM_SETGROUPINFO";
		case 4245: return "LVM_GETGROUPINFO";
		case 4246: return "LVM_REMOVEGROUP";
		case 4247: return "LVM_MOVEGROUP";
		case 4250: return "LVM_MOVEITEMTOGROUP";
		case 4251: return "LVM_SETGROUPMETRICS";
		case 4252: return "LVM_GETGROUPMETRICS";
		case 4253: return "LVM_ENABLEGROUPVIEW";
		case 4254: return "LVM_SORTGROUPS";
		case 4255: return "LVM_INSERTGROUPSORTED";
		case 4256: return "LVM_REMOVEALLGROUPS";
		case 4257: return "LVM_HASGROUP";
		case 4258: return "LVM_SETTILEVIEWINFO";
		case 4259: return "LVM_GETTILEVIEWINFO";
		case 4260: return "LVM_SETTILEINFO";
		case 4261: return "LVM_GETTILEINFO";
		case 4262: return "LVM_SETINSERTMARK";
		case 4263: return "LVM_GETINSERTMARK";
		case 4264: return "LVM_INSERTMARKHITTEST";
		case 4265: return "LVM_GETINSERTMARKRECT";
		case 4266: return "LVM_SETINSERTMARKCOLOR";
		case 4267: return "LVM_GETINSERTMARKCOLOR";
		case 4269: return "LVM_SETINFOTIP";
		case 4270: return "LVM_GETSELECTEDCOLUMN";
		case 4271: return "LVM_ISGROUPVIEWENABLED";
		case 4272: return "LVM_GETOUTLINECOLOR";
		case 4273: return "LVM_SETOUTLINECOLOR";
		case 4275: return "LVM_CANCELEDITLABEL";
		case 4276: return "LVM_MAPINDEXTOID";
		case 4277: return "LVM_MAPIDTOINDEX";
		case 4278: return "LVM_ISITEMVISIBLE";
		case 8192: return "OCM__BASE";
		case 8197: return "LVM_SETUNICODEFORMAT";
		case 8198: return "LVM_GETUNICODEFORMAT";
		case 8217: return "OCM_CTLCOLOR";
		case 8235: return "OCM_DRAWITEM";
		case 8236: return "OCM_MEASUREITEM";
		case 8237: return "OCM_DELETEITEM";
		case 8238: return "OCM_VKEYTOITEM";
		case 8239: return "OCM_CHARTOITEM";
		case 8249: return "OCM_COMPAREITEM";
		case 8270: return "OCM_NOTIFY";
		case 8465: return "OCM_COMMAND";
		case 8468: return "OCM_HSCROLL";
		case 8469: return "OCM_VSCROLL";
		case 8498: return "OCM_CTLCOLORMSGBOX";
		case 8499: return "OCM_CTLCOLOREDIT";
		case 8500: return "OCM_CTLCOLORLISTBOX";
		case 8501: return "OCM_CTLCOLORBTN";
		case 8502: return "OCM_CTLCOLORDLG";
		case 8503: return "OCM_CTLCOLORSCROLLBAR";
		case 8504: return "OCM_CTLCOLORSTATIC";
		case 8720: return "OCM_PARENTNOTIFY";
		case 32768: return "WM_APP";
		case 52429: return "WM_RASDIALEVENT";
	}
	return "";
}
#pragma endregion

namespace gui {

	enum class Flow {
		Horizontal = 0,
		Vertical
	};

	struct Rect { int x, y, width, height; };

	using WID = size_t;
	
	class Container;
	class Manager;
	class Widget {
		friend class Manager;
	public:
		Widget() = default;
		~Widget() {
			DestroyWindow(handle);
		}

		virtual void create() = 0;
		virtual void updateAttributes() {};

		void update() {
			SetWindowPos(handle, nullptr, m_actualBounds.x, m_actualBounds.y, m_actualBounds.width, m_actualBounds.height, 0);
			updateAttributes();
		}

		HWND parentHandle{ nullptr };
		HWND handle{ nullptr };
		Rect bounds{};

		int flex{ 1 };

		WID parent{ 0 };

		const WID& id() const { return m_id; }

		bool m_alreadyInitialized{ false };
	protected:
		Rect m_actualBounds;
		WID m_id{ 0 };
	};

	using WidgetMap = std::unordered_map<size_t, std::unique_ptr<Widget>>;

	enum class Alignment {
		Left = 0,
		Center,
		Right
	};

	class Text : public Widget {
	public:
		Text(const std::wstring& text = L"") : text(text) { }

		void create() {
			handle = CreateWindow(
				L"STATIC",
				text.c_str(),
				WS_VISIBLE | WS_CHILD,
				bounds.x, bounds.y, bounds.width, bounds.height,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE) GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
		}

		void updateAttributes() {
			LONG_PTR align = SS_LEFT;
			switch (alignment) {
				case Alignment::Left: break;
				case Alignment::Center: align = SS_CENTER; break;
				case Alignment::Right: align = SS_RIGHT; break;
			}

			LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);
			SetWindowLongPtr(handle, GWL_STYLE, style | align);

			SetWindowText(handle, text.c_str());

			m_font = CreateFont(
				fontSize, 0, 0, 0, FW_DONTCARE,
				FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				fontFamily.c_str()
			);
			SendMessage(handle, WM_SETFONT, WPARAM(m_font), TRUE);
		}

		Alignment alignment{ Alignment::Center };
		std::wstring text{ L"Text" };

		std::wstring fontFamily{ L"Segoe UI" };
		int fontSize{ 16 };
	private:
		HFONT m_font{ nullptr };
	};

	class Button : public Widget {
	public:
		Button(const std::wstring& text = L"") : text(text) {}

		void create() {
			handle = CreateWindow(
				L"BUTTON",
				text.c_str(),
				WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,
				bounds.x, bounds.y, bounds.width, bounds.height,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
		}

		void updateAttributes() {
			SetWindowText(handle, text.c_str());

			m_font = CreateFont(
				fontSize, 0, 0, 0, FW_DONTCARE,
				FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				fontFamily.c_str()
			);
			SendMessage(handle, WM_SETFONT, WPARAM(m_font), TRUE);
		}

		std::wstring text{ L"Button" };
		std::function<void()> onPressed;

		std::wstring fontFamily{ L"Segoe UI" };
		int fontSize{ 16 };
	private:
		HFONT m_font{ nullptr };
	};

	class TextBox : public Widget {
	public:
		~TextBox() {
			Widget::~Widget();
			DestroyWindow(m_wrapper);
		}

		void create() {
			m_wrapper = CreateWindow(
				L"STATIC",
				nullptr,
				WS_VISIBLE | WS_CHILD,
				bounds.x, bounds.y, bounds.width, bounds.height,
				parentHandle,
				(HMENU)-1,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(m_wrapper, GWLP_USERDATA, (LONG_PTR)this);
			SetWindowSubclass(m_wrapper, TextBox::WndProc, 0, 0);

			handle = CreateWindow(
				L"EDIT",
				text.c_str(),
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				bounds.x, bounds.y, bounds.width, bounds.height,
				m_wrapper,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			switch (uMsg) {
				case WM_COMMAND: {
					if (HIWORD(wParam) > 1) {
						auto id = LOWORD(wParam);
						auto cmd = HIWORD(wParam);

						if (id < 1000) break;

						switch (cmd) {
							case EN_CHANGE: {
								HWND widgetHnd = (HWND)lParam;

								gui::Widget* widget = (gui::Widget*)GetWindowLongPtr(widgetHnd, GWLP_USERDATA);
								gui::TextBox* tb = static_cast<gui::TextBox*>(widget);
								if (id == tb->id()) {
									wchar_t buf[1024];
									GetWindowText(widgetHnd, buf, 1024);
									tb->text = std::wstring(buf);
								}
							} break;
						}
					}
				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}

		void updateAttributes() {
			SetWindowPos(m_wrapper, nullptr, m_actualBounds.x, m_actualBounds.y, m_actualBounds.width, m_actualBounds.height, 0);

			LONG_PTR align = ES_LEFT;
			switch (alignment) {
				case Alignment::Left: break;
				case Alignment::Center: align = ES_CENTER; break;
				case Alignment::Right: align = ES_RIGHT; break;
			}

			LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);

			if (multiLine) style |= ES_MULTILINE;
			if (password) style |= ES_PASSWORD;
			if (readOnly) {
				style |= ES_READONLY;
				SendMessage(handle, EM_SETREADONLY, TRUE, 0);
			}
			style |= align;

			SetWindowLongPtr(handle, GWL_STYLE, style);
			SetWindowText(handle, text.c_str());

			m_font = CreateFont(
				fontSize, 0, 0, 0, FW_DONTCARE,
				FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				fontFamily.c_str()
			);
			SendMessage(handle, WM_SETFONT, WPARAM(m_font), TRUE);
		}

		bool multiLine{ false }, password{ false }, readOnly{ false };
		Alignment alignment{ Alignment::Left };
		std::wstring text{ L"" };
		
		std::wstring fontFamily{ L"Segoe UI" };
		int fontSize{ 16 };

	private:
		HFONT m_font{ nullptr };
		HWND m_wrapper;
	};

	class Container : public Widget {
	public:
		Container() = default;
		Container(Rect b, Flow flow = Flow::Horizontal) { bounds = b; this->flow = flow; }

		void create() {
			handle = CreateWindow(
				L"STATIC",
				nullptr,
				WS_VISIBLE | WS_CHILD,
				bounds.x, bounds.y, bounds.width, bounds.height,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
			SetWindowSubclass(handle, Container::WndProc, 0, 0);
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			switch (uMsg) {
				case WM_COMMAND: {
					auto id = LOWORD(wParam);

					if (id < 1000) return DefSubclassProc(hwnd, uMsg, wParam, lParam);
					HWND widgetHnd = GetDlgItem(hwnd, id);
					if (!widgetHnd) return DefSubclassProc(hwnd, uMsg, wParam, lParam);

					Widget* widget = (Widget*)GetWindowLongPtr(widgetHnd, GWLP_USERDATA);
					// TODO: Make this not hacky
					Button* btn = dynamic_cast<Button*>(widget);
					if (btn && btn->onPressed && id == btn->id()) {
						btn->onPressed();
					}

				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}

		int border{ 4 }, spacing{ 4 };
		Flow flow{ Flow::Horizontal };

		std::vector<WID> children{};
	};

	class Spacer : public Widget {
	public:
		void create() {
			handle = CreateWindow(
				L"STATIC",
				nullptr,
				WS_CHILD,
				0, 0, 1, 1,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
		}
	};

	class Manager {
	public:
		Manager() = default;
		~Manager() = default;

		template <class W, typename... Args>
		W& create(WID parent, Args&&... args) {
			size_t id = m_genID++;
			auto widget = std::make_unique<W>(std::forward<Args>(args)...);
			widget->m_id = id;

			if (parent) {
				Container* cont = dynamic_cast<Container*>(get(parent));
				if (cont) {
					cont->children.push_back(id);
					widget->parent = parent;
				}
			}

			m_widgets[id] = std::move(widget);
			return *((W*)m_widgets[id].get());
		}

		Widget* get(WID id) {
			return m_widgets[id].get();
		}

		void createWidgets(HWND parentWindow) {
			for (auto& [id, widget] : m_widgets) {
				widget->parentHandle = parentWindow;
				widget->create();
			}

			// Rearrange parents
			for (auto& [id, widget] : m_widgets) {
				if (widget->parent) {
					widget->parentHandle = get(widget->parent)->handle;
					SetParent(widget->handle, widget->parentHandle);
				}
			}

			updateWidgets();
		}

		void updateWidgets() {
			performContainerLayout(1000);
			for (auto& [id, widget] : m_widgets) {
				widget->update();
			}
		}

		std::vector<WID> widgets() {
			std::vector<WID> ids;
			for (auto& [id, widget] : m_widgets) {
				ids.push_back(id);
			}
			return ids;
		}

	private:
		size_t m_genID{ 1000 };
		WidgetMap m_widgets;

		void performContainerLayout(WID id) {
			if (m_widgets.find(id) == m_widgets.end()) return;

			Widget* wid = get(id);
			if (wid->parent == 0) {
				wid->m_actualBounds = wid->bounds;
			}

			int totalProportions = 0;
			Container* cont = dynamic_cast<Container*>(wid);
			if (cont == nullptr) return;

			if (cont->children.empty()) return;

			for (auto& e : cont->children) {
				totalProportions += get(e)->flex;
			}
			totalProportions = totalProportions <= 0 ? 1 : totalProportions;

			Rect parentBounds = cont->m_actualBounds;
			int& compSize = cont->flow == Flow::Horizontal ? parentBounds.width : parentBounds.height;
			int& comp = cont->flow == Flow::Horizontal ? parentBounds.x : parentBounds.y;

			for (auto& e : cont->children) {
				Widget* c = get(e);
				int size = cont->flow == Flow::Horizontal ? c->bounds.width : c->bounds.height;
				if (c->flex == 0) {
					compSize -= size;
				}
			}
			const int size = (cont->flow == Flow::Horizontal ? parentBounds.width : parentBounds.height) - cont->border * 2;

			int pbx = parentBounds.x,
				pby = parentBounds.y;
			for (auto& e : cont->children) {
				Widget* c = get(e);

				Rect ret = c->bounds;
				if (cont->parent) {
					ret.x -= pbx;
					ret.y -= pby;
				}
				ret.x += parentBounds.x + cont->border;
				ret.y += parentBounds.y + cont->border;

				switch (cont->flow) {
					case Flow::Horizontal: {
						const int sizeSlice = (size - (c->flex <= 0 ? ret.width : 0)) / totalProportions;
						if (c->flex >= 1) {
							ret.width = sizeSlice * c->flex;
						}
						ret.height = parentBounds.height - cont->border * 2;
						parentBounds.x += ret.width;
						parentBounds.width -= ret.width;
						if (e != cont->children.back()) ret.width -= cont->spacing;
					} break;
					case Flow::Vertical: {
						const int sizeSlice = (size - (c->flex <= 0 ? ret.height : 0)) / totalProportions;
						if (c->flex >= 1) {
							ret.height = sizeSlice * c->flex;
						}
						ret.width = parentBounds.width - cont->border * 2;
						parentBounds.y += ret.height;
						parentBounds.height -= ret.height;
						if (e != cont->children.back()) ret.height -= cont->spacing;
					} break;
				}

				c->m_actualBounds = ret;
				performContainerLayout(e);
			}
		}
	};

}

#endif // GUI_HPP
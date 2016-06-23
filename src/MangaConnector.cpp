#include "MangaConnector.h"

MCEntry::MCEntry()
{
    //
}

MCEntry::MCEntry(wxString Name, wxString Url)
{
    SetSafeLabel(Name);
    Label = Name;
    Link = Url;
}

MCEntry::~MCEntry()
{
    //
}

#ifdef __LINUX__
wxString MCEntry::invalidFileCharacters = wxT("/\r\n\t");
#endif
#ifdef __WINDOWS__
wxString MCEntry::invalidFileCharacters = wxT("\\/:*?\"<>|\r\n\t");
#endif

void MCEntry::SetSafeLabel(wxString Name)
{
    SafeLabel = wxEmptyString;

    // variable to store position of last non space/period in fixed string
    int safeLength = 0;

    for(unsigned int i=0; i<Name.Len(); i++)
    {
        // only append characters that are not forbidden
        // NULL character can't be added to invalidFileCharacters because it is termiantor for wxString
        if(Name[i] > 0 && invalidFileCharacters.Find(Name[i]) < 0)
        {
            if(Name[i] != ' ' && Name[i] != '.')
            {
                SafeLabel.Append(Name[i]);
                safeLength = SafeLabel.Len();
            }
            else
            {
                // don't append spaces/periods at beginning
                if(!SafeLabel.IsEmpty())
                {
                    SafeLabel.Append(Name[i]);
                }
            }
        }
    }

    // remove spaces/periods from end
    SafeLabel = SafeLabel.Mid(0, safeLength);
}

MangaConnector::MangaConnector()
{
    InitializeHtmlEntities();
    curl_global_init(CURL_GLOBAL_ALL);
}

MangaConnector::~MangaConnector()
{
    curl_global_cleanup();
}

wxString MangaConnector::GetConfigurationPath()
{
    #ifdef PORTABLE
        #ifdef __LINUX__
            return wxStandardPaths::Get().GetExecutablePath().BeforeLast('/') + wxT("/connectors");
        #endif
        #ifdef __WINDOWS__
            return wxStandardPaths::Get().GetLocalDataDir() + wxT("\\connectors");
        #endif
    #else
        #ifdef __LINUX__
            wxString EnvironmentFilePath;
            wxGetEnv(wxT("XDG_CONFIG_HOME"), &EnvironmentFilePath);
            EnvironmentFilePath = EnvironmentFilePath.BeforeFirst(':');
            if(EnvironmentFilePath.IsEmpty())
            {
                return wxStandardPaths::Get().GetUserConfigDir() + wxT("/.config/hakuneko");
            }
            return EnvironmentFilePath + wxT("/hakuneko");
        #endif
        #ifdef __WINDOWS__
            return wxStandardPaths::Get().GetConfigDir();
        #endif
    #endif
}

unsigned long MangaConnector::CheckSum(wxString Value)
{
    // FNV-1a algorithm
    unsigned long prime = 0x01000193; //   16777619
    unsigned long seed  = 0x811C9DC5; // 2166136261
    unsigned long checksum = seed;

    for(size_t i=0; i<Value.Len(); i++)
    {
        checksum = (checksum ^ ((unsigned long)Value[i])) * prime;
    }

    return checksum;
}

wxCharHashMap MangaConnector::htmlEntities;
void MangaConnector::InitializeHtmlEntities()
{
    //(* Filling HashMaps with values...

    htmlEntities[wxT("nbsp")] = wxChar(160);
    htmlEntities[wxT("iexcl")] = wxChar(161);
    htmlEntities[wxT("cent")] = wxChar(162);
    htmlEntities[wxT("pound")] = wxChar(163);
    htmlEntities[wxT("curren")] = wxChar(164);
    htmlEntities[wxT("yen")] = wxChar(165);
    htmlEntities[wxT("brvbar")] = wxChar(166);
    htmlEntities[wxT("sect")] = wxChar(167);
    htmlEntities[wxT("uml")] = wxChar(168);
    htmlEntities[wxT("copy")] = wxChar(169);
    htmlEntities[wxT("ordf")] = wxChar(170);
    htmlEntities[wxT("laquo")] = wxChar(171);
    htmlEntities[wxT("not")] = wxChar(172);
    htmlEntities[wxT("shy")] = wxChar(173);
    htmlEntities[wxT("reg")] = wxChar(174);
    htmlEntities[wxT("macr")] = wxChar(175);
    htmlEntities[wxT("deg")] = wxChar(176);
    htmlEntities[wxT("plusmn")] = wxChar(177);
    htmlEntities[wxT("sup2")] = wxChar(178);
    htmlEntities[wxT("sup3")] = wxChar(179);
    htmlEntities[wxT("acute")] = wxChar(180);
    htmlEntities[wxT("micro")] = wxChar(181);
    htmlEntities[wxT("para")] = wxChar(182);
    htmlEntities[wxT("middot")] = wxChar(183);
    htmlEntities[wxT("cedil")] = wxChar(184);
    htmlEntities[wxT("sup1")] = wxChar(185);
    htmlEntities[wxT("ordm")] = wxChar(186);
    htmlEntities[wxT("raquo")] = wxChar(187);
    htmlEntities[wxT("frac14")] = wxChar(188);
    htmlEntities[wxT("frac12")] = wxChar(189);
    htmlEntities[wxT("frac34")] = wxChar(190);
    htmlEntities[wxT("iquest")] = wxChar(191);
    htmlEntities[wxT("Agrave")] = wxChar(192);
    htmlEntities[wxT("Aacute")] = wxChar(193);
    htmlEntities[wxT("Acirc")] = wxChar(194);
    htmlEntities[wxT("Atilde")] = wxChar(195);
    htmlEntities[wxT("Auml")] = wxChar(196);
    htmlEntities[wxT("Aring")] = wxChar(197);
    htmlEntities[wxT("AElig")] = wxChar(198);
    htmlEntities[wxT("Ccedil")] = wxChar(199);
    htmlEntities[wxT("Egrave")] = wxChar(200);
    htmlEntities[wxT("Eacute")] = wxChar(201);
    htmlEntities[wxT("Ecirc")] = wxChar(202);
    htmlEntities[wxT("Euml")] = wxChar(203);
    htmlEntities[wxT("Igrave")] = wxChar(204);
    htmlEntities[wxT("Iacute")] = wxChar(205);
    htmlEntities[wxT("Icirc")] = wxChar(206);
    htmlEntities[wxT("Iuml")] = wxChar(207);
    htmlEntities[wxT("ETH")] = wxChar(208);
    htmlEntities[wxT("Ntilde")] = wxChar(209);
    htmlEntities[wxT("Ograve")] = wxChar(210);
    htmlEntities[wxT("Oacute")] = wxChar(211);
    htmlEntities[wxT("Ocirc")] = wxChar(212);
    htmlEntities[wxT("Otilde")] = wxChar(213);
    htmlEntities[wxT("Ouml")] = wxChar(214);
    htmlEntities[wxT("times")] = wxChar(215);
    htmlEntities[wxT("Oslash")] = wxChar(216);
    htmlEntities[wxT("Ugrave")] = wxChar(217);
    htmlEntities[wxT("Uacute")] = wxChar(218);
    htmlEntities[wxT("Ucirc")] = wxChar(219);
    htmlEntities[wxT("Uuml")] = wxChar(220);
    htmlEntities[wxT("Yacute")] = wxChar(221);
    htmlEntities[wxT("THORN")] = wxChar(222);
    htmlEntities[wxT("szlig")] = wxChar(223);
    htmlEntities[wxT("agrave")] = wxChar(224);
    htmlEntities[wxT("aacute")] = wxChar(225);
    htmlEntities[wxT("acirc")] = wxChar(226);
    htmlEntities[wxT("atilde")] = wxChar(227);
    htmlEntities[wxT("auml")] = wxChar(228);
    htmlEntities[wxT("aring")] = wxChar(229);
    htmlEntities[wxT("aelig")] = wxChar(230);
    htmlEntities[wxT("ccedil")] = wxChar(231);
    htmlEntities[wxT("egrave")] = wxChar(232);
    htmlEntities[wxT("eacute")] = wxChar(233);
    htmlEntities[wxT("ecirc")] = wxChar(234);
    htmlEntities[wxT("euml")] = wxChar(235);
    htmlEntities[wxT("igrave")] = wxChar(236);
    htmlEntities[wxT("iacute")] = wxChar(237);
    htmlEntities[wxT("icirc")] = wxChar(238);
    htmlEntities[wxT("iuml")] = wxChar(239);
    htmlEntities[wxT("eth")] = wxChar(240);
    htmlEntities[wxT("ntilde")] = wxChar(241);
    htmlEntities[wxT("ograve")] = wxChar(242);
    htmlEntities[wxT("oacute")] = wxChar(243);
    htmlEntities[wxT("ocirc")] = wxChar(244);
    htmlEntities[wxT("otilde")] = wxChar(245);
    htmlEntities[wxT("ouml")] = wxChar(246);
    htmlEntities[wxT("divide")] = wxChar(247);
    htmlEntities[wxT("oslash")] = wxChar(248);
    htmlEntities[wxT("ugrave")] = wxChar(249);
    htmlEntities[wxT("uacute")] = wxChar(250);
    htmlEntities[wxT("ucirc")] = wxChar(251);
    htmlEntities[wxT("uuml")] = wxChar(252);
    htmlEntities[wxT("yacute")] = wxChar(253);
    htmlEntities[wxT("thorn")] = wxChar(254);
    htmlEntities[wxT("yuml")] = wxChar(255);
    htmlEntities[wxT("fnof")] = wxChar(402);
    //Greek
    htmlEntities[wxT("Alpha")] = wxChar(913);
    htmlEntities[wxT("Beta")] = wxChar(914);
    htmlEntities[wxT("Gamma")] = wxChar(915);
    htmlEntities[wxT("Delta")] = wxChar(916);
    htmlEntities[wxT("Epsilon")] = wxChar(917);
    htmlEntities[wxT("Zeta")] = wxChar(918);
    htmlEntities[wxT("Eta")] = wxChar(919);
    htmlEntities[wxT("Theta")] = wxChar(920);
    htmlEntities[wxT("Iota")] = wxChar(921);
    htmlEntities[wxT("Kappa")] = wxChar(922);
    htmlEntities[wxT("Lambda")] = wxChar(923);
    htmlEntities[wxT("Mu")] = wxChar(924);
    htmlEntities[wxT("Nu")] = wxChar(925);
    htmlEntities[wxT("Xi")] = wxChar(926);
    htmlEntities[wxT("Omicron")] = wxChar(927);
    htmlEntities[wxT("Pi")] = wxChar(928);
    htmlEntities[wxT("Rho")] = wxChar(929);
    htmlEntities[wxT("Sigma")] = wxChar(931);
    htmlEntities[wxT("Tau")] = wxChar(932);
    htmlEntities[wxT("Upsilon")] = wxChar(933);
    htmlEntities[wxT("Phi")] = wxChar(934);
    htmlEntities[wxT("Chi")] = wxChar(935);
    htmlEntities[wxT("Psi")] = wxChar(936);
    htmlEntities[wxT("Omega")] = wxChar(937);
    htmlEntities[wxT("alpha")] = wxChar(945);
    htmlEntities[wxT("beta")] = wxChar(946);
    htmlEntities[wxT("gamma")] = wxChar(947);
    htmlEntities[wxT("delta")] = wxChar(948);
    htmlEntities[wxT("epsilon")] = wxChar(949);
    htmlEntities[wxT("zeta")] = wxChar(950);
    htmlEntities[wxT("eta")] = wxChar(951);
    htmlEntities[wxT("theta")] = wxChar(952);
    htmlEntities[wxT("iota")] = wxChar(953);
    htmlEntities[wxT("kappa")] = wxChar(954);
    htmlEntities[wxT("lambda")] = wxChar(955);
    htmlEntities[wxT("mu")] = wxChar(956);
    htmlEntities[wxT("nu")] = wxChar(957);
    htmlEntities[wxT("xi")] = wxChar(958);
    htmlEntities[wxT("omicron")] = wxChar(959);
    htmlEntities[wxT("pi")] = wxChar(960);
    htmlEntities[wxT("rho")] = wxChar(961);
    htmlEntities[wxT("sigmaf")] = wxChar(962);
    htmlEntities[wxT("sigma")] = wxChar(963);
    htmlEntities[wxT("tau")] = wxChar(964);
    htmlEntities[wxT("upsilon")] = wxChar(965);
    htmlEntities[wxT("phi")] = wxChar(966);
    htmlEntities[wxT("chi")] = wxChar(967);
    htmlEntities[wxT("psi")] = wxChar(968);
    htmlEntities[wxT("omega")] = wxChar(969);
    htmlEntities[wxT("thetasym")] = wxChar(977);
    htmlEntities[wxT("upsih")] = wxChar(978);
    htmlEntities[wxT("piv")] = wxChar(982);
    //GeneralPunctuation
    htmlEntities[wxT("bull")] = wxChar(8226);
    htmlEntities[wxT("hellip")] = wxChar(8230);
    htmlEntities[wxT("prime")] = wxChar(8242);
    htmlEntities[wxT("Prime")] = wxChar(8243);
    htmlEntities[wxT("oline")] = wxChar(8254);
    htmlEntities[wxT("frasl")] = wxChar(8260);
    //LetterlikeSymbols
    htmlEntities[wxT("weierp")] = wxChar(8472);
    htmlEntities[wxT("image")] = wxChar(8465);
    htmlEntities[wxT("real")] = wxChar(8476);
    htmlEntities[wxT("trade")] = wxChar(8482);
    htmlEntities[wxT("alefsym")] = wxChar(8501);
    //Arrows
    htmlEntities[wxT("larr")] = wxChar(8592);
    htmlEntities[wxT("uarr")] = wxChar(8593);
    htmlEntities[wxT("rarr")] = wxChar(8594);
    htmlEntities[wxT("darr")] = wxChar(8595);
    htmlEntities[wxT("harr")] = wxChar(8596);
    htmlEntities[wxT("crarr")] = wxChar(8629);
    htmlEntities[wxT("lArr")] = wxChar(8656);
    htmlEntities[wxT("uArr")] = wxChar(8657);
    htmlEntities[wxT("rArr")] = wxChar(8658);
    htmlEntities[wxT("dArr")] = wxChar(8659);
    htmlEntities[wxT("hArr")] = wxChar(8660);
    //MathematicalOperators
    htmlEntities[wxT("forall")] = wxChar(8704);
    htmlEntities[wxT("part")] = wxChar(8706);
    htmlEntities[wxT("exist")] = wxChar(8707);
    htmlEntities[wxT("empty")] = wxChar(8709);
    htmlEntities[wxT("nabla")] = wxChar(8711);
    htmlEntities[wxT("isin")] = wxChar(8712);
    htmlEntities[wxT("notin")] = wxChar(8713);
    htmlEntities[wxT("ni")] = wxChar(8715);
    htmlEntities[wxT("prod")] = wxChar(8719);
    htmlEntities[wxT("sum")] = wxChar(8721);
    htmlEntities[wxT("minus")] = wxChar(8722);
    htmlEntities[wxT("lowast")] = wxChar(8727);
    htmlEntities[wxT("radic")] = wxChar(8730);
    htmlEntities[wxT("prop")] = wxChar(8733);
    htmlEntities[wxT("infin")] = wxChar(8734);
    htmlEntities[wxT("and")] = wxChar(8743);
    htmlEntities[wxT("or")] = wxChar(8744);
    htmlEntities[wxT("cap")] = wxChar(8745);
    htmlEntities[wxT("cup")] = wxChar(8746);
    htmlEntities[wxT("int")] = wxChar(8747);
    htmlEntities[wxT("there4")] = wxChar(8756);
    htmlEntities[wxT("sim")] = wxChar(8764);
    htmlEntities[wxT("cong")] = wxChar(8773);
    htmlEntities[wxT("asymp")] = wxChar(8776);
    htmlEntities[wxT("ne")] = wxChar(8800);
    htmlEntities[wxT("equiv")] = wxChar(8801);
    htmlEntities[wxT("le")] = wxChar(8804);
    htmlEntities[wxT("ge")] = wxChar(8805);
    htmlEntities[wxT("sub")] = wxChar(8834);
    htmlEntities[wxT("sup")] = wxChar(8835);
    htmlEntities[wxT("nsub")] = wxChar(8836);
    htmlEntities[wxT("sube")] = wxChar(8838);
    htmlEntities[wxT("supe")] = wxChar(8839);
    htmlEntities[wxT("oplus")] = wxChar(8853);
    htmlEntities[wxT("otimes")] = wxChar(8855);
    htmlEntities[wxT("perp")] = wxChar(8869);
    htmlEntities[wxT("sdot")] = wxChar(8901);
    //MiscellaneousTechnical
    htmlEntities[wxT("lceil")] = wxChar(8968);
    htmlEntities[wxT("rceil")] = wxChar(8969);
    htmlEntities[wxT("lfloor")] = wxChar(8970);
    htmlEntities[wxT("rfloor")] = wxChar(8971);
    htmlEntities[wxT("lang")] = wxChar(9001);
    htmlEntities[wxT("rang")] = wxChar(9002);
    //GeometricShapes
    htmlEntities[wxT("loz")] = wxChar(9674);
    //MiscellaneousSymbols
    htmlEntities[wxT("spades")] = wxChar(9824);
    htmlEntities[wxT("clubs")] = wxChar(9827);
    htmlEntities[wxT("hearts")] = wxChar(9829);
    htmlEntities[wxT("diams")] = wxChar(9830);
    htmlEntities[wxT("quot")] = wxChar(34);
    htmlEntities[wxT("amp")] = wxChar(38);
    htmlEntities[wxT("lt")] = wxChar(60);
    htmlEntities[wxT("gt")] = wxChar(62);
    //LatinExtended-A
    htmlEntities[wxT("OElig")] = wxChar(338);
    htmlEntities[wxT("oelig")] = wxChar(339);
    htmlEntities[wxT("Scaron")] = wxChar(352);
    htmlEntities[wxT("scaron")] = wxChar(353);
    htmlEntities[wxT("Yuml")] = wxChar(376);
    //SpacingModifierLetters
    htmlEntities[wxT("circ")] = wxChar(710);
    htmlEntities[wxT("tilde")] = wxChar(732);
    //GeneralPunctuation
    htmlEntities[wxT("ensp")] = wxChar(8194);
    htmlEntities[wxT("emsp")] = wxChar(8195);
    htmlEntities[wxT("thinsp")] = wxChar(8201);
    htmlEntities[wxT("zwnj")] = wxChar(8204);
    htmlEntities[wxT("zwj")] = wxChar(8205);
    htmlEntities[wxT("lrm")] = wxChar(8206);
    htmlEntities[wxT("rlm")] = wxChar(8207);
    htmlEntities[wxT("ndash")] = wxChar(8211);
    htmlEntities[wxT("mdash")] = wxChar(8212);
    htmlEntities[wxT("lsquo")] = wxChar(8216);
    htmlEntities[wxT("rsquo")] = wxChar(8217);
    htmlEntities[wxT("sbquo")] = wxChar(8218);
    htmlEntities[wxT("ldquo")] = wxChar(8220);
    htmlEntities[wxT("rdquo")] = wxChar(8221);
    htmlEntities[wxT("bdquo")] = wxChar(8222);
    htmlEntities[wxT("dagger")] = wxChar(8224);
    htmlEntities[wxT("Dagger")] = wxChar(8225);
    htmlEntities[wxT("permil")] = wxChar(8240);
    htmlEntities[wxT("lsaquo")] = wxChar(8249);
    htmlEntities[wxT("rsaquo")] = wxChar(8250);
    htmlEntities[wxT("euro")] = wxChar(8364);

    //*)
}

wxString MangaConnector::HtmlEscapeUrl(wxString IllegalUrl)
{
    // NOTE: wxURI(EscapedString).BuildURI() don't work in wx unicode build 2.8.x
    // -> need function to convert wxCharacters to utf8 '%' representation

    wxString EscapedString;
    wxChar c;

    for(size_t i=0; i<IllegalUrl.Len(); i++)
    {
        c = IllegalUrl[i];

        if(c < 0x80)
        {
            EscapedString.Append(c);
            //EscapedString.Append('%');
            //EscapedString.Append( (wxChar)(c>>0  & 0x7F | 0x00) );
        }
        else if(c < 0x0800)
        {
            EscapedString.Append('%');
            EscapedString.Append( wxString::Format(wxT("%02X"), ((c>>6  & 0x1F) | 0xC0)) );
            EscapedString.Append('%');
            EscapedString.Append( wxString::Format(wxT("%02X"), ((c>>0  & 0x3F) | 0x80)) );
        }
        else if(c < 0x010000)
        {
            EscapedString.Append('%');
            EscapedString.Append( wxString::Format(wxT("%02X"), ((c>>12 & 0x0F) | 0xE0)) );
            EscapedString.Append('%');
            EscapedString.Append( wxString::Format(wxT("%02X"), ((c>>6  & 0x3F) | 0x80)) );
            EscapedString.Append('%');
            EscapedString.Append( wxString::Format(wxT("%02X"), ((c>>0  & 0x3F) | 0x80)) );
        }
        else if(c < 0x110000)
        {
            EscapedString.Append('%');
            EscapedString.Append( wxString::Format(wxT("%02X"), ((c>>18 & 0x07) | 0xF0)) );
            EscapedString.Append('%');
            EscapedString.Append( wxString::Format(wxT("%02X"), ((c>>12 & 0x3F) | 0x80)) );
            EscapedString.Append('%');
            EscapedString.Append( wxString::Format(wxT("%02X"), ((c>>6  & 0x3F) | 0x80)) );
            EscapedString.Append('%');
            EscapedString.Append( wxString::Format(wxT("%02X"), ((c>>0  & 0x3F) | 0x80)) );
        }
    }

    return EscapedString;
}

wxString MangaConnector::HtmlUnescapeString(wxString IllegalString)
{
    wxString EscapedString;
    wxString entity;
    unsigned int pos_start; // position of '&'
    unsigned int pos_end; // position of ';'
    long characterL;
    wxChar character;
    int base;

    for(unsigned int i=0; i<IllegalString.Len(); i++)
    {
        // the '&' itself might be masked as '&amp;', which is a common bug on mangafox.me
        if(i+4 < IllegalString.Len() && IllegalString[i] == '&' && IllegalString[i+1] == 'a' && IllegalString[i+2] == 'm' && IllegalString[i+3] == 'p' && IllegalString[i+4] == ';')
        {
            i += 4;
            IllegalString[i] = '&';
        }

        if(IllegalString[i] == '&')
        {
            pos_start = i;
            pos_end = 0;
            for(unsigned int c=0; c<8; c++)
            {
                if(i+c >= IllegalString.Len() || (c > 1 && IllegalString[i+c] == '&'))
                {
                    break;
                }
                if(IllegalString[i+c] == ';')
                {
                    pos_end = i+c;
                    break;
                }
            }

            if(pos_end > pos_start)
            {
                entity = IllegalString.SubString(pos_start+1, pos_end-1);
                if(entity.StartsWith(wxT("#")))
                {
                    entity = entity.AfterFirst('#');
                    base = 10;
                    if(entity.StartsWith(wxT("x")))
                    {
                        entity = entity.AfterFirst('x');
                        base = 16;
                    }

                    if(entity.ToLong(&characterL, base))
                    {
                        character = (wxChar)characterL;
                        EscapedString.Append(character);
                    }
                    else
                    {
                        EscapedString.Append(wxT("?"));
                    }
                }
                else
                {
                    character = htmlEntities[entity];
                    if(character > 0)
                    {
                        EscapedString.Append(character);
                    }
                    else
                    {
                        EscapedString.Append(wxT("?"));
                    }
                }

                i = pos_end;
            }
            else
            {
                EscapedString.Append(IllegalString[i]);
            }
        }
        else
        {
            EscapedString.Append(IllegalString[i]);
        }
    }

    return EscapedString;
}

void MangaConnector::FormatChapterNumberStyle(wxString* ChapterNumber)
{
    // the "tail" (length after the major chapter number) is excluded in 4 digit processing of major chapter number
    size_t chNumberResidualLength = 0;

    // plain chapter variations:
    /*
        %c = "13-18"

        %c = "13"
        %c = "13.5"
        %c = "13v2"
        %c = "13v.2"
        %c = "13.5v2"
        %c = "13.5v.2"
    */
    // variations combined with text:
    /*
        %ct = "Text Text"
        %ct = "%c Text Text"
        %ct = "Text Text %c"
    */

    // remove preceding/succeeding text from chapter number
    int posFirstSpace = ChapterNumber->find(wxT(" ")); // sometimes chapter numbers are followed by text (i.e. 13 Bla Bla)
    int posLastSpace = ChapterNumber->rfind(wxT(" ")); // sometimes chapter numbers are preceded by text (i.e. Bla Bla 13)
    if(posFirstSpace > -1 && posLastSpace > -1)
    {
        // NOTE: in case numbers are at beginning and ending, the number at beginning got higher priority (assigned first to *ChapterNumber)
        if(wxAtoi(ChapterNumber->Mid(0, posFirstSpace)))
        {
            //wxMessageBox(wxT("integer at beginning\n\n") + *ChapterNumber);
            *ChapterNumber = ChapterNumber->Mid(0, posFirstSpace);
        }

        if(wxAtoi(ChapterNumber->Mid(posLastSpace+1)))
        {
            //wxMessageBox(wxT("integer at ending\n\n") + *ChapterNumber);
            *ChapterNumber = ChapterNumber->Mid(posLastSpace+1);
        }
    }

    // sometimes chapter numbers are spanned with a hyphen (i.e. 13-18)
    int posHyphen = ChapterNumber->find(wxT("-"));
    if(posHyphen > -1)
    {
        wxString from = ChapterNumber->Mid(0, posHyphen);
        wxString to = ChapterNumber->Mid(posHyphen+1);

        FormatChapterNumberStyle(&from);
        FormatChapterNumberStyle(&to);

        *ChapterNumber = from + wxT("-") + to;
        return;
    }

    // sometimes chapter numbers are followed by version (i.e. 13v2, 13v.2)
    int posVchar = ChapterNumber->find(wxT("v"));
    if(posVchar > -1)
    {
        chNumberResidualLength = wxMax(chNumberResidualLength, ChapterNumber->Len() - posVchar);
    }

    // sometimes chapter numbers contains a dot (i.e. 13.5, 13.5v2, 13.5.v2)
    int posDot = ChapterNumber->find(wxT("."));
    if(posDot > -1)
    {
        chNumberResidualLength = wxMax(chNumberResidualLength, ChapterNumber->Len() - posDot);
    }

    while(ChapterNumber->Len()-chNumberResidualLength < 4)
    {
        *ChapterNumber = wxT("0") + *ChapterNumber;
    }
}

wxString MangaConnector::GetLabel()
{
    return label;
}

void MangaConnector::LoadLocalMangaList()
{
    mangaListEntries.Clear();
/*
    if(!mangaListFile.IsDirWritable())
    {
        wxMessageBox(wxT("Access denied!\nConfiguration directory: ") + mangaListFile.GetPath());
        return;
    }
*/
    mangaListFile.Mkdir(0755, wxPATH_MKDIR_FULL);

    wxTextFile f;

    if(mangaListFile.FileExists() && f.Open(mangaListFile.GetFullPath()))
    {
        wxString line = f.GetFirstLine();

        while(!f.Eof())
        {
            if(line.Contains(wxT("\t")))
            {
                mangaListEntries.Add(new MCEntry(line.Before(L'\t'), line.After(L'\t')));
            }

            line = f.GetNextLine();

            wxYield();
        }

        f.Close();
    }
}

void MangaConnector::UpdateMangaList()
{
    //
}

wxArrayMCEntry MangaConnector::GetMangaList()
{
    return mangaListEntries;
}

wxArrayMCEntry MangaConnector::GetChapterList(MCEntry* MangaEntry)
{
    return wxArrayMCEntry();
}

wxArrayString MangaConnector::GetPageLinks(wxString ChapterLink)
{
    return wxArrayString();
}

wxString MangaConnector::GetImageLink(wxString PageLink)
{
    return wxString();
}

MCJob MangaConnector::CreateJob(MCEntry* MangaEntry, MCEntry* ChapterEntry)
{
    MCJob job;
    job.DownloadCompleted = false;
    job.Connector = GetLabel();
    job.ChapterLink = ChapterEntry->Link;
    job.ChapterLabel = ChapterEntry->Label;
    job.ChapterSafeLabel = ChapterEntry->SafeLabel;
    job.MangaLink = MangaEntry->Link;
    job.MangaLabel = MangaEntry->Label;
    job.MangaSafeLabel = MangaEntry->SafeLabel;
    job.ID = GenerateJobID(ChapterEntry);

    return job;
}

unsigned long MangaConnector::GenerateJobID(MCEntry* ChapterEntry)
{
    return CheckSum(ChapterEntry->Link + ChapterEntry->Label);
}

bool MangaConnector::AddJob(MCJob Job)
{
    if(!ContainsJob(Job.ID))
    {
        jobList[Job.ID] = Job;
        return true;
    }
    else
    {
        return false;
    }
}

void MangaConnector::RemoveJob(unsigned long JobID)
{
    while(ContainsJob(JobID))
    {
        jobList.erase(JobID);
    }
}

void MangaConnector::RemoveAllJobs()
{
    jobList.clear();
}

bool MangaConnector::ContainsJob(unsigned long JobID)
{
    if(jobList.count(JobID)>0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

size_t MangaConnector::GetJobCount()
{
    return jobList.size();
}

wxArrayJobID MangaConnector::GetCompletedJobIDs()
{
    wxArrayJobID completed;

    for(wxJobHashMap::iterator it=jobList.begin(); it!=jobList.end(); ++it)
    {
        if(it->second.DownloadCompleted)
        {
            completed.Add(it->second.ID);
        }
    }

    return completed;
}

void MangaConnector::SetJobDownloadCompleted(unsigned long JobID, bool Value)
{
    if(ContainsJob(JobID))
    {
        jobList[JobID].DownloadCompleted = Value;
    }
}

wxArrayString MangaConnector::DownloadJobs(wxFileName BaseDirectory, wxStatusBar* StatusBar, bool* Abort, bool CompressChapters)
{
    wxArrayString errorLog;
    wxArrayString pageLinks;
    wxString sourceImageLink;
    wxFileName targetImageFile;
    wxFileName targetArchiveFile;

    if(!(type & CONNECTOR_TYPE_MANGA))
    {
        CompressChapters = false;
    }

    wxArrayJob jobListSorted(CompareMCJob);
    for(wxJobHashMap::iterator it=jobList.begin(); it!=jobList.end(); ++it)
    {
        jobListSorted.Add(&(it->second));
    }

    // loop through all chapters of joblist
    MCJob* job;
    bool noError;
    wxUint16 countError;
    wxUint16 attempts = 5;
    for(unsigned int j=0; j<jobListSorted.Count(); j++)
    {
        noError = true;
        job = jobListSorted[j];

        // NOTE: jobs marked as already downloaded will be skipped, use SetJobDownloadCompleted() to change a job's state
        if(!job->DownloadCompleted)
        {
            StatusBar->SetStatusText(wxString::Format(GetLabel() + wxT(" (%u/%lu)"), j, (unsigned long)GetJobCount()), 2);

            targetImageFile.AssignDir(BaseDirectory.GetPath());
            targetImageFile.AppendDir(job->MangaSafeLabel);
            if(type & CONNECTOR_TYPE_MANGA)
            {
                targetImageFile.AppendDir(job->ChapterSafeLabel);
            }
            targetArchiveFile.Assign(targetImageFile.GetPath() + wxT(".cbz"));

            if(CompressChapters)
            {
                if(!targetArchiveFile.Mkdir(0755, wxPATH_MKDIR_FULL))
                {
                    wxMessageBox(wxT("Error creating directory structure!"));
                    goto ABORT;
                }
            }
            else
            {
                if(!targetImageFile.Mkdir(0755, wxPATH_MKDIR_FULL))
                {
                    wxMessageBox(wxT("Error creating directory structure!"));
                    goto ABORT;
                }
            }

            wxFileOutputStream archiveFileStream(targetArchiveFile.GetFullPath()); // overwrite existing archive file
            wxZipOutputStream archiveCompressionStream(archiveFileStream);

            if(!CompressChapters)
            {
                archiveCompressionStream.Close();
                archiveFileStream.Close();
                wxRemoveFile(targetArchiveFile.GetFullPath());
            }

            CurlRequest cr;
            //cr.SetUrl(sourceImageLink);
            // NOTE: KissAnime Referrer is blacklisted on Google Video
            if(label != wxT("KissAnime")) {
                cr.SetReferer(referrerURL);
            }
            cr.SetStatusBar(StatusBar, 1);
            cr.SetAbort(Abort);

            pageLinks = GetPageLinks(job->ChapterLink);
            for(unsigned int i=0; i<pageLinks.GetCount(); i++)
            {
                if(type & CONNECTOR_TYPE_MANGA)
                {
                    StatusBar->SetStatusText(wxString::Format(job->MangaLabel + wxT(" - ") + job->ChapterLabel + wxT(" (%u/%lu)"), i, (unsigned long)pageLinks.GetCount()));
                }
                else
                {
                    StatusBar->SetStatusText(job->MangaLabel + wxT(" - ") + job->ChapterLabel);
                }
                sourceImageLink = GetImageLink(pageLinks[i]);
                cr.SetUrl(sourceImageLink);

                if(type & CONNECTOR_TYPE_MANGA)
                {
                    targetImageFile.SetName(wxString::Format(wxT("%03u"), i+1));
                    targetImageFile.SetExt(sourceImageLink.AfterLast('.').BeforeFirst('?'));
                }
                if(type & CONNECTOR_TYPE_ANIME)
                {
                    targetImageFile.SetName(job->ChapterSafeLabel.BeforeLast('.'));
                    targetImageFile.SetExt(job->ChapterSafeLabel.AfterLast('.'));
                    if(targetImageFile.GetExt().IsEmpty()) {
                        targetImageFile.SetExt(wxT("mp4"));
                    }
                }

                if(CompressChapters)
                {
                    // NOTE: retry download max. 5 attempts on fail and remove and re-add the zip entry
                    countError = 0;
                    while(countError < attempts) {
                        wxMemoryOutputStream mos;
                        cr.SetOutputStream(&mos);
                        if(cr.ExecuteRequest()) {
                            // write zip entryand data
                            archiveCompressionStream.PutNextEntry(targetImageFile.GetFullName());
                            wxMemoryInputStream mis(mos);
                            archiveCompressionStream.Write(mis);
                            // pretend no error occured, even if loop was repeated multiple times
                            countError = 0;
                            break;
                        }
                        // if this was the last attempt, add the broken file
                        if(countError == attempts-1) {
                            // write zip entryand data
                            archiveCompressionStream.PutNextEntry(targetImageFile.GetFullName());
                            wxMemoryInputStream mis(mos);
                            archiveCompressionStream.Write(mis);
                        }
                        Logger::Log(wxT("WARNING +++ MangaConnector::DownloadJobs(...) -> failed download attempt '") + sourceImageLink + wxT("'"));
                        countError++;
                        // wait 3 second before next attempt
                        wxSleep(3);
                    }
                    if(countError > 0) {
                        errorLog.Add(sourceImageLink + wxT("|") + targetImageFile.GetFullPath());
                        Logger::Log(wxT("ERROR +++ MangaConnector::DownloadJobs(...) -> failed downloading '") + sourceImageLink + wxT("'"));
                        noError = false;
                    }
                }
                else
                {
                    // retry download max. 5 attempts on fail and remove and overwrite the file
                    countError = 0;
                    while(countError < attempts) {
                        // overwrites the file created by previous attempt (no duplicate files...)
                        wxFFileOutputStream fileDestinationStream(targetImageFile.GetFullPath());
                        cr.SetOutputStream(&fileDestinationStream);
                        if(cr.ExecuteRequest()) {
                            fileDestinationStream.Close();
                            // pretend no error occured, even if loop was repeated multiple times
                            countError = 0;
                            break;
                        }
                        fileDestinationStream.Close();
                        Logger::Log(wxT("WARNING +++ MangaConnector::DownloadJobs(...) -> failed download attempt '") + sourceImageLink + wxT("'"));
                        countError++;
                        // wait 3 second before next attempt
                        wxSleep(3);
                    }
                    if(countError > 0) {
                        errorLog.Add(sourceImageLink + wxT("|") + targetImageFile.GetFullPath());
                        Logger::Log(wxT("ERROR +++ MangaConnector::DownloadJobs(...) -> failed downloading '") + sourceImageLink + wxT("'"));
                        noError = false;
                    }
                }

                wxYield();
                if(*Abort)
                {
                    if(CompressChapters)
                    {
                        archiveCompressionStream.Close();
                        archiveFileStream.Close();
                    }
                    goto ABORT;
                }
            }

            if(CompressChapters)
            {
                archiveCompressionStream.Close();
                archiveFileStream.Close();
            }
        }

        // mark job as done
        if(noError)
        {
            job->DownloadCompleted = true;
        }
    }

    ABORT:

    return errorLog;
}

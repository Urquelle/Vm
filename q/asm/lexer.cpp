#include "asm/lexer.hpp"

#include "allgemein/spanne.hpp"
#include "allgemein/zeichen.hpp"

namespace Asm {

Lexer::Lexer(std::string q, std::string text)
    : _q(q)
    , _text(text)
    , _index(0)
{
}

std::vector<Token *>
Lexer::starten()
{
    uint32_t ziffern['g'];

    ziffern['0'] = 0;
    ziffern['1'] = 1;
    ziffern['2'] = 2;
    ziffern['3'] = 3;
    ziffern['4'] = 4;
    ziffern['5'] = 5;
    ziffern['6'] = 6;
    ziffern['7'] = 7;
    ziffern['8'] = 8;
    ziffern['9'] = 9;
    ziffern['A'] = 10;
    ziffern['B'] = 11;
    ziffern['C'] = 12;
    ziffern['D'] = 13;
    ziffern['E'] = 14;
    ziffern['F'] = 15;
    ziffern['a'] = 10;
    ziffern['b'] = 11;
    ziffern['c'] = 12;
    ziffern['d'] = 13;
    ziffern['e'] = 14;
    ziffern['f'] = 15;

    std::vector<Token *> erg;

    for (;;)
    {
        leerzeichen_einlesen();
        auto z = zeichen();
        auto anfang = z;

        if (z == '\0')
        {
            erg.push_back(Token::Eof(Spanne(anfang, weiter())));

            break;
        }
        else if (z == '\n' || z == '\r')
        {
            z = weiter();

            while (z == '\n' || z == '\r')
            {
                z = weiter();
            }

            erg.push_back(Token::Zeilenumbruch(Spanne(anfang, zeichen(-1))));
        }
        else if (z == '(')
        {
            weiter();
            erg.push_back(Token::Runde_Klammer_Auf(Spanne(anfang, z)));
        }
        else if (z == ')')
        {
            weiter();
            erg.push_back(Token::Runde_Klammer_Zu(Spanne(anfang, z)));
        }
        else if (z == '[')
        {
            weiter();
            erg.push_back(Token::Eckige_Klammer_Auf(Spanne(anfang, z)));
        }
        else if (z == ']')
        {
            weiter();
            erg.push_back(Token::Eckige_Klammer_Zu(Spanne(anfang, z)));
        }
        else if (z == '{')
        {
            weiter();
            erg.push_back(Token::Geschweifte_Klammer_Auf(Spanne(anfang, z)));
        }
        else if (z == '}')
        {
            weiter();
            erg.push_back(Token::Geschweifte_Klammer_Zu(Spanne(anfang, z)));
        }
        else if (z == '.')
        {
            weiter();
            erg.push_back(Token::Punkt(Spanne(anfang, z)));
        }
        else if (z == ',')
        {
            weiter();
            erg.push_back(Token::Komma(Spanne(anfang, z)));
        }
        else if (z == '+')
        {
            weiter();
            erg.push_back(Token::Plus(Spanne(anfang, z)));
        }
        else if (z == '-')
        {
            weiter();
            erg.push_back(Token::Minus(Spanne(anfang, z)));
        }
        else if (z == '*')
        {
            weiter();
            erg.push_back(Token::Stern(Spanne(anfang, z)));
        }
        else if (z == '/')
        {
            weiter();
            erg.push_back(Token::Pisa(Spanne(anfang, z)));
        }
        else if (z == '\\')
        {
            weiter();
            erg.push_back(Token::Pisa_Links(Spanne(anfang, z)));
        }

        else if (z == '#')
        {
            z = weiter();
            if (z.c() >= '0' && z.c() <= '9' || z.c() >= 'a' && z.c() <= 'f' || z.c() >= 'A' && z.c() <= 'F')
            {
                uint16_t wert = 0;
                while (z.c() >= '0' && z.c() <= '9' || z.c() >= 'a' && z.c() <= 'f' || z.c() >= 'A' && z.c() <= 'F')
                {
                    if (z == '_')
                    {
                        z = weiter();
                        continue;
                    }

                    wert *= 16;
                    wert += ziffern[z.c()];
                    z = weiter();
                }

                erg.push_back(Token::Hex(Spanne(anfang, zeichen(-1)), wert));
            }
            else
            {
                erg.push_back(Token::Raute(Spanne(anfang, z)));
            }
        }
        else if (z == '%')
        {
            weiter();
            erg.push_back(Token::Prozent(Spanne(anfang, z)));
        }
        else if (z == '@')
        {
            weiter();
            erg.push_back(Token::Klammeraffe(Spanne(anfang, z)));
        }
        else if (z == ':')
        {
            weiter();
            erg.push_back(Token::Doppelpunkt(Spanne(anfang, z)));
        }
        else if (z == ';')
        {
            z = weiter();

            if (z == ';')
            {
                // INFO: kommentar einlesen
                while (z != '\n' && z != '\r')
                {
                    z = weiter();
                }
            }
            else
            {
                erg.push_back(Token::Semikolon(Spanne(anfang, zeichen(-1))));
            }
        }
        else if (z == '!')
        {
            weiter();
            erg.push_back(Token::Ausrufezeichen(Spanne(anfang, z)));
        }
        else if (z == '&')
        {
            weiter();
            erg.push_back(Token::Kaufmannsund(Spanne(anfang, z)));
        }
        else if (z == '|')
        {
            weiter();
            erg.push_back(Token::Balken(Spanne(anfang, z)));
        }
        else if (z == '>')
        {
            z = weiter();

            if (z == '=')
            {
                weiter();
                erg.push_back(Token::Groesser_Gleich(Spanne(anfang, z)));
            }
            else
            {
                erg.push_back(Token::Groesser(Spanne(anfang, z)));
            }
        }
        else if (z == '<')
        {
            z = weiter();

            if (z == '=')
            {
                weiter();
                erg.push_back(Token::Kleiner_Gleich(Spanne(anfang, z)));
            }
            else
            {
                erg.push_back(Token::Kleiner(Spanne(anfang, z)));
            }
        }
        else if (z == '=')
        {
            z = weiter();

            if (z == '=')
            {
                weiter();
                erg.push_back(Token::Gleich_Gleich(Spanne(anfang, z)));
            }
            else
            {
                erg.push_back(Token::Gleich(Spanne(anfang, z)));
            }
        }

        // INFO: text
        else if (z == '"')
        {
            z = weiter();
            anfang = z; // um das initiale " nicht in der länge mitzuzählen
            auto ende = z;

            while (z != '"')
            {
                if (z == '\\')
                {
                    z = weiter();
                }

                ende = z;
                z = weiter();
            }

            if (z == '"')
            {
                weiter();
            }

            erg.push_back(Token::Text(Spanne(anfang, zeichen(-2))));
        }

        // INFO: name
        else if (std::isalpha(z.c()) || z == '_')
        {
            while (std::isalnum(z.c()) || z == '_')
            {
                z = weiter();
            }

            erg.push_back(Token::Name(Spanne(anfang, zeichen(-1))));
        }

        // INFO: zahlen
        else if (std::isdigit(z.c()))
        {
            uint32_t wert = 0;

            while (z.c() >= '0' && z.c() <= '9' || z.c() >= 'a' && z.c() <= 'f' || z.c() >= 'A' && z.c() <= 'F')
            {
                if (z == '_')
                {
                    z = weiter();
                    continue;
                }

                wert *= 16;
                wert += ziffern[z.c()];

                z = weiter();
            }

            if (z == 'b')
            {
                z = weiter();
                uint32_t basis = wert;
                wert = 0;

                while (std::isalnum(z.c()) || z == '_')
                {
                    if (z == '_')
                    {
                        z = weiter();
                        continue;
                    }

                    if (z.c() >= '0' && z.c() <= '9' || z.c() >= 'a' && z.c() <= 'f' || z.c() >= 'A' && z.c() <= 'F')
                    {
                        wert *= basis;
                        wert += ziffern[z.c()];
                        z = weiter();
                    }
                    else
                    {
                        break;
                    }
                }

                erg.push_back(Token::Ganzzahl(Spanne(anfang, z), wert, basis));
            }
            else
            {
                erg.push_back(Token::Hex(Spanne(anfang, z), wert));
            }
        }
    }

    return erg;
}

Zeichen
Lexer::zeichen(int16_t versatz)
{
    int32_t  index = _index + versatz;

    if (index >= _text.size() || index < 0)
    {
        return Zeichen('\0', index, _q, _text);
    }

    return Zeichen(_text[index], index, _q, _text);
}

Zeichen
Lexer::weiter(uint16_t z)
{
    _index += z;

    return zeichen();
}

void
Lexer::leerzeichen_einlesen()
{
    auto z = zeichen();
    while (z == ' ' || z == '\t')
    {
        z = weiter();
    }
}

}

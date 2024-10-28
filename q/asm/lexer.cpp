#include "asm/lexer.hpp"
#include <cctype>
#include <cstdint>

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
            weiter();
            erg.push_back(Token::Eof(anfang));

            break;
        }
        else if (z == '\n' || z == '\r')
        {
            z = weiter();

            while (z == '\n' || z == '\r')
            {
                z = weiter();
            }

            erg.push_back(Token::Zeilenumbruch(anfang));
        }
        else if (z == '(')
        {
            weiter();
            erg.push_back(Token::Runde_Klammer_Auf(anfang));
        }
        else if (z == ')')
        {
            weiter();
            erg.push_back(Token::Runde_Klammer_Zu(anfang));
        }
        else if (z == '[')
        {
            weiter();
            erg.push_back(Token::Eckige_Klammer_Auf(anfang));
        }
        else if (z == ']')
        {
            weiter();
            erg.push_back(Token::Eckige_Klammer_Zu(anfang));
        }
        else if (z == '{')
        {
            weiter();
            erg.push_back(Token::Geschweifte_Klammer_Auf(anfang));
        }
        else if (z == '}')
        {
            weiter();
            erg.push_back(Token::Geschweifte_Klammer_Zu(anfang));
        }
        else if (z == '.')
        {
            weiter();
            erg.push_back(Token::Punkt(anfang));
        }
        else if (z == ',')
        {
            weiter();
            erg.push_back(Token::Komma(anfang));
        }
        else if (z == '+')
        {
            weiter();
            erg.push_back(Token::Plus(anfang));
        }
        else if (z == '-')
        {
            weiter();
            erg.push_back(Token::Minus(anfang));
        }
        else if (z == '*')
        {
            weiter();
            erg.push_back(Token::Stern(anfang));
        }
        else if (z == '/')
        {
            weiter();
            erg.push_back(Token::Pisa(anfang));
        }
        else if (z == '\\')
        {
            weiter();
            erg.push_back(Token::Pisa_Links(anfang));
        }

        // INFO: '$'
        else if (z == 36)
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

                erg.push_back(Token::Hex(anfang, z, wert));
            }
            else
            {
                // AUFGABE: Raute ist hier nicht mehr korrekt
                erg.push_back(Token::Raute(anfang));
            }
        }
        else if (z == '%')
        {
            weiter();
            erg.push_back(Token::Prozent(anfang));
        }
        else if (z == '@')
        {
            weiter();
            erg.push_back(Token::Klammeraffe(anfang));
        }
        else if (z == '#')
        {
            weiter();
            erg.push_back(Token::Raute(anfang));
        }
        else if (z == ':')
        {
            weiter();
            erg.push_back(Token::Doppelpunkt(anfang));
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
                erg.push_back(Token::Semikolon(anfang));
            }
        }
        else if (z == '!')
        {
            weiter();
            erg.push_back(Token::Ausrufezeichen(anfang));
        }
        else if (z == '&')
        {
            weiter();
            erg.push_back(Token::Kaufmannsund(anfang));
        }
        else if (z == '|')
        {
            weiter();
            erg.push_back(Token::Balken(anfang));
        }
        else if (z == '>')
        {
            z = weiter();

            if (z == '=')
            {
                weiter();
                erg.push_back(Token::Groesser_Gleich(anfang));
            }
            else
            {
                erg.push_back(Token::Groesser(anfang));
            }
        }
        else if (z == '<')
        {
            z = weiter();

            if (z == '=')
            {
                weiter();
                erg.push_back(Token::Kleiner_Gleich(anfang));
            }
            else
            {
                erg.push_back(Token::Kleiner(anfang));
            }
        }
        else if (z == '=')
        {
            z = weiter();

            if (z == '=')
            {
                weiter();
                erg.push_back(Token::Gleich_Gleich(anfang));
            }
            else
            {
                erg.push_back(Token::Gleich(anfang));
            }
        }

        // INFO: text
        else if (z == '"')
        {
            z = weiter();
            anfang = z; // um das initiale " nicht in der länge mitzuzählen

            while (z != '"')
            {
                if (z == '\\')
                {
                    z = weiter();
                }

                z = weiter();
            }

            if (z == '"')
            {
                weiter();
            }

            erg.push_back(Token::Text(anfang, z));
        }

        // INFO: name
        else if (std::isalpha(z.c()) || z == '_')
        {
            while (std::isalnum(z.c()) || z == '_')
            {
                z = weiter();
            }

            erg.push_back(Token::Name(anfang, z));
        }

        // INFO: zahlen
        else if (std::isdigit(z.c()))
        {
            uint32_t zahl = 0;
            while (z.c() >= '0' && z.c() <= '9' || z.c() >= 'a' && z.c() <= 'f' || z.c() >= 'A' && z.c() <= 'F')
            {
                if (z == '_')
                {
                    z = weiter();
                    continue;
                }

                zahl *= 16;
                zahl += ziffern[z.c()];

                z = weiter();
            }

            printf("wert: %d, hex: 0x%04X\n", zahl, zahl);
            erg.push_back(Token::Hex(anfang, z, zahl));

 #if 0
            if (z == 'b')
            {
                z = weiter();
                uint32_t basis = zahl;
                zahl = 0;

                while (std::isalnum(z.c()) || z == '_')
                {
                    if (z == '_')
                    {
                        z = weiter();
                        continue;
                    }

                    if (z.c() >= '0' && z.c() <= '9' || z.c() >= 'a' && z.c() <= 'f' || z.c() >= 'A' && z.c() <= 'F')
                    {
                        zahl *= basis;
                        zahl += ziffern[z.c()];
                        z = weiter();
                    }
                    else
                    {
                        break;
                    }
                }

                erg.push_back(Token::Ganzzahl(anfang, z, zahl));
            }
            else
            {
                erg.push_back(Token::Ganzzahl(anfang, z, zahl));
            }
 #endif
        }
    }

    return erg;
}

Zeichen
Lexer::zeichen()
{
    if (_index >= _text.size())
    {
        return Zeichen('\0', _index, _q, (char *)_text.c_str());
    }

    return Zeichen(_text[_index], _index, _q, (char *)_text.c_str());
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

/*
 *  CPPStringUtils.cpp
 *  Wiki2Touch/wikisrvd
 *
 *  Copyright (c) 2008 by Tom Haukap.
 * 
 *  This file is part of Wiki2Touch.
 * 
 *  Wiki2Touch is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Wiki2Touch is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with Wiki2Touch. If not, see <http://www.gnu.org/licenses/>.
 */

#include "CPPStringUtils.h"

inline char    _to_lower(const char c)     {if (((unsigned char)c)<0x80) return tolower(c); else if (((unsigned char)c)>=0xc0 && ((unsigned char) c)<0xdf) return (unsigned char)c+0x20; else return c;};
inline wchar_t _to_wlower(const wchar_t c) {if (c<0x80) return towlower(c); else if (c>=0xc0 && c<0xdf) return c+0x20; else return c;};

std::string CPPStringUtils::to_string(const std::wstring source)
{
	string dest = string();
	for (int i=0; i<source.length(); i++)
		dest += (unsigned char) source[i];
	
	return dest;
}

std::wstring CPPStringUtils::to_wstring(const std::string source)
{
	wstring dest = wstring();
	for (int i=0; i<source.length(); i++)
		dest += (unsigned char) source[i];
	
	return dest;
}

std::string CPPStringUtils::to_string(int source)
{
	string dest = string();
	bool negativ = (source<0);
	
	if ( source==0 )
		dest = '0';
	else
	{
		while ( source )
		{
			char digit = (source % 10) + '0';
			dest = digit + dest;
			source /= 10;
		}
	}
	
	if ( negativ )
		dest = '-'+ dest;
	
	return dest;
}

std::wstring CPPStringUtils::to_wstring(int source)
{
	wstring dest = wstring();
	bool negativ = (source<0);
	
	if ( source==0 )
		dest = L'0';
	else
	{
		while ( source )
		{
			wchar_t digit = (source % 10) + '0';
			dest = digit + dest;
			
			source /= 10;
		}
	}
	
	if ( negativ )
		dest = L'-' + dest;
	
	return dest;
}

std::string CPPStringUtils::to_utf8(const std::string source)
{
	// as the string itself only holds bytes it's not necessary to encode to more than two bytes 
	string dest = string();
	int length = source.length();

	for(int i=0; i<length; i++)
	{
		unsigned char c = (unsigned char) source[i]; 
		if ( c<0x80 )
			dest += c;
		else {
			dest += (0xc0 | (c>>6));
			dest += (0x80 | (c&0x3f));
		}
	}
	
	return dest;
}

std::string CPPStringUtils::to_utf8(const std::wstring source)
{
	string dest = string();
	int length = source.length();

	for(int i=0; i<length; i++)
	{
		unsigned int c = (unsigned int) source[i]; 
		if ( c<0x00080 )
			dest += c;
		else if ( c<0x00800 ) 
		{
			dest += (0xc0 | (c>>6));
			dest += (0x80 | (c & 0x3f));
		}
		else if ( c<0x010000 )
		{
			dest += (0xe0 | (c>>12));
			dest += (0x80 | (c>>6 & 0x3f));
			dest += (0x80 | (c & 0x3f));
		}
		else {
			dest += (0xf0 | (c>>18));
			dest += (0x80 | (c>>12 & 0x3f));
			dest += (0x80 | (c>>6 & 0x3f));
			dest += (0x80 | (c & 0x3f));
		}
	}
	
	return dest;
}

std::string CPPStringUtils::from_utf8(const std::string source)
{
	string dest = string();
	int length = source.length();
	
	for(int i=0; i<length; i++)
	{
		unsigned char c1 = (unsigned char) source[i];
		if ( c1 < 0x80 ) {
			dest += c1;
		}
		else if ( (c1 & 0xe0)==0xc0 )
		{
			if ( i+1 < length ) 
			{
				i++;
				unsigned char c2 = (unsigned char) source[i];
				dest += ((c1<<6) | (c2 & 0x3f));
			}
			else
				i = length-1;
		}
		else if ( (c1 & 0xf0)==0xe0 ) 
		{
			if ( i+2<length ) 
			{
				i += 2;
				dest += '#';
				dest += '#';
			}
			else
				i = length-1;
		}
		else if ( (c1 & 0xf8)==0xf0 ) 
		{
			if ( i+3<length ) {
				i += 3;
				dest += '#';
				dest += '#';
				dest += '#';
			}
			else
				i = length-1;				
		}
		else {
			// illegal coding, skip that char
			dest += '?';
		}
	}
	
	return dest;
}

std::wstring CPPStringUtils::from_utf8w(const std::string source)
{
	wstring dest = wstring();
	int length = source.length();
	
	for(int i=0; i<length; i++)
	{
		unsigned int c1 = (unsigned char) source[i];
		if ( c1<0x80 ) {
			dest += c1;
		}
		else if ( (c1 & 0xe0)==0xc0 )
		{
			if ( i+1 < length ) 
			{
				i++;
				unsigned int c2 = (unsigned char) source[i];
				
				dest += (((c1 & 0x1f)<<6) | (c2 & 0x3f));
			}
			else
				i = length-1;
		}
		else if ( (c1 & 0xf0)==0xe0 ) 
		{
			if ( i+2<length ) 
			{
				i++;
				unsigned int c2 = (unsigned char) source[i];

				i++;
				unsigned int c3 = (unsigned char) source[i];

				dest += (((c1 & 0x0f)<<12) | ((c2 & 0x3f)<<6) | (c3 & 0x3f));
			}
			else
				i = length-1;
		}
		else if ( (c1 & 0xf8)==0xf0 ) 
		{
			if ( i+3<length ) 
			{
				i++;
				unsigned int c2 = (unsigned char) source[i];
				
				i++;
				unsigned int c3 = (unsigned char) source[i];

				i++;
				unsigned int c4 = (unsigned char) source[i];

				dest += (((c1 & 0x07)<<18) | ((c2 & 0x3f)<<12) | ((c3 & 0x3f)<<6) | (c4 & 0x3f));
			}
			else
				i = length-1;				
		}
		else {
			// illegal coding, skip that char
			dest += '?';
		}
	}
	
	return dest;
}	

std::string CPPStringUtils::to_lower(std::string src)
{ 
	string dest = string(src);
	transform(dest.begin(), dest.end(), dest.begin(), _to_lower);
	
	return dest;
}

std::wstring CPPStringUtils::to_lower(std::wstring src)
{ 
	wstring dest = wstring(src);
	transform(dest.begin(), dest.end(), dest.begin(), _to_wlower);
	
	return dest;
}

std::string CPPStringUtils::to_lower_utf8(std::string utf8_src)
{
	if ( utf8_src.empty() )
		return utf8_src;
	
	return to_utf8(to_lower(from_utf8w(utf8_src)));
}

std::string CPPStringUtils::trim(std::string src)
{
	if (src.empty() )
		return src;
	
	int start = 0;
	int length = src.length();
	while (start<length && src[start]<=0x20)
		start++;
	
	if ( start==length )
		return string();
	
	int end = length-1;
	while (end>0 && src[end]<=0x20 )
		end--;
	
	return src.substr(start, end-start+1);
}

std::wstring CPPStringUtils::trim(std::wstring src)
{
	if (src.empty() )
		return src;
	
	int start = 0;
	int length = src.length();
	while (start<length && src[start]<=0x20)
		start++;
	
	if ( start==length )
		return wstring();
	
	int end = length-1;
	while (end>0 && src[end]<=0x20 )
		end--;
	
	return src.substr(start, end-start+1);
}

std::string CPPStringUtils::url_encode(std::string src)
{
	if ( src.empty() )
		return src;
	
	string dst = string();
	int i=0;
	int length = src.length();
	while ( i<length )
	{
		unsigned char c = src[i];
		if ( c==' ' || c=='<' || c=='>' || c=='"' || c=='#' || c=='%' || c=='<' || c>0x7f )
		{
			dst += "%";
			
			unsigned char d = (c & 0xf0) >> 4;
			if ( d<10 )
				dst += '0' + d;
			else
				dst += 'A' - 10 + d;

			d = (c & 0x0f);
			if ( d<10 )
				dst += '0' + d;
			else
				dst += 'A' - 10 + d;
		}
		else
			dst += c;
		i++;
	}
	
	return dst;
}

std::wstring CPPStringUtils::url_encode(std::wstring src)
{
	if ( src.empty() )
		return src;
	
	wstring dst = wstring();
	int i = 0;
	int length = src.length();
	while ( i<length )
	{
		unsigned int c = src[i];
		if ( c=='<' || c=='>' || c=='"' || c=='#' || c=='%' || c=='<' || c>0x7f )
		{
			dst += L"%";
			
			unsigned char d = (c & 0xf0) >> 4;
			if ( d<10 )
				dst += '0' + d;
			else
				dst += 'A' - 10 + d;
			
			d = (c & 0x0f);
			if ( d<10 )
				dst += '0' + d;
			else
				dst += 'A' - 10 + d;
		}
		else
			dst += c;
		i++;
	}
	
	return dst;
}

std::string CPPStringUtils::url_decode(std::string src)
{
	if ( src.empty() )
		return src;
	
	string dst = string();
	
	int i = 0;
	int length = src.length();
	while ( i<length )
	{
		unsigned int c = src[i++];
		if ( c=='%' && (i+1<length) )
		{
			int number = 0;
			
			unsigned char digit = (unsigned char) src[i++];
			digit = toupper(digit);
			if ( digit<='9' )
				digit -= 48;
			else
				digit -= 55;
			number = digit;
			
			digit = (unsigned char) src[i++];
			digit = toupper(digit);
			if ( digit<='9' )
				digit -= 48;
			else
				digit -= 55;
			
			number = number*16 + digit;
			
			dst += number;
		}
		else
			dst += c;
	}
	
	return dst;
}

const unsigned char diacriticExchangeTable[] =
{
	// this table contains the char code for any diacritic char
    // 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f    
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x5a, 0x00, // 0x80 - 0x8f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x7a, 0x59, // 0x90 - 0x9f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xa0 - 0xaf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xb0 - 0xbf
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, // 0xc0 - 0xcf
	0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x00, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x59, 0x00, 0x00, // 0xd0 - 0xdf
	0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x00, 0x63, 0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69, // 0xe0 - 0xef
	0x00, 0x6E, 0x6F, 0x6F, 0x6F, 0x6F, 0x6F, 0x00, 0x6f, 0x75, 0x75, 0x75, 0x75, 0x79, 0x00, 0x00, // 0xf0 - 0xff
};

std::string CPPStringUtils::exchange_diacritic_chars_utf8(string src)
{	
	if ( src.empty() )
		return src;
	
	string dst = string();
	
	int i = 0;
	int length = src.length();
	while ( i<length )
	{
		unsigned int c = src[i++];
	
		if ( (c&0xc0)==0xc0 )
		{
			int d = ((c&0x1f)<<6) + (((unsigned char) src[i]) & 0x3f);
			if ( d>=0x80 && d<=0xff )
			{
				unsigned char ex = diacriticExchangeTable[d-0x80];
				if ( ex )
				{
					dst += ex;
					i++;
					continue;
				}
			}

			dst += c;
			dst += src[i++];
		}
		else
			dst += c;
	}
	
	return dst;
}

#include "tc_sc.inc"

/* 
 Converts characters in tradional chineses to simplified chineses. Because an index
 tables is used the convertion is rather fast.
 */
std::string CPPStringUtils::tc2sc_utf8(string src)
{
	if ( src.empty() )
		return src;
	
	string dst = string();
	
	int i = 0;
	int length = src.length();
	while ( i<length )
	{
		unsigned char c = src[i++];

		if ( ((c & 0xE0)==0xE0) && (c>=0xE4) && (c<=0xE9) && (i+1<length) ) // three byte encoding in utf-8 starts with 1110xxxx
		{
			bool ready = false;

			// only these are used int traditional chinese encoding
			unsigned char d = src[i];
			
			unsigned short idx = tc_secondCodePos[c-0xE4][d-0x80];
			if ( idx!=0xffff )
			{
				unsigned char e = src[i+1];
				unsigned char* second = (unsigned char*) &chars_tc[idx][1];
				
				while ( d==*second++ )
				{
					if ( *second<e )
						second += 4;
					else 
					{
						if ( *second==e )
						{
							int pos = *(second+1) + *(second+2)*0x100;
							
							unsigned char* sc = (unsigned char*) chars_sc + (pos*5);
							
							dst += *sc++;
							dst += *sc++;
							dst += *sc++;
							i += 2;
							
							ready = true;
						}
						break;
					}
				}
			}
			
			if ( !ready )
			{
				dst += c;
				dst += src[i++];
				dst += src[i++];
			}
		}
		else
			dst += c;
	}
	
	return dst;
}



DBH::DBH(const wchar_t* arg)
{
	if ( arg )
		result = CPPStringUtils::to_string(wstring(arg));
	else
		result = string();
	
	data = CPPStringUtils::to_wstring(result);
}

DBH::DBH(std::wstring arg)
{
	result = CPPStringUtils::to_string(arg);
	data = arg;
}

void DBH::Print()
{
	const wchar_t* src = data.c_str();
	while ( *src )
		printf("x%X", *src++);
}





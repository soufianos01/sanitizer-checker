/*
 * AttackPatterns.cpp
 *
 * Copyright (C) 2013-2014 University of California Santa Barbara.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the  Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335,
 * USA.
 *
 * Authors: Abdulbaki Aydin, Muath Alkhalaf, Thomas Barber
 */
#include "AttackPatterns.hpp"

// Set of RegExps used to describe various attack patten contexts
std::string AttackPatterns::m_htmlEscapedRegExp          =  "/([^<>'\"&\\/]+|(&[a-zA-Z]+;|&#[xX][0-9a-zA-Z]+;|&#[0-9]+;)+)+/";
std::string AttackPatterns::m_htmlAttrEscapedRegExp      =  "/([a-zA-Z0-9]+|((&[a-zA-Z]+;|&#[xX][0-9]+;|&#[0-9]+;)))+/";
std::string AttackPatterns::m_javascriptEscapedRegExp    =  "/([a-zA-Z0-9,._\\s]+|((\\\\u[a-fA-F0-9]{4})|(\\\\x[a-fA-F0-9]{2})))+/";
std::string AttackPatterns::m_urlEscapedRegExp           =  "/([a-zA-Z0-9-_.!~*'()]+|((%[a-fA-F0-9]{2})))+/";

std::string AttackPatterns::m_htmlPayload                = "<script>alert(\"XSS\");</script>";

std::string AttackPatterns::m_htmlRemovedRegExp          =  "/[^<>'\"&\\/]*/";
std::string AttackPatterns::m_htmlRemovedNoSlashRegExp   =  "/[^<>'\"&]*/";

/*
 * getAllowedPatternFromRegEx
 *
 * Gets an automaton representing the regex provided (will be the inverse of getAttackPatternFromAllowedRegEx)
 * in order to help classify certain santizer functions
 */
StrangerAutomaton* AttackPatterns::getAllowedFromRegEx(const std::string& regex) {
    // Allowed characters in innerHTML
    StrangerAutomaton* retMe = StrangerAutomaton::regExToAuto(regex);
    // Also accept the empty string
    StrangerAutomaton* retMeEmpty = retMe->unionWithEmptyString();
    delete retMe;
    return retMeEmpty;
}

/*
 * getAttackPatternFromAllowedRegEx
 *
 * Generate an attack pattern using a regex which describes the set of string *allowed*
 * in the output. The attack pattern is generated by taking the complement of this regex.
 * The union with the empty string is required so that empty strings are allowed in the output.
 */
StrangerAutomaton* AttackPatterns::getAttackPatternFromAllowedRegEx(const std::string& regex) {
    StrangerAutomaton* retMe = getAllowedFromRegEx(regex);
    // Take the complement to generate strings which are not allowed
    StrangerAutomaton* complement = retMe->complement(int32_t(0));
    delete retMe;
    return complement;
}

StrangerAutomaton* AttackPatterns::getLiteralPattern()
{
    return StrangerAutomaton::regExToAuto("/foobarz/");
}

StrangerAutomaton* AttackPatterns::lessThanPattern()
{
    // Exclused just the "<" char
    return getAttackPatternFromAllowedRegEx("/[^<]+/");
}

StrangerAutomaton* AttackPatterns::getHtmlPattern()
{
    // Allowed characters in innerHTML, excludes ">", "<", "'", """, "\"
    // "&" is only considered harmful if it is not escaped
    return getAttackPatternFromAllowedRegEx(AttackPatterns::m_htmlEscapedRegExp);
}

StrangerAutomaton* AttackPatterns::getHtmlAttributePattern()
{
    // Allowed characters in HTML attribute, excludes all non alphanumeric chars, except & escaped
    return getAttackPatternFromAllowedRegEx(AttackPatterns::m_htmlAttrEscapedRegExp);
}

StrangerAutomaton* AttackPatterns::getJavascriptPattern()
{
    // Only allow alphanumeric, "," "." "_" and whitespace, all others must be JS escaped
    return getAttackPatternFromAllowedRegEx(AttackPatterns::m_javascriptEscapedRegExp);
}

StrangerAutomaton* AttackPatterns::getUrlPattern()
{
    // Only allow alphanumeric, "-", "_", "." "~" and URL escaped characters
    return getAttackPatternFromAllowedRegEx(AttackPatterns::m_urlEscapedRegExp);
}

StrangerAutomaton* AttackPatterns::getHtmlPayload()
{
    return StrangerAutomaton::makeString(AttackPatterns::m_htmlPayload);
}

StrangerAutomaton* AttackPatterns::getHtmlEscaped()
{
    return getAllowedFromRegEx(AttackPatterns::m_htmlEscapedRegExp);
}

StrangerAutomaton* AttackPatterns::getHtmlRemoved()
{
    return getAllowedFromRegEx(AttackPatterns::m_htmlRemovedRegExp);
}

StrangerAutomaton* AttackPatterns::getHtmlRemovedNoSlash()
{
    return getAllowedFromRegEx(AttackPatterns::m_htmlRemovedNoSlashRegExp);
}

StrangerAutomaton* AttackPatterns::getEncodeHtmlNoQuotes()
{
    StrangerAutomaton* star = StrangerAutomaton::makeAnyString();
    StrangerAutomaton* encoded = StrangerAutomaton::htmlSpecialChars(star, "ENT_NOQUOTES");
    delete star;
    return encoded;
}

StrangerAutomaton* AttackPatterns::getEncodeHtmlCompat()
{
    StrangerAutomaton* star = StrangerAutomaton::makeAnyString();
    StrangerAutomaton* encoded = StrangerAutomaton::htmlSpecialChars(star, "ENT_COMPAT");
    delete star;
    return encoded;
}

StrangerAutomaton* AttackPatterns::getEncodeHtmlQuotes()
{
    StrangerAutomaton* star = StrangerAutomaton::makeAnyString();
    StrangerAutomaton* encoded = StrangerAutomaton::htmlSpecialChars(star, "ENT_QUOTES");
    delete star;
    return encoded;
}

StrangerAutomaton* AttackPatterns::getEncodeHtmlSlash()
{
    StrangerAutomaton* star = StrangerAutomaton::makeAnyString();
    StrangerAutomaton* encoded = StrangerAutomaton::htmlSpecialChars(star, "ENT_SLASH");
    delete star;
    return encoded;
}

StrangerAutomaton* AttackPatterns::getHtmlAttrEscaped()
{
    return getAllowedFromRegEx(AttackPatterns::m_htmlAttrEscapedRegExp);
}

StrangerAutomaton* AttackPatterns::getJavascriptEscaped()
{
    return getAllowedFromRegEx(AttackPatterns::m_javascriptEscapedRegExp);
}

StrangerAutomaton* AttackPatterns::getUrlEscaped()
{
    return getAllowedFromRegEx(AttackPatterns::m_urlEscapedRegExp);
}

StrangerAutomaton* AttackPatterns::getUrlComponentEncoded()
{
    StrangerAutomaton* star = StrangerAutomaton::makeAnyString();
    StrangerAutomaton* encoded = StrangerAutomaton::encodeURIComponent(star);
    delete star;
    return encoded;
}

StrangerAutomaton* AttackPatterns::getUndesiredSQLTest()
{
    
    //    debug("Building automaton for SQL vulnerabilities");
    //    debug("----------------------------------------");
    //    StrangerAutomaton* autoAttackPattern = regExToAuto("/.*'or 1=1'.*/", true, int32_t(0));
    //    debug("----------------------------------------");
    //    return autoAttackPattern;
	throw new std::runtime_error("not implemented");
    
}

StrangerAutomaton* AttackPatterns::getUndesiredMFETest()
{
    
    //    StrangerAutomaton* retMe = regExToAuto("//evil/", true, int32_t(0));
    //    return retMe;
	throw new std::runtime_error("not implemented");   
}


StrangerAutomaton* AttackPatterns::getAttackPatternForContext(AttackContext context)
{
    switch (context) {
    case AttackContext::Html:
        return getHtmlPattern();
    case AttackContext::HtmlPayload:
        return getHtmlPayload();
    case AttackContext::HtmlAttr:
        return getHtmlAttributePattern();
    case AttackContext::JavaScript:
        return getJavascriptPattern();
    case AttackContext::Url:
        return getUrlPattern();
    default:
        return StrangerAutomaton::makeEmptyString();
    }
    return StrangerAutomaton::makeEmptyString();
}

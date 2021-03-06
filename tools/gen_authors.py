#!/usr/bin/python
#
# Copyright (C) 2008-2013 by Alexey S. Malakhov <brezerk@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import argparse
import os
import yaml

D_DATADIR = os.path.join(os.path.dirname(__file__), '../src/q4wine-gui')
D_TXTDIR = os.path.join(os.path.dirname(__file__), '../')

class AuthrosTxtTemplate:
    def __init__(self):
        filename = os.path.join(D_TXTDIR, 'AUTHORS')
        print("-- Writing %s --\n" % filename)
        self.header__ = open(filename, 'w')
        self.header()
        self.index__=2

    def close(self):
        self.header__.close()

    def header(self):
        self.header__.write("""
	                      ==BrezBlock==

	                                      Volume 0x00, Issue 0x00

|=-------------------------=[ Introduction ]=----------------------------=|

                "A journey of a thousand miles begins with a single step."
                                             --Lao-tzu, The Way of Lao-tzu
                                     Chinese philosopher (604 BC - 531 BC)

Thank you all who made this software become available to the public
Thank you for your hard work, debugging, translations, packaging and so forth.
Tremendous work was done. But we were successful :)

There are also a lot of people who provide feature requests, bug reports
and feedback. Despite the fact that they are not listed in this file,
their contribution is invaluable.

""")

    def head(self, group):
        self.header__.write("""
|=-------------------------=[ %s ]=----------------------------=|
""" % group)

    def line(self, text=" "):
        self.header__.write("""
%s""" % text)

class AuthrosHtmlTemplate:
    def __init__(self):
        filename = os.path.join(D_DATADIR, 'authors.h')
        print("-- Writing %s --\n" % filename)
        self.header__ = open(filename, 'w')
        self.header()

    def close(self):
        self.header__.close()

    def header(self):
        self.header__.write("""/***************************************************************************
*   Copyright (C) 2008-2013 by Alexey S. Malakhov <brezerk@gmail.com>     *
*                                                                         *
*   This program is free software: you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation, either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
*                                                                         *
***************************************************************************/

/*
 * Do NOT edit this file
 *
 * This file will be replaced by cmake and toos/gen_authors.py.
 * Edit src/q4wine-gui/authors.yaml file instead.
 *
 */

""")

    def head(self, group):
        self.header__.write(u"""
#define T_%s \"<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN' 'http://www.w3.org/TR/REC-html40/strict.dtd'>\" \\
\"<html>\" \\
    \"<head>\" \\
        \"<meta name='qrichtext' content='1' />\" \\
	\"<style type='text/css'>p, li { white-space: pre-wrap; }</style>\" \\
    \"</head>\" \\
    \"<body>\" \\""" % group.upper())

    def footer(self):
        self.header__.write(u"""
     \"</body>\" \\
\"<html>\"
""")

    def line(self, text=" "):
        self.header__.write(u"""
        \"<p style='margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'>\" \\
	    \"%s\" \\
	\"</p>\" \\""" % text)

    def mail_to(self, mail, text='E-Mail'):
        self.header__.write(u"""
        \"<p style='margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'>\" \\
	    \"<span style='color:#6495ed;'>%s</span>: <a href='mailto:%s' style='text-decoration: none; color:#5f9ea0;'>%s</a>\" \\
	\"</p>\" \\""" % (text, mail, mail))

    def web(self, web):
        self.header__.write(u"""
        \"<p style='margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'>\" \\
	    \"<span style='color:#6495ed;'>Web</span>: <a href='%s' style='color:#5f9ea0;'>%s</a>\" \\
	\"</p>\" \\""" % (web, web))

def generate_authors():
    print("-- Generating authors.h --\n")
    filename = os.path.join(D_DATADIR, 'authors.yaml')
    print("-- Reading %s --\n" % filename)

    with open(filename, 'r') as f:
        data = yaml.safe_load(f)

    html_tpl = AuthrosHtmlTemplate()
    txt_tpl = AuthrosTxtTemplate()

    for group, persons in data.iteritems():
        print("-- Parse group T_%s --\n" % group.upper())
        html_tpl.head(group)
        txt_tpl.head(group)
        for person in persons:
            nick = ""
            if person.has_key('nick'):
                nick = u"(%s)" % person['nick']
            html_tpl.line(u"%s %s" % (person['name'], nick))
            html_tpl.line(person['info'])
            txt_tpl.line(u"%s %s" % (person['name'], nick))
            txt_tpl.line(person['info'])
            if person.has_key('mail'):
                html_tpl.mail_to(person['mail'])
                txt_tpl.line("mail: %s" % person['mail'])
            if person.has_key('jid'):
                html_tpl.mail_to(person['jid'], "Jabber")
                txt_tpl.line("jabber: %s" % person['jid'])
            if person.has_key('web'):
                html_tpl.web(person['web'])
                txt_tpl.line("web: %s" % person['web'])
            if person.has_key('location'):
                html_tpl.line(person['location'])
                txt_tpl.line(person['location'])
            html_tpl.line()
            txt_tpl.line()
        html_tpl.footer()

    html_tpl.close()
    txt_tpl.close()

    print("-- Done --\n")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Parse authors.yaml file and create authors.h file.')
    parser.add_argument('--generate', dest='generate', action='store_true',
                        required=True,
                        help='Generate authors.h file')

    args = parser.parse_args()

    if args.generate:
        generate_authors()

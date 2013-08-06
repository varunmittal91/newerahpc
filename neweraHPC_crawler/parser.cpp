/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
 *	NeweraHPC program is distributed under the terms of the GNU General Public License v2
 *
 *	This file is part of NeweraHPC.
 *
 *	NeweraHPC is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 2 of the License.
 *
 *	NeweraHPC is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>

#include <neweraHPC/neweraHPC.h>

#include <include/html_parser.h>
#include <include/url.h>
#include <include/fetch_page.h>

using namespace std;
using namespace search;

int main(int argc, char **argv)
{
   neweraHPC_init(argc, argv);
   http_setup();
   
   const char *file_name = nhpc_get_cmdline_argument("l");
   
   if(file_name)
   {
      FetchPage *page = new FetchPage(file_name, "/tmp/out");
      page->get();
      delete page;

      HTMLParser *parser = new HTMLParser("/tmp/out", "/tmp/parsed");
      (*parser).parse();
      (*parser).write_output();
      delete parser;      
   }
}   

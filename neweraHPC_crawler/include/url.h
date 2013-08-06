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

#include <neweraHPC/strings.h>

using namespace std;
using namespace neweraHPC;

namespace search
{
#define _URL_COMPLETE_URL 0
#define _URL_PROTO        1
#define _URL_DOMAIN       2
#define _URL_PORT         3
#define _URL_REQUEST      4
   
#define _URL_set_property(u, p, v)  (u->strings[p] = (char *)v)
#define _URL_get_property(u, p)     ((char *)(u->strings[p]))
#define _URL_set_domain(u, v)       _URL_set_property(u, _URL_DOMAIN, v)
#define _URL_set_port(u, v)         _URL_set_property(u, _URL_PORT, v)
#define _URL_set_request(u, v)      _URL_set_property(u, _URL_REQUEST, v)
#define _URL_set_proto(u, v)        _URL_set_property(u, _URL_PROTO, v)
#define _URL_set_complete_url(u, v) _URL_set_property(u, _URL_COMPLETE_URL, v)
#define _URL_get_domain(u)          _URL_get_property(u, _URL_DOMAIN)
#define _URL_get_port(u)            _URL_get_property(u, _URL_PORT)
#define _URL_get_request(u)         _URL_get_property(u, _URL_REQUEST)
#define _URL_get_proto(u)           _URL_get_property(u, _URL_PROTO)
#define _URL_get_complete_url(u, v) _URL_get_property(u, _URL_COMPLETE_URL)
   
   string_t *getURL(const char *link, const char *parentLink = NULL);
   void destructURL(string_t *url);
};
/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
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

namespace neweraHPC
{
#define HTTP_STATUS_OK               0   // 200
#define HTTP_STATUS_MOVED            1   // 301
#define HTTP_STATUS_FOUND            2   // 302
#define HTTP_STATUS_BAD_REQUEST      3   // 400
#define HTTP_STATUS_UNAUTHORIZED     4   // 401
#define HTTP_STATUS_FORBIDDEN        5   // 403
#define HTTP_STATUS_NOTFOUND         6   // 404
#define HTTP_STATUS_SERVER_ERROR     7   // 500
#define HTTP_STATUS_NOT_INMPLEMENTED 8   // 501
   typedef short int http_status_t;
   
   static struct _http_status_codes
   {
      const char *string;
      int         code;
   }http_status_codes[] = {
      "OK", 200,
      "Moved Permanently", 301,
      "Found", 302,
      "Bad Request", 400,
      "Unauthorized", 401,
      "Forbidden", 403,
      "Not Found", 404,
      "Internal Server Error", 500,
      "Not Implemented", 501
   };
   
#define _http_status_set_status(hs, s)   (hs = s)
#define _http_status_check_status(hs, s) (hs == s)
#define http_status_get_code(hs)         (http_status_codes[hs].code)
#define http_status_get_string(hs)       (http_status_codes[hs].string)
   
#define HTTP_ERROR_PAGE "<html><head><title>Content Not Found</title></head><body><center><h1>404 - Content Not Found</h1></center></body></html>"
};
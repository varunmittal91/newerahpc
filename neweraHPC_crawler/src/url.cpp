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

#include <include/url.h>

using namespace std;
using namespace neweraHPC;

namespace search
{
   const char *getURLProto(const char *link, const char **bareLink)
   {
      int pos = nhpc_strfind(link, ':');
      if(pos == 0 || link[pos] != '/' || link[pos + 1] != '/')
      {
	 (*bareLink) = link;
	 return NULL;
      }
      
      const char *proto = nhpc_substr(link, 1, pos - 1);
      
      if(bareLink)
	 (*bareLink) = (link + pos + 2);
      
      return proto;
   }
   
   bool checkURLhasProto(const char *link)
   {
      int pos = nhpc_strfind(link, ':');
      if(pos == 0 || link[pos] != '/' || link[pos + 1] != '/')
	 return false;
      
      return true;
   }
   
   const char *getURLDomain(const char **bareLink)
   {
      int pos = nhpc_strfind((*bareLink), ':');
      if(pos == 0)
      {
	 pos = nhpc_strfind((*bareLink), '/');
	 if(pos == 0)
	    pos = strlen(*bareLink);
	 else 
	    pos--;	 
      }
      else 
	 pos--;
      
      const char *domain = nhpc_substr((*bareLink), 1, pos);
      (*bareLink) = (*bareLink + pos);
      
      return domain;
   }
   
   const char *getURLRequest(const char *complete_url)
   {
      const char *request;
      
      int pos = nhpc_strfind(complete_url, '/');
      if(complete_url[pos - 2] == ':')
      {
	 pos = pos + 2;
	 pos = nhpc_strfind(complete_url, '/', pos);
      }
      
      if(pos == 0)
	 nhpc_strcpy((char **)&request, "/");
      else 
      {
	 request = nhpc_substr(complete_url, pos, strlen(complete_url));
      }
      
      return request;
   }
   
   void destructURL(string_t *url)
   {
      for(int i = 0; i < 5; i++)
	 delete[] (url->strings[i]);
      delete url;
   }
   
   const char *getURLPort(const char *bareLink, const char *proto)
   {
      const char *port = NULL;
      
      int pos = nhpc_strfind(bareLink, ':');
      int pos2;
      if(pos != 0)
      {
	 pos2 = nhpc_strfind(bareLink, '/', pos);
	 if(pos2 == 0)
	 {
	    pos2 = nhpc_strfind(bareLink, '?', pos);
	    if(pos2 != 0)
	       pos2--;
	 }
	 else 
	    pos2--;
	 
	 if(pos2 != 0)
	    port = nhpc_substr(bareLink, pos + 1, pos2);
      }
      
      if(!port)
      {
	 if(nhpc_strcmp(proto, "http") == NHPC_SUCCESS)
	    nhpc_strcpy((char **)&port, "80");
      }
      
      return port;
   }
   
   string_t *getURL(const char *link, const char *parentLink)
   {
      string_t *url = NULL;
      const char *complete_url = NULL;
      const char *proto        = NULL;
      const char *domain       = NULL;
      const char *bareLink     = NULL;
      const char *request      = NULL;
      const char *port         = NULL;
      
      if(*link == '/' && parentLink)
      {
         proto = getURLProto(parentLink, &bareLink);	 

	 if(*(link + 1) == '/')
	 {
            complete_url = nhpc_strconcat(proto, ":", link);
	 }
	 else 
	 {
	    if(*link == '/')
	       link++;
	    complete_url = nhpc_strconcat(parentLink, "/", link);
	 }	 
      }
      else if(*link == '#' && parentLink)
      {
	 proto        = getURLProto(parentLink, &bareLink);
	 complete_url = nhpc_strconcat(proto, link);
      }
      else 
      {
	 proto = getURLProto(link, &bareLink);
	 if(!proto)
	 {
	    nhpc_strcpy((char **)&proto, "http");
	 }
	 
	 nhpc_strcpy((char **)&complete_url, link);
      }
      
      if(!complete_url || !proto || !bareLink) 
      {
	 cout << "component missng" << endl;
	 goto return_url;
      }
      
      port = getURLPort(bareLink, proto);
      
      if(!port)
      {
	 cout << port << endl;
	 goto return_url;
      }
      
      domain  = getURLDomain(&bareLink);
      request = getURLRequest(complete_url);
	 
      cout << "Protocol:" << proto << endl;
      cout << "Complete URL:" << complete_url << endl;
      cout << "Domain:" << domain << endl;
      cout << "Port:" << port << endl;
      cout << "Request:" << request << endl;      
      cout << endl;
      
      url = new string_t;
      url->count = 5;
      url->strings = new char* [5];

      _URL_set_domain(url, domain);
      _URL_set_port(url, port);
      _URL_set_proto(url, proto);
      _URL_set_request(url, request);
      _URL_set_complete_url(url, complete_url);
      
   return_url:
      if(!url)
      {
	 if(complete_url)
	    delete[] complete_url;
	 if(proto)
	    delete[] proto;
	 if(domain)
	    delete[] domain;
	 if(request)
	    delete[] request;
	 if(port)
	    delete[] port;
      }
      
      return url;
   }
};

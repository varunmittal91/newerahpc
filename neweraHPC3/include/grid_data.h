/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com> & Varun Dhawan <varundhawan5792@gmail.com>
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

#ifndef _GRID_DATA_H_
#define _GRID_DATA_H_

/* Grid configuration file which will contain peer list */
#define grid_conf_file "/etc/grid.conf"

typedef void *(fnc_ptr)(void *);

namespace neweraHPC
{
   /* Plugin details for the modules that are added to be run in a grid */
   struct plugin_details_t
   {
      /* Address of the base plugin function that server will run*/
      fnc_ptr ptr_exec;
      
      /* Address of the plugin function that client will run 
       for assembly of data and other purposes */
      fnc_ptr ptr_client;

      /* Address of processing function for actual implementation 
       of the algorith in plugin */
      fnc_ptr ptr_processor;
      
      /* Path of plugin on the server */
      char *path_plugin;
      
      /* Path of nxi file for plugin on server */
      char *path_nxi;
   };
   
   /* Peer details maintained at the server */
   struct peer_details_t
   {
      /* Unique Peer id */
      int id;
      
      /* Port for peer server */
      int port;
      
      /* Address for peer server */
      char *host;
      
      /* Number of processors on perr server */
      int processors;
      
      /* Weight of the peer server, calculated on the basis
       of processors and network latency */
      int weight;
      
      /* Status of peer server, wether busy or available */
      bool status;
   };
   
   struct plugin_request_t
   {
      int peer_id;
      int remote_plugin_id;
      bool status;
   };   
};

#endif
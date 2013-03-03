#include "hashtable.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

static hash_size def_hashfunc(const char *key);
static char *mystrdup(const char *s);
static int DEBUG= 0;

static bool isDir(struct dirent* dent){
       char name[100];
       struct stat s;

       sprintf(name, "/proc/%s", dent->d_name);//need to see if it is full path
       if(stat(name, &s)){
           printf("Error in stat\n");
           exit(-1);
       }
       if(s.st_mode&S_IFDIR)
             return true;
       return false;
}

HASHTBL *hashtbl_create(hash_size size, hash_size (*hashfunc)(const char *)){
         HASHTBL *hashtbl;

         if(!(hashtbl=malloc(sizeof(HASHTBL)))) return NULL;

         if(!(hashtbl->nodes=calloc(size, sizeof(struct hashnode_s*)))) {
              free(hashtbl);
              return NULL;
         }
         hashtbl->size=size;

         if(hashfunc) hashtbl->hashfunc=hashfunc;
         else hashtbl->hashfunc=def_hashfunc;

         hashtbl_fill(hashtbl);
         return hashtbl;
}

void hashtbl_fill(HASHTBL *hashtbl)
{
       DIR *dir;
       char* proc = "/proc";
       struct dirent* dent;

       dir = opendir(proc);
       if(!dir){
          printf("%s doesn't exist\n", proc);
          exit(1);
       }

      while(dent=readdir(dir)){
          if(isDir(dent)){
               char cmdline_file[100];
               FILE* fh;
               if(!strcmp(dent->d_name,".") || !strcmp(dent->d_name, "..")) continue;

               pid_t pid;
               char cmd_name[100];

               sprintf(cmdline_file,"/proc/%s/cmdline",dent->d_name);
               if(fh = fopen(cmdline_file,"r")){
                     sscanf(dent->d_name, "%d", &pid); //nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnote
                     if(fgets(cmd_name, 100, fh)){
                           if(DEBUG)
                                 printf("Found pair (%s %d)\n", cmd_name, pid);//nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnote
                            hashtbl_insert(hashtbl, cmd_name, pid);
                     }else
                          if(DEBUG)
                                 printf("Cannot get cmd_name from %s\n", cmdline_file);
                     fclose(fh);
               }
         }
   }
}


void hashtbl_destroy(HASHTBL *hashtbl)
{
      hash_size n;
      struct hashnode_s *node, *oldnode;

      for(n=0; n<hashtbl->size; ++n) {
             node=hashtbl->nodes[n];
             while(node) {
                       free(node->key);
                       oldnode=node;
                       node=node->next;
                       free(oldnode);
             }
      }
      free(hashtbl->nodes);
      free(hashtbl);
}

int hashtbl_insert(HASHTBL *hashtbl, const char *key, pid_t data)
{
      struct hashnode_s *node;
      hash_size hash=hashtbl->hashfunc(key)%hashtbl->size;
      node=hashtbl->nodes[hash];

      if(DEBUG)
           //printf("Ready to insert (%s, %hu)\n",key, data);
           printf("Ready to insert (%s, %d)\n",key, data);
      while(node) {
//           if(!strcmp(node->key, key)) {
//               node->data=data;
//               return 0;
//            }
            node=node->next;
     }
     if(!(node=malloc(sizeof(struct hashnode_s)))) return -1;
     if(!(node->key=mystrdup(key))) {
               free(node);
               return -1;
     }
     node->data=data;
     node->next=hashtbl->nodes[hash];
     hashtbl->nodes[hash]=node;

     return 0;
}

void hashtbl_traverse(HASHTBL *hashtbl, hash_size size)
{
      int i;

      printf("size: %d\n",size);
      for(i=0;i<size;i++){
            struct hashnode_s *node = hashtbl->nodes[i];

            while(node){
               printf("(%s %hu)", node->key,node->data);
               node = node->next;
            }
            printf("\n");
      }
}

int hashtbl_remove(HASHTBL *hashtbl, const char *key)
{
      struct hashnode_s *node, *prevnode=NULL;
      hash_size hash=hashtbl->hashfunc(key)%hashtbl->size;

      node=hashtbl->nodes[hash];
      while(node) {
             if(!strcmp(node->key, key)) {
                   free(node->key);
                   if(prevnode) prevnode->next=node->next;
                   else hashtbl->nodes[hash]=node->next;
                      free(node);
                       return 0;
              }
              prevnode=node;
              node=node->next;
     }

     return -1;
}

pid_t hashtbl_get(HASHTBL *hashtbl, const char *key)
{
       struct hashnode_s *node;
       hash_size hash=hashtbl->hashfunc(key)%hashtbl->size;

       /* fprintf(stderr, "hashtbl_get() key=%s, hash=%d\n", key, hash);*/

        node=hashtbl->nodes[hash];
        while(node) {
                if(!strcmp(node->key, key)) return node->data;
                node=node->next;
        }

        return 0;
}

int hashtbl_resize(HASHTBL *hashtbl, hash_size size)
{
      HASHTBL newtbl;
      hash_size n;
      struct hashnode_s *node,*next;

      newtbl.size=size;
      newtbl.hashfunc=hashtbl->hashfunc;

      if(!(newtbl.nodes=calloc(size, sizeof(struct hashnode_s*)))) return -1;

       for(n=0; n<hashtbl->size; ++n) {
                for(node=hashtbl->nodes[n]; node; node=next) {
                            next = node->next;
                            hashtbl_insert(&newtbl, node->key, node->data);
                            hashtbl_remove(hashtbl, node->key);

                 }
       }

       free(hashtbl->nodes);
       hashtbl->size=newtbl.size;
       hashtbl->nodes=newtbl.nodes;

       return 0;
}

static char *mystrdup(const char *s)
{
      char *b;
      if(!(b=malloc(strlen(s)+1))) return NULL;
      strcpy(b, s);
      return b;
}

static hash_size def_hashfunc(const char *key)
{
      hash_size hash=0;

      while(*key) hash+=(unsigned char)*key++;

      return hash;
}


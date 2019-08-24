/* 
   ARBEL is a REGISTER BASED ENVIRONMENT AND LANGUAGE
   Copyright 2019 Zach Flynn

   This file is part of ARBEL.

   ARBEL is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ARBLE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ARBEL (in COPYING file).  If not, see <https://www.gnu.org/licenses/>.
   
*/

#include "arbel.h"

int
save_registry (FILE* f, registry* reg)
{

  reg = tail(reg);
  int size;

  while (reg != NULL)
    {
      if (reg->value->type != OPERATION &&
          reg->value->type != ACTIVE_INSTRUCTION &&
          reg->value->type != REFERENCE &&
          reg->value->type != NOTHING)
        {
          fwrite(&reg->value->type, sizeof(data_type), 1, f);
      
          switch (reg->value->type)
            {
            case INTEGER:
              fwrite(reg->value->data, sizeof(int), 1, f);
              break;
            case DECIMAL:
              fwrite(reg->value->data, sizeof(double), 1, f);
              break;
            case STRING:
              size = strlen((char*) reg->value->data);
              fwrite(&size, sizeof(int), 1, f);
              fwrite(reg->value->data, sizeof(char),
                     strlen((char*) reg->value->data), f);
              break;
            case REGISTER:
              size = strlen((char*) reg->value->data);
              fwrite(&size, sizeof(int), 1, f);
              fwrite(reg->value->data, sizeof(char),
                     strlen((char*) reg->value->data), f);
              break;
            case REGISTRY:
              save_registry(f, (registry*) reg->value->data);
              size = NOTHING;
              fwrite(&size, sizeof(data_type), 1, f);
              break;
            case INSTRUCTION:
              size = strlen(((instruction*) reg->value->data)->code);
              fwrite(&size, sizeof(int), 1, f);
              fwrite(((instruction*) reg->value->data)->code, sizeof(char),
                     strlen(((instruction*) reg->value->data)->code), f);
              break;
	    default:
	      break;
            }

          size = strlen(reg->key);
          fwrite(&size, sizeof(int), 1, f);
          fwrite(reg->key, sizeof(char), strlen(reg->key), f);

        }
      
      reg = reg->right;

    }

  return 0;
}

int
read_registry (FILE* f, registry* reg)
{
  data_type* type_cache = malloc(sizeof(data_type));
  void* cache;
  data* d;
  registry* r;
  int size;
  statement* stmt = NULL;
  parser_state state;
  FILE* f_sub;
  while (fread(type_cache, sizeof(data_type), 1, f)
         && (*type_cache != NOTHING))
    {
      switch (*type_cache)
        {
        case INTEGER:
          cache = malloc(sizeof(int));
          fread(cache, sizeof(int), 1, f);
          assign_int(&d, *((int*) cache));
          free(cache);
          break;
        case DECIMAL:
          cache = malloc(sizeof(double));
          fread(cache, sizeof(double), 1, f);
          assign_dec(&d, *((double*) cache));
          free(cache);
          break;
        case STRING:
          cache = malloc(sizeof(int));
          fread(cache, sizeof(int), 1, f);
          size = *((int*) cache);
          free(cache);
          cache = malloc(sizeof(char)*(size+1));
          fread(cache, sizeof(char), size, f);
          *((char*) (cache+size)) = '\0';
          assign_str(&d, (char*) cache);
          free(cache);
          break;
        case REGISTER:
          cache = malloc(sizeof(int));
          fread(cache, sizeof(int), 1, f);
          size = *((int*) cache);
          free(cache);
          cache = malloc(sizeof(char)*(size+1));
          fread(cache, sizeof(char), size, f);
          *((char*) (cache+size)) = '\0';
          assign_regstr(&d, (char*) cache);
          free(cache);          
          break;
        case REGISTRY:
          r = new_registry(reg);
          read_registry(f, r);
          assign_registry(&d, r);
          free_registry(r);
          break;
        case INSTRUCTION:
          cache = malloc(sizeof(int));
          fread(cache, sizeof(int), 1, f);
          size = *((int*) cache);
          free(cache);
          cache = malloc(sizeof(char)*(size+1));
          fread(cache, sizeof(char), size, f);
          *((char*) (cache+size)) = '\0';
          f_sub = fmemopen(cache, strlen(cache), "r");
          state = fresh_state(0);
          parse(f_sub, &state, &stmt);
          assign_instr(&d, stmt, (char*) cache);
          free_statement(stmt);
          stmt = NULL;
          free(cache);
          fclose(f_sub);
          break;
	default:
	  break;
        }
      cache = malloc(sizeof(int));
      fread(cache, sizeof(int), 1, f);
      size = *((int*) cache);
      free(cache);
      cache = malloc(sizeof(char)*(size+1));
      fread(cache, sizeof(char), size, f);
      *((char*) (cache+size)) = '\0';

      set(reg, d, (char*) cache);
      free(cache);

    }
      
  reg = reg->right;

  return 0;
  
}

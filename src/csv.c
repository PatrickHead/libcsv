/*
 *  Copyright 2019, 2020, 2022, 2025 Patrick Head
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file csv.c
 * @brief  Source file for libcsv
 *
 * A simple library to aid in manipulating data in CSV files.
 */

	// System headers

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

	// Project headers

#include "csv.h"

	// Defines and macros

#define CHUNK 256  /**<  CHUNK size used for block memory reallocation */

	// Internal function declarations

static char *in_quote(FILE *f);
static int need_quote(char *s);

  /**
   *  @fn csv *csv_new(void)
   *
   *  @brief create a new @a csv struct
   *
   *  @par Parameters
   *  None.
   *
   *  @return pointer to new @a csv struct
   */
  
csv *csv_new(void)
{
  csv *data;

  data = (csv *)malloc(sizeof(csv));
  if (!data)
    return NULL;

  memset(data, 0, sizeof(csv));

  return data;
}

  /**
   *  @fn void csv_free(csv *data)
   *
   *  @brief frees all memory allocated to @p data
   *
   *  @param data - pointer to existing @a csv struct
   *
   *  @par Returns
   *  Nothing.
   */
  
void csv_free(csv *data)
{
  int i;

  if (!data)
    return;

  if (data->record)
  {
    for (i = 0; i < data->n_records; i++)
      csv_record_free(data->record[i]);

    free(data->record);
  }

  free(data);

  return;
}

  /**
   *  @fn csv *csv_read(FILE *f)
   *
   *  @brief creates a new @a csv, loading data from an open file
   *
   *  @param f - FILE pointer
   *
   *  @return pointer to new @a csv struct
   */

csv *csv_read(FILE *f)
{
  csv *data = NULL;
  int c;
  char *sub = NULL;
  int cnt;
  static int ssize = 0;
  static char *s = NULL;
  int l;
  char *field;
  csv_record *rec;
  int need_rec = 1;

  if (!f)
  {
    if (s)
      free(s);
    s = NULL;
    ssize = 0;
    return NULL;
  }

  if (s)
    memset(s, 0, ssize);

  cnt = 0;

  data = csv_new();
  if (!data)
    return NULL;

  while ((c = fgetc(f)) != EOF)
  {
    if (need_rec)
    {
      rec = csv_record_new();
      if (!rec)
        break;
      need_rec = 0;
    }

    switch (c)
    {
      case '"':

        l = 0;
        sub = in_quote(f);
        if (sub)
        {
          l = strlen(sub);
          while ((cnt + l) >= ssize)
          {
            if (s)
              s = (char *)realloc(s, ssize + CHUNK);
            else
              s = (char *)malloc(CHUNK);
            memset(&s[cnt], 0, CHUNK);
            ssize += CHUNK;
          }
          strcpy(&s[cnt], sub);
          cnt += l;
        }

        break;

      case '\r':
        break;

      case ',':
      case '\n':

        field = strdup(s);
        memset(s, 0, ssize);
        cnt = 0;

        csv_field_append(rec, field);

        if (c == '\n')
        {
          csv_record_append(data, rec);
          need_rec = 1;
        }

        break;

      default:

        if (ssize <= cnt)
        {
          if (s)
            s = (char *)realloc(s, ssize + CHUNK);
          else
            s = (char *)malloc(CHUNK);
          memset(&s[cnt], 0, CHUNK);
          ssize += CHUNK;
        }

        s[cnt] = c;
        ++cnt;

        break;
    }
  }

  return data;
}

  /**
   *  @fn int csv_write(FILE *f, csv *data)
   *
   *  @brief writes data in @p data to open file
   *
   *  @param f - FILE pointer
   *  @param data - pointer to existing @a csv struct
   *
   *  @return STDIO error, 0 for success.
   */


int csv_write(FILE *f, csv *data)
{
  int i, j;
  int quote = 0;

  if (!f || !data)
    return -1;

  for (i = 0; i < data->n_records; i++)
  {
    for (j = 0; j < data->record[i]->n_fields; j++)
    {
      if (j)
        fprintf(f, ",");
      quote = need_quote(data->record[i]->field[j]);
      if (quote)
        fprintf(f, "\"");
      fprintf(f, "%s", data->record[i]->field[j]);
      if (quote)
        fprintf(f, "\"");
    }
    fprintf(f, "\r\n");
  }

  return 0;
}

  /**
   *  @fn csv_record *csv_record_new(void)
   *
   *  @brief creates a new @a csv_record
   *
   *  @par Parameters
   *  None.
   *
   *  @return pointer to new @a csv_record struct
   */


csv_record *csv_record_new(void)
{
  csv_record *r;

  r = (csv_record *)malloc(sizeof(csv_record));
  if (!r)
    return NULL;

  memset(r, 0, sizeof(csv_record));

  return r;
}

  /**
   *  @fn void csv_record_free(csv_record *record)
   *
   *  @brief frees all memory allocated to @p record
   *
   *  @param record - pointer to existing @a csv_record struct
   *
   *  @par Returns
   *  Nothing.
   */

void csv_record_free(csv_record *record)
{
  int i;

  if (!record)
    return;

  if (record->field)
  {
    for (i = 0; i < record->n_fields; i++)
      free(record->field[i]);

    free(record->field);
  }

  free(record);

  return;
}

  /**
   *  @fn csv_record *csv_record_copy(csv_record *record)
   *
   *  @brief creates a deep copy of @p record
   *
   *  @param record - pointer to existing @a csv_record
   *
   *  @return pointer to new @a csv struct
   */

csv_record *csv_record_copy(csv_record *record)
{
	csv_record *nr;
	int i;

	if (!record)
		return NULL;

	nr = csv_record_new();
	if (!nr)
		return NULL;

	for (i = 0; i < record->n_fields; i++)
		csv_field_append(nr, strdup(record->field[i]));

	return nr;
}

  /**
   *  @fn void csv_record_insert(csv *data, int index)
   *
   *  @brief inserts a new @a csv_record into a @a csv before the record in
   *  @p index.
   *
   *  NOTE:  The newly inserted record contains no data
   *
   *  @param data - pointer to existing @a csv structure
   *  @param index - index of record to insert @b before
   *
   *  @par Returns
   *  Nothing.
   */


void csv_record_insert(csv *data, int index)
{
  int i;

  if (!data) return;
  if (index < 0) return;
  if (index >= data->n_records) return;

  data->record = (csv_record **)malloc(sizeof(csv_record **) * data->n_records+1);
  if (!data->record) return;  // Something is VERY wrong here

    // Move all records to the "right" one slot
  for (i = data->n_records; i >= index; i--)
    data->record[i+1] = data->record[i];

  data->record[index] = csv_record_new();

  ++data->n_records;

  return;
}

  /**
   *  @fn void csv_record_remove(csv *data, int index)
   *
   *  @brief removes a record from @p data at @p index
   *
   *  @param data - pointer to existing @a csv struct
   *  @param index - index of record to remove
   *
   *  @par Returns
   *  Nothing.
   */

void csv_record_remove(csv *data, int index)
{
  int i;

  if (!data) return;
  if (index < 0) return;
  if (index >= data->n_records) return;

    // Free record that going to be removed

  csv_record_free(data->record[index]);

    // Move all records to the "left" one slot
  for (i = index; i < data->n_records; i++)
    data->record[i] = data->record[i+1];

  --data->n_records;

  data->record = (csv_record **)malloc(sizeof(csv_record **) * data->n_records);

  return;
}

  /**
   *  @fn void csv_record_append(csv *data, csv_record *record)
   *
   *  @brief appends @p record to @p data.
   *
   *  This function retains the memory allocated to @p record, therefore do
   *  NOT free the memory for @p record.
   *
   *  @param data - pointer to exising @a csv struct
   *  @param record - pointer to exising @a csv_record struct
   *
   *  @par Returns
   *  Nothing.
   */

void csv_record_append(csv *data, csv_record *record)
{
  if (!data || !record)
    return;

  if (data->record)
    data->record = (csv_record **)realloc(data->record,
                                          sizeof(csv_record *)
                                            * (data->n_records + 1));
  else
    data->record = (csv_record **)malloc(sizeof(csv_record *));

  data->record[data->n_records] = record;

  ++data->n_records;

  return;
}

  /**
   *  @fn void csv_field_insert(csv_record *record, int index)
   *
   *  @brief inserts a new field into @p record at @p index
   *
   *  NOTE:  The newly created field contains no data
   *
   *  @param record - pointer to existing @a csv_record struct
   *  @param index - index of field to insert @b before
   *
   *  @par Returns
   *  Nothing.
   */

void csv_field_insert(csv_record *record, int index)
{
  int i;

  if (!record) return;
  if (index < 0) return;
  if (index >= record->n_fields) return;

  record->field = (char **)malloc(sizeof(char **) * record->n_fields+1);
  if (!record->field) return;  // Something is VERY wrong here

    // Move all fields to the "right" one slot
  for (i = record->n_fields; i >= index; i--)
    record->field[i+1] = record->field[i];

  record->field[index] = NULL;

  ++record->n_fields;

  return;
}

  /**
   *  @fn void csv_field_remove(csv_record *record, int index)
   *
   *  @brief removes a field from @p record at @p index
   *
   *  @param record - pointer to existing @a csv_record struct
   *  @param index - index of field to remove
   *
   *  @par Returns
   *  Nothing.
   */

void csv_field_remove(csv_record *record, int index)
{
  int i;

  if (!record) return;
  if (index < 0) return;
  if (index >= record->n_fields) return;

    // Free field that going to be removed

  free(record->field[index]);

    // Move all fields to the "left" one slot
  for (i = index; i < record->n_fields; i++)
    record->field[i] = record->field[i+1];

  --record->n_fields;

  record->field = (char **)malloc(sizeof(char **) * record->n_fields);

  return;
}

  /**
   *  @fn void csv_field_append(csv_record *record, char *field)
   *
   *  @brief appends @p field to @p record.
   *
   *  This function retains the memory allocated to @p field, therefore do
   *  NOT free the memory for @p field.
   *
   *  @param record - pointer to exising @a csv_record struct
   *  @param field - pointer to string to append
   *
   *  @par Returns
   *  Nothing.
   */

void csv_field_append(csv_record *record, char *field)
{
  if (!record || !field)
    return;

  if (record->field)
    record->field = (char **)realloc(record->field,
                                     sizeof(char *)
                                       * (record->n_fields + 1));
  else
    record->field = (char **)malloc(sizeof(char *));

  record->field[record->n_fields] = field;

  ++record->n_fields;

  return;
}

  /**
   *  @fn char *in_quote(FILE *f)
   *
   *  @brief reads a quoted string from a FILE
   *
   *  Reading ends when a '"' character is read.  This assists in stripping any
   *  quoted fields from CSV input.  The reading starts at the current
   *  file position.
   *
   *  @param f - FILE pointer
   *
   *  @return pointer to the quoted string
   */

static char *in_quote(FILE *f)
{
  int cnt = 0;
  static int ssize = 0;
  static char *s = NULL;
  int c;

  if (!f)
  {
    if (s)
      free(s);
    s = NULL;
    ssize = 0;
    return NULL;
  }

  if (s)
    memset(s, 0, ssize);

  while ((c = fgetc(f)) != EOF)
  {
    if (c == '"')
      return s;

    if (cnt >= ssize)
    {
      if (s)
        s = (char *)realloc(s, ssize + CHUNK);
      else
        s = (char *)malloc(CHUNK);

      memset(&s[ssize], 0, CHUNK);

      ++ssize;
    }

    s[cnt] = c;

    ++cnt;
  }

  return NULL;
}

  /**
   *  @fn int need_quote(char *s)
   *
   *  @brief returns a flag indicating if string needs quotes
   *
   *  Flag indicates if a given string would need to be quoted for use in a CSV
   *  file.  If the string contains a carriage-return, newline, double-quote,
   *  or comma, then true is returned, otherwise the string is considered
   *  "CSV safe" as is, and a false is returned.
   *
   *  @param s - string to examine
   *
   *  @return 1 if quotes are requred, 0 otherwise
   */

static int need_quote(char *s)
{
  if (!s)
    return 0;

  while (*s)
  {
    if (*s == '\r' || *s == '\n' || *s == '"' || *s == ',')
      return 1;

    if (!isprint((int)*s))
      return 1;

    ++s;
  }

  return 0;
}


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
 * @file csv.h
 * @brief  Header file for libcsv
 *
 * A simple library to aid in manipulating data in CSV files.
 */

#ifndef CSV_H
#define CSV_H

#define VERSION "1.0.0"  /**<  Version of libcsv  */

	// Data types

		// CSV record (row), which consists of fields (column)

  /**
   * @typedef csv_record
   * @brief creates a type for @a csv_record struct
   */

typedef struct csv_record csv_record;

  /**
   * @struct csv_record
   * @brief structure holding fields in a CSV record
   */

struct csv_record
{
  int n_fields;						/**<  Number of fields in this record       */
  char **field;						/**<  Array of strings, one for each field  */
};

	// CSV file, which consists of records (row)

  /**
   * @typedef csv
   * @brief creates a type for @a csv struct
   */

typedef struct csv csv;

  /**
   * @struct csv
   * @brief structure which holds all CSV data
   */

struct csv
{
  int header_line;				/**<  0-indexed line number of header record  */
  int n_records;					/**<  Number of records in this CSV           */
  csv_record **record;		/**<  Array of records                        */
};

	// Functions for CSV data

csv *csv_new(void);
void csv_free(csv *data);
csv *csv_read(FILE *f);
int csv_write(FILE *f, csv *csv_data);

	// Functions for CSV records (rows)

csv_record *csv_record_new(void);
void csv_record_free(csv_record *record);
csv_record *csv_record_copy(csv_record *record);
void csv_record_insert(csv *data, int index);
void csv_record_remove(csv *data, int index);
void csv_record_append(csv *data, csv_record *record);
void csv_field_insert(csv_record *record, int index);
void csv_field_remove(csv_record *record, int index);
void csv_field_append(csv_record *record, char *field);

#endif //CSV_H

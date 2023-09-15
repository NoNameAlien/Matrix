#ifndef SRC_S21_MATRIX_H_
#define SRC_S21_MATRIX_H_

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 1
#define FAILURE 0

typedef enum errors { OK, ERROR_MATRIX, ERROR_CALCULATION } errors;

typedef struct matrix_struct {
  double **matrix;
  int rows;
  int columns;
} matrix_t;

// Main Funcnion
int s21_create_matrix(int rows, int columns, matrix_t *result);
void s21_remove_matrix(matrix_t *A);
int s21_eq_matrix(matrix_t *A, matrix_t *B);
int s21_sum_matrix(matrix_t *A, matrix_t *B, matrix_t *result);
int s21_sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result);
int s21_mult_number(matrix_t *A, double number, matrix_t *result);
int s21_mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result);
int s21_transpose(matrix_t *A, matrix_t *result);
int s21_calc_complements(matrix_t *A, matrix_t *result);
int s21_determinant(matrix_t *A, double *result);
int s21_inverse_matrix(matrix_t *A, matrix_t *result);

// Tools
bool validate(matrix_t *matrix);
int sum_sub(matrix_t *A, matrix_t *B, matrix_t *result, bool sign);
void minor_matrix(int row, int column, matrix_t *matrix, matrix_t *result);
double det_matrix(matrix_t *matrix);

#endif  // SRC_S21_MATRIX_H_
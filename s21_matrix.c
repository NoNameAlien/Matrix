#include "s21_matrix.h"

// Main Funcnion

int s21_create_matrix(int rows, int columns, matrix_t *result) {
  int status = OK;

  if (rows < 1 || columns < 1 || result == NULL) {
    status = ERROR_MATRIX;
  } else {
    result->rows = rows;
    result->columns = columns;
    result->matrix = (double **)calloc(rows, sizeof(double *));
    if (result->matrix == NULL)
      status = ERROR_MATRIX;
    else
      for (int i = 0; i < rows; i++)
        result->matrix[i] = (double *)calloc(columns, sizeof(double));
  }
  return status;
}

void s21_remove_matrix(matrix_t *A) {
  for (int i = 0; i < A->rows; i++) free(A->matrix[i]);
  free(A->matrix);
  A->matrix = NULL;
  A->columns = 0;
  A->rows = 0;
}

int s21_eq_matrix(matrix_t *A, matrix_t *B) {
  int status = A->rows == B->rows && A->columns == B->columns && validate(A) &&
               validate(B);

  if (status == SUCCESS)
    for (int i = 0; i < A->rows; i++)
      for (int j = 0; j < A->columns; j++)
        status &= fabs(A->matrix[i][j] - B->matrix[i][j]) <= 1e-7;

  return status;
}

int s21_sum_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  return sum_sub(A, B, result, true);
}

int s21_sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  return sum_sub(A, B, result, false);
}

int s21_mult_number(matrix_t *A, double number, matrix_t *result) {
  int status = OK;

  if (!validate(A))
    status = ERROR_MATRIX;
  else if (s21_create_matrix(A->rows, A->columns, result) != OK)
    status = ERROR_MATRIX;
  else
    for (int i = 0; i < A->rows; i++)
      for (int j = 0; j < A->columns; j++)
        result->matrix[i][j] = A->matrix[i][j] * number;

  return status;
}

int s21_mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int status = OK;

  if (!validate(A) || !validate(B))
    status = ERROR_MATRIX;
  else if (A->columns != B->rows)
    status = ERROR_CALCULATION;
  else if (s21_create_matrix(A->rows, B->columns, result) != OK)
    status = ERROR_MATRIX;
  else
    for (int i = 0; i < A->rows; i++)
      for (int j = 0; j < B->columns; j++)
        for (int k = 0; k < A->columns; k++)
          result->matrix[i][j] += A->matrix[i][k] * B->matrix[k][j];
  return status;
}

int s21_transpose(matrix_t *A, matrix_t *result) {
  int status = OK;

  if (!validate(A))
    status = ERROR_MATRIX;
  else if (s21_create_matrix(A->columns, A->rows, result) != OK)
    status = ERROR_MATRIX;
  else
    for (int i = 0; i < A->rows; i++)
      for (int j = 0; j < A->columns; j++)
        result->matrix[j][i] = A->matrix[i][j];

  return status;
}

int s21_calc_complements(matrix_t *A, matrix_t *result) {
  int status = OK;

  if (!validate(A))
    status = ERROR_MATRIX;
  else if (A->rows != A->columns)
    status = ERROR_CALCULATION;
  else {
    s21_create_matrix(A->rows, A->columns, result);

    if (A->rows == 1)
      result->matrix[0][0] = A->matrix[0][0];
    else {
      int sign;
      matrix_t temp;

      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          minor_matrix(i, j, A, &temp);
          sign = ((i + j) % 2 == 0) ? 1 : -1;
          result->matrix[i][j] = det_matrix(&temp) * sign;
          s21_remove_matrix(&temp);
        }
      }
    }
  }

  return status;
}

int s21_determinant(matrix_t *A, double *result) {
  int status = OK;

  if (!validate(A))
    status = ERROR_MATRIX;
  else if (A->rows != A->columns)
    status = ERROR_CALCULATION;
  else
    *result = det_matrix(A);

  return status;
}

int s21_inverse_matrix(matrix_t *A, matrix_t *result) {
  int status = OK;

  if (!validate(A))
    status = ERROR_MATRIX;
  else if (A->rows != A->columns)
    status = ERROR_CALCULATION;
  else {
    double determinant = det_matrix(A);

    if (fabs(determinant) < 1e-7)
      status = ERROR_CALCULATION;
    else {
      matrix_t comp, trans;

      s21_calc_complements(A, &comp);
      s21_transpose(&comp, &trans);

      s21_mult_number(&trans, 1 / determinant, result);

      s21_remove_matrix(&comp);
      s21_remove_matrix(&trans);
    }
  }

  return status;
}

// Tools

bool validate(matrix_t *matrix) {
  return matrix && (matrix->rows > 0) && (matrix->columns > 0) &&
         matrix->matrix;
}

/// @brief Операция сложения или вычитания двух матриц
/// @param A первая матрица
/// @param B вторая матрица
/// @param result матрица-результат
/// @result Код статуса
int sum_sub(matrix_t *A, matrix_t *B, matrix_t *result, bool sign) {
  int status = OK;

  if (!validate(A) || !validate(B))
    status = ERROR_MATRIX;
  else if (A->rows != B->rows || A->columns != B->columns)
    status = ERROR_CALCULATION;
  else if (s21_create_matrix(A->rows, A->columns, result) != OK)
    status = ERROR_MATRIX;
  else
    for (int i = 0; i < A->rows; i++)
      for (int j = 0; j < A->columns; j++)
        if (sign)
          result->matrix[i][j] = A->matrix[i][j] + B->matrix[i][j];
        else
          result->matrix[i][j] = A->matrix[i][j] - B->matrix[i][j];

  return status;
}

/// @brief Вычисление минора квадратной матрицы
/// @param row строка
/// @param column столбец
/// @param matrix исходная матрица
/// @param result матрица-результат
void minor_matrix(int row, int column, matrix_t *matrix, matrix_t *result) {
  s21_create_matrix(matrix->rows - 1, matrix->columns - 1, result);

  int di = 0, dj = 0;

  for (int i = 0; i < result->rows; i++) {
    if (i == row) di = 1;
    dj = 0;

    for (int j = 0; j < result->columns; j++) {
      if (j == column) dj = 1;
      result->matrix[i][j] = matrix->matrix[i + di][j + dj];
    }
  }
}

/// @brief Вычисление определителья квадратной матрицы
/// @param matrix матрица
/// @result Определитель матрицы
double det_matrix(matrix_t *matrix) {
  double result = 0;

  if (matrix->rows == 1)
    result = matrix->matrix[0][0];
  else if (matrix->rows == 2)
    result = (matrix->matrix[0][0] * matrix->matrix[1][1]) -
             (matrix->matrix[0][1] * matrix->matrix[1][0]);
  else {
    int sign = 1;
    for (int i = 0; i < matrix->rows; i++) {
      matrix_t temp;
      minor_matrix(0, i, matrix, &temp);

      result += sign * matrix->matrix[0][i] * det_matrix(&temp);
      sign *= -1;

      s21_remove_matrix(&temp);
    }
  }

  return result;
}